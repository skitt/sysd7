#include <m68kboard.h>
#include <board.h>

volatile struct mc68230fm * pit = 0x05000000;

void main (void)
{
  int i;
  pit->ddrc = 0x03;
#if 0
  starttimer (10000000);
  pit->drc = 0xFD;
  while (gettimer () > 10000000 - 30000);
  pit->drc = 0xFF;
  while (gettimer () > 10000000 - 40000);
  pit->drc = 0xFD;
  while (gettimer () > 10000000 - 70000);
  pit->drc = 0xFF;
#endif
  pit->drc = 0xFC;
  for (i = 0; i < 5; i++)
    {
      starttimer (1010000);
      while (gettimer () > 10000);
      stoptimer ();
    }
  pit->drc = 0xFF;
}
