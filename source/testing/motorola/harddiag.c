#include <board.h>

void main (void)
{
  int i;
  unsigned char k;
  ringinfo ri;
  
  puts ("G7 hardware diagnostics\n\r");

  puts ("\n\rInitialising board...");
  boardinit ();
  puts (" done\n\r");

  puts ("\n\r ** Timer test\n\r");
  puts ("Waiting for 3 seconds...");
  starttimer (1010000);
  while (gettimer () > 10000);
  puts (" done\n\r");

  puts ("\n\r ** iButton reader test\n\r");
  puts ("Place an iButton in the reader...\n\r");
  i = 0;
  do
    {
      while (!checkring ());
      ri = getringinfo ();
      i++;
    }
  while (ri.crc == 0xFF);
  puts ("iButton read (");
  putn (i, 10, 2);
  puts (" times); product family ");
  putn (ri.product, 16, 2);
  puts ("; user ID ");
  for (i = 0; i < 6; i++)
    putn (ri.id [i], 16, 2);
  puts ("; CRC ");
  putn (ri.crc, 16, 2);
  puts ("\n\r");

  puts ("\n\r ** Keypad test\n\r");
  puts ("Press all the keys and the doorbell...\n\r");
  for (i = 0; i < 13; i++)
    {
      k = getkeypad ();
      starttimer (40000);
      buzzerled (BUZZER);
      while (gettimer () > 10000);
      buzzerled (ALLOFF);
      stoptimer ();
      puts ("Key pressed: ");
      if (k < 10)
	putn (k, 10, 1);
      else
	switch (k)
	  {
	  case KP_STAR:
	    puts ("*");
	    break;
	  case KP_HASH:
	    puts ("#");
	    break;
	  case KP_DOORBELL:
	    puts ("doorbell");
	    break;
	  case KP_UNDEF:
	    puts ("undefined (");
	    putn (k, 16, 2);
	    puts (")");
	    break;
	  }
      puts ("\n\r");
    }

  puts ("\n\r ** Beeper test\n\r");
  starttimer (10000000);
  buzzerled (BUZZER);
  while (gettimer () > 10000000 - 30000);
  buzzerled (ALLOFF);
  while (gettimer () > 10000000 - 40000);
  buzzerled (BUZZER);
  while (gettimer () > 10000000 - 70000);
  buzzerled (ALLOFF);
  stoptimer ();

  puts ("\n\r ** Door open test\n\r");
  for (i = 0; i < 5; i++)
    {
      starttimer (210000);
      buzzerled (LEDGREEN);
      while (gettimer () > 110000);
      buzzerled (ALLOFF);
      while (gettimer () > 10000);
      stoptimer ();
    }

  puts ("\n\r ** Entry denied test\n\r");
  for (i = 0; i < 5; i++)
    {
      starttimer (210000);
      buzzerled (LEDRED);
      while (gettimer () > 110000);
      buzzerled (ALLOFF);
      while (gettimer () > 10000);
      stoptimer ();
    }

  puts ("\n\r ** Door status test\n\r");
  puts ("Close the door, then press a key\n\r");
  mon_getc ();
  puts ("Door status is ");
  putn (getdoorstatus (), 10, 1);
  puts ("\n\r");
  puts ("Open the door, then press a key\n\r");
  mon_getc ();
  puts ("Door status is ");
  putn (getdoorstatus (), 10, 1);
  puts ("\n\r");

  puts ("\n\r ** Testing done\n\r");
}
