/*
 * virtual machine simulator implementation
 *
 * Author: Constantin Serban-Radoi 333CA
 * Tema 3 SO - Aprilie 2013
 */
#ifndef VMSIM_MEM_H_
#define VMSIM_MEM_H_
#include "common.h"
#include "vmsim.h"
#include "string.h"

#include <vector>

using namespace std;

typedef enum page_state_ {
	STATE_IN_RAM,
	STATE_IN_SWAP,
	STATE_NOT_ALLOC
}page_state;
 
struct frame_;
 
/* handle pages (virtual pages) */
typedef struct page_table_entry_ {
	page_state state;
	page_state prev_state;
	w_boolean_t dirty;
	w_prot_t protection;
	w_ptr_t start;
	struct frame_ *frame;	/* NULL in case page is not mapped */
}page_table_entry_t;
 
/* handle frames (physical pages) */
typedef struct frame_ {
	w_size_t index;
	/* "backlink" to page_table_entry; NULL in case of free frame */
	page_table_entry_t *pte;
}frame_t;

/* Handles a mapping */
typedef struct mem_tables_ {
	vector<page_table_entry_t> virtual_pages;
	vector<frame_t> ram_frames;
	vm_map_t* map;
	int pages_in_ram;
}mem_tables_t;

#define RAM_PREFIX "ram"
#define SWAP_PREFIX "swap"

#define MAX_PATH 256

/*
 * Fills a file with a given byte
 */
void fill_file(w_handle_t handle, w_size_t size, char byte);

/*
 * Returns the protection used by mmap given a w_prot_t type
 */
int get_prot(w_prot_t prot);

/*
 * Fills a file with zeros
 */
#define zero_file(handle, size)		fill_file(handle, size, 0)

#endif
