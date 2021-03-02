#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{

   char *tokens[64] = { NULL };
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
      printf("%d %s\n", i, tokens[i]);

   /*printf("Executing...\n");
   printf("Printing arg1:\n");
   printf("%s\n", argv[1]);
   printf("Finished!\n");*/

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