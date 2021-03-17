#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "shell_func.h"

/*
*  functions used in myshell.c
*/

/*
* params: char*, char*[], int
* description: checks if the string passed is inside the array passed. Returns the
* index of the string if it is present and -1 if it is not.
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

/*
* params: char**, char*
* description: checks if the string passed is in the array passed. In myshell we are 
* checking if the redirection arguments are present (<, >, >>). If they are present it
* returns the index of the file to be redirected to, if not present returns -1.
*/
int redir_io(char **tokens, char *io_dir)
{
   for (int i = 0; tokens[i] != NULL; ++i)
   {
      if (strcmp(tokens[i], io_dir) == 0)
         return i + 1;
   }
   return -1;
}

/*
* params: char**
* description: checks if the argument for background execution is present (&). If it is
* present, points that char* at null ands returns 1, if not present returns -1.
*/
int bg_exec(char **tokens)
{
   for (int i = 0; tokens[i] != NULL; ++i)
      if (strcmp(tokens[i], "&") == 0)
      {
         tokens[i] = NULL;
         return 1;  // exec in bg
      }
   return -1;  // exec in fg
}

/*
* params: void
* description: sets the SHELL environment variable to the full path of the shell executable.
* Has to find the shell executable first, could be started in any directory within 2021-ca216-myshell.
*/
void set_shell_env(void)
{
   char *curr = malloc(sizeof(char) * 200);
   char *mysh_path = malloc(sizeof(char) * 256);
   char *mysh_env = malloc(strlen("SHELL=") + sizeof(char) * 257);

   getcwd(curr, 200);                 // get current directory
   if (access("myshell", F_OK) == 0)  // if executable is in current directory
   {
      getcwd(mysh_path, 256);         // get full path of myshell executable
   }
   else if (access("../bin/myshell", F_OK) == 0)  // if executable is in parent then bin directory
   {
      chdir("../bin/");         // change to directory with myshell
      getcwd(mysh_path, 256);
      chdir(curr);              // change back to original directory
   }
   else if (access("bin/myshell", F_OK) == 0)  // if executable is in bin directory
   {
      chdir("bin/");
      getcwd(mysh_path, 256);
      chdir(curr);
   }
   else  // if can't find executable
   {
      printf("couldn't set shell environment variable\n");  // print error message
   }

   strcat(mysh_path, "/myshell");  // build environmnet variable string
   strcpy(mysh_env, "SHELL=");
   strcat(mysh_env, mysh_path);
   putenv(mysh_env);               // set shell environment variable

   free(mysh_path);
   free(curr);
}

// internal command functions:

/*
* params: char**
* description: clears the shell screen. Forks and uses exec to execute the clear command.
*/
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
   else if (pid == 0)  // fork succeeded, this is the child process
   {
      int rcode = execvp(tokens[0], tokens);
      if (rcode == -1)
         printf("error: command failed\n");
      _exit(3);  // exit child process if execvp fails
   }
   else  // this is the parent process
   {
      waitpid(pid, &status, WUNTRACED);  // waits for child process to complete
   }
}

/*
* params: char**
* description: exits the shell.
*/
void quit(char **tokens)
{
   exit(0);
}

/*
* params: char**
* description: changes the current directory. Also sets pwd environment variable to the
* directory changed into.
*/
void change_dir(char **tokens)
{
   char *buf = malloc(sizeof(char) * 256);
   struct stat filestats;        // variable for file stats
   stat(tokens[1], &filestats);  // get file stats
   if (tokens[1] == NULL)        // if no argument given print current directory
   {
      printf("%s\n", getcwd(buf, 256));
   }
   else if (access(tokens[1], F_OK) != 0)  // if directory doesn't exist print error message
   {
      printf("error: does not exist\n");
   }
   else if (S_ISDIR(filestats.st_mode) == 0)  // check file is not a directory
   {
      printf("error: not a directory\n");
   }
   else
   {
      chdir(tokens[1]);  // change directory to argument given
      getcwd(buf, 256);
      char *e_var = malloc(strlen("PWD=") + sizeof(char) * 257);
      strcpy(e_var, "PWD=");
      strcat(e_var, buf);
      putenv(e_var);     // set pwd environment variable
   }
   free(buf);
}

