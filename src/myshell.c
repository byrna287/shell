#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER 1024                // max line length that can be read
#define MAX_ARGS 64                    // max number of arguments i.e. words in input
#define SEP " \t\n"                    // what to split on

extern char **environ;

int findIndex(char *command, char *intern_com[], int size_intc);
void clear(char **tokens);
void quit(char **tokens);
void change_dir(char **tokens);
int redir_io(char **tokens, char *io_dir);

int main(int argc, char *argv[])
{
   char *buf = malloc(sizeof(char) * MAX_BUFFER);
   char *tokens[MAX_ARGS] = { NULL };

   char curr_dir[100];
   getcwd(curr_dir, 100);
   strcat(curr_dir, " >> ");

   char *intern_com[] = {"clr", "quit", "cd"};
   void (*functions[])(char **tokens) = {clear, quit, change_dir};
   int size_intc = sizeof(intern_com) / sizeof(intern_com[0]);

   pid_t pid;
   int status;

   while (!feof(stdin))
   {
      fputs(curr_dir, stdout);
      fgets(buf, MAX_BUFFER, stdin);

      char *token = strtok(buf, SEP);
      int i = 0;
      while (token != NULL)
      {
         tokens[i] = token;
         token = strtok(NULL, SEP);
         ++i;
      }

      // if only enter is pressed (blank line) go back to top of while loop
      if (tokens[0] == NULL)
         continue;

      int index = findIndex(tokens[0], intern_com, size_intc);

      // if the input is an internal command pass it to the array of functions
      if (index != -1)
         (*functions[index])(tokens);
      else
      {
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
               if (access(tokens[input], F_OK) == 0)  // if the file exists
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
            pid = wait(&status);  // waits for child process to complete
         }
      }

      // change the input array back to null to read the next line
      for (int i = 0; tokens[i] != NULL; ++i)
         tokens[i] = NULL;

      getcwd(curr_dir, 100);
      strcat(curr_dir, " >> ");

   }

   free(buf);
   return 0;
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
   char buf[100];
   if (tokens[1] == NULL)
   {
      printf("%s\n", getcwd(buf, 100));
   }
   else
   {
      chdir(tokens[1]);
      getcwd(buf, 100);
      char *e_var = malloc(strlen("PWD=") + strlen(buf) + 1);
      strcpy(e_var, "PWD=");
      strcat(e_var, buf);
      putenv(e_var);
   }
}