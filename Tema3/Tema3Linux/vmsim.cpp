/*
 * virtual machine simulator implementation
 *
 * Author: Constantin Serban-Radoi 333CA
 * Tema 3 SO - Aprilie 2013
 */

#define DLL_EXPORTS

#include "vmsim.h"
#include "vmsim_mem.h"
#include "util.h"
#include "debug.h"

#include <map>

using namespace std;

/* Holds the table of mappings */
static map<w_ptr_t, mem_tables_t> alloc_states;

/* initialize and cleanup library -- consider exception handler */
w_boolean_t vmsim_init(void) {
	return w_set_exception_handler(vmsim_exception_handler);
}
w_boolean_t vmsim_cleanup(void) {
	w_exception_handler_t handler;
	w_get_previous_exception_handler(&handler);
	return w_set_exception_handler(handler);
}

/*
 * Create a handle for ram or swap given a prefix for name and the number of
 * pages that the file will contain
 */
static w_handle_t create_handle(char *prefix, w_size_t num_pages) {
	char buf[MAX_PATH];
	w_handle_t handle;
	int rc;

	sprintf(buf, "%sXXXXXX", prefix);

	dlog(LOG_DEBUG, "prefix for mkstemp: {%s}\n", prefix);
	handle = mkstemp(buf);
	DIE(handle == INVALID_HANDLE, "mkstemp");

	rc = ftruncate(handle, p_sz * num_pages);
	DIE(rc < 0, "ftruncate");

	return handle;
}

/*
 * Initialize a page entry
 */
static void init_page_entry(page_table_entry_t &page_entry) {
	page_entry.dirty = FALSE;
	page_entry.frame = NULL;
	page_entry.prev_state = STATE_NOT_ALLOC;
	page_entry.protection = PROTECTION_NONE;
	page_entry.start = NULL;
	page_entry.state = STATE_NOT_ALLOC;
}

/*
 * Allocate granullar pages
 */
