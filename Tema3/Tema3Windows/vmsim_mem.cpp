/*
 * virtual machine simulator implementation
 *
 * Author: Constantin Serban-Radoi 333CA
 * Tema 3 SO - Aprilie 2013
 */
#include "vmsim_mem.h"
#include "util.h"
#include "debug.h"

/*
 * Create a file mapping
 */
w_handle_t w_create_file_mapping(w_handle_t fd, DWORD pages_to_map) {
	w_handle_t file_map;
	file_map = CreateFileMapping(
			fd,
			NULL,
			PAGE_READWRITE,
			0,
			p_sz * pages_to_map,
			NULL);
	DIE(file_map == NULL, "CreateFileMapping");
	return file_map;
}

w_boolean_t w_close_file_mapping(w_handle_t file_mapping) {
	BOOL rc = CloseHandle(file_mapping);
	DIE(rc == FALSE, "CloseHandle");
	return rc;
}

/*
 * Map a base address to a file
 */
w_ptr_t w_map(HANDLE file_map, DWORD offset_in_file, DWORD size_to_map, w_ptr_t address, w_prot_t prot) {
	w_ptr_t p;
	DWORD file_access;

	switch (prot) {
	case PROTECTION_READ:
		file_access = FILE_MAP_READ;
		break;
	case PROTECTION_WRITE:
		file_access = FILE_MAP_WRITE;
		break;
	default:
		break;
	}

	dlog(LOG_DEBUG, "w_map: prot is %d and file_access is %lu\n", prot, file_access);

	/* use MapViewOfFileEx to map file to that exact location */
	p = MapViewOfFileEx(
		file_map,
		file_access,
		0,
		offset_in_file,
		size_to_map,
		address);
	DIE(p == NULL, "MapViewOfFileEx");

	dlog(LOG_DEBUG, "MapViewOfFileEx returned %p\n", p);

	return p;
}

/*
 * Unmap view of file
 */
w_boolean_t w_unmap(w_ptr_t address) {
	BOOL rc = UnmapViewOfFile(address);
	DIE(rc == FALSE, "UnmapViewOfFile");
	return rc;
}

/*
 * Allocate virtual address range from a base address
 */
w_ptr_t w_virtual_alloc(w_ptr_t address, w_size_t size) {
	w_ptr_t start;
	start = VirtualAlloc(
			address,
			size,
			MEM_RESERVE,
			PAGE_NOACCESS);
	DIE(start == NULL, "VirtualAlloc");
	return start;
}

void fill_file(w_handle_t handle, w_size_t size, char byte)
{
	char buf[BUFSIZ];
	w_size_t to_write;
	w_size_t left;

	w_set_file_pointer(handle, 0);

	memset(buf, byte, BUFSIZ);
	left = size;
	while (left > 0) {
		to_write = (BUFSIZ > left ? left : BUFSIZ);
		w_write_file(handle, buf, to_write);
		left -= to_write;
	}
}