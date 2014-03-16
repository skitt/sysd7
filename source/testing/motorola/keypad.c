#include <m68kboard.h>

volatile struct mc68230fm * pit = 0x05000000;

void main (void)
{
  unsigned char c, key;
  int i;
  pit->ddra = 0xC0;
  for (i = 0; i < 20; i++)
    {
      while (!(pit->ara & 0x10));
      mon_putc ('.');
      while (pit->ara & 0x10);
      mon_putc ('.');
      key = pit->ara & 0x0F;
      mon_putc (' ');
      if (key > 9)
	mon_putc (key + 'A' - 10);
      else
	mon_putc (key + '0');
      mon_putc ('\n');
      mon_putc ('\r');
    }
}
