#include <serial.h>

packet pkt;
unsigned char buf [256];

void main (void)
{
  int i;
  pkt.datatype = 3;
  pkt.length = 256;
  pkt.data = buf;
  serialinit ();
  for (i = 0; i < 10; i++)
    senddata (&pkt);
  serialclose ();
}
