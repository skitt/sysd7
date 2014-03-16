#include <m68kboard.h>

struct mk48t02fm {
    unsigned char f00, f01, f02;
  unsigned char ccr;
    unsigned char f10, f11, f12;
  unsigned char csr;
    unsigned char f20, f21, f22;
  unsigned char cmr;
};

volatile struct mc68230fm * pit = 0x05000000;
volatile struct mk48t02fm * rtc = 0x00A01FE0;

void number (int c)
{
  int i;
  for (i = 100; i > 0; i /= 10)
    {
      mon_putc (c / i + '0');
      c %= i;
    }
}

void main (void)
{
  unsigned char c;
  int i;

  mon_putc ('G');
  mon_getc ();
  mon_putc ('.');
  pit->ddra = 0xC0;
  do
    {
      c = pit->ara;
      asm ("nop");
    }
  while (!(c & 0x10));
  mon_putc ('E');
}
