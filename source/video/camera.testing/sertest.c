/* Quickcam test program  - outputs an XPM to the terminal */

/* $Id: test.c,v 1.4 1998/04/23 13:31:28 skk Exp skk $ */

#include <m68kboard.h>

void puts (char *);
void hex (unsigned char);

void main (void)
{
  int errno;
  int i, j;
  unsigned char c;

  struct mc68681wfm * uartw;

  uartw = (struct mc68681wfm *) 0x00B00003;
  puts ("UART OPCR: ");
  hex (uartw->opcr);
  puts ("\n\r");
}

void puts (char * string)
{
  while (*string)
    {
      mon_putc (*string);
      string++;
    }
}

void realhex (unsigned char value)
{
  if (value < 10)
    mon_putc (value + '0');
  else
    mon_putc (value + 'A' - 10);
}

void hex (unsigned char value)
{
  unsigned char high, low;
  high = (value & 0xF0) >> 4;
  low = value & 0x0F;
  realhex (high);
  realhex (low);
}

void number (int value, int max)
{
  int i;
  for (i = max; i > 0; i /= 10)
    {
      mon_putc (value / i + '0');
      value %= i;
    }
}
