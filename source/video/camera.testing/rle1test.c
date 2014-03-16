/* Quickcam/RLE test program */

/* $Id: test.c,v 1.4 1998/04/23 13:31:28 skk Exp skk $ */

#include <m68kboard.h>
#include <quickcam.h>

void puts (char *);
void hex (unsigned char);
void number (int, int);

unsigned char buffer [19200];
unsigned char rle [19200];

void main (void)
{
  int errno;
  int i, j;
  unsigned char c, last, count;

  puts ("Initialising camera\n\r");
  QC_init ();
  puts ("Grabbing frame\n\r");
  QC_grabframe (buffer);
  puts ("Compressing\n\r");
  j = 0;
  last = buffer [0];
  count = 0;
  for (i = 1; i < 19200; i++)
    {
      if (buffer [i] == last)
	count++;
      if (buffer [i] != last || count == 15)
	{
	  rle [j] = (count << 4) + last;
	  j++;
	  last = buffer [i];
	  count = 0;
	}
    }
  puts ("RLE compressed to ");
  number (j, 10000);
}

void puts (char * string)
{
  while (*string)
    {
      mon_putc (*string);
      string++;
    }
}

void hex (unsigned char value)
{
  if (value < 10)
    mon_putc (value + '0');
  else
    mon_putc (value + 'A' - 10);
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
