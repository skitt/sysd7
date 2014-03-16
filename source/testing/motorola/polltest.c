#include <m68kboard.h>
#include <serial.h>
#include <support.h>

void main (void)
{
  packet out;
  int i;
  unsigned char * buffer = "040 says hello!";
  /* Initialise serial */
  serialinit ();
  /* Build packet */
  out.datatype = 2;
  out.length = 16;
  out.data = buffer;
  /* Send the data */
  for (i = 0; i < 1000; i++)
    senddata (&out);
  /* Close serial */
  serialclose ();
}

