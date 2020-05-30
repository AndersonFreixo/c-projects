#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <editline/readline.h>
#include <editline/history.h>
#include "dir.h"

int main(void){
  char comm_str[COMM_MAX_LEN];

  root_ptr = malloc(sizeof(tree_node));
  root_ptr -> parent_ptr = root_ptr;
  root_ptr -> sibling_ptr = root_ptr;
  root_ptr -> child_ptr = NULL;
  root_ptr -> type = DIR_FILE;
  strcpy(root_ptr -> name, "/");

  cwd_ptr = root_ptr;

  puts(":::Directory tree simulator:::");
  puts("Press Ctrl-C or type <quit> to quit the program.");
  puts("Type <menu> to check available commands.");
  while(1){
    char * input = readline(">");
    if (strlen(input)){
      add_history(input);
      strncpy(comm_str, strtok(input, " "), COMM_MAX_LEN);
      char * tmp = strtok(NULL, " ");
      if (tmp){
        strncpy(path, tmp, PATH_MAX_LEN);
      }
      else{
        path[0] = '\0';
      }
      free(input);

      int comm_id = get_command_id(comm_str);
      if (comm_id != NULL_COMM) comm_func[comm_id]();
      else puts("Invalid command.");

    }

  }
}


int get_command_id(char *input){

  for (int i = 0; input[i]; i++){
    input[i] = tolower(input[i]);
  }
  for (int i = 0; i < NULL_COMM; i++){
    if(!strcmp(input, comm_str[i])) return i;
  }
  return NULL_COMM;
}

void decompose_path(char * path){
  if (strlen(path) == 0){
    strncpy(dir_name, "",1);
  }
  char tmp[PATH_MAX_LEN];
  strncpy(tmp, path, PATH_MAX_LEN); //Escrever versao segura!
  strncpy(dir_name, dirname(tmp), DIR_MAX_LEN);
  strncpy(tmp, path, PATH_MAX_LEN);
  strncpy(base_name, basename(tmp), BASE_MAX_LEN);


  if (strlen(basename(path)) == 0){
    base_name[0] = '\0';
  }
}

int add_child(char * path, int ftype){
  decompose_path(path);
  tree_node * parent_ptr = get_target_dir();
  if (!parent_ptr) return -1;

  /* create new node */
  tree_node * new_ptr = malloc(sizeof(tree_node));
  strncpy(new_ptr -> name, base_name, BASE_MAX_LEN);
  new_ptr -> sibling_ptr = NULL;
  new_ptr -> child_ptr = NULL;
  new_ptr -> parent_ptr = parent_ptr;
  new_ptr -> type = ftype;

  tree_node * aux_ptr = parent_ptr -> child_ptr;
  tree_node * prev_ptr = NULL;

  if (!aux_ptr){
    printf("Creating first children of %s directory..."
          "Name: %s\n", parent_ptr -> name, new_ptr -> name);
    parent_ptr -> child_ptr = new_ptr;
    return 0;
  }

  /*If current dir has children
  check if there's already a file with the given name*/
  do{
    if (strcmp(aux_ptr -> name, new_ptr ->name) == 0){
      printf("Error: There's already a file with the name '%s'!", new_ptr -> name);
      free(new_ptr);
      return -1;
    }
    prev_ptr = aux_ptr;
  }while(aux_ptr = aux_ptr -> sibling_ptr);

  printf("New file/directory %s added to directory %s.\n", new_ptr -> name, parent_ptr);
  prev_ptr -> sibling_ptr = new_ptr;
  return 0;
}

tree_node * get_target_dir(void){
  tree_node * aux_ptr;
  tree_node * curr_ptr;
  curr_ptr = dir_name[0] == '/'? root_ptr : cwd_ptr;
  if (!strcmp(dir_name, ".") && !strcmp(base_name, ".")){
    return cwd_ptr;
  }
  char tmp[DIR_MAX_LEN];
  strncpy(tmp, dir_name, DIR_MAX_LEN);
  char * s = strtok(tmp, "/");

 //Directory is current directory, skip while loop.
  if (dir_name[0] == '.' && strlen(dir_name) == 1) s = NULL;


  while (s){
    aux_ptr = curr_ptr -> child_ptr;

  /*If current dir has no children, there's no sense in searching*/
    if (!aux_ptr){
      printf("Error: directory %s has no subdirectories!\n",
        curr_ptr -> name);
      return NULL;
    }

    while (aux_ptr){
      if (strcmp(aux_ptr -> name, s) == 0){
        if (aux_ptr -> type == DIR_FILE){
          curr_ptr = aux_ptr;
          break;
        }
      }
      aux_ptr = aux_ptr -> sibling_ptr;
    }
    /*None of the siblings matches the searched directory name*/

    if (!aux_ptr){
      printf("Error: directory not found among subdirectories of %s!\n",
        curr_ptr -> name);
      return NULL;
    }
    s = strtok(NULL, "/");
  }
  return curr_ptr;
}

tree_node * get_target_wbase(void){
    if (!strcmp(dir_name, ".") && !strcmp(base_name, ".")){
      return cwd_ptr;
    }
    tree_node * dir_ptr = get_target_dir();

    if (dir_ptr){
      tree_node * aux_ptr = dir_ptr -> child_ptr;
      while (aux_ptr){
        if (!strcmp(aux_ptr -> name, base_name)){
          return aux_ptr;
        }
        aux_ptr = aux_ptr -> sibling_ptr;
      }

    }
    puts("The directory doesn't exist.");
    return NULL;
}

