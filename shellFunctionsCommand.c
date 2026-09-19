#include "shellFunctions.h"

char *built_in = {
    "cd",
    "help",
    "exit"
};

int (*built_in_func)(char**) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(built_in) / sizeof(char *);
}

