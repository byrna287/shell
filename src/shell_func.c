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

void set_shell_env(void)
{
   char *curr = malloc(sizeof(char) * 200);
   char *myshell = malloc(sizeof(char) * 200);
   char *mysh_env = malloc(strlen("SHELL=") + sizeof(char) * 201);

   getcwd(curr, 200);
   if (access("myshell", F_OK) == 0)  // if executable is in directory
   {
      getcwd(myshell, 200);
   }
   else if (access("../bin/myshell", F_OK) == 0)  // if executable is in parent then bin directory
   {
      chdir("../bin/");
      getcwd(myshell, 200);
      chdir(curr);
   }
   else if (access("bin/myshell", F_OK) == 0)  // if executable is in bin directory
   {
      chdir("bin/");
      getcwd(myshell, 200);
      chdir(curr);
   }
   else  // if can't find executable
   {
      printf("couldn't set shell environment variable\n");
   }

   strcat(myshell, "/myshell");  // build environmnet variable string
   strcpy(mysh_env, "SHELL=");
   strcat(mysh_env, myshell);
   putenv(mysh_env);
   free(myshell);
   free(curr);
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
   else if (access(tokens[1], F_OK) != 0)
   {
      printf("error: this directory does not exist\n");
   }
   else
   {
      chdir(tokens[1]);
      getcwd(buf, 100);
      char *e_var = malloc(strlen("PWD=") + sizeof(char) * 101);
      strcpy(e_var, "PWD=");
      strcat(e_var, buf);
      putenv(e_var);
   }
   free(buf);
}


void echo(char **tokens)
{
   // check if output is being redirected
   int output = redir_io(tokens, ">");
   if (output != -1)
      freopen(tokens[output], "w", stdout);  // open file for writing
      
   // check if output is being redirected and appended
   int app_output = redir_io(tokens, ">>");
   if (app_output != -1)
      freopen(tokens[app_output], "a", stdout);  // open file for appending

   // remove redirection args if output is being redirected
   int j = 0;
   if (output != -1)
      j = output - 1;
   else if (app_output != -1)
      j = app_output - 1;
   if (j != 0)
      for (int i = j; tokens[i] != NULL; ++i)
         tokens[i] = NULL;

   for (int i = 1; tokens[i] != NULL; ++i)
      printf("%s ", tokens[i]);
   printf("\n");

   freopen("/dev/tty", "w", stdout);   // from stack overflow to resume stdout
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

   pid = fork();
   if (pid == -1)  // fork failed
   {
      printf("Fork failed: exiting.");
      exit(1);
   }
   else if (pid == 0)  //fork succeeded, this is the child process
   {
      // check if output is being redirected
      int output = redir_io(tokens, ">");
      if (output != -1)
         freopen(tokens[output], "w", stdout);  // open file for writing
      
      // check if output is being redirected and appended
      int app_output = redir_io(tokens, ">>");
      if (app_output != -1)
         freopen(tokens[app_output], "a", stdout);  // open file for appending

      tokens[0] = "more";
      // find readme file
      if (access("readme", F_OK) == 0)                  // manual in current directory
         tokens[1] = "readme";
      else if (access("../manual/readme", F_OK) == 0)   // manual in parent and then manual directory
         tokens[1] = "../manual/readme";
      else if (access("manual/readme", F_OK) == 0)      // manual in manual directory
         tokens[1] = "manual/readme";
      else
      {
         printf("error: readme file not found\n");  // print an error message if can't find readme
         _exit(3);
      }
      tokens[2] = NULL;

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

void dir(char **tokens)
{
   pid_t pid;
   int status;

   pid = fork();
   if (pid == -1)  // fork failed
   {
      printf("Fork failed: exiting.");
      exit(1);
   }
   else if (pid == 0)  //fork succeeded, this is the child process
   {
      // check if output is being redirected
      int output = redir_io(tokens, ">");
      if (output != -1)
         freopen(tokens[output], "w", stdout);  // open file for writing
      
      // check if output is being redirected and appended
      int app_output = redir_io(tokens, ">>");
      if (app_output != -1)
         freopen(tokens[app_output], "a", stdout);  // open file for appending

      tokens[2] = tokens[1];  // make directory the third arg
      tokens[0] = "ls";
      tokens[1] = "-al";
      tokens[3] = NULL;

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

void envir(char **tokens)
{
   extern char **environ;

   // check if output is being redirected
   int output = redir_io(tokens, ">");
   if (output != -1)
      freopen(tokens[output], "w", stdout);  // open file for writing
      
   // check if output is being redirected and appended
   int app_output = redir_io(tokens, ">>");
   if (app_output != -1)
      freopen(tokens[app_output], "a", stdout);  // open file for appending

   for (int i = 0; environ[i] != NULL; ++i)
      printf("%s\n", environ[i]);

   freopen("/dev/tty", "w", stdout);   // from stack overflow to resume stdout

}