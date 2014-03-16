#include <serial.h>
#include <support.h>

void pkthandler (packet * pkt)
{
  int i;
  puts (pkt->data);
}

void main (void)
{
  int i;
  packet pkt;
  
  puts ("Initialising serial\n\r");
  serialinit ();
  puts ("Registering handler\n\r");
  registerhandler (2, pkthandler);
  pkt.datatype = 2;
  pkt.length = 4;
  pkt.data = "Blah";
  while (1);
  puts ("Closing serial\n\r");
  serialclose ();
}
