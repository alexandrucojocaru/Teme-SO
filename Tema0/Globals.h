/*
 * @author: Constantin Serban-Radoi 333CA
 *
 * This file contains Global variables and macros
 *
 * */

/* Checks for errors */
#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdlib.h>
#include <stdio.h>

#define DBG 0
#define BUFSZ 20001
#define CMDSZ 32

#define DIE(assertion, call_description)				\
	do {												\
		if (assertion) {								\
			fprintf(stderr, "(%s, %d): ",				\
					__FILE__, __LINE__);				\
			perror(call_description);					\
			exit(EXIT_FAILURE);							\
		}												\
	} while(0)

enum Bool {
	FALSE = 0,
	TRUE = 1
};

#endif
