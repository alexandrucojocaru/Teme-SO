/**
  * SO, 2011
  * Lab #7
  *
  * Task #4, lin
  *
  * Implementing 'cp' with mmap
  */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

int main (int argc, char *argv[])
{
	int fdin, fdout, rc;
	char *src, *dst;
	struct stat statbuf;

	DIE(argc != 3, "Usage: ./mycp <from_file> <to_file>");

	/* TODO - open the input file */
	fdin = open (argv[1], O_RDONLY);
	DIE(fdin == -1, "open fdin");

	/* TODO - open/create the output file */
	fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
	DIE(fdout == -1, "open fdout");

	/* TODO - truncate the output file to the input file size */
	rc = fstat (fdin, &statbuf);
	DIE(rc == -1, "fstat");

	rc = ftruncate(fdout, statbuf.st_size);
	DIE(rc == -1, "ftruncate");

	/* TODO - mmap the input and output file */
	src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0);
	DIE(src == MAP_FAILED, "mmap src");

	dst = mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE,MAP_SHARED, fdout, 0);
	DIE(dst == MAP_FAILED, "mmap dst");

	/* TODO - copy the input file to the output file */
	memcpy (dst, src, statbuf.st_size);

	/* TODO - clean up */
	rc = munmap(src, statbuf.st_size);
	DIE(rc == -1, "munmap source");

	rc = munmap(dst, statbuf.st_size);
	DIE(rc == -1, "munmap dest");

	rc = close(fdin);
	DIE(rc == -1, "close source");

	rc = close(fdout);
	DIE(rc == -1, "close dest");

	return 0;
}

