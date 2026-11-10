#include "shellFunctions.h"

int lsh_last_status = 0;

struct builtin {
  const char *name;
  int (*func)(char **);
};
 
static const struct builtin builtins[] = {
  { "cd",     &lsh_cd     },
  { "help",   &lsh_help   },
  { "exit",   &lsh_exit   },
  { "export", &lsh_export }
};

static int check_syntax(char **args){
  int i, simple = 1, empty = 1;

  for (i = 0; ; i++){
    if (args[i] == NULL || strcmp(args[i], "|") == 0){
      if (empty) return -1;
      if (args[i] == NULL) return simple;
      simple = 0;
      empty = 1;
    }
    else if (strcmp(args[i], "&") == 0){
      return -1;
    }
    else if (is_redirect(args[i])){
      if (args[i + 1] == NULL || is_operator(args[i + 1][0])) return -1;
      simple = 0;
      i++;
    }
    else{
      empty = 0;
    }
  }
}
 
int lsh_execute(char **args){
  assert(args != NULL);

  glob_t g;
  char status_buf[12];
  const char *word;
  char **argv;
  int i, n, simple, ret = -1, background = 0, flags = GLOB_NOCHECK;
 
  if (args[0] == NULL){
    return 1;
  }

  snprintf(status_buf, sizeof(status_buf), "%d", lsh_last_status);
  for (i = 0; args[i] != NULL; i++){
    word = args[i];
    if (word[0] == '$' && word[1] != '\0'){
      word = strcmp(word, "$?") == 0 ? status_buf : getenv(word + 1);
      if (word == NULL) word = "";
    }
    if (glob(word, flags, NULL, &g) != 0){
      fprintf(stderr, "lsh: allocation error\n");
      globfree(&g);
      return 1;
    }
    flags |= GLOB_APPEND;
  }

  n = g.gl_pathc;
  argv = malloc((n + 1) * sizeof(char *));
  if (!argv){
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
  memcpy(argv, g.gl_pathv, (n + 1) * sizeof(char *));

  if (strcmp(argv[n - 1], "&") == 0){
    background = 1;
    argv[--n] = NULL;
  }

  simple = n > 0 ? check_syntax(argv) : -1;
  if (simple == -1){
    fprintf(stderr, "lsh: syntax error\n");
    lsh_last_status = 2;
    ret = 1;
  }
  else{
    if (simple && !background) ret = lsh_run_builtin(argv);
    if (ret == -1) ret = lsh_launch(argv, background);
    else lsh_last_status = 0;
  }

  free(argv);
  globfree(&g);
  return ret;
}

int lsh_run_builtin(char **args){
  int i;

  for (i = 0; i < lsh_num_builtins(); i++){
    if (strcmp(args[0], builtins[i].name) == 0){
      return builtins[i].func(args);
    }
  }
  return -1;
}
 
int lsh_num_builtins(void){
  return sizeof(builtins) / sizeof(builtins[0]);
}
 
int lsh_cd(char **args){
  assert(args != NULL);

  const char *dir = args[1] != NULL ? args[1] : getenv("HOME");
 
  if(dir == NULL){
    fprintf(stderr, "lsh: cd: HOME not set\n");
  }
  else if(chdir(dir) != 0){
    perror("lsh");
  }
  return 1;
}

int lsh_export(char **args){
  assert(args != NULL);

  int i;
  char *eq;

  for (i = 1; args[i] != NULL; i++){
    eq = strchr(args[i], '=');
    if (eq == NULL || eq == args[i]){
      fprintf(stderr, "lsh: export: usage: export NAME=value\n");
      continue;
    }
    *eq = '\0';
    if (setenv(args[i], eq + 1, 1) != 0) perror("lsh");
    *eq = '=';
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
