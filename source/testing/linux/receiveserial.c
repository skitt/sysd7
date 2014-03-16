#include <serial.h>
#include <stdio.h>

void main (void)
{
  int i;
  serialinit ();
  for (i = 0; i < 21000; i++)
    putchar (serialreceive ());
  serialclose ();
}
