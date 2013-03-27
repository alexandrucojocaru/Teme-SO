/**
 * Operating Systems 2013 - Assignment 1
 *
 */
/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#define READ		0
#define WRITE		1

#define MAX_SIZE_ENVIRONMENT_VARIABLE 100

static LPTSTR get_word(word_t *s);
static LPTSTR get_argv(simple_command_t *command);
static BOOL redirect_std_handles(STARTUPINFO *psi, HANDLE hFileIn,
								 HANDLE hFileOut, HANDLE hFileErr,
								 simple_command_t *s, BOOL internal);

/**
 * Debug method, used by DIE macro.
 */
static VOID PrintLastError(const PCHAR message)
{
	CHAR errBuff[1024];

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		GetLastError(),
		0,
		errBuff,
		sizeof(errBuff) - 1,
		NULL);

	fprintf(stderr, "%s: %s\n", message, errBuff);
}

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir, simple_command_t *s)
{
	/* TODO execute cd */
	HANDLE hFileIn = NULL, hFileOut = NULL, hFileErr = NULL;
	
	LPTSTR directory = NULL;
	BOOL ret, out_is_err;

	out_is_err = redirect_std_handles(NULL, &hFileIn, &hFileOut, &hFileErr, s, INTERNAL);

	directory = get_word(dir);

	ret = SetCurrentDirectory(directory);

	free(directory);

	if (hFileIn != NULL && hFileIn != INVALID_HANDLE_VALUE) {
		DIE(CloseHandle(hFileIn) == FALSE, "CloseHandleIn");
		hFileIn = INVALID_HANDLE_VALUE;
	}
	if (hFileOut != NULL && hFileOut != INVALID_HANDLE_VALUE) {
		DIE(CloseHandle(hFileOut) == FALSE, "CloseHandleOut");
		hFileOut = INVALID_HANDLE_VALUE;
	}
	if (hFileErr != NULL && hFileErr != INVALID_HANDLE_VALUE && out_is_err == FALSE) {
		DIE(CloseHandle(hFileErr) == FALSE, "CloseHandleErr");
		hFileErr = INVALID_HANDLE_VALUE;
	}

	SetStdHandle(STD_INPUT_HANDLE, GetStdHandle(STD_INPUT_HANDLE));
	SetStdHandle(STD_OUTPUT_HANDLE, GetStdHandle(STD_OUTPUT_HANDLE));
	SetStdHandle(STD_ERROR_HANDLE, GetStdHandle(STD_ERROR_HANDLE));

	return ret;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit()
{
	return SHELL_EXIT;
}

/**
 * Concatenate parts of the word to obtain the command
 */
static LPTSTR get_word(word_t *s)
{
	DWORD string_length = 0;
	DWORD substring_length = 0;

	LPTSTR string = NULL;
	CHAR substring[MAX_SIZE_ENVIRONMENT_VARIABLE];

	DWORD dwret;

	while (s != NULL) {
		strcpy(substring, s->string);

		if (s->expand == true) {
			dwret = GetEnvironmentVariable(substring, substring, MAX_SIZE_ENVIRONMENT_VARIABLE);
			if (!dwret)
				/* Environment Variable does not exist. */
				strcpy(substring, "");
		}

		substring_length = strlen(substring);

		string = realloc(string, string_length + substring_length + 1);
		memset(string + string_length, 0, substring_length + 1);

		strcat(string, substring);
		string_length += substring_length;

		s = s->next_part;
	}

	return string;
}

/**
 * Parse arguments in order to succesfully process them using CreateProcess
 */
static LPTSTR get_argv(simple_command_t *command)
{
	LPTSTR argv = NULL;
	LPTSTR substring = NULL;
	word_t *param;

	DWORD string_length = 0;
	DWORD substring_length = 0;

	argv = get_word(command->verb);
	assert(argv != NULL);

	string_length = strlen(argv);

	param = command->params;
	while (param != NULL) {
		substring = get_word(param);
		substring_length = strlen(substring);

		argv = realloc(argv, string_length + substring_length + 4);
		assert(argv != NULL);

		strcat(argv, " ");

		/* Surround parameters with ' ' */
		strcat(argv, "'");
		strcat(argv, substring);
		strcat(argv, "'");

		string_length += substring_length + 3;
		param = param->next_word;

		free(substring);
	}

	return argv;
}

/*
 * Open the file filename, with the mode MODE and creation CREATE_DISP
 */
static HANDLE my_open_file(LPTSTR filename, long MODE, int CREATE_DISP)
{
	SECURITY_ATTRIBUTES sa;

	ZeroMemory(&sa, sizeof(sa));
	sa.bInheritHandle = TRUE;

	return CreateFile(
		filename,
		MODE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&sa,
		CREATE_DISP,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
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

/*
 * @psi		- STATRTUPINFO of the child process
 * @hFile	- file handle for redirect
 * @opt		- redirect option is one of the following
 *		 STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE
 */
static void redirect_handle(STARTUPINFO *psi, HANDLE hFile, INT opt)
{
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	psi->dwFlags |= STARTF_USESTDHANDLES;

	switch (opt) {
	case STD_INPUT_HANDLE:
		psi->hStdInput = hFile;
		break;
	case STD_OUTPUT_HANDLE:
		psi->hStdOutput = hFile;
		break;
	case STD_ERROR_HANDLE:
		psi->hStdError = hFile;
		break;
	}
}

/*
 * Redirect std handles for a newly created process.
 * If internal is set to TRUE, it will set handles for current process
 * Returns TRUE if out has the same name as err
 */
static BOOL redirect_std_handles(STARTUPINFO *psi, HANDLE *hFileIn,
								 HANDLE *hFileOut, HANDLE *hFileErr,
								 simple_command_t *s, BOOL internal) {
	LPTSTR in_name = NULL, out_name = NULL, err_name = NULL;
	BOOL out_is_err = FALSE;

	in_name = get_word(s->in);
	if (in_name != NULL) {
		*hFileIn = my_open_file(in_name, GENERIC_READ, OPEN_EXISTING);
		DIE(*hFileIn == INVALID_HANDLE_VALUE, "CreateFile");
		if (!internal)
			redirect_handle(psi, *hFileIn, STD_INPUT_HANDLE);
		else
			SetStdHandle(STD_INPUT_HANDLE, *hFileIn);
	}

	out_name = get_word(s->out);
	if (out_name != NULL) {
		if (s->io_flags & IO_OUT_APPEND) {
			DWORD ret;
			*hFileOut = my_open_file(out_name, GENERIC_WRITE, OPEN_ALWAYS);
			ret = SetFilePointer(*hFileOut, 0, NULL, FILE_END);
			DIE(ret == INVALID_SET_FILE_POINTER, "SetFilePointer");
		}
		else
			*hFileOut = my_open_file(out_name, GENERIC_WRITE, CREATE_ALWAYS);
		DIE(*hFileOut == INVALID_HANDLE_VALUE, "CreateFile");
		if (!internal)
			redirect_handle(psi, *hFileOut, STD_OUTPUT_HANDLE);
		else
			SetStdHandle(STD_OUTPUT_HANDLE, *hFileOut);
	}

	err_name = get_word(s->err);
	if (err_name != NULL) {
		if (out_name != NULL && lstrcmp(err_name, out_name) == 0) {
			*hFileErr = *hFileOut;
			out_is_err = TRUE;
		}
		else {
			if (s->io_flags & IO_ERR_APPEND) {
				DWORD ret;
				*hFileErr = my_open_file(err_name, GENERIC_WRITE, OPEN_ALWAYS);
				ret = SetFilePointer(*hFileErr, 0, NULL, FILE_END);
				DIE(ret == INVALID_SET_FILE_POINTER, "SetFilePointer");
			}
			else
				*hFileErr = my_open_file(err_name, GENERIC_WRITE, CREATE_ALWAYS);
		}
		DIE(*hFileErr == INVALID_HANDLE_VALUE, "CreateFile");
		if (!internal)
			redirect_handle(psi, *hFileErr, STD_ERROR_HANDLE);
		else
			SetStdHandle(STD_ERROR_HANDLE, *hFileErr);
	}

	free(in_name);
	free(out_name);
	free(err_name);
	return out_is_err;
}

/*
 * Executes a simple command
 */
static int run_simple_command(LPTSTR command, simple_command_t *s)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dwRes;
	BOOL bRes;
	HANDLE hFileIn = NULL, hFileOut = NULL, hFileErr = NULL;
	BOOL out_is_err = FALSE;
	LPTSTR verb = NULL;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	out_is_err = redirect_std_handles(&si, &hFileIn, &hFileOut, &hFileErr, s, EXTERNAL);
	
	bRes = CreateProcess(
			NULL,
			command,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si,
			&pi);
	/*DIE(!bRes, "Create Process");*/
	if (!bRes) {
		verb = get_word(s->verb);
		fprintf(stderr, "Execution failed for '%s'\n", verb);
		fflush(stderr);
		fflush(stderr);
		free(verb);
		return CREATE_PROCESS_FAILED;
	}

	dwRes = WaitForSingleObject(pi.hProcess, INFINITE);
	DIE(dwRes == WAIT_FAILED, "WaitForSingleObject");

	bRes = GetExitCodeProcess(pi.hProcess, &dwRes);
	DIE(bRes == FALSE, "GetExitCodeProcess");

	close_process(&pi);
	/*
#ifdef DEBUG
	fprintf(stderr, "hFileOut==NULL {%s}; hFileOut==INVALID {%s}", hFileOut == NULL?"true":"false", hFileOut == INVALID_HANDLE_VALUE?"true":"false");
#endif*/
	if (hFileIn != NULL && hFileIn != INVALID_HANDLE_VALUE) {
		DIE(CloseHandle(hFileIn) == FALSE, "CloseHandleIn");
		hFileIn = INVALID_HANDLE_VALUE;
	}
	if (hFileOut != NULL && hFileOut != INVALID_HANDLE_VALUE) {
		DIE(CloseHandle(hFileOut) == FALSE, "CloseHandleOut");
		hFileOut = INVALID_HANDLE_VALUE;
	}
	if (hFileErr != NULL && hFileErr != INVALID_HANDLE_VALUE && out_is_err == FALSE) {
		DIE(CloseHandle(hFileErr) == FALSE, "CloseHandleErr");
		hFileErr = INVALID_HANDLE_VALUE;
	}

	return dwRes;
}

/*
 * Environment variable assignment
 */
static int assign_env_var(const char *var, const char *val) {
	BOOL ret_code;
	/*
#ifdef DEBUG
	fprintf(stderr, "var {%s} = val{%s}\n", var, val);
	fflush(stderr);
#endif*/
	
	ret_code = SetEnvironmentVariable(var, val);
	DIE(ret_code == 0, "SetEnvironmentVariable");

	if (ret_code == 0)
		return VAR_ASSIGN_FAILED;
	else
		return 0;
}

/**
 * Parse and execute a simple command, by either creating a new processing or
 * internally process it.
 */
static int parse_simple(simple_command_t *s, int level, command_t *father, HANDLE *h)
{
	LPTSTR argv = NULL;
	LPTSTR verb = NULL;
	int ret = 0;
	/* TODO sanity checks */
	assert(s != NULL);
	assert(s->up == father);

	verb = get_word(s->verb);

	/* TODO if builtin command, execute the command */
	if (strcmp((char *)verb, "exit") == 0 || strcmp((char *)verb, "quit") == 0) {
		ret = shell_exit();
		goto clear;
	}
	if (strcmp((char *)verb, "cd") == 0) {
		ret = shell_cd(s->params, s);	/* Only the first parameter matters on cd */
		goto clear;
	}

	/* TODO if variable assignment, execute the assignment and return
	 * the exit status */
	if (s->verb->next_part != NULL) {
		CHAR second[MAX_SIZE_ENVIRONMENT_VARIABLE];
		CHAR third[MAX_SIZE_ENVIRONMENT_VARIABLE];
		strcpy(second, s->verb->next_part->string);
		if (second != NULL && (strcmp(second, "=") == 0)) {
			if (s->verb->next_part->next_part != NULL) {
				strcpy(third, s->verb->next_part->next_part->string);
				if (third != NULL) {
					ret = assign_env_var(s->verb->string, third);
				}
				else
					ret = VAR_ASSIGN_FAILED;
			}
			else
				ret = VAR_ASSIGN_FAILED;
			goto clear;
		}
	}

	argv = get_argv(s);
	/* TODO if external command:
	 *  1. set handles
	 *  2. redirect standard input / output / error
		 *  3. run command
	 *  4. get exit code
	 */
	ret = run_simple_command(argv, s);
	free(argv);


clear:
	free(verb);

	return ret; /* TODO replace with actual exit status */
}

/**
 * Process two commands in parallel, by creating two children.
 */
static BOOL do_in_parallel(command_t *cmd1, command_t *cmd2, int level, command_t *father)
{
	/* TODO execute cmd1 and cmd2 simultaneously */

	return TRUE; /* TODO replace with actual exit status */
}

/**
 * Run commands by creating an annonymous pipe (cmd1 | cmd2)
 */
static int do_on_pipe(command_t *cmd1, command_t *cmd2, int level, command_t *father)
{
	/* TODO redirect the output of cmd1 to the input of cmd2 */
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;
	BOOL bRes;
	STARTUPINFO si1, si2;
	PROCESS_INFORMATION pi1, pi2;
	DWORD dwRes;
	BOOL out_is_err1, out_is_err2;

	HANDLE hFileIn1 = NULL, hFileOut1 = NULL, hFileErr1 = NULL;
	HANDLE hFileIn2 = NULL, hFileOut2 = NULL, hFileErr2 = NULL;

	/* Init security attributes structure */
	ZeroMemory(&sa, sizeof(sa));
	sa.bInheritHandle = TRUE;

	/* Init processes */
	ZeroMemory(&pi1, sizeof(pi1));
	ZeroMemory(&pi2, sizeof(pi2));

	/* Create pipe */
	bRes = CreatePipe(&hRead, &hWrite, &sa, 0);
	DIE(bRes == FALSE, "CreatePipe");

	/* Redirect first process output */
	redirect_handle(&si1, hWrite, STD_OUTPUT_HANDLE);

	/* Execute the first process */
	if (cmd1->op == OP_NONE) {
		LPTSTR argv;
		simple_command_t *s1 = cmd1->scmd;

		argv = get_argv(s1);

		/*
		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);*/

		out_is_err1 = redirect_std_handles(&si1, &hFileIn1, &hFileOut1, &hFileErr1, s1, EXTERNAL);

		bRes = CreateProcess(
			NULL,
			argv,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si1,
			&pi1);
		DIE(bRes == FALSE, "Create Process1");

		free(argv);
	}


	DIE(CloseHandle(hWrite) == FALSE, "CloseHandleWrite");

	/* Redirect second process input */
	redirect_handle(&si2, hRead, STD_INPUT_HANDLE);


	DIE(CloseHandle(hRead) == FALSE, "CloseHandleRead");

	/*
		RedirectHandle(&si1, hWrite, STD_OUTPUT_HANDLE);

		bRes = CreateProcess(
			NULL,
			command1,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si1,
			&pi1);
		DIE(bRes == FALSE, "Create Process1");

		CloseHandle(hWrite);


		RedirectHandle(&si2, hRead, STD_INPUT_HANDLE);

		bRes = CreateProcess(
			NULL,
			command2,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si2,
			&pi2);
		DIE(bRes == FALSE, "Create Process2");

		CloseHandle(hRead);

		dwRes = WaitForSingleObject(pi1.hProcess, INFINITE);
		DIE(dwRes == WAIT_FAILED, "WaitForSingleObject");

		CloseProcess(&pi1);

		dwRes = WaitForSingleObject(pi2.hProcess, INFINITE);
		DIE(dwRes == WAIT_FAILED, "WaitForSingleObject");

		CloseProcess(&pi2);
	*/

	return 0; /* TODO replace with actual exit status */
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father, void *h)
{
	int ret;

	/* TODO sanity checks */
	assert(c != NULL);
	assert(c->up == father);

	if (c->op == OP_NONE) {
		/* TODO execute a simple command */
		assert(c->cmd1 == NULL);
		assert(c->cmd2 == NULL);

		return parse_simple(c->scmd, level + 1, c, (HANDLE*)h);
	}

	switch (c->op) {
	case OP_SEQUENTIAL:
		/* TODO execute the commands one after the other */
		ret = parse_command(c->cmd1, level + 1, c, h);
		ret = parse_command(c->cmd2, level + 1, c, h);
		return ret;

	case OP_PARALLEL:
		/* TODO execute the commands simultaneously */
		break;

	case OP_CONDITIONAL_NZERO:
		/* TODO execute the second command only if the first one
		 * returns non zero */
		ret = parse_command(c->cmd1, level + 1, c, h);
		if (ret != 0) {
			ret = parse_command(c->cmd2, level + 1, c, h);
		}
		return ret;

	case OP_CONDITIONAL_ZERO:
		/* TODO execute the second command only if the first one
		 * returns zero */
		ret = parse_command(c->cmd1, level + 1, c, h);
		if (ret == 0) {
			ret = parse_command(c->cmd2, level + 1, c, h);
		}
		return ret;

	case OP_PIPE:
		/* TODO redirect the output of the first command to the
		 * input of the second */
		break;

	default:
		return SHELL_EXIT;
	}

	return 0; /* TODO replace with actual exit code of command */
}

/**
 * Readline from mini-shell.
 */
char *read_line()
{
	char *instr;
	char *chunk;
	char *ret;

	int instr_length;
	int chunk_length;

	int endline = 0;

	chunk = calloc(CHUNK_SIZE, sizeof(char));
	if (chunk == NULL) {
		fprintf(stderr, ERR_ALLOCATION);
		exit(EXIT_FAILURE);
	}

	instr = NULL;
	instr_length = 0;

	while (!endline) {
		ret = fgets(chunk, CHUNK_SIZE, stdin);
		if (ret == NULL) {
			break;
		}

		chunk_length = strlen(chunk);
		if (chunk[chunk_length - 1] == '\n') {
			chunk[chunk_length - 1] = 0;
			endline = 1;
		}

		instr = realloc(instr, instr_length + CHUNK_SIZE);
		if (instr == NULL) {
			free(ret);
			return instr;
		}

		memset(instr + instr_length, 0, CHUNK_SIZE);
		strcat(instr, chunk);
		instr_length += chunk_length;
	}

	free(chunk);

	return instr;
}

