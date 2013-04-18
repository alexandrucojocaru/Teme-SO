/*
 * virtual machine simulator implementation
 *
 * Author: Constantin Serban-Radoi 333CA
 * Tema 3 SO - Aprilie 2013
 */
#include "vmsim_mem.h"
#include "util.h"
#include "debug.h"

LPVOID w_map(HANDLE fd, DWORD size) {
	HANDLE hFileMap;
	LPVOID p;

	hFileMap = CreateFileMapping(
			fd,
			NULL,
			PAGE_READWRITE,
			0,
			size,
			NULL);
	DIE(hFileMap == NULL, "CreateFileMapping");

	p = MapViewOfFile(
			hFileMap,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			0);
	DIE(p == NULL, "MapViewOfFile");

	return p;
}