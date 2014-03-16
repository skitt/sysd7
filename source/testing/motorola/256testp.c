#include <serial.h>
#include <support.h>

unsigned char _buf [256];

void main (void)
{
  int i;
  serialinit ();
  for (i = 0; i < 256; i++)
    _buf [i] = (unsigned char) serialreceive ();
  for (i = 0; i < 256; i++)
    {
      if (!(i % 25))
	puts ("\n\r");
      putn (_buf [i], 16, 2);
      puts (" ");
    }
  serialclose ();
}
