#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell_func.h"

/*
*  functions used in myshell.c
*/

int findIndex(char *command, char *intern_com[], int size_intc)
{
   for (int i = 0; i < size_intc; ++i)
   {
      if (strcmp(intern_com[i], command) == 0)
         return i;
   }
   return -1;
}

void clear(char **tokens)
{
   system("clear");
}

void quit(char **tokens)
{
   exit(0);
}

void change_dir(char **tokens)
{
   char *buf = malloc(sizeof(char) * 100);
   if (tokens[1] == NULL)
   {
      printf("%s\n", getcwd(buf, 100));
   }
   else
   {
      chdir(tokens[1]);
      getcwd(buf, 100);
      char *e_var = malloc(strlen("PWD=") + 101);
      strcpy(e_var, "PWD=");
      strcat(e_var, buf);
      putenv(e_var);
   }
   free(buf);
}

int redir_io(char **tokens, char *io_dir)
{
   for (int i = 0; tokens[i] != NULL; ++i)
   {
      if (strcmp(tokens[i], io_dir) == 0)
         return i + 1;
   }
   return -1;
}

void echo(char **tokens)
{
   for (int i = 1; tokens[i] != NULL; ++i)
   {
      printf("%s ", tokens[i]);
   }
   printf("\n");
}

void pause_enter(char **tokens)
{
   char *buf = malloc(sizeof(char) * 1024);
   printf("Press Enter to continue...\n");
   fgets(buf, 1024, stdin);
   free(buf);
}