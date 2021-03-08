#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
   if (access("src", F_OK) == 0)
   {
      printf("exists\n");
   }
   return 0;
}