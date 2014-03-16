#include <stdio.h>

void main(int argc, char *argv[])
{
  int t,c;
  while ((t=getchar()) != EOF) {
    printf ("%X ", t);
    if (!(++c % 25)){
      c=0;
      printf ("\n");
    }
  }
}
