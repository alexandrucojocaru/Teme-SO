/**
 * Operating Sytems 2013 - Assignment 1
 *
 */

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include "utils.h"

#define READ		0
#define WRITE		1

/**
 * Redirect a file descriptor to a file
 * @filedes  - file descriptor to be redirected
 * @filename - filename used for redirection
 * @mode	 - mode of redirection (append or normal)
 */
static void do_redirect(int filedes, const char *filename, int mode)
{
	int ret;
	int fd;

	switch(filedes) {
	case STDIN_FILENO:
		fd = open(filename, O_RDONLY);
		break;
	case STDOUT_FILENO:
	case STDERR_FILENO:
		if (mode == APPEND)
			fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		DIE(fd < 0, "open");
		break;
	default:
		break;
	}

	ret = dup2(fd, filedes);
	DIE(ret < 0, "dup2");

	close(fd);
}

static void do_redirect_descriptor(int std_filedes, int new_fd)
{
	int ret;
	ret = dup2(new_fd, std_filedes);
	DIE(ret < 0, "dup2");

	close(new_fd);
}

/**
 * Redirect all std file descriptors
 */
static void redirect_std_files(char *file_in, char *file_out, char *file_err,
		int mode_out, int mode_err)
{
	if (file_in != NULL)
		do_redirect(STDIN_FILENO, file_in, NORMAL);
	if (file_out != NULL)
		do_redirect(STDOUT_FILENO, file_out, mode_out);
	if (file_err != NULL) {
		if (file_out != NULL && strcmp(file_out, file_err) == 0) {
			int ret = dup2(STDOUT_FILENO, STDERR_FILENO);
			DIE(ret < 0, "dup2");
		}
		else
			do_redirect(STDERR_FILENO, file_err, mode_err);
	}
}

/**
 * Internal change-directory command.
 */
