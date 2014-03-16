#include <unistd.h>
#include <stdio.h>

#include <serial.h>

void pkthandler (packet * pkt)
{
  int i;
  puts (pkt->data);
}

void main (void)
{
  unsigned int s = 30;
  puts ("Initialising serial");
  serialinit ();
  puts ("Registering handler");
  registerhandler (2, pkthandler);
  puts ("Sleeping for half a minute");
  while (s)
    s = sleep (s);
  puts ("Closing serial");
  serialclose ();
}
