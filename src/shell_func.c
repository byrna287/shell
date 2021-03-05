#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell_func.h"

/*
*  functions used in myshell.c
*/

int find_index(char *command, char *intern_com[], int size_intc)
{
   for (int i = 0; i < size_intc; ++i)
   {
      if (strcmp(intern_com[i], command) == 0)
         return i;
   }
   return -1;
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

// internal command functions:

void clear(char **tokens)
{
   pid_t pid;
   int status;
   tokens[0] = "clear";

   pid = fork();
   if (pid == -1)  // fork failed
   {
      printf("Fork failed: exiting.");
      exit(1);
   }
   else if (pid == 0)  //fork succeeded, this is the child process
   {
      int rcode = execvp(tokens[0], tokens);
      if (rcode == -1)
         printf("error: command failed\n");
      _exit(3);
   }
   else  // this is the parent process
   {
      pid = wait(&status);  // waits for child process to complete
   }
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

void help(char **tokens)
{
   pid_t pid;
   int status;
   tokens[0] = "more";

   // find readme file
   if (access("readme", F_OK) == 0)                  // manual in current directory
      tokens[1] = "readme";
   else if (access("../manual/readme", F_OK) == 0)   // manual in parent and then manual directory
      tokens[1] = "../manual/readme";
   else if (access("manual/readme", F_OK) == 0)      // manual in manual directory
      tokens[1] = "manual/readme";

   pid = fork();
   if (pid == -1)  // fork failed
   {
      printf("Fork failed: exiting.");
      exit(1);
   }
   else if (pid == 0)  //fork succeeded, this is the child process
   {
      if (tokens[1] == NULL)  // couldn't find readme file, not in directory of the shell
      {
         printf("error: outside project, readme file not found\n");  // print an error message
         _exit(3);
      }
      int rcode = execvp(tokens[0], tokens);
      if (rcode == -1)
         printf("error: command failed");
      _exit(3);
   }
   else  // this is the parent process
   {
      pid = wait(&status);  // waits for child process to complete
   }
}

int bg_exec(char **tokens)
{
   for (int i = 0; tokens[i] != NULL; ++i)
      if (strcmp(tokens[i], "&") == 0)
      {
         tokens[i] = NULL;
         return 1;
      }
   return -1;
}