static int shell_cd(word_t *dir, simple_command_t *s)
{
	/* TODO execute cd */
	int ret;
	char *directory = NULL;
	char *file_out = NULL, *file_err = NULL;
	int fd;

	file_out = get_word(s->out);
	file_err = get_word(s->err);

	if (file_out) {
		fd = open(file_out, O_WRONLY | O_CREAT, 0644);
		close(fd);
	}
	if (file_err) {
		fd = open(file_err, O_WRONLY | O_CREAT, 0644);
		close(fd);
	}

	free(file_out);
	free(file_err);

	directory = get_word(dir);

	ret = chdir(directory);

	free(directory);

	/*if (hFileIn != NULL && hFileIn != INVALID_HANDLE_VALUE) {
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
	SetStdHandle(STD_ERROR_HANDLE, GetStdHandle(STD_ERROR_HANDLE));*/

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
static char *get_word(word_t *s)
{
	int string_length = 0;
	int substring_length = 0;

	char *string = NULL;
	char *substring = NULL;

	while (s != NULL) {
		substring = strdup(s->string);

		if (substring == NULL) {
			return NULL;
		}

		if (s->expand == true) {
			char *aux = substring;
			substring = getenv(substring);

			/* prevents strlen from failing */
			if (substring == NULL) {
				substring = calloc(1, sizeof(char));
				if (substring == NULL) {
					free(aux);
					return NULL;
				}
			}

			free(aux);
		}

		substring_length = strlen(substring);

		string = realloc(string, string_length + substring_length + 1);
		if (string == NULL) {
			if (substring != NULL)
				free(substring);
			return NULL;
		}

		memset(string + string_length, 0, substring_length + 1);

		strcat(string, substring);
		string_length += substring_length;

		if (s->expand == false) {
			free(substring);
		}

		s = s->next_part;
	}

	return string;
}

/**
 * Concatenate command arguments in a NULL terminated list in order to pass
 * them directly to execv.
 */
static char **get_argv(simple_command_t *command, int *size)
{
	char **argv;
	word_t *param;

	int argc = 0;
	argv = calloc(argc + 1, sizeof(char *));
	assert(argv != NULL);

	argv[argc] = get_word(command->verb);
	assert(argv[argc] != NULL);

	argc++;

	param = command->params;
	while (param != NULL) {
		argv = realloc(argv, (argc + 1) * sizeof(char *));
		assert(argv != NULL);

		argv[argc] = get_word(param);
		assert(argv[argc] != NULL);

		param = param->next_word;
		argc++;
	}

	argv = realloc(argv, (argc + 1) * sizeof(char *));
	assert(argv != NULL);

	argv[argc] = NULL;
	*size = argc;

	return argv;
}

static int run_simple_command(simple_command_t *s, char **argv,
		int level, command_t *father, int pipe_read, int pipe_write, int size)
{
	/* TODO if external command:
		 * 	1. fork new process
		 * 	2c. perform redirections in child
		 *	3c. load executable in child
		 * 	2. wait for child
		 *  3. return exit status
	 */
	pid_t pid, wait_ret;
	int status;

	pid = fork();
	switch (pid) {
	case -1:	/* error */
		DIE(true, "fork");
		break;

	case 0:	{ /* child process */
		char *file_in = NULL, *file_out = NULL, *file_err = NULL;
		int mode_out, mode_err;

		file_in = get_word(s->in);
		file_out = get_word(s->out);
		file_err = get_word(s->err);

		mode_out = (s->io_flags & IO_OUT_APPEND) == IO_OUT_APPEND;
		mode_err = (s->io_flags & IO_ERR_APPEND) == IO_ERR_APPEND;

		if (pipe_read != INVALID_PIPE) {
			do_redirect_descriptor(STDIN_FILENO, pipe_read);
			free(file_in);
			file_in = NULL;
		}
		if (pipe_write != INVALID_PIPE) {
			do_redirect_descriptor(STDOUT_FILENO, pipe_write);
			free(file_out);
			file_out = NULL;
		}

		redirect_std_files(file_in, file_out, file_err, mode_out, mode_err);
		free(file_in);
		free(file_out);
		free(file_err);

		execvp(argv[0], (char *const *) argv);

		fprintf(stderr, "Execution failed for '%s'\n", argv[0]);
		fflush(stdout);
		{
			int i;
			for (i = 0; i < size; ++i) {
				free(argv[i]);
				argv[i] = NULL;
			}
			free(argv);
			argv = NULL;
		}

		exit(EXIT_FAILURE);
	}

	default:	/* parent process */
		wait_ret = waitpid(pid, &status, 0);
		DIE(wait_ret < 0, "waitpid");
		if (WIFEXITED(status))
			/*printf("Child process (pid %d) terminated normally, "
					"with exit code %d\n",
					pid, WEXITSTATUS(status));*/
			return WEXITSTATUS(status);

	}
	return 0;
}

/**
 * Set environment variable
 */
static int set_var(const char *var, const char *value)
{
	return setenv(var, value, 1);
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father,
		int pipe_read, int pipe_write)
{
	char *verb = NULL;
	char **argv = NULL;
	int size;
	int ret = 0;
	/* TODO sanity checks */
	assert(s != NULL);
	assert(s->up == father);

	verb = get_word(s->verb);
	/* TODO if builtin command, execute the command */
	if (strcmp(verb, "exit") == 0 || strcmp(verb, "quit") == 0) {
		ret = shell_exit();
		goto clear;
	}
	if (strcmp(verb, "cd") == 0) {
		ret = shell_cd(s->params, s);	/* Only the first parameter matters on cd */
		goto clear;
	}

	/* TODO if variable assignment, execute the assignment and return
         * the exit status */
	if (s->verb->next_part != NULL) {
		char second[MAX_SIZE_ENVIRONMENT_VARIABLE];
		char third[MAX_SIZE_ENVIRONMENT_VARIABLE];
		strcpy(second, s->verb->next_part->string);
		if (second != NULL && (strcmp(second, "=") == 0)) {
			if (s->verb->next_part->next_part != NULL) {
				strcpy(third, s->verb->next_part->next_part->string);
				if (third != NULL) {
					ret = set_var(s->verb->string, third);
				}
				else
					ret = VAR_ASSIGN_FAILED;
			}
			else
				ret = VAR_ASSIGN_FAILED;
			goto clear;
		}
	}

	/* TODO if external command:
	 	 * 	1. fork new process
         * 	2c. perform redirections in child
         *	3c. load executable in child
         * 	2. wait for child
         *  3. return exit status
	 */
	argv = get_argv(s, &size);
	ret = run_simple_command(s, argv, level, father, pipe_read, pipe_write,
			size);
	{
		int i;
		for (i = 0; i < size; ++i) {
			free(argv[i]);
			argv[i] = NULL;
		}
		free(argv);
		argv = NULL;
	}

clear:
	free(verb);

	return ret; /* TODO replace with actual exit status */
}

/**
 * Process two commands in parallel, by creating two children.
 */
static int do_in_parallel(command_t *cmd1, command_t *cmd2, int level, command_t *father,
		int pipe_read, int pipe_write)
{
	assert(cmd1 != NULL && cmd1->up == father);
	assert(cmd2 != NULL && cmd2->up == father);
	/* TODO execute cmd1 and cmd2 simultaneously */
	pid_t pid, wait_ret;
	int status;
	int ret_val = 0;

	pid = fork();
	switch (pid) {
	case -1:	/* error */
		DIE(true, "fork");
		break;

	case 0:	{ /* child process */
		int ret;
		ret = parse_command(cmd1, level + 1, father, pipe_read, pipe_write);
		exit(ret);
	}

	default:	/* parent process */
		ret_val = parse_command(cmd2, level + 1, father, pipe_read, pipe_write);

		wait_ret = waitpid(pid, &status, 0);
		DIE(wait_ret < 0, "waitpid");
	}

	return ret_val; /* TODO replace with actual exit status */
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2)
 */
static int do_on_pipe(command_t *cmd1, command_t *cmd2, int level, command_t *father,
		int pipe_read, int pipe_write)
{
	/* TODO redirect the output of cmd1 to the input of cmd2 */
	assert(cmd1 != NULL && cmd1->up == father);
	assert(cmd2 != NULL && cmd2->up == father);
	pid_t pid, wait_ret;
	int status;
	int ret_val = 0;
	int pipefd[2];
	int pipe_ret;

	pipe_ret = pipe(pipefd);
	DIE(pipe_ret < 0, "pipe");

	pid = fork();
	switch (pid) {
	case -1:	/* error */
		DIE(true, "fork");
		break;

	case 0:	{ /* child process */
		int ret;
		close(pipefd[0]);
		ret = parse_command(cmd1, level + 1, father, pipe_read, pipefd[1]);
		exit(ret);
	}

	default:	/* parent process */
		close(pipefd[1]);
		ret_val = parse_command(cmd2, level + 1, father, pipefd[0], pipe_write);

		wait_ret = waitpid(pid, &status, 0);
		DIE(wait_ret < 0, "waitpid");
	}

	return ret_val; /* TODO replace with actual exit status */
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father, int pipe_read,
		int pipe_write)
{
	int ret = 0;

	/* TODO sanity checks */
	assert(c != NULL);
	assert(c->up == father);

	if (c->op == OP_NONE) {
		/* TODO execute a simple command */
		assert(c->cmd1 == NULL);
		assert(c->cmd2 == NULL);

		ret = parse_simple(c->scmd, level + 1, c, pipe_read, pipe_write);

		return ret; /* TODO replace with actual exit code of command */
	}

	switch (c->op) {
	case OP_SEQUENTIAL:
		/* TODO execute the commands one after the other */
		ret = parse_command(c->cmd1, level + 1, c, pipe_read, pipe_write);
		ret = parse_command(c->cmd2, level + 1, c, pipe_read, pipe_write);
		return ret;

	case OP_PARALLEL:
		/* TODO execute the commands simultaneously */
		ret = do_in_parallel(c->cmd1, c->cmd2, level + 1, c, pipe_read, pipe_write);
		return ret;

	case OP_CONDITIONAL_NZERO:
		/* TODO execute the second command only if the first one
                 * returns non zero */
		ret = parse_command(c->cmd1, level + 1, c, pipe_read, pipe_write);
		if (ret != 0) {
			ret = parse_command(c->cmd2, level + 1, c, pipe_read, pipe_write);
		}
		return ret;

	case OP_CONDITIONAL_ZERO:
		/* TODO execute the second command only if the first one
                 * returns zero */
		ret = parse_command(c->cmd1, level + 1, c, pipe_read, pipe_write);
		if (ret == 0) {
			ret = parse_command(c->cmd2, level + 1, c, pipe_read, pipe_write);
		}
		return ret;

	case OP_PIPE:
		/* TODO redirect the output of the first command to the
		 * input of the second */
		ret = do_on_pipe(c->cmd1, c->cmd2, level + 1, c, pipe_read, pipe_write);
		break;

	default:
		assert(false);
	}

	return ret; /* TODO replace with actual exit code of command */
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

	instr = NULL;
	instr_length = 0;

	chunk = calloc(CHUNK_SIZE, sizeof(char));
	if (chunk == NULL) {
		fprintf(stderr, ERR_ALLOCATION);
		return instr;
	}

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

		ret = instr;
		instr = realloc(instr, instr_length + CHUNK_SIZE + 1);
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

