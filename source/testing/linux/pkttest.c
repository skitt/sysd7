#include <stdio.h>

#include <serial.h>
#include <support.h>

void handler (packet * pkt)
{
  int i;
  printf ("Got packet type 2, length %d\n\r", pkt->length);
  for (i = 0; i < 16384; i++)
    if (pkt->data [i] != i % 256)
      printf ("Error at offset %d; value %d\n\r", i, (int) pkt->data [i]);
  puts ("Packet check done");
}

void main (void)
{
  serialinit ();
  registerhandler (2, handler);
  while (1);
}
