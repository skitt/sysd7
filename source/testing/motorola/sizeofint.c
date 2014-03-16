#include <m68kboard.h>

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

void main (void)
{
  puts ("Size of int: ");
  number (sizeof (int), 10);
  puts (" bytes\n\rSize of long: ");
  number (sizeof (long), 10);
  puts (" bytes\n\r");
}
