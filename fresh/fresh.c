#include <unistd.h> //chdir(), execve(), getenv(), access()
#include <stdlib.h> //free(),
#include <sys/wait.h> //wait()
#include <stdio.h>  //printf(), sprintf(), perror()
#include <string.h> //strtok(), strlen(), strchr(), strsep()
#include <editline/readline.h> //readline()
#include <editline/history.h> //add_history()
#include <errno.h> //errno

#define MAX_PATH 16
#define RD_FD 0
#define WR_FD 1

char * pathv[MAX_PATH] = {
  "/bin/",
  "/usr/local/bin",
  "/usr/bin",
  NULL
};

char * * get_argv(char * input);

int exec_command(char * argv[]){

  /*tries to execute command (argv[0]) in each path
  of the path array*/
  for (int i = 0; pathv[i]; i++){
    int p_sz = strlen(pathv[i]) + strlen(argv[0]) + 1;
    char path[p_sz];
    sprintf(path, "%s/%s", pathv[i], argv[0]);
    execve(path, argv, NULL);
  //  printf("Not found on %s\n", pathv[i]);
  }
  /*If procedure reaches here, it did not execute the command.*/
  printf("Command not found.\n");

}

int run_command(char * argv[]){
/*Executes commands 'cd' and 'exit' directly. For any other command,
  process is forked and child executes the command. */

  /*cd change directory with chdir directly*/
  printf("run_command: running %s\n", argv[0]);
  if (strcmp(argv[0], "cd") == 0){
        if (!argv[1]) chdir(getenv("HOME")); //cd with no args change to home
        else if (chdir(argv[1])) perror("Error");
        return 0;
  }

  /*exit = quit the program */
  if (strcmp(argv[0], "exit") == 0){
    exit(0);
  }

  /*any other case: fork and exec*/
  /*parent process*/
  int pid = fork();
  if (pid){
    int stat;
    wait(&stat);
  }
  /*child process*/
  else{
    exec_command(argv);
  }
}

int do_pipe(char * head, char * tail){
/*TO DO*/
}

int parse_input(char * ipt){
  if (strchr(ipt, '|')){
    puts("A pipe detected!");
    char * head_end = strchr(ipt, '|');
    * head_end = '\0';
    puts(ipt);
    do_pipe(ipt, head_end+1);
  }
  else{
    char * * argv = get_argv(ipt);
    for (int i = 0; argv[i]; i++) puts(argv[i]);
    run_command(argv);
  }
}

char * * get_argv(char * input){

  int max_args = 8;
  char * * argv = malloc(sizeof(char *) * max_args);
  char * tok = strtok(input, " ");
  int i = 0;

  while(tok){
    argv[i] = tok;
    i++;
    if (i == max_args){
      max_args += max_args;
      argv = realloc(argv, sizeof(char *) * max_args);
    }
    tok = strtok(NULL, " ");
  }
  argv[i] = NULL;
  return argv;
}

int cd(char * argv[]){
  char * home = getenv("HOME");
  if (argv[1]){
    chdir(argv[1]);
  }
  else{
    chdir(home);
  }

}
int main(void){
  char ** argv;

  while(1){
    char * ipt = readline(">");
    if(strlen(ipt)) add_history(ipt);
    parse_input(ipt);
    free(ipt);

  }
}
