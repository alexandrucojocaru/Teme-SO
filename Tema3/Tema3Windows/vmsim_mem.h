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

/* Handles for the file mappings */
typedef struct mapping_handles_ {
	w_handle_t ram_map_handle;
	w_handle_t swap_map_handle;
} mapping_handles_t;

/* Handles a mapping */
typedef struct mem_tables_ {
	vector<page_table_entry_t> virtual_pages;
	vector<frame_t> ram_frames;
	vm_map_t* map;
	mapping_handles_t mapping_handles;
	int pages_in_ram;
}mem_tables_t;

#define RAM_PREFIX "ram"
#define SWAP_PREFIX "swa"

/*
 * File mapping/unmapping
 */
w_handle_t w_create_file_mapping(w_handle_t fd, DWORD pages_to_map);
w_boolean_t w_close_file_mapping(w_handle_t file_mapping);
w_ptr_t w_map(HANDLE file_map, DWORD offset_in_file, DWORD size_to_map, w_ptr_t address, w_prot_t prot);
w_boolean_t w_unmap(w_ptr_t address);


void fill_file(w_handle_t handle, w_size_t size, char byte);

/*
 * Allocate virtual address range from a base address
 */
w_ptr_t w_virtual_alloc(w_ptr_t address, w_size_t size);

/*
 * Fills a file with zeros
 */
#define zero_file(handle, size)		fill_file(handle, size, 0)

#endif