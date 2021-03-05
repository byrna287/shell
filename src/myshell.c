#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell_func.h"

#define MAX_BUFFER 1024            // max line length that can be read
#define MAX_ARGS 64                // max number of arguments i.e. words in input
#define SEP " \t\n"                // what to split on

extern char **environ;

int find_index(char *command, char *intern_com[], int size_intc);
int redir_io(char **tokens, char *io_dir);
int bg_exec(char **tokens);
// internal command functions:
void clear(char **tokens);
void quit(char **tokens);
void change_dir(char **tokens);
void echo(char **tokens);
void pause_enter(char **tokens);
void help(char **tokens);

int main(int argc, char *argv[])
{
   char *buf = malloc(sizeof(char) * MAX_BUFFER);       // buffer to read input
   char **tokens = malloc(sizeof(char *) * MAX_ARGS);   // array to store input
   for (int i = 0; i < MAX_ARGS; ++i)                   // initialise array to null
      tokens[i] = NULL;

   char *dir_prompt = malloc(sizeof(char) * 100);       // get current directory to use as prompt
   getcwd(dir_prompt, 100);
   strcat(dir_prompt, " >> ");

   char *intern_com[] = {"clr", "quit", "cd", "echo", "pause", "help"};                        // array of internal command strings
   void (*functions[])(char **tokens) = {clear, quit, change_dir, echo, pause_enter, help};    // array of functions to complete internal commands
   int size_intc = sizeof(intern_com) / sizeof(intern_com[0]);        // size of array of internal commands

   pid_t pid;
   int status;

   // if a file is given as command line argument, read commands from there instead
   if (argc == 2)
   {
      if (access(argv[1], F_OK) == 0)
         freopen(argv[1], "r", stdin);
   }

   while (!feof(stdin))
   {
      if (argc < 2)                         // if not reading commands from a file
         fputs(dir_prompt, stdout);         // print prompt
      fgets(buf, MAX_BUFFER, stdin);        // read input

      char *token = strtok(buf, SEP);       // split input on whitespace
      int i = 0;
      while (token != NULL)                 // add input to array
      {
         tokens[i] = token;
         token = strtok(NULL, SEP);
         ++i;
      }

      // if only enter is pressed (blank line) go to next line (back to top of while loop)
      if (tokens[0] == NULL)
         continue;

      // check if the input is an internal command and find the function's index if it is
      int ind_f = find_index(tokens[0], intern_com, size_intc);

      // internal command: pass to array of functions
      if (ind_f != -1)
         (*functions[ind_f])(tokens);
      // external command: fork and use exec to complete
      else
      {
         int bg;
         if (bg_exec(tokens) == -1)  // check for &
            bg = 0; // no & so exec in fg (wait)
         else
            bg = 1; // & so exec in bg

         // got help for these from labs @ ca216.computing.dcu.ie
         pid = fork();

         if (pid == -1)  // fork failed
         {
            printf("Fork failed: exiting.");
            exit(1);
         }
         else if (pid == 0)  //fork succeeded, this is the child process
         {
            // check if input is being redirected
            int input = redir_io(tokens, "<");
            if (input != -1)
            {
               if (access(tokens[input], F_OK) == 0)   // if the file exists
                  freopen(tokens[input], "r", stdin);  // open file for reading
            }
            // check if output is being redirected
            int output = redir_io(tokens, ">");
            if (output != -1)
            {
               freopen(tokens[output], "w", stdout);  // open file for writing
            }
            // check if output is being redirected and appended
            int app_output = redir_io(tokens, ">>");
            if (app_output != -1)
            {
               freopen(tokens[app_output], "a", stdout);  // open file for appending
            }

            // if there is redirection, change the arguments after redirection to null (including redirection arg)
            int j = 0;
            if (input != -1)
               j = input - 1;
            else if (output != -1)
               j = output - 1;
            else if (app_output != -1)
               j = app_output - 1;
            if (j != 0)
               for (int i = j; tokens[i] != NULL; ++i)
                  tokens[i] = NULL;

            int rcode = execvp(tokens[0], tokens);
            if (rcode == -1)
               printf("command not found: %s\n", tokens[0]);
            _exit(3);  // exits the child process if execvp fails i.e if someone spells a command wrong
         }
         else  // this is the parent process
         {
            if (bg == 0)  // wait, run child process in foreground
               waitpid(pid, &status, WUNTRACED); // waits for child process to complete
            //else run child process in background
         }
      }

      // change the input array back to null to read the next line
      for (int i = 0; tokens[i] != NULL; ++i)
         tokens[i] = NULL;

      // get current directory for prompt
      getcwd(dir_prompt, 100);
      strcat(dir_prompt, " >> ");

   }

   // free memory dynamically allocated
   free(buf);
   free(tokens);
   free(dir_prompt);
   return 0;   // returning also closes all open files
}
