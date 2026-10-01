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

int lsh_launch(char **args);

int lsh_cd(char **args);

int lsh_help(char **args);

int lsh_execute(char **args);

int lsh_exit(char **args);

int lsh_num_builtins();

/*
 * Commandes intégrées actuellement disponibles :
 *
 *   - cd     : changer de répertoire avec chdir().
 *   - help   : afficher l'aide de la mini-shell.
 *   - exit   : quitter la mini-shell.
 *
 * Les autres commandes, comme ls, pwd ou echo, sont des programmes
 * externes recherchés grâce à la variable d'environnement PATH.
 *
 * Ce projet permet d'étudier plusieurs concepts de programmation bas niveau :
 *
 *   - processus et appels système ;
 *   - fork(), execvp() et waitpid() ;
 *   - pointeurs et tableaux de chaînes de caractères ;
 *   - gestion de la mémoire dynamique ;
 *   - entrées et sorties standard ;
 *   - gestion des erreurs en C.
 *
 * Fonctionnalités qui pourront être ajoutées ultérieurement :
 *
 *   - redirections avec >, < et >> ;
 *   - pipes avec | ;
 *   - exécution en arrière-plan avec & ;
 *   - gestion des signaux comme SIGINT et SIGCHLD ;
 *   - prise en charge des guillemets et des wildcards.
 */