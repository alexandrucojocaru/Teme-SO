/*
 * mpi.c
 *
 *  Created on: Apr 8, 2013
 *      Author: Constantin Serban-Radoi 333CA
 *
 * 	Tema 2 SO
 */
#include "mpi.h"
#include "mpi_err.h"
#include "generic_queue.h"
#include "generic_sem.h"
#include "generic_shm.h"

MPI_Comm MPI_COMM_WORLD = NULL;

/**
 * Checks if data type is valid
 */
static int is_data_type_valid(MPI_Datatype datatype) {
	switch (datatype) {
	case MPI_CHAR:
	case MPI_INT:
	case MPI_DOUBLE:
		return TRUE;
	default:
		return FALSE;
	}
	return FALSE;
}

int MPI_Init(int *argc, char ***argv) {
	if (MPI_COMM_WORLD != NOT_INITIALIZED) {
		return MPI_ERR_OTHER;
	}

	if (*argc < 3) {
		return MPI_ERR_IO;
	}

	MPI_COMM_WORLD = calloc(1, sizeof(struct mpi_comm));
	if (MPI_COMM_WORLD == NULL) {
		return MPI_ERR_IO;
	}

	MPI_COMM_WORLD->rank = atoi((*argv)[0]);
	MPI_COMM_WORLD->size = atoi((*argv)[1]);

	*argc -= 2;
	*argv += 2;

	return MPI_SUCCESS;
}

int MPI_Finalize() {
	if (MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}

	free(MPI_COMM_WORLD);
	MPI_COMM_WORLD = FINALIZED;

	return MPI_SUCCESS;
}

int MPI_Initialized(int *flag) {
	*flag = (MPI_COMM_WORLD != NOT_INITIALIZED);

	return MPI_SUCCESS;
}

int MPI_Finalized(int *flag) {
	*flag = (MPI_COMM_WORLD == FINALIZED);

	return MPI_SUCCESS;
}

int MPI_Comm_size(MPI_Comm comm, int *size) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}
	if (comm != MPI_COMM_WORLD) {
		return MPI_ERR_COMM;
	}

	*size = MPI_COMM_WORLD->size;

	return MPI_SUCCESS;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}
	if (comm != MPI_COMM_WORLD) {
		return MPI_ERR_COMM;
	}

	*rank = MPI_COMM_WORLD->rank;

	return MPI_SUCCESS;
}

int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest,
			int tag, MPI_Comm comm) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}
	if (comm != MPI_COMM_WORLD) {
		return MPI_ERR_COMM;
	}
	if (dest < 0 || dest >= MPI_COMM_WORLD->size) {
		return MPI_ERR_RANK;
	}
	if (!is_data_type_valid(datatype)) {
		return MPI_ERR_TYPE;
	}

	return MPI_SUCCESS;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype,
			int source, int tag, MPI_Comm comm, MPI_Status *status) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		dprintf("Recv: MPI_COMM_WORLD: %d\n", MPI_COMM_WORLD);
		return MPI_ERR_OTHER;
	}
	if (comm != MPI_COMM_WORLD) {
		dprintf("Recv: comm != MPI_COMM_WORLD: %d != %d\n", comm, MPI_COMM_WORLD);
		return MPI_ERR_COMM;
	}
	if (source != MPI_ANY_SOURCE) {
		dprintf("Recv: source not ok: %d\n", source);
		return MPI_ERR_RANK;
	}
	if (tag != MPI_ANY_TAG) {
		dprintf("Recv: tag != MPI_ANY_TAG: %d != %d\n", tag, MPI_ANY_TAG);
		return MPI_ERR_TAG;
	}
	if (!is_data_type_valid(datatype)) {
		dprintf("Recv: datatype invalid: %d\n", datatype);
		return MPI_ERR_TYPE;
	}

	dprintf("Recv should return success here\n");

	return MPI_SUCCESS;
}

int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}
	if (!is_data_type_valid(datatype)) {
		return MPI_ERR_TYPE;
	}

	return MPI_SUCCESS;
}
