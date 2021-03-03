#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int io_in(char **tokens)
{
   for (int i = 0; tokens[i] != NULL; ++i)
   {
      if (strcmp(tokens[i], "<") == 0)
      {
         return i + 1;
      }
   }
   return -1;
}

int main(int argc, char *argv[])
{

   /*char *tokens[] = {"progname", "arg1", "arg2", "<", "filename", NULL, NULL};
   int ind = io_in(tokens);
   if (access(tokens[ind], F_OK) == 0)
   {
      freopen(tokens[ind], "r", stdin);

   }*/

   // TESTING FILES
   printf("Printing args...\n");
   for (int i = 0; i < argc; ++i)
      printf("This is arg %d: %s\n", i, argv[i]);
   printf("Finished!\n");
   /*int num = 0, num2 = 4, num3 = 5;


   if (num != -1)
      printf("first if\n");
   else if (num2 != -1)
      printf("second if\n");
   else if (num3 != -1)
      printf("third if\n");*/


   // TESTING ONLY ENTER PRESSED
   /*char *tokens[64] = { NULL };
   char buf[100];
   fgets(buf, 1024, stdin);

   char *token = strtok(buf, " \t\n");
   int i = 0;
   while (token != NULL)
   {
      tokens[i] = token;
      token = strtok(NULL, " \t\n");
      ++i;
   }
   for (int i = 0; i < 10; ++i)
      printf("%d %s\n", i, tokens[i]);*/

   // TESTING FORK AND EXEC
   /*pid_t pid;
   int status;
   char* tokens[] = {"la", "-al", NULL};
   
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
         printf("command not found: %s\n", tokens[0]);
      _exit(3);
   }
   else  // this is the parent process
   {
      pid = wait(&status);  // waits for child process to complete
   }*/

   return 0;
}