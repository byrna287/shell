#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
   printf("a1 %s\n", argv[1]);
   printf("a2 %s\n", argv[2]);
   int num;
   scanf("%d", &num);
   printf("input %d\n", num);
   return 0;
}