/*Unix/Linux File System Tree Simulator
Author: Anderson Freixo
e-mail: anderson.freixo@gmail.com
This is my implementation of the suggested project contained in chapter 2
of the book "Systems Programming in Unix-Linux", by K C Wang.
Besides the implementation of all suggested commands it was also implemented
the print command, which prints all the directory tree. */

/*string length constants*/
#define DIR_MAX_LEN 64
#define BASE_MAX_LEN 64
#define PATH_MAX_LEN 256
#define COMM_MAX_LEN 20

enum types{REG_FILE, DIR_FILE};

enum commands{MKDIR_COMM, RMDIR_COMM, CD_COMM, LS_COMM, PWD_COMM, CREAT_COMM,
  RM_COMM, MENU_COMM, QUIT_COMM, LOAD_COMM, SAVE_COMM, PRINT_COMM, NULL_COMM};


/*The file system is implemented as a binary tree
where child_ptr points to the oldest subdirectory/file added to the directory
and sibling_ptr points to the other subdirectories/files of parent directory.*/
typedef struct tree_node{
    char name[BASE_MAX_LEN];
    int type;
    struct tree_node * child_ptr, * sibling_ptr, *parent_ptr;
  }tree_node;

/*functions for implementation of the shell commands*/
int mkdir(void); //creates new directory
int rmdir(void); //removes target directory
int cd(void);    //changes directory
int ls(void);    //lists files and directories inside current directory
int pwd(void);   //prints working directory path
int creat(void); //creates new file
int rm(void);    //removes target file
int menu(void);  //prints the menu
int quit(void);  //quits the program
int load(void);  //load file tree from file
int save(void);  //save file tree to file
int print(void); //print the complete directory tree

/*helper functions*/
void print_dir(tree_node * node); //pwd() recursive helper function
int get_command_id(char * comm);  //finds the command id based on input.
void decompose_path(char * path); //modify globals dir_name and base_name
int add_child(char * path, int ftype); //used by mkdir and create
int remove_node(char * path, int ftype);//used by rmdir and rm
tree_node * get_target_dir(void);   //Returns target *node based on dir_name.
tree_node * get_target_wbase(void); //Returns target dir with base included.
void rec_print_dir(int depth, tree_node * n_ptr); //print() recursive helper function.
void rec_save_dir(FILE * fp, char * p, tree_node * n_ptr); //save() rec helper.

/*globals*/
tree_node * root_ptr, * cwd_ptr;

char path[PATH_MAX_LEN];
char dir_name[DIR_MAX_LEN];
char base_name[BASE_MAX_LEN];

char * comm_str[] = {
    [MKDIR_COMM] = "mkdir",
    [RMDIR_COMM] = "rmdir",
    [CD_COMM] = "cd",
    [LS_COMM] = "ls",
    [PWD_COMM] = "pwd",
    [CREAT_COMM] = "creat",
    [RM_COMM] = "rm",
    [MENU_COMM] = "menu",
    [QUIT_COMM] = "quit",
    [LOAD_COMM] = "load",
    [SAVE_COMM] = "save",
    [PRINT_COMM] = "print"};

int (*comm_func[])() = {
  [MKDIR_COMM] = mkdir,
  [RMDIR_COMM] = rmdir,
  [CD_COMM] = cd,
  [LS_COMM] = ls,
  [PWD_COMM] = pwd,
  [CREAT_COMM] = creat,
  [RM_COMM] = rm,
  [MENU_COMM] = menu,
  [QUIT_COMM] = quit,
  [LOAD_COMM] = load,
  [SAVE_COMM] = save,
  [PRINT_COMM] = print
};
