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
w_ptr_t w_map(HANDLE fd, DWORD size, w_ptr_t address) {
	HANDLE file_map;
	w_ptr_t p;

	file_map = CreateFileMapping(
			fd,
			NULL,
			PAGE_READWRITE,
			0,
			size,
			NULL);
	DIE(file_map == NULL, "CreateFileMapping");

	/* use MapViewOfFileEx to map file to that exact location */
	p = MapViewOfFileEx(
		file_map,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		size,
		address);
	DIE(p == NULL, "MapViewOfFileEx");

	dlog(LOG_DEBUG, "MapViewOfFileEx returned %p\n", p);

	return p;
}