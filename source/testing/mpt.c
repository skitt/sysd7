#include <serial.h>
#include <stdio.h>

int pr = 0;

void pkthandler3 (packet * pkt)
{
  puts ("Received packet 3");
  pr = 1;
}

void main (void)
{
  int s = 120;
  serialinit ();
  registerhandler (3, pkthandler3);
  while (!pr)
    sleep (10);
  changestate (2);
  while (s)
    s = sleep (s);
  serialclose ();
}
