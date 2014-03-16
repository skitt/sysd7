#include <serial.h>

void main (void)
{
  int i;

  serialinit ();

  for (i = 0; i < 256; i++)
    {
      serialtransmit ((unsigned char ) i);
    }

  serialclose ();
}
