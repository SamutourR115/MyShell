#ifndef __SHELLFUNCTIONS__
#define __SHELLFUNCTIONS__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define LSH_BUFFER 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
//Declaration of functions

void lsh_loop(void);

char *lsh_read_line(void);

char **lsh_split_line(char *line);

int lsh_launch(char **args);

int lsh_cd(char **args);

int lsh_help(char **args);

int lsh_execute(char **args);

int lsh_exit(char **args);

int lsh_num_builtins();

/*
 * TODO list (in order — each step builds on the previous one)
 *
 * TODO: Rewrite the tokenizer
 *       - recognize operators (|, <, >, >>, &) even without spaces
 *       - handle quotes ("hello world" = one argument)
 *
 * TODO: Redirections (<, >, >>)
 *       - open() the file, then dup2() in the child before execvp()
 *
 * TODO: Pipes (|)
 *       - pipe() + one fork() per command + dup2()
 *       - support chains: cmd1 | cmd2 | cmd3
 *
 * TODO: Signal handling
 *       - Ctrl+C (SIGINT) kills the running command, not the shell
 *
 * TODO: Background execution (&)
 *       - don't wait for the child
 *       - handle SIGCHLD to reap zombie processes
 *
 * TODO: Wildcards (*, ?)
 *       - expand with glob()
 *
 * Small improvements (anytime):
 * TODO: cd with no argument goes to $HOME
 * TODO: Exit status of the last command ($?)
 * TODO: Environment variables ($HOME, export)
 * TODO: Command history (readline)
 */

#endif
