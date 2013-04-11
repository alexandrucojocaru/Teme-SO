/**
 * main.c
 *
 *  Created on: Apr 8, 2013
 *      Author: Constantin Serban-Radoi 333CA
 *
 * 	Tema 2 SO
 */
#include <string.h>
#include "generic_queue.h"
#include "generic_sem.h"
#include "generic_shm.h"
#include "mpi.h"
#include "utils.h"
#include "common.h"

#define MAX_CMD_LINE 256

/**
 * Convert command line for being used in CreateProcess
 */
void convert_command(int argc, char *argv[], char command_line[MAX_CMD_LINE], int rank) {
	int i;
	char rank_char[4];
	strcat(command_line, argv[3]);
	strcat(command_line, " ");
	snprintf(rank_char, 3, "%d", rank);
	strcat(command_line, rank_char);
	strcat(command_line, " ");

	for (i = 2; i < argc; ++i) {
		if (i == 3) {
			continue;
		}
		strcat(command_line, argv[i]);
		strcat(command_line, " ");
	}
}

/*
 * Closes handles for a process
 */
static void close_process(LPPROCESS_INFORMATION lppi)
{
	BOOL bRes;
	bRes = CloseHandle(lppi->hThread);
	DIE(bRes == FALSE, "CloseHandle hThread");
	bRes = CloseHandle(lppi->hProcess);
	DIE(bRes == FALSE, "CloseHandle hProcess");
}

int main(int argc, char *argv[]) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL bRes;

	PROCESS_INFORMATION *processes;
	char command_line[MAX_CMD_LINE];

	int i, N;

	/* Check arguments */
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

	/* Alocate memory for processes the to be created */
	processes = calloc(N, sizeof(PROCESS_INFORMATION));
	DIE(processes == NULL, "calloc");

	/* Create processes */
	for (i = 0; i < N; ++i) {
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

		memset(command_line, 0, sizeof(command_line));
		convert_command(argc, argv, command_line, i);
		dprintf("command {%s}\n", command_line);

		bRes = CreateProcess(
			NULL,
			command_line,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si,
			&pi);

		if (!bRes) {
			DIE(TRUE, "CreateProcess");
		}
		
		memcpy(&processes[i], &pi, sizeof(pi));
	}

	/* Wait for children and close process */
	for (i = 0; i < N; ++i) {
		DWORD dwRes;
		dwRes = WaitForSingleObject(processes[i].hProcess, INFINITE);
		DIE(dwRes == WAIT_FAILED, "WaitForSingleObject");

		bRes = GetExitCodeProcess(processes[i].hProcess, &dwRes);
		DIE(bRes == FALSE, "GetExitCodeProcess");

		close_process(&processes[i]);
	}

	free(processes);

	return MPI_SUCCESS;
}
