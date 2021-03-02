#include <stdio.h>

int main(int argc, char *argv[])
{
   int input;
   scanf("%d", &input);
   for (int i = 1; i < argc; ++i)
      printf("This is arg %d: %s\n", i, argv[i]);
   printf("Coming from input: %d\n", input);
   return 0;
}