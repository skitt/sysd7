#include <serial.h>

packet pkt;

void main (void)
{
  int i;
  int s = 120;
  serialinit ();
  pkt.datatype = 2;
  pkt.data = "This is a longer version of the test string, to see how the serial copes with\n\rmore data\n\r";
  pkt.length = 0;
  while (pkt.data [pkt.length])
    pkt.length++;
  pkt.length++;
  for (i = 0; i < 1; i++)
    senddata (&pkt);
  serialclose ();
}