/*
* params: char**
* description: prints all elements of the array except the first to a new line, followed
* by a new line. Checks for output redirection.
*/
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

   for (int i = 1; tokens[i] != NULL; ++i)  // print args
      printf("%s ", tokens[i]);
   printf("\n");

   if (output != -1 || app_output != -1)  // if there was output redirection
      freopen("/dev/tty", "w", stdout);   // resume stdout (from stack overflow)
}

/*
* params: char**
* description: pauses the shell until enter is pressed. To ignore other input keeps reading in
* input until enter is pressed but then does nothing with that input.
*/
void pause_enter(char **tokens)
{
   char *buf = malloc(sizeof(char) * 1024); // buffer for input the user might enter
   printf("Press Enter to continue...\n");
   fgets(buf, 1024, stdin);                 // pressing enter ends the reading of input
   free(buf);
}

/*
* params: char**
* description: prints the user manual. Has to find manual file, could be called from any directory
* within 2021-ca216-myshell. Forks and uses exec to display the output with the more unix utility.
* Checks for output redirection.
*/
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
   else if (pid == 0)  // fork succeeded, this is the child process
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
         _exit(3);  // exit child process
      }
      tokens[2] = NULL;

      int rcode = execvp(tokens[0], tokens);
      if (rcode == -1)
         printf("error: command failed");
      _exit(3);  // exit child process if execvp fails
   }
   else  // this is the parent process
   {
      waitpid(pid, &status, WUNTRACED);  // waits for child process to complete
   }
}

/*
* params: char**
* description: lists the contents of the directory given or the current directory if none
* given. Forks and uses exec to execute the "ls -al" command. Checks for output redirection.
*/
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
   else if (pid == 0)  // fork succeeded, this is the child process
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
      _exit(3);  // exit child process if execvp fails
   }
   else  // this is the parent process
   {
      waitpid(pid, &status, WUNTRACED);  // waits for child process to complete
   }
}

/*
* params: char**
* description: prints the environment variables. Checks for output redirection.
*/
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

   if (output != -1 || app_output != -1)
      freopen("/dev/tty", "w", stdout);   // resume stdout (from stack overflow)
}

/*
* params: char**
* description: copies contents of a file to another file. If file to copy into is
* a directory, copies file into directory with name of file being copied.
* Got help from stack overflow on how to check if a file is a directory.
*/
void copy(char **tokens)
{
   int MAX_BUFFER = 1024;                          // max line size to be read
   char *buf = malloc(sizeof(char) * MAX_BUFFER);  // to store lines from file being read

   if (access(tokens[1], F_OK) == 0)               // if the file to be copied exists
   {
      struct stat filestats;                       // variable to hold the stats of a file
      
      stat(tokens[1], &filestats);                 // get the stats of a file
      if (S_ISDIR(filestats.st_mode) == 0)         // check file not a directory, returns 0 if not
      {
         stat(tokens[2], &filestats);
         if (S_ISDIR(filestats.st_mode) != 0)      // check file is a directory
         {
            strcat(tokens[2], "/");
            strcat(tokens[2], tokens[1]);          // change file name to copy into directory
         }
         
         FILE *fp = fopen(tokens[1], "r");         // open file for reading
         FILE *outfp = fopen(tokens[2], "w");      // open file for writing
         while (!feof(fp))
         {
            fgets(buf, MAX_BUFFER, fp);            // read line from file to be copied
            fprintf(outfp, "%s", buf);             // print line to file copying into

         }
         fclose(fp);                               // close files
         fclose(outfp);
      }
      else
      {
         printf("error: can't copy a directory\n");  // error message if file to be copied is a directory
      }
   }
   else
   {
      printf("error: file does not exist\n");        // error message if file to be copied does not exist
   }
   free(buf);
}

