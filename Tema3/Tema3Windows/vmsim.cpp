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

#include <map>

using namespace std;

static map<w_ptr_t, mem_tables_t> alloc_states;

/* initialize and cleanup library -- consider exception handler */
w_boolean_t vmsim_init(void) {
	return TRUE;
}
w_boolean_t vmsim_cleanup(void) {
	return TRUE;
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
 * allocate physical pages in RAM mapped by virt_pages in swap
 * map is to be filled with start address and handle to RAM and swap
 * files
 */
w_boolean_t vm_alloc(w_size_t num_pages, w_size_t num_frames, vm_map_t *map) {
	if (num_frames > num_pages) {
		return FALSE;
	}

	mem_tables_t mem_tables;
	mem_tables.virtual_pages.resize(num_pages);
	
	page_table_entry_t page_entry;
	w_handle_t ram_handle, swap_handle;

	ram_handle = create_handle(RAM_PREFIX, num_frames);
	swap_handle = create_handle(SWAP_PREFIX, num_pages);

	page_entry.dirty = FALSE;
	page_entry.frame = NULL;
	page_entry.prev_state = STATE_NOT_ALLOC;
	page_entry.protection = PROTECTION_NONE;
	page_entry.start = NULL;
	page_entry.state = STATE_NOT_ALLOC;


	map->ram_handle = ram_handle;
	map->swap_handle = swap_handle;
	map->start = NULL;

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
	if (start == NULL)
		return FALSE;
	return TRUE;
}

LONG vmsim_exception_handler(PEXCEPTION_POINTERS eptr) {
	return 0;
}