int remove_node(char * path, int ftype){
  decompose_path(path);
  tree_node * target_ptr = get_target_dir();
  char * tstr = ftype == DIR_FILE? "directory" : "regular";
  if (!target_ptr) return -1;

  if (!(target_ptr -> child_ptr)){
    printf("Error: This directory doesn't contain any files or subdirectories.\n");
    return -1;
  }

  tree_node * aux_ptr, * prev_ptr = NULL;
  aux_ptr = target_ptr -> child_ptr;
  while (aux_ptr){
    if (strcmp(aux_ptr -> name, base_name) == 0){
      if (aux_ptr -> type == ftype){
        if (prev_ptr){
          prev_ptr -> sibling_ptr = aux_ptr -> sibling_ptr;

        }
        else{
          target_ptr -> child_ptr =
            aux_ptr -> sibling_ptr?
              aux_ptr -> sibling_ptr
              : NULL;
        }
        free(aux_ptr);
        printf("%s sucessfully removed!\n", base_name);
        return 0;
      }
      else{
        printf("%s is not a %s file!\n", base_name, tstr);
        return -1;
      }
    }
    prev_ptr = aux_ptr;
    aux_ptr = aux_ptr -> sibling_ptr;
  }
  printf("Error: directory doesn't exist!\n");
  return -1;

}
void rec_save_dir(FILE * fp, char * p, tree_node * n_ptr){
  char cp[PATH_MAX_LEN]; //complete path
  strncpy(cp, p, DIR_MAX_LEN);
  strncat(cp, "/", 2);
  strncat(cp, n_ptr -> name, BASE_MAX_LEN);
  fprintf(fp, "%d %s\n", n_ptr -> type, cp);

  if (n_ptr -> child_ptr){
    rec_save_dir(fp, cp, n_ptr -> child_ptr);
  }

  if (n_ptr -> sibling_ptr){
    rec_save_dir(fp, p, n_ptr -> sibling_ptr);
  }
}

void rec_print_dir(int depth, tree_node * n_ptr){
    for (int i = 0; i < depth; i++) putchar('\t');
    printf("%s\n", n_ptr -> name);

    if (n_ptr -> child_ptr){
        rec_print_dir(depth+1, n_ptr -> child_ptr);
    }

    if (n_ptr -> sibling_ptr && n_ptr -> sibling_ptr != root_ptr){
      rec_print_dir(depth, n_ptr -> sibling_ptr);
    }
}

int mkdir(void){
  return add_child(path, DIR_FILE);
}

int rmdir(void){
  return remove_node(path, DIR_FILE);
}

int cd(void){
  if (!strcmp(path, "..")){
    cwd_ptr = cwd_ptr -> parent_ptr;
    return 0;
  }

  decompose_path(path);
  tree_node * target_ptr = get_target_wbase();
  if (target_ptr){
    if (target_ptr -> type == DIR_FILE){
        cwd_ptr = target_ptr;
    }
    else printf("Target is not a directory.\n");
  }
  else printf("Invalid path.");
}

int ls(void){
  decompose_path(path);
  tree_node * target_ptr = get_target_wbase();

  if (!target_ptr) return -1;

  tree_node * aux_ptr = target_ptr -> child_ptr;
  while (aux_ptr){
    if (aux_ptr -> type == REG_FILE) printf("*");
    printf("%s ", aux_ptr -> name);
    aux_ptr = aux_ptr -> sibling_ptr;
  }
  puts("");

}

void print_dir(tree_node * node){
  if (node){
    if (node != root_ptr){
      print_dir(node -> parent_ptr);
      printf("%s/", node -> name);
    }
    else printf("/");
  }
}

int pwd(void){
  print_dir(cwd_ptr);
  printf("\n");
}

int creat(void){
  add_child(path, REG_FILE);
}

int rm(void){
  return remove_node(path, REG_FILE);
}

int menu(void){
  printf("\nAvailable commands:\n"
          "mkdir - create a new directory\n"
          "rmdir - remove target directory\n"
          "creat - create a new file\n"
          "rm    - remove target file\n"
          "pwd   - print working directory\n"
          "cd    - change working directory to target directory\n"
          "ls    - list files and directories contained in current directory\n"
          "menu  - print menu\n"
          "quit  - quit the program\n"
          "save  - save directory tree to file\n"
          "load  - load directory tree from file\n"
          "print - print the complete directory tree.\n\n");
}

int quit(void){
  exit(0);
}


int print(void){
  rec_print_dir(0, root_ptr);
}

int load(void){
    FILE * fd;
  //  char p[PATH_MAX_LEN];
    int t;
    if(fd = fopen("tree.dat", "r+")){

      while(!feof(fd)){
        fscanf(fd, "%d %s", &t, path);
        if (feof(fd)) break;
        if (t == 0){
          creat();
        }
        else{
          mkdir();
        }

      }
      fclose(fd);
    }
    else perror("Unable to open file for reading.");
}



int save(void){
  FILE * fp;
  if (fp = fopen("tree.dat", "w+")){
    rec_save_dir(fp, "", root_ptr -> child_ptr);
    fclose(fp);
  }
  else perror("Unable to open file for writing...");
}
