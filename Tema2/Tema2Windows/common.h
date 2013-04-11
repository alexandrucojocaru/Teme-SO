/**
  * SO, 2011
  * Lab #5
  *
  * Task #2, lin
  *
  * Types and structures
  */
#ifndef COMMON_H_
#define COMMON_H_	1

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "mpi.h"

/**
 * Maximum size for the name of an IPC name
 */
#define MAX_IPC_NAME		128

#ifndef WIN32


#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <semaphore.h>
#include <mqueue.h>

struct _msgq_t {
	mqd_t	mq;
	char	name[MAX_IPC_NAME];
};

struct _sema_t {
	sem_t*	sem;
	char	name[MAX_IPC_NAME];
};

struct _shm_t {
	int 	fd;
	char 	name[MAX_IPC_NAME];
};

typedef struct _msgq_t*	msgq_t;
typedef struct _sema_t*	sema_t;
typedef struct _shm_t*	shm_t;

#define INVALID (void*)-1

#else

#include <windows.h>

#define FATAL_ERROR_ACTION(place)

typedef HANDLE msgq_t;
typedef HANDLE sema_t;
typedef HANDLE shm_t;

/* should be C99... but Windows is unaware... */
#define snprintf(...)	sprintf_s(__VA_ARGS__)

#endif //WIN32

#define _DEBUG

#ifdef _DEBUG
# define dprintf(...) fprintf(stderr, __VA_ARGS__)
#else
# define dprintf(...) do {} while(0)
#endif


#define BASE_QUEUE_NAME 	"my_queue"

#define BASE_SEMA_NAME 		"sem"

#define BASE_SHM_NAME 		"my_shm"

#define BUF_SIZE (1 << 12)

/**
 * the command message to be sent via the queue, with
 * variable length payload of text
 */
struct _message_t {
	MPI_Status status;
	char buf[BUF_SIZE];
};

typedef struct _message_t message_t;


#endif