static w_ptr_t allocate_granullar(
		w_size_t num_pages,
		page_table_entry_t &page_entry,
		mem_tables_t &mem_tables) {
	w_ptr_t base_address;
	int rc;
	w_size_t size;
	w_size_t i;

	size = num_pages * p_sz;

	/* initial allocation (NUM_PAGES size) */
	base_address = (w_ptr_t)mmap(NULL, size, PROT_NONE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	DIE(base_address == MAP_FAILED, "mmap");
	dlog(LOG_DEBUG, "VirtualAlloc (reserve) returned %p\n", base_address);

	/* free allocation - leave room for page granular allocations */
	rc = munmap(base_address, size);
	DIE(rc < 0, "munmap");

	/* do page granular allocations at given addresses */
	for (i = 0; i < num_pages; i++) {
		page_entry.start = (w_ptr_t)mmap((char *)base_address + i * p_sz,
				p_sz, PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		DIE(page_entry.start == MAP_FAILED, "mmap");
		dlog(LOG_DEBUG, "granular VirtualAlloc (reserve) returned %p\n",
			page_entry.start);

		memcpy(&mem_tables.virtual_pages[i], &page_entry,
			sizeof(page_table_entry_t));
	}

	return base_address;
}

/*
 * allocate physical pages in RAM mapped by virt_pages in swap
 * map is to be filled with start address and handle to RAM and swap
 * files
 */
w_boolean_t vm_alloc(w_size_t num_pages, w_size_t num_frames, vm_map_t *map) {
	if (num_frames > num_pages) {
		return FALSE;
	}

	mem_tables_t mem_tables;
	/* Create entries in arrays for ram and swap */
	mem_tables.virtual_pages.resize(num_pages);
	mem_tables.ram_frames.resize(num_frames);
	
	page_table_entry_t page_entry;
	w_handle_t ram_handle, swap_handle;

	/* Create files for ram and swap */
	ram_handle = create_handle((char *)RAM_PREFIX, num_frames);
	swap_handle = create_handle((char *)SWAP_PREFIX, num_pages);

	init_page_entry(page_entry);

	/* Allocate the virtual memory */
	map->start = allocate_granullar(num_pages, page_entry, mem_tables);

	map->ram_handle = ram_handle;
	map->swap_handle = swap_handle;

	mem_tables.map = map;
	mem_tables.pages_in_ram = 0;

	/* Add mapping to the global table */
	alloc_states[map->start] = mem_tables;

	return TRUE;
}

/*
 * free space previously allocated with vm_alloc
 * start is the start address of the previously allocated area
 *
 * implementation has to also close handles corresponding to RAM and
 * swap files
 */

w_boolean_t vm_free(w_ptr_t start) {
	/* Sanity checks */
	if (start == NULL) {
		dlog(LOG_DEBUG, "vm_free was called with NULL start pointer\n");
		return FALSE;
	}

	if (alloc_states.find(start) == alloc_states.end()) {
		dlog(LOG_DEBUG, "vm_free was called with bad start address\n");
		return FALSE;
	}

	mem_tables_t table;
	unsigned int i;

	table = alloc_states[start];

	for (i = 0; i < table.virtual_pages.size(); ++i) {
		int rc = munmap(table.virtual_pages[i].start, p_sz);
		DIE(rc < 0, "munmap");

		free(table.virtual_pages[i].frame);
		table.virtual_pages[i].frame = NULL;
	}

	w_close_file(table.map->ram_handle);
	w_close_file(table.map->swap_handle);

	alloc_states.erase(alloc_states.find(start));

	return TRUE;
}

/*
 * Make the swap in copying
 */
static void swap_in(page_table_entry_t *page, w_handle_t swap_handle,
					w_handle_t ram_handle, int page_no,
					w_ptr_t page_addr) {
	char *buf = (char *)malloc(p_sz * sizeof(char));
	dlog(LOG_DEBUG, "Swapping in address %p\n", page->start);

	/* Unmap already mapped annonymous swap */
	int rc = munmap(page_addr, p_sz);
	DIE(rc < 0, "munmap");

	w_ptr_t mapped_addr = mmap(page_addr, p_sz, PROT_READ, MAP_SHARED,
			swap_handle, page_no * p_sz);
	DIE(mapped_addr == MAP_FAILED, "mmap");

	dlog(LOG_DEBUG, "Mapped address %p for copying from swap\n", mapped_addr);

	memcpy(buf, mapped_addr, p_sz);

	rc = munmap(mapped_addr, p_sz);
	DIE(rc < 0, "munmap");

	mapped_addr = mmap(page_addr, p_sz, PROT_WRITE, MAP_SHARED, ram_handle, 0);
	DIE(mapped_addr == MAP_FAILED, "mmap");

	dlog(LOG_DEBUG, "Mapped address %p for copying from buf to ram\n",
			mapped_addr);

	memcpy(mapped_addr, buf, p_sz);
	rc = munmap(mapped_addr, p_sz);
	DIE(rc < 0, "munmap");

	free(buf);
}

/*
 * Make the swap out copying
 */
static void swap_out(frame_t *swapped_frame, w_handle_t swap_handle,
					 int page_no) {
	dlog(LOG_DEBUG, "Swapping out address which seems to be dirty or never swapped %p\n",
		swapped_frame->pte->start);
					
	char *buf = (char *)malloc(p_sz * sizeof(char));

	memcpy(buf, swapped_frame->pte->start, p_sz);

	int rc = munmap(swapped_frame->pte->start, p_sz);
	DIE(rc < 0, "munmap");

	/* Map with prot write to make the swapping out */
	swapped_frame->pte->start = mmap(swapped_frame->pte->start,
			p_sz, PROT_WRITE, MAP_SHARED, swap_handle, page_no * p_sz);
	DIE(swapped_frame->pte->start == MAP_FAILED, "mmap");

	dlog(LOG_DEBUG, "Before MEMPCY\n");
	memcpy(swapped_frame->pte->start, buf, p_sz);

	/* Unmap the swapped page after swapping out */
	rc = munmap(swapped_frame->pte->start, p_sz);
	DIE(rc < 0, "munmap");

	swapped_frame->pte->start = mmap(swapped_frame->pte->start,
			p_sz, PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	DIE(swapped_frame->pte->start == MAP_FAILED, "mmap");

	dlog(LOG_DEBUG, "Swaped out address %p and written to disk\n",
			swapped_frame->pte->start);

	free(buf);
}

/*
 * Create a frame
 */
static frame_t *create_frame(w_size_t index, page_table_entry_t *page) {
	frame_t * frame = (frame_t *) malloc(sizeof(frame_t));
	frame->index = index;
	frame->pte = page;

	return frame;
}

/*
 * Handles a specific fault given its address
 */
static void handle_address_fault(w_ptr_t addr, map<w_ptr_t,
		mem_tables_t>::iterator it) {
	int page_no = (int)((unsigned long)addr -
			(unsigned long)it->second.map->start) / p_sz;
	dlog(LOG_DEBUG, "Page number %d at address %p\n",
		page_no, addr);

	w_ptr_t page_addr = (w_ptr_t)((unsigned long)it->first +
			page_no * (unsigned long)p_sz);
	dlog(LOG_DEBUG, "Computed page_addr is %p\n", page_addr);

	page_table_entry_t *page = &(it->second.virtual_pages[page_no]);

	/* Increase protection of the page */
	page->protection = (w_prot_t)(((int)page->protection + 1) % 3);

	/* Page was already allocated and must be unmapped to remap it */
	if (page->state == STATE_IN_RAM || page->state == STATE_NOT_ALLOC) {
		int rc = munmap(page->start, p_sz);
		DIE(rc < 0, "munmap");

		if (page->protection == PROTECTION_WRITE)
			page->dirty = TRUE;
	}

	int num_frames = (int)it->second.ram_frames.size();

	w_ptr_t mapped_addr;

	/* Allocate in RAM */
	if (it->second.pages_in_ram < num_frames || page->state == STATE_IN_RAM) {
		unsigned long index;
		if (page->frame == NULL && it->second.pages_in_ram < num_frames) {
			index = it->second.pages_in_ram;
		}
		else if (page->frame != NULL) {
			index = page->frame->index;
		}
		else {
			index = -1;
		}

		if ((int)index != -1) {
			dlog(LOG_DEBUG, "Alocating in ram for index %lu\n", index);
			mapped_addr = mmap(page_addr, p_sz, get_prot(page->protection),
					MAP_SHARED, it->second.map->ram_handle, index * p_sz);
			DIE(mapped_addr == MAP_FAILED, "mmap");

			page->prev_state = page->state;
			page->state = STATE_IN_RAM;
			page->start = mapped_addr;

			if (page->frame == NULL) {
				page->frame = create_frame(it->second.pages_in_ram++, page);

				it->second.ram_frames[page->frame->index] = *page->frame;
			}
			dlog(LOG_DEBUG, "Alocated in RAM address %p\n", mapped_addr);
		}
	}

	/* Swap out if RAM is full. Also manage swap in here */
	else if (it->second.pages_in_ram == num_frames &&
			page->state != STATE_IN_RAM) {
		/* Swap out the first page from ram */
		frame_t *swapped_frame = &it->second.ram_frames[0];

		dlog(LOG_DEBUG, "Swapping out address %p\n", swapped_frame->pte->start);

		/* If dirty, or has never been copied to swap, copy to SWAP */
		if ((swapped_frame->pte->dirty == FALSE &&
				(swapped_frame->pte->prev_state == STATE_NOT_ALLOC ||
				swapped_frame->pte->prev_state == STATE_IN_RAM)) ||
				swapped_frame->pte->dirty == TRUE) {

			swap_out(swapped_frame, it->second.map->swap_handle, page_no);
		}
		/* Skip the copying to swap. Just unmap the page */
		else if (swapped_frame->pte->state == STATE_IN_RAM) {
			int rc = munmap(swapped_frame->pte->start, p_sz);
			DIE(rc < 0, "munmap");

			swapped_frame->pte->start = mmap(swapped_frame->pte->start,
					p_sz, PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
			DIE(swapped_frame->pte->start == MAP_FAILED, "mmap");

			dlog(LOG_DEBUG, "Swaped out address %p and didn't write to disk\n",
				swapped_frame->pte->start);
		}
		swapped_frame->pte->dirty = FALSE;
		swapped_frame->pte->prev_state = swapped_frame->pte->state;
		swapped_frame->pte->state = STATE_IN_SWAP;
		swapped_frame->pte->protection = PROTECTION_NONE;

		/* Swap in */
		if (page->state == STATE_IN_SWAP) {
			swap_in(page,
				it->second.map->swap_handle,
				it->second.map->ram_handle,
				page_no,
				page_addr);
		}

		mapped_addr = mmap(page_addr, p_sz, get_prot(page->protection),
				MAP_SHARED, it->second.map->ram_handle, 0);
		DIE(mapped_addr == MAP_FAILED, "mmap");

		page->prev_state = page->state;
		page->state = STATE_IN_RAM;
		page->start = mapped_addr;

		if (page->frame == NULL) {
			page->frame = create_frame(0, page);
		}
		it->second.ram_frames[page->frame->index] = *page->frame;
				
		dlog(LOG_DEBUG, "Replaced old ram with address %p\n", page->start);
	}

	dlog(LOG_DEBUG, "mapped_addr is %p and protection is %d\n", page->start,
		page->protection);
}

/*
 * Check if a given address is in range
 */
static w_boolean_t is_address_in_range(w_ptr_t addr, w_ptr_t start,
		w_ptr_t end) {
	return ((unsigned long)addr >= (unsigned long)start &&
		(unsigned long)addr < (unsigned long)end) ? TRUE : FALSE;
}

void vmsim_exception_handler(int sig, siginfo_t *siginfo, void *aux) {
	w_ptr_t addr;

	if (sig != SIGSEGV) {
		return;
	}

	/* Get the address of page witch generated the page fault */
	addr = (w_ptr_t)siginfo->si_addr;

	dlog(LOG_DEBUG, "Page fault at address %p\n", addr);

	map<w_ptr_t, mem_tables_t>::iterator it;

	/* Walk through all mappings to find the one that caused the fault */
	for (it = alloc_states.begin(); it != alloc_states.end(); ++it) {
		w_ptr_t start = it->first;
		w_ptr_t end = (w_ptr_t)(((unsigned long)it->second.virtual_pages.size()
				* p_sz) + (unsigned long)(it->first));

		dlog(LOG_DEBUG, "Testing if address %p is betweem %p and %p\n",
				addr, start, end);

		w_boolean_t in_range = is_address_in_range(addr, start, end);

		if (in_range == TRUE) {
			dlog(LOG_DEBUG, "Address %p is between %p and %p\n",
					addr, start, end);

			handle_address_fault(addr, it);

			break;
		}
	}

}
