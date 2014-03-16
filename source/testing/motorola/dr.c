#include <m68kboard.h>

volatile struct mc68230fm * pit = 0x05000000;

void main (void)
{
  int i;
  pit->ddra = 0xE0;
  pit->dra = 0x00;
  for (i = 0; i < 6000; i++)
    asm ("nop");
  pit->ddra = 0xC0;
  for (i = 0; i < 600; i++)
    asm ("nop");
  if (!(pit->dra & 0x20))
    mon_putc ('R');
}
