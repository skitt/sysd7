/* Quickcam test program  - outputs an XPM to the terminal */

/* $Id: test.c,v 1.4 1998/04/23 13:31:28 skk Exp skk $ */

#include <m68kboard.h>
#include <quickcam.h>

void puts (char *);

unsigned char buffer [19200];
unsigned char compressed [19200];
unsigned char result [19200];

void realhex (unsigned char value)
{
  if (value < 10)
    mon_putc (value + '0');
  else
    mon_putc (value + 'A' - 10);
}

void setbit (unsigned char * ptr, unsigned long off, int val)
{
  unsigned int idx, bit;
  idx = (unsigned int) (off / 8);
  bit = (unsigned int) (off % 8);
  if (val)
    ptr [idx] |= 1 << bit;
  else
    ptr [idx] &= ~(1 << bit);
}

void setval (unsigned char * ptr, unsigned long off, int val, int
	     bits)
{
  int i;
  for (i = 0; i < bits; i++)
    setbit (ptr, off + i, val & (1 << (bits - i)));
}

int getbit (unsigned char * ptr, unsigned long off)
{
  unsigned int idx, bit;
  idx = (unsigned int) (off / 8);
  bit = (unsigned int) (off % 8);
  return (ptr [idx] & (1 << bit)) >> bit;
}

int getval (unsigned char * ptr, unsigned long off, int bits)
{
  int result, i;
  result = 0;
  for (i = 0; i < bits; i++)
    result |= getbit (ptr, off + i) << (bits - i);
  return result;
}

unsigned long compress (unsigned char * src, unsigned long slen,
			unsigned char * dest, unsigned long dlen)
{
  unsigned long soff, doff;
  unsigned char last, count;
  last = src [0];
  count = 0;
  doff = 0;
  for (soff = 1; soff < slen && doff < 8 * dlen; soff++)
    {
      if (src [soff] == last && count < 31)
	count++;
      else
	{
	  if (count)
	    {
	      setbit (dest, doff, 1);
	      setval (dest, doff + 1, last, 4);
	      setval (dest, doff + 5, count, 5);
	      doff += 10;
	      last = src [soff];
	      count = 0;
	    }
	  else
	    {
	      setbit (dest, doff, 0);
	      setval (dest, doff + 1, last, 4);
	      doff += 5;
	      last = src [soff];
	    }
	}
    }
  return doff;
}

void decompress (unsigned char * src, unsigned long slen, unsigned
		 char * dest, unsigned long dlen)
{
  unsigned char soff, doff;
  int i;
  doff = 0;
  soff = 0;
  while (soff < slen)
    {
      if (getbit (src, soff))
	{
	  for (i = 0; i < getval (src, soff + 5, 5); i++)
	    {
	      dest [doff + i] = getval (src, soff + 1, 4);
	      doff++;
	    }
	  soff += 10;
	}
      else
	{
	  dest [doff] = getval (src, soff + 1, 4);
	  doff++;
	  soff += 5;
	}
    };
}

void main (void)
{
  int errno;
  int i, j;
  unsigned char c;
  unsigned long len;

  puts ("Initialising camera\n\r");
  if ((errno = QC_init ()) == QC_SUCCESS)
    {
      puts ("Grabbing frame\n\r");
      if ((errno = QC_grabframe (buffer)) == QC_SUCCESS)
	{
	  for (i = 0; i < 120; i++)
	    {
	      for (j = 0; j < 160; j++)
		realhex (buffer [i * 160 + j]);
	      puts ("\n\r");
	    }
	  puts ("Compressing\n\r");
	  len = compress (buffer, 19200L, compressed, 19200L);
	  for (i = 0; i < 120; i++)
	    {
	      for (j = 0; j < 160 && (8 * (i * 160 + j) < len); j++)
		realhex (compressed [i * 160 + j]);
	      puts ("\n\r");
	    }
	  puts ("Decompressing\n\r");
	  decompress (compressed, len, result, 19200L);
	  for (i = 0; i < 120; i++)
	    {
	      for (j = 0; j < 160; j++)
		realhex (result [i * 160 + j]);
	      puts ("\n\r");
	    }
	}
    }
  if (errno != QC_SUCCESS)
    {
      switch (errno)
	{
	case QC_BAD_RESPONSE:
	  puts ("Bad response from camera\n\r");
	  break;
	case QC_TIMEOUT:
	  puts ("Camera timed out\n\r");
	  break;
	}
    }
}

void puts (char * string)
{
  while (*string)
    {
      mon_putc (*string);
      string++;
    }
}
