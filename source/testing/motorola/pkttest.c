#include <serial.h>
#include <support.h>

unsigned char buf [16384];

void main (void)
{
  packet pkt;
  int i;

  puts ("Going\n\r");
  serialinit ();

  pkt.datatype = 2;
  pkt.length = 16384;
  pkt.data = buf;
  for (i = 0; i < 16384; i++)
    buf [i] = i % 256;
  senddata (&pkt);

  serialclose ();
}
