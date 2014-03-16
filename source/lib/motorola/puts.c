#include <support.h>
#include <m68kboard.h>

void puts (char * string)
{
  while (*string)
    {
      mon_putc (*string);
      string++;
    }
}
