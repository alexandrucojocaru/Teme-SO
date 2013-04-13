/*
 * virtual machine simulator implementation
 *
 * Author: Constantin Serban-Radoi 333CA
 * Tema 3 SO - Aprilie 2013
 */

#define DLL_EXPORTS

#include "vmsim.h"

/* initialize and cleanup library -- consider exception handler */
w_boolean_t vmsim_init(void) {
	return TRUE;
}
w_boolean_t vmsim_cleanup(void) {
	return TRUE;
}

/*
 * allocate physical pages in RAM mapped by virt_pages in swap
 * map is to be filled with start address and handle to RAM and swap
 * files
 */

w_boolean_t vm_alloc(w_size_t num_pages, w_size_t num_frames, vm_map_t *map) {
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
	return TRUE;
}

LONG vmsim_exception_handler(PEXCEPTION_POINTERS eptr) {
	return 0;
}
