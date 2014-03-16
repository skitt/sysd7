#include <serial.h>
#include <support.h>

packet pkt;

void main (void)
{
  int i;
  serialinit ();
  pkt.datatype = 2;
  pkt.data = "This is a longer version of the test string, to see how the serial copes with more data";
  pkt.length = 0;
  while (pkt.data [pkt.length])
    pkt.length++;
  for (i = 0; i < 1; i++)
    senddata (&pkt);
  serialclose ();
}

