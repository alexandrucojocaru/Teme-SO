/**
 * Operating Sytems 2013 - Assignment 1
 *
 */

#ifndef _UTILS_H
#define _UTILS_H

#include "parser.h"

#define CHUNK_SIZE 100
#define ERR_ALLOCATION "unable to allocate memory"

#define SHELL_EXIT -100
#define VAR_ASSIGN_FAILED -300
#define WAIT_ERROR -200

#define MAX_SIZE_ENVIRONMENT_VARIABLE 100

#define APPEND 1
#define NORMAL 0

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

/**
 * Readline from mini-shell.
 */
char *read_line();

/**
 * Parse and execute a command.
 */
int parse_command(command_t *, int, command_t *);

/**
 * Concatenate parts of the word to obtain the command
 */
static char *get_word(word_t *s);

#endif
