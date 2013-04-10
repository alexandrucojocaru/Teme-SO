/*
 * main.c
 *
 *  Created on: Apr 8, 2013
 *      Author: Constantin Serban-Radoi 333CA
 *
 * 	Tema 2 SO
 */
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mpi.h"
#include "utils.h"
#include "common.h"

int main(int argc, char *argv[]) {
	pid_t *pids;
	pid_t pid;
	int i, N;
	int rc, status;

	if (argc < 4 || strcmp(argv[1], "-np") != 0) {
		fprintf(stderr, "Usage: ./mpirun -np NR_PROC command\n");
		return MPI_ERR_OTHER;
	}

	N = atoi(argv[2]);
	dprintf("N = %d\n", N);
	if (N < 0) {
		fprintf(stderr, "Incorrect number of processes. Must be positive\n");
		return MPI_ERR_OTHER;
	}

	pids = calloc(N, sizeof(pids));

	for (i = 0; i < N; ++i) {
		pid = fork();
		if (pid < 0) {
			DIE(TRUE, "fork");
		}
		if (pid == 0) {
			/* Child */
			int rc;

			sprintf(argv[1], "%d", i);	/* Replace with rank. must be < 1000 */
			rc = execvp(argv[3], argv + 1);
			DIE(rc < 0, "execvp");
		}
		if (pid > 0) {
			dprintf("Executing %s\n", argv[3]);
			pids[i] = pid;
		}
	}

	for (i = 0; i < N; ++i) {
		rc = waitpid(pids[i], &status, 0);
		DIE(rc < 0, "waitpid");
	}

	free(pids);

	return MPI_SUCCESS;
}
