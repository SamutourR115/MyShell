#include "shellFunctions.h"

//Definition of functions
void lsh_loop(void){
  char *lines;
  char **args;
  int status;

  do{
    printf(">");
    fflush(stdout);
    lines = lsh_read_line();
    args = lsh_split_line(lines);
    status = lsh_execute(args);

    free(lines);
    free(args[0]);
    free(args);
  }while(status);

}

char *lsh_read_line(void){
  int buffSize = LSH_BUFFER;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffSize);
  char *tmp;
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();

   if (position >= buffSize) {
      buffSize += LSH_BUFFER;
      tmp = realloc(buffer, buffSize);   
      if (!tmp) {
        fprintf(stderr, "lsh: allocation error\n");
        free(buffer);                    
        exit(EXIT_FAILURE);
      }
      buffer = tmp;
    }

    if(c == '\n'){
      buffer[position] = '\0';
      return buffer;
    }

    buffer[position] = c;
    position++;
  }
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

int lsh_launch(char **args){
  assert(args != NULL);
 
  pid_t pid;           
  int status;
 
  pid = fork();
  if(pid == 0){
    if(execvp(args[0],args) == -1){
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  }
  else if(pid < 0){
    perror("lsh");
  }
  else{
    do{

      if(waitpid(pid, &status, WUNTRACED) == -1){
        perror("lsh");
        break;
      }
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
 
  return 1;
}