#include <serial.h>

void main (void)
{
  packet pkt;
  char buf [256];
  int i;

  serialinit ();

  pkt.datatype = 2;
  pkt.length = 256;
  pkt.data = buf;
  for (i = 0; i < 256; i++)
    buf [i] = i;
  senddata (&pkt);
}
