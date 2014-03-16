#include <m68kboard.h>

main ()
{
  char c;
  while ((c = mon_getc ()) != 4)
    mon_putc (c);
}
