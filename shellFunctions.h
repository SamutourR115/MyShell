#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define LSH_BUFFER 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
//Declaration of functions

void lsh_loop();

char *lsh_read_line(void);

char **lsh_split_line(char *line);

int lsh_launch(char **argc);

int lsh_cd(char **args);

int lsh_help(char **args);

int lsh_exit(char **args);

int lsh_num_builtins();