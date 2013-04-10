/**
 * utils.h
 *
 *  Created on: Apr 8, 2013
 *      Author: Constantin Serban-Radoi 333CA
 *
 * 	Tema 2 SO
 */

#ifndef LIN_UTILS_H_
#define LIN_UTILS_H_	1

#include <stdio.h>
#include <stdlib.h>

/* useful macro for handling error codes */
#define DIE(assertion, call_description)		\
	do {										\
		if (assertion) {						\
			fprintf(stderr, "(%s, %d): ",		\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(EXIT_FAILURE);					\
		}										\
	} while(0)

#endif

#define TRUE 1
#define FALSE 0

#define NOT_INITIALIZED NULL
#define FINALIZED ((void *)-1)
