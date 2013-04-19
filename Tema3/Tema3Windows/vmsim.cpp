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

static map<w_ptr_t, mem_tables_t> alloc_states;
static w_handle_t access_violation_handler = INVALID_HANDLE;

/* initialize and cleanup library -- consider exception handler */
w_boolean_t vmsim_init(void) {
	access_violation_handler = w_add_exception_handler(vmsim_exception_handler);

	return access_violation_handler != NULL;
}
w_boolean_t vmsim_cleanup(void) {
	return w_remove_exception_handler(access_violation_handler);
}

/*
 * Create a handle for ram or swap given a prefix for name and the number of
 * pages that the file will contain
 */
static w_handle_t create_handle(char *prefix, w_size_t num_pages) {
	char buf[MAX_PATH];
	UINT ret;
	w_handle_t handle;

	ret = GetTempFileName(".", prefix, 0, buf);
	DIE(ret == 0, "GetTempFileName");
	handle = w_open_file(buf, MODE_FULL_OPEN);

	w_set_file_pointer(handle, p_sz * num_pages);
	SetEndOfFile(handle);
	w_set_file_pointer(handle, 0);
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

	ram_handle = create_handle(RAM_PREFIX, num_frames);
	swap_handle = create_handle(SWAP_PREFIX, num_pages);

	init_page_entry(page_entry);

	{
		w_ptr_t base_address;
		w_boolean_t rc;
		w_size_t size;
		w_size_t i;

		size = num_pages * p_sz;

		/* initial allocation (NUM_PAGES size) */
		base_address = VirtualAlloc(
				NULL,
				size,
				MEM_RESERVE,
				PAGE_NOACCESS);
		DIE(base_address == NULL, "VirtualAlloc");

		dlog(LOG_DEBUG, "VirtualAlloc (reserve) returned %p\n", base_address);

		/* free allocation - leave room for page granular allocations */
		rc = VirtualFree(base_address, 0, MEM_RELEASE);
		DIE(rc == FALSE, "VirtualFree");

		/* do page granular allocations at given addresses */
		for (i = 0; i < num_pages; i++) {
			page_entry.start = VirtualAlloc(
					(char *) base_address + i * p_sz,
					p_sz,
					MEM_RESERVE,
					PAGE_NOACCESS);
			DIE(page_entry.start == NULL, "Virtual Alloc");
			dlog(LOG_DEBUG, "granular VirtualAlloc (reserve) returned %p\n", page_entry.start);

			memcpy(&mem_tables.virtual_pages[i], &page_entry, sizeof(page_table_entry_t));
		}

		map->start = base_address;
	}

	map->ram_handle = ram_handle;
	map->swap_handle = swap_handle;

	mem_tables.map = map;
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
	if (start == NULL) {
		dlog(LOG_DEBUG, "vm_free was called with NULL start pointer\n");
		return FALSE;
	}

	if (alloc_states.find(start) == alloc_states.end()) {
		dlog(LOG_DEBUG, "vm_free was called with bad start address\n");
		return FALSE;
	}

	w_boolean_t rc;
	mem_tables_t table;
	unsigned int i;

	table = alloc_states[start];

	for (i = 0; i < table.virtual_pages.size(); ++i) {
		rc = VirtualFree(table.virtual_pages[i].start,
			0,
			MEM_RELEASE);
		DIE(rc == FALSE, "VirtualFree");
	}

	w_close_file(table.map->ram_handle);
	w_close_file(table.map->swap_handle);

	alloc_states.erase(alloc_states.find(start));

	return TRUE;
}

LONG vmsim_exception_handler(PEXCEPTION_POINTERS eptr) {
	PBYTE addr;
	int page_no;

	/* Get the address of page witch generated the page fault */
	addr = (PBYTE)eptr->ExceptionRecord->ExceptionInformation[1];

	map<w_ptr_t, mem_tables_t>::iterator it;
	bool found = false;

	/* Walk through all mappings to find  */
	for (it = alloc_states.begin(); it != alloc_states.end(); ++it) {
		if (addr >= (PBYTE)(it->first) &&
			addr < (PBYTE)((PBYTE)(it->second.virtual_pages.size() * p_sz) -
			(PBYTE)(it->second.map->start))) {
			found = true;

			page_no = (int)(addr - (PBYTE)it->second.map->start) / p_sz;
			dlog(LOG_DEBUG, "Page fault for page number %d at address %p\n",
				page_no, addr);

			/* free allocation - leave room for MapViewOfFileEx */
			PBYTE page_addr = (PBYTE)((UINT32)it->second.map->start + page_no * (int)p_sz);
			dlog(LOG_DEBUG, "Computed page_addr is %p\n", page_addr);
			BOOL rc = VirtualFree(page_addr, 0, MEM_RELEASE);
			DIE(rc == FALSE, "VirtualFree");

			//page_table_entry_t *page = &(it->second.virtual_pages[page_no]);
			w_ptr_t mapped_addr = w_map(it->second.map->ram_handle, p_sz, page_addr);

			break;
		}
	}

	if (!found)
		return EXCEPTION_CONTINUE_SEARCH;


	return EXCEPTION_CONTINUE_EXECUTION;
}
