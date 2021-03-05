#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int bg_exec(char **tokens)
{
   for (int i = 0; tokens[i] != NULL; ++i)
      if (strcmp(tokens[i], "&") == 0)
         return 1;
   return -1;
}

int main(int argc, char *argv[])
{

   /*char *tokens[] = {"oh", "my", "god", "blah", NULL, NULL};
   printf("%d\n", bg_exec(tokens));*/

   pid_t pid;
   int status;
   char* tokens[] = {"more", "../manual/readme", NULL};
   
   pid = fork();
   if (pid == -1)  // fork failed
   {
      printf("Fork failed: exiting.");
      exit(1);
   }
   else if (pid == 0)  //fork succeeded, this is the child process
   {
      printf("This is child.\n");
      /*int rcode = execvp(tokens[0], tokens);
      if (rcode == -1)
         printf("command not found: %s\n", tokens[0]);
      _exit(3);*/
   }
   else  // this is the parent process
   {
      //pid = wait(&status);  // waits for child process to complete
      printf("This is parent\n");
   }
}