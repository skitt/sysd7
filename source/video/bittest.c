/* Quickcam/RLE test program */

/* $Id: test.c,v 1.4 1998/04/23 13:31:28 skk Exp skk $ */

#include <m68kboard.h>
#include <quickcam.h>

typedef struct
{
  unsigned char * index;
  unsigned char bit;
} bitpointer;

void puts (char *);
void hex (unsigned char);
void number (int, int);

int getbit (bitpointer bp)
{
  unsigned char c = *(bp.index);
  return ((c & (1 << bp.bit)) >> bp.bit);
}

void setbit (bitpointer bp, int value)
{
  unsigned char c = *(bp.index);
  if (value)
    c |= 1 << bp.bit;
  else
    c &= ~(1 << bp.bit);
  *(bp.index) = c;
}

unsigned char buffer1 [19200];
unsigned char buffer2 [19200];
unsigned char diff [19200];
unsigned char rle [19200];

void main (void)
{
  int errno;
  int i, j;
  unsigned char c, last, count;
  bitpointer bp;

  c = 1;
  bp.index = &c;
  puts ("1, set bit 3: ");
  bp.bit = 3;
  setbit (bp, 1);
  hex (c);
  puts ("; clear bit 0: ");
  bp.bit = 0;
  setbit (bp, 0);
  hex (c);
  puts ("\n\r");
  puts ("Initialising camera\n\r");
  QC_init ();
  puts ("Grabbing frame 1\n\r");
  QC_grabframe (buffer1);
  puts ("Grabbing frame 2\n\r");
  QC_grabframe (buffer2);
  puts ("Calculating difference\n\r");
  for (i = 0; i < 19200; i++)
    {
      diff [i] = buffer1 [i] ^ buffer2 [i];
      hex (diff [i]);
      if (!(i % 160))
	puts ("\n\r");
    }
  puts ("Compressing\n\r");
  j = 0;
  last = diff [0];
  count = 0;
  for (i = 1; i < 19200; i++)
    {
      if (diff [i] == last)
	count++;
      if (diff [i] != last || count == 15)
	{
	  rle [j] = (count << 4) + last;
	  j++;
	  last = diff [i];
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
