#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{

   char *dir_prompt = malloc(sizeof(char) * 100);
   getcwd(curr_dir, 100);
   strcat(curr_dir, " >> ");
   printf("%s\n", curr_dir);

   /*char **tokens = malloc(sizeof(char *) * 64);
   for (int i = 0; i < 64; ++i)
      tokens[i] = NULL;

   for (int i = 0; i < 64; ++i)
      printf("%d %s\n", i, tokens[i]);*/

   /*int input;
   scanf("%d", &input);
   for (int i = 1; i < argc; ++i)
      printf("This is arg %d: %s\n", i, argv[i]);
   printf("Coming from input: %d\n", input);*/
   return 0;
}