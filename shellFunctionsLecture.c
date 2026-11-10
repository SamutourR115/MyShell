#include "shellFunctions.h"

static void reap_children(int sig){
  int saved_errno = errno;
  (void)sig;

  while (waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}

void lsh_loop(void){
  char *lines;
  char **args;
  int status;
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = reap_children;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, NULL);
  signal(SIGINT, SIG_IGN);

  do{
    lines = lsh_read_line();
    args = lsh_split_line(lines);
    status = lsh_execute(args);

    free(lines);
    free(args[0]);
    free(args);
  }while(status);

}

char *lsh_read_line(void){
  char *line = readline(">");

  if (line == NULL){
    printf("\n");
    exit(EXIT_SUCCESS);
  }
  if (*line != '\0') add_history(line);
  return line;
}

char **lsh_split_line(char *line){
  assert(line != NULL);
 
  int buffSize = LSH_TOK_BUFSIZE, positions = 0;
  char **tokens = malloc(buffSize * sizeof(char*));
  char **tmp;
  
  char *buf = malloc(strlen(line) * 2 + 1);
  char *p = line, *out = buf;
  int inQuote;

  if(!tokens || !buf){
    fprintf(stderr,"lsh: Allocation error \n");
    exit(EXIT_FAILURE);
  }

  while(1){
    while(*p != '\0' && strchr(LSH_TOK_DELIM, *p)) p++;
    if(*p == '\0') break;

    tokens[positions] = out;
    positions ++;

    if(is_operator(*p)){
      if(p[0] == '>' && p[1] == '>') *out++ = *p++;
      *out++ = *p++;
    }
    else{
      inQuote = 0;
      while(*p != '\0'){
        if(*p == '"'){
          inQuote = !inQuote;
          p++;
        }
        else if(!inQuote && (strchr(LSH_TOK_DELIM, *p) || is_operator(*p))) break;
        else *out++ = *p++;
      }
      if(inQuote){
        fprintf(stderr,"lsh: unclosed quote\n");
        positions = 0;
        break;
      }
    }
    *out++ = '\0';

    if(positions >= buffSize){
      buffSize += LSH_TOK_BUFSIZE;
      tmp = realloc(tokens,buffSize * sizeof(char*));
 
      if(!tmp){
        fprintf(stderr,"lsh: Allocation error \n");
        free(tokens);                  
        exit(EXIT_FAILURE);
      }
      tokens = tmp;
    }
  }
  tokens[positions] = NULL;
  if(positions == 0) free(buf);
  return tokens;
}

int is_operator(char c){
  return c == '|' || c == '<' || c == '>' || c == '&';
}

int is_redirect(const char *tok){
  return strcmp(tok, "<") == 0 || strcmp(tok, ">") == 0 || strcmp(tok, ">>") == 0;
}

static void lsh_run_child(char **cmd, int in, int out){
  int i, n = 0, fd, flags, target;

  if (in != STDIN_FILENO){ dup2(in, STDIN_FILENO); close(in); }
  if (out != STDOUT_FILENO){ dup2(out, STDOUT_FILENO); close(out); }

  for (i = 0; cmd[i] != NULL; i++){
    if (!is_redirect(cmd[i])){
      cmd[n++] = cmd[i];
      continue;
    }
    if (cmd[i][0] == '<'){
      flags = O_RDONLY;
      target = STDIN_FILENO;
    }
    else{
      flags = O_WRONLY | O_CREAT | (cmd[i][1] == '>' ? O_APPEND : O_TRUNC);
      target = STDOUT_FILENO;
    }
    i++;
    fd = open(cmd[i], flags, 0644);
    if (fd == -1){
      perror(cmd[i]);
      exit(EXIT_FAILURE);
    }
    dup2(fd, target);
    close(fd);
  }
  cmd[n] = NULL;

  if (lsh_run_builtin(cmd) != -1) exit(EXIT_SUCCESS);
  execvp(cmd[0], cmd);
  perror("lsh");
  exit(127);
}

int lsh_launch(char **args, int background){
  assert(args != NULL);
 
  int i, k = 0, ncmds = 1, last, status, in = STDIN_FILENO, fd[2];
  pid_t pid, *pids;
  char **cmd = args;
  sigset_t chld, old;

  for (i = 0; args[i] != NULL; i++){
    if (strcmp(args[i], "|") == 0) ncmds++;
  }
  pids = malloc(ncmds * sizeof(pid_t));
  if (!pids){
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  sigemptyset(&chld);
  sigaddset(&chld, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chld, &old);
  fflush(stdout);

  for (i = 0; ; i++){
    if (args[i] != NULL && strcmp(args[i], "|") != 0) continue;

    last = args[i] == NULL;
    fd[0] = -1;
    fd[1] = STDOUT_FILENO;
    if (!last){
      args[i] = NULL;
      if (pipe(fd) == -1){
        perror("lsh");
        break;
      }
    }

    pid = fork();
    if (pid == 0){
      sigprocmask(SIG_SETMASK, &old, NULL);
      if (!background) signal(SIGINT, SIG_DFL);
      if (fd[0] != -1) close(fd[0]);
      lsh_run_child(cmd, in, fd[1]);
    }
    if (pid < 0) perror("lsh");
    else pids[k++] = pid;

    if (in != STDIN_FILENO) close(in);
    in = STDIN_FILENO;
    if (last) break;
    close(fd[1]);
    if (pid < 0){
      close(fd[0]);
      break;
    }
    in = fd[0];
    cmd = &args[i + 1];
  }
  if (in != STDIN_FILENO) close(in);

  if (background){
    if (k > 0) printf("[%d]\n", (int)pids[k - 1]);
    lsh_last_status = 0;
  }
  else{
    for (i = 0; i < k; i++){
      if (waitpid(pids[i], &status, 0) == -1){
        perror("lsh");
        continue;
      }
      if (WIFEXITED(status)) lsh_last_status = WEXITSTATUS(status);
      else if (WIFSIGNALED(status)){
        lsh_last_status = 128 + WTERMSIG(status);
        if (WTERMSIG(status) == SIGINT && i == k - 1) printf("\n");
      }
    }
  }

  sigprocmask(SIG_SETMASK, &old, NULL);
  free(pids);
  return 1;
}
