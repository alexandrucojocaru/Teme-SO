/*
 * virtual machine simulator implementation
 *
 * Author: Constantin Serban-Radoi 333CA
 * Tema 3 SO - Aprilie 2013
 */
#include "vmsim_mem.h"
#include "util.h"
#include "debug.h"


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

int get_prot(w_prot_t prot) {
	switch (prot) {
	case PROTECTION_NONE:
		return PROT_NONE;
	case PROTECTION_READ:
		return PROT_READ;
	case PROTECTION_WRITE:
		return PROT_WRITE;
	}
	return -1;
}
