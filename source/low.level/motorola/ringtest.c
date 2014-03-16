#include <board.h>
#include <support.h>

void ringwritebyte (unsigned char);
unsigned char ringreadbyte (void);

void main (void)
{
  int i;
  ringinfo ri;
  if (checkring ())
    {
      ri = getringinfo ();
      puts ("Ring found; family ");
      putn (ri.product, 16, 2);
      puts ("; ID ");
      for (i = 0; i < 6; i++)
	putn (ri.id [i], 16, 2);
      puts ("; CRC ");
      putn (ri.crc, 16, 2);
      puts ("\n\r");
    }
  else
    puts ("Ring not found\n\r");
}
