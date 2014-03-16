/* Quickcam test program  - outputs an XPM to the terminal */

/* $Id: test.c,v 1.4 1998/04/23 13:31:28 skk Exp skk $ */

#include <m68kboard.h>
#include <quickcam.h>

void puts (char *);

unsigned char buffer [19200];

void main (void)
{
  int errno;
  int i, j;
  unsigned char c;

  puts ("Initialising camera\n\r");
  if ((errno = QC_init ()) == QC_SUCCESS)
    {
      puts ("Grabbing frame\n\r");
      if ((errno = QC_grabframe (buffer)) == QC_SUCCESS)
	{
	  puts ("Received frame (XPM):\n\r");
	  puts ("/* XPM */\n\r");
	  puts ("static char * xpm = {\n\r");
	  puts ("\"160 120 16 1\", \"0 c #000000\", \"1 c #111111\", ");
	  puts ("\"2 c #222222\", \"3 c #333333\", \"4 c #444444\", ");
	  puts ("\"5 c #555555\", \"6 c #666666\", \"7 c #777777\", ");
	  puts ("\"8 c #888888\", \"9 c #999999\", \"A c #aaaaaa\", ");
	  puts ("\"B c #bbbbbb\", \"C c #cccccc\", \"D c #dddddd\", ");
	  puts ("\"E c #eeeeee\", \"F c #ffffff\",\n");
	  for (j = 0; j < 120; j++)
	    {
	      mon_putc ('\"');
	      for (i = 0; i < 160; i++)
		{
		  c = buffer [160 * j + i];
		  if (c != 0)
		    c = 16 - c;
		  if (c < 10)
		    mon_putc (c + '0');
		  else
		    mon_putc (c + 'A' - 10);
		}
	      puts ("\",\n");
	    }
	  puts ("} ;\n");
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
