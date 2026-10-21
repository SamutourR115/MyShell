#ifndef SHELLFUNCTIONS_H
#define SHELLFUNCTIONS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>
#include <ctype.h>

#define LSH_BUFFER 1024          /* Initial (and growth) size of the input buffer */
#define LSH_TOK_BUFSIZE 64       /* Initial (and growth) size of the token array */
#define LSH_TOK_DELIM " \t\r\n\a" /* Characters that separate tokens */

/* ------------------------------------------------------------------------- */
/*  Main loop and input parsing                                              */
/* ------------------------------------------------------------------------- */

/**
 * @brief Main shell loop: prompt, read, parse and execute commands
 *        until a command asks the shell to stop.
 */
void lsh_loop(void);

/**
 * @brief Read one line from standard input.
 *
 * The buffer grows dynamically as needed. Exits the shell on EOF (Ctrl+D).
 *
 * @return A heap-allocated, null-terminated string (without the trailing
 *         newline). The caller must free() it.
 */
char *lsh_read_line(void);

/**
 * @brief Split a line into tokens separated by LSH_TOK_DELIM.
 *
 * Operators (|, <, >, >>, &) are separate tokens even without spaces.
 * Text between double quotes is kept as one token (quotes removed).
 * On an unclosed quote, an error is printed and no token is returned.
 * The line is not modified: tokens are copied into one buffer.
 *
 * @param line The line read from the user.
 * @return A heap-allocated, NULL-terminated array of tokens.
 *         The caller must free(tokens[0]) (the shared buffer), then free(tokens).
 */
char **lsh_split_line(char *line);

/**
 * @brief Tell whether a character is a shell operator (|, <, >, &).
 */
int is_operator(char c);

/* ------------------------------------------------------------------------- */
/*  Command execution                                                        */
/* ------------------------------------------------------------------------- */

/**
 * @brief Run an external program in a child process and wait for it.
 *
 * @param args NULL-terminated argument list (args[0] is the program name).
 * @return 1 so the shell keeps running.
 */
int lsh_launch(char **args);

/**
 * @brief Dispatch a command: run it as a builtin if it is one,
 *        otherwise launch it as an external program.
 *
 * @param args NULL-terminated argument list.
 * @return 1 to keep the shell running, 0 to exit.
 */
int lsh_execute(char **args);

/* ------------------------------------------------------------------------- */
/*  Builtin commands                                                         */
/* ------------------------------------------------------------------------- */

/**
 * @brief Builtin: change the current directory to args[1].
 * @return 1 so the shell keeps running.
 */
int lsh_cd(char **args);

/**
 * @brief Builtin: print the help message and the list of builtins.
 * @return 1 so the shell keeps running.
 */
int lsh_help(char **args);

/**
 * @brief Builtin: exit the shell.
 * @return 0 to stop the main loop.
 */
int lsh_exit(char **args);

/**
 * @brief Number of builtin commands.
 */
int lsh_num_builtins(void);

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
