#include <serial.h>
#include <support.h>

void handler (packet * pkt)
{
  int i;
  puts ("Got packet type 2, length ");
  putn (pkt->length, 10, 3);
  puts ("\n\r");
  for (i = 0; i < 256; i++)
    if (pkt->data [i] != i)
      {
	puts ("Error at offset ");
	putn (i, 10, 3);
	puts ("\n\r");
      }
}

void main (void)
{
  serialinit ();
  registerhandler (2, handler);
  while (1);
}
