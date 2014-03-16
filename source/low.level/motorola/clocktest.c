#include <board.h>
#include <m68kboard.h>

volatile struct mc68230fm * bpit = 0x05000000;

void main (void)
{
  /* Set to output, expecting high */
  bpit->dra = 0xE0;
  bpit->ddra = 0xE0;
  while (1)
    {
      starttimer (10000000);
      /* Pull low */
      bpit->dra = 0xC0;
      /* Start the timer and wait for 15 us */
#if 0
      while (gettimer () > 10000000 - 2);
#endif
      bpit->dra = 0xE0;
#if 0
      while (gettimer () > 10000000 - 4);
#endif
      bpit->dra = 0xC0;
      bpit->dra = 0xC0;
      bpit->dra = 0xC0;
      bpit->dra = 0xE0;
    };
}
