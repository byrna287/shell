/*
*  functions used in myshell.c
*/

int findIndex(char *command, char *intern_com[], int size_intc);
void clear(char **tokens);
void quit(char **tokens);
void change_dir(char **tokens);
int redir_io(char **tokens, char *io_dir);
void echo(char **tokens);
void pause_enter(char **tokens);
