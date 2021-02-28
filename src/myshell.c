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

int main(int argc, char *argv[])
{
   char *buf = malloc(sizeof(char) * MAX_BUFFER);
   char *line = malloc(sizeof(char) * MAX_BUFFER);
   char *tokens[MAX_ARGS] = { NULL };

   char curr_dir[100];
   getcwd(curr_dir, 100);
   strcat(curr_dir, " >> ");

   char *intern_com[] = {"clr", "quit", "cd"};
   void (*functions[])(char **tokens) = {clear, quit, change_dir};
   int size_intc = sizeof(intern_com) / sizeof(intern_com[0]);

   while (!feof(stdin))
   {
      fputs(curr_dir, stdout);
      fgets(buf, MAX_BUFFER, stdin);
      line = strdup(buf);
      
      char *token = strtok(buf, SEP);
      int i = 0;
      while (token != NULL)
      {
         tokens[i] = token;
         token = strtok(NULL, SEP);
         ++i;
      }

      int index = findIndex(tokens[0], intern_com, size_intc);

      if (index != -1)
         (*functions[index])(tokens);
      else
         system(line);

      for (int i = 0; tokens[i] != NULL; ++i)
         tokens[i] = NULL;

      getcwd(curr_dir, 100);
      strcat(curr_dir, " >> ");

   }

   free(buf);
   free(line);
   return 0;
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