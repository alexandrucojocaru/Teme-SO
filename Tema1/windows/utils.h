/**
 * Operating Systems 2013 - Assignment 1
 *
 */


#ifndef _UTILS_H
#define _UTILS_H

#include "parser.h"

#define CHUNK_SIZE 100
#define ERR_ALLOCATION "unable to allocate memory"

#define SHELL_EXIT -100
#define VAR_ASSIGN_FAILED -200
#define CREATE_PROCESS_FAILED -300

#define INTERNAL TRUE
#define EXTERNAL FALSE

#define NO_PIPE 0
#define PIPE_IN 1
#define PIPE_OUT 2

#define DEBUG 1

/* useful macro for handling error codes */
#define DIE(assertion, call_description)			\
       do {							\
               if (assertion) {					\
                       fprintf(stderr, "(%s, %s, %d): ",	\
                       __FILE__, __FUNCTION__, __LINE__);	\
                       PrintLastError(call_description);	\
                       exit(EXIT_FAILURE);			\
               }						\
       } while(0)


/**
 * Readline from mini-shell.
 */
char *read_line();

/**
 * Parse and execute a command.
 */
int parse_command(command_t *, int, command_t *, void *);

#endif
