#ifndef SHELLFUNCTIONS_H
#define SHELLFUNCTIONS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#define LSH_TOK_BUFSIZE 64       /* Initial (and growth) size of the token array */
#define LSH_TOK_DELIM " \t\r\n\a" /* Characters that separate tokens */

/** Exit status of the last command, used for $? */
extern int lsh_last_status;

/* ------------------------------------------------------------------------- */
/*  Main loop and input parsing                                              */
/* ------------------------------------------------------------------------- */

/**
 * @brief Main shell loop: prompt, read, parse and execute commands
 *        until a command asks the shell to stop.
 *
 * The shell ignores SIGINT (Ctrl+C only kills the running command)
 * and reaps finished background children on SIGCHLD.
 */
void lsh_loop(void);

/**
 * @brief Read one line with readline (line editing + history).
 *
 * Non-empty lines are added to the history. Exits the shell on EOF (Ctrl+D).
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

/**
 * @brief Tell whether a token is a redirection (<, >, >>).
 */
int is_redirect(const char *tok);

/* ------------------------------------------------------------------------- */
/*  Command execution                                                        */
/* ------------------------------------------------------------------------- */

/**
 * @brief Run a pipeline (cmd1 | cmd2 | ...) with one child per command.
 *
 * Each child applies its own redirections (<, >, >>) before execvp().
 * Updates lsh_last_status with the status of the last command.
 *
 * @param args NULL-terminated, already expanded tokens, "|" between commands.
 *             Modified: each "|" is replaced by NULL.
 * @param background If non-zero, don't wait for the children (cmd &).
 * @return 1 so the shell keeps running.
 */
int lsh_launch(char **args, int background);

/**
 * @brief Expand $VAR, $? and wildcards (*, ?), check the syntax, then run
 *        the command: a lone builtin runs in the shell itself, anything else
 *        (pipes, redirections, &) goes through lsh_launch().
 *
 * @param args NULL-terminated token list from lsh_split_line().
 * @return 1 to keep the shell running, 0 to exit.
 */
int lsh_execute(char **args);

/**
 * @brief Run args as a builtin if args[0] names one.
 * @return The builtin's result, or -1 if args[0] is not a builtin.
 */
int lsh_run_builtin(char **args);

/* ------------------------------------------------------------------------- */
/*  Builtin commands                                                         */
/* ------------------------------------------------------------------------- */

/**
 * @brief Builtin: change the current directory to args[1] ($HOME if absent).
 * @return 1 so the shell keeps running.
 */
int lsh_cd(char **args);

/**
 * @brief Builtin: set environment variables (export NAME=value ...).
 * @return 1 so the shell keeps running.
 */
int lsh_export(char **args);

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


#endif
