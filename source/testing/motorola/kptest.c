#include <board.h>

void main (void)
{
  while (!checkkeypad ());
  mon_putc (getkeypad () + '0');
}
