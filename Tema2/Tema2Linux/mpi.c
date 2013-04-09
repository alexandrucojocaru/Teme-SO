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

int MPI_Init(int *argc, char ***argv) {
	if (MPI_COMM_WORLD != NOT_INITIALIZED) {
		return MPI_ERR_OTHER;
	}

	MPI_COMM_WORLD = calloc(1, sizeof(struct mpi_comm));

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

	return MPI_SUCCESS;
}

int MPI_Comm_rank(MPI_Comm comm, int *rank) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}

	return MPI_SUCCESS;
}

int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest,
			int tag, MPI_Comm comm) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}

	return MPI_SUCCESS;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype,
			int source, int tag, MPI_Comm comm, MPI_Status *status) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}

	return MPI_SUCCESS;
}

int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count) {
	if (MPI_COMM_WORLD == NOT_INITIALIZED || MPI_COMM_WORLD == FINALIZED) {
		return MPI_ERR_OTHER;
	}

	return MPI_SUCCESS;
}
