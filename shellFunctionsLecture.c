#include "shellFunctions.h"

//Definition of functions
void lsh_loop(){
    char *lines;
    char **args;
    int status;

    do{
        printf(">");
        lines = lsh_read_line();
        args = lsh_split_line(lines);
        status = lsh_execute(args);

        free(lines);
        free(args);
    }while(status);

}

char *lsh_read_line(void){
  int buffSize = LSH_BUFFER;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffSize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= buffSize) {
      buffSize += LSH_BUFFER;
      buffer = realloc(buffer, buffSize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **lsh_split_line(char *line){
    int buffSize = LSH_TOK_BUFSIZE, positions = 0;
    char **tokens = malloc(buffSize * sizeof(char*));
    char *token; 

    if(!tokens){
        fprintf(stderr,"lsh: Allocation error \n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line,LSH_TOK_DELIM);

    while(token != NULL){
        token[positions] = token;
        positions ++;

        if(positions >= token){
            buffSize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens,buffSize * sizeof(char*));

            if(!tokens){
                fprintf(stderr,"lsh: Allocation error \n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL,LSH_TOK_DELIM);
    }
    tokens[positions] = NULL;
    return tokens;
}

int lsh_launch(char **args){
  pid_t pid,wpid;
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
      wpid = waitpid(pid, &status, WUNTRACED);
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}