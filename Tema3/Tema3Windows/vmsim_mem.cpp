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
 * Map a base address to a file
 */
w_ptr_t w_map(HANDLE fd, DWORD size, w_ptr_t address, w_prot_t prot) {
	HANDLE file_map;
	w_ptr_t p;
	DWORD file_access, handle_access;

	switch (prot) {
	case PROTECTION_READ:
		file_access = FILE_MAP_READ;
		handle_access = PAGE_READONLY;
		break;
	case PROTECTION_WRITE:
		file_access = FILE_MAP_WRITE;
		handle_access = PAGE_READWRITE;
		break;
	default:
		break;
	}

	dlog(LOG_DEBUG, "w_map: prot is %d and file_access is %lu handle_access %lu\n", prot, file_access, handle_access);

	file_map = CreateFileMapping(
			fd,
			NULL,
			handle_access,
			0,
			size,
			NULL);
	DIE(file_map == NULL, "CreateFileMapping");

	/* use MapViewOfFileEx to map file to that exact location */
	p = MapViewOfFileEx(
		file_map,
		file_access,
		0,
		0,
		size,
		address);
	DIE(p == NULL, "MapViewOfFileEx");

	DIE(CloseHandle(file_map) == FALSE, "CloseHandle");

	dlog(LOG_DEBUG, "MapViewOfFileEx returned %p\n", p);

	return p;
}

w_boolean_t w_unmap(w_ptr_t address) {
	BOOL rc = UnmapViewOfFile(address);
	DIE(rc == FALSE, "UnmapViewOfFile");
	return rc;
}