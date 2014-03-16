#include <serial.h>
#include <support.h>

void main (void)
{
  int i, j;
  char * buf = "Stephen is a plonker\n";

  serialinit ();
  for (j = 0; j < 1000; j++)
    for (i = 0; i < 21; i++)
      serialtransmit (buf [i]);
  serialclose ();
}
