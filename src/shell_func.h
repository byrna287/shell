/*
*  functions used in myshell.c
*
*  Ailbhe Byrne
*  Student number: 19424402
*  I acknowledge the DCU academic integrity policy.
*/

int find_index(char *command, char *intern_com[], int size_intc);
int redir_io(char **tokens, char *io_dir);
int bg_exec(char **tokens);
void set_shell_env(void);
// internal command functions:
void change_dir(char **tokens);
void clear(char **tokens);
void dir_list(char **tokens);
void envir(char **tokens);
void echo(char **tokens);
void help(char **tokens);
void pause_enter(char **tokens);
void quit(char **tokens);
void copy(char **tokens);
