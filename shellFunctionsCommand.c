#include "shellFunctions.h"

struct builtin {
  const char *name;
  int (*func)(char **);
};
 
static const struct builtin builtins[] = {
  { "cd",   &lsh_cd   },
  { "help", &lsh_help },
  { "exit", &lsh_exit }
};
 
int lsh_execute(char **args){
  assert(args != NULL);
 
  int i;
 
  if (args[0] == NULL){
    return 1;
  }
 
  for (i = 0; i < lsh_num_builtins(); i++){
    if (strcmp(args[0], builtins[i].name) == 0){
      return builtins[i].func(args);
    }
  }
 
  return lsh_launch(args);
}
 
int lsh_num_builtins(void){
  return sizeof(builtins) / sizeof(builtins[0]);
}
 
int lsh_cd(char **args){
  assert(args != NULL);
 
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
  assert(args != NULL);
  (void)args;  
 
  int i;
 
  printf("Sam's shell\n");
  printf("The following are built in:\n");
 
  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtins[i].name);
  }
 
  printf("Use the man command for information on other programs.\n");
  return 1;
}
 
int lsh_exit(char **args){
  assert(args != NULL);
  (void)args;
 
  return 0;
}