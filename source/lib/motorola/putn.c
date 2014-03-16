#include <support.h>

void putn (unsigned long number, int base, int digits)
{
  unsigned long magnitude;
  unsigned char digit;
  
  magnitude = 1;
  while (digits > 1)
    {
      magnitude *= base;
      digits--;
    };
  while (magnitude > 0)
    {
      digit = number / magnitude;
      if (digit < 10)
	mon_putc ('0' + digit);
      else
	mon_putc ('A' - 10 + digit);
      number %= magnitude;
      magnitude /= base;
    };
}
