#include "shellFunctions.h"

char *built_in = {
    "cd",
    "help",
    "exit"
};

int (*built_in_func[])(char**) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], built_in[i]) == 0) {
      return (*built_in_func[i])(args);
    }
  }

  return lsh_launch(args);
}

int lsh_num_builtins() {
  return sizeof(built_in) / sizeof(char *);
}

int lsh_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }
    else{
        if(chdir(args[1]) != 0){
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args){

    int i;

    printf("Sam's shell\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", built_in[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args)
{
  return 0;
}