#include <m68kboard.h>
#include <support.h>
#include <board.h>

volatile struct mc68230fm * spit = (volatile struct mc68230fm *)
     0x05000000;

/* Secondary 230 port layout:
     - port A: bits 7-6, DAC control; bit 5, iButton; bit 4, keypad
               control; bits 3-0, keypad data
     - port B: DAC/ADC data
     - port C: bits 1-0, buzzer and LEDs
*/

/* Initialise the board */
void boardinit (void)
{
  /* Set up timer - PC3/TOUT carries TOUT, CLK and the prescaler are
     used, and the timer is disabled */
  spit->tcr = 0x42;
  /* Set up port A  - top three lines are outputs */
  spit->ddra = 0xE0;
  /* Set up port C - bottom two lines are outputs */
  spit->ddrc = 0x03;
}

/* Timer functions - one timer tick occurs every 3.2us */

/* Start the timer */
void starttimer (int count)
{
  /* Disable the timer */
  spit->tcr = 0x42;
  /* Load the preloads */
  spit->cph = (count & 0x00FF0000) >> 16;
  spit->cpm = (count & 0x0000FF00) >> 8;
  spit->cpl = count & 0x000000FF;
  /* Enable the timer */
  spit->tcr = 0x43;
}

/* Stop the timer */
int stoptimer (void)
{
  /* Disable the timer */
  spit->tcr = 0x42;
  /* Return the current count */
  return gettimer ();
}

/* Get the current timer value */
inline int gettimer (void)
{
  /* Return the current count */
  return (((int) spit->crh << 16) + ((int) spit->crm << 8) + (int)
	  spit->crl);
}

/* Keycode translation table */
unsigned char kptable[] = { 1, 2, 3, KP_DOORBELL,
			    4, 5, 6, KP_UNDEF,
			    7, 8, 9, KP_UNDEF,
			    KP_STAR, 0, KP_HASH, KP_UNDEF };

/* Get a keypad digit */
unsigned char getkeypad (void)
{
  /* Initialise port A direction */
  spit->ddra = 0xC0;

  /* Wait for a key to be pressed */
  while (!(spit->ara & 0x10));

  /* Wait for the key to be released */
  while (spit->ara & 0x10);

  /* Get the key, convert it and return */
  return kptable [spit->ara & 0x0F];
}

/* Check whether a key is pressed */
int checkkeypad (void)
{
  /* Set port A direction */
  spit->ddra = 0xC0;

  /* Check and return */
  return (spit->ara & 0x10);
}

/* Activate buzzer and LEDs, according to above */
int buzzerled (int what)
{
  switch (what)
    {
    case ALLOFF:
      /* Everything off */
      spit->drc = 0xFF;
      break;
    case BUZZER:
      /* Buzzer */
      spit->drc = 0xFD;
      break;
    case LEDGREEN:
      /* Green LED */
      spit->drc = 0xFC;
      break;
    case LEDRED:
      /* Red LED */
      spit->drc = 0xFE;
      break;
    }

  return 0;
}

/* Return door status */
int getdoorstatus (void)
{
  int olddrc;
  olddrc = spit->drc;
  spit->drc = olddrc;
  return ((olddrc & 0x40) >> 6);
}

/* Open the door */
int opendoor (void)
{
  buzzerled (LEDGREEN);
  return 0;
}

/* Closes the door */
int closedoor (void)
{
  buzzerled (ALLOFF);
  return 0;
}

/* LED codes - not all tested, alphabetic ones are probably wrong ('b' 
   certainly is :-) */

#define LEDCODES 37

unsigned char ledcodes [] =
{
  '0', 0xC0,
  '1', 0xF9,
  '2', 0xA4,
  '3', 0xB0,
  '4', 0x99,
  '5', 0x92,
  '6', 0x82,
  '7', 0xF8,
  '8', 0x80,
  '9', 0x98,
  ' ', 0xFF,
  'a', 0xDF,
  'b', 0xFC,
  'c', 0xD8,
  'd', 0xDE,
  'e', 0xFB,
  'f', 0xF1,
  'g', 0xEF,
  'h', 0xF4,
  'i', 0x90,
  'j', 0x8C,
  'k', 0xF6,
  'l', 0xB0,
  'm', 0xD4,
  'n', 0xD4,
  'o', 0xDC,
  'p', 0xF3,
  'q', 0xE7,
  'r', 0xD0,
  's', 0xED,
  't', 0xF8,
  'u', 0x9C,
  'v', 0x9C,
  'w', 0x9C,
  'x', 0xF6,
  'y', 0xEE,
  'z', 0xA4
};

/* Display the given character on the IDP's LED */
void leddisplay (unsigned char c)
{
  int i;
  unsigned char * ledreg = (unsigned char *) 0x00D00003;

#ifdef DEBUG
  puts ("LED: ");
  mon_putc (c);
  puts ("\n\r");
#endif
  /* Search for the correct code to display */
  for (i = 0; i < LEDCODES; i++)
    if (ledcodes [2 * i] == c)
      *ledreg = ledcodes [2 * i + 1];
}

/* Note that the CRC-8 functionality isn't yet functional - the CRC-8
   code is in theory correct (it's adapted from standard CRC-8 code),
   but the order in which to perform the calculations for the iButton
   isn't certain. */

/* Ring CRC-8 */
unsigned char __ringcrc;

/* Check whether an iButton is present */
int checkring (void)
{
  int t, i, r;

  for (i = 0; i < 50; i++)
    {
      /* Set to output, expecting high */
      spit->dra = 0xE0;
      spit->ddra = 0xE0;
      starttimer (10000000);
      /* Pull low */
      spit->dra = 0xC0;
      /* Start the timer and wait for 480 us */
      while (gettimer () > 10000000 - 150);
      /* Set to input */
      spit->ddra = 0xC0;
      /* Get current time */
      t = gettimer ();
      /* Wait for high */
      while (!(spit->ara & 0x20));
      /* Wait for low or 960 us */
      while ((spit->ara & 0x20) && ((t - gettimer ()) < 300));
      /* If low, found ring */
      if (!(spit->ara & 0x20))
	r = 1;
      /* Otherwise, no ring */
      else
	r = 0;
      while ((t - gettimer ()) < 300);
      stoptimer ();
      if (r)
	return 1;
    }
  return 0;
}

/* Write a 1 */
void ringwriteone (void)
{
  /* Set to output, expecting high */
  spit->dra = 0xE0;
  spit->ddra = 0xE0;
  starttimer (10000000);
  /* Pull low for 10 us - this works... */
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  /* Set to input */
  spit->ddra = 0xC0;
  /* Wait for a total of 120 us */
  while (gettimer () > 10000000 - 40);
}

/* Write a 0 */
void ringwritezero (void)
{
  /* Set to output, expecting high */
  spit->dra = 0xE0;
  spit->ddra = 0xE0;
  starttimer (10000000);
  /* Pull low - again, this works... */
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  /* Set to input */
  spit->ddra = 0xC0;
  /* Wait for a total of 120 us */
  while (gettimer () > 10000000 - 40);
}

/* Write a byte */
void ringwritebyte (unsigned char c)
{
  int i;
  for (i = 0; i < 8; i++)
    {
      if (c & (1 << i))
	ringwriteone ();
      else
	ringwritezero ();
    }
}

/* Read a bit */
int ringreadbit (int updcrc)
{
  int t, r;
  
  /* Set to output, expecting high */
  spit->dra = 0xE0;
  spit->ddra = 0xE0;
  starttimer (10000000);
  /* Pull low */
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  spit->dra = 0xC0;
  /* Set to input */
  spit->ddra = 0xC0;
  /* If high, 1 */
  if (spit->ara & 0x20)
    r = 1;
  else
    r = 0;
  /* Wait for a total of 120 us */
  while (gettimer () > 10000000 - 40);
  stoptimer ();
  /* Non-functional CRC code */
  if (updcrc)
    {
      t = __ringcrc & 0x80;
      __ringcrc = (__ringcrc << 1) + r;
      if (t)
	__ringcrc ^= 0x18;
    }
  return r;
}

/* Read a byte */
unsigned char ringreadbyte (int updcrc)
{
  unsigned char c;
  int i;

  c = 0;
  for (i = 0; i < 8; i++)
    if (ringreadbit (updcrc))
      c |= (1 << i);
  return c;
}

/* CRC-8 calculations - calculated backwards to cope with the
   iButton's LSB to MSB operation; doesn't work */
unsigned char crc8 (unsigned char crc, unsigned char value)
{
  int shiftreg, shiftcrc, i;
  shiftreg = value;
  shiftcrc = crc;
  for (i = 0; i < 8; i++)
    {
      if (value & (1 << i))
	shiftcrc = (shiftcrc << 1) | 1;
      else
	shiftcrc = (shiftcrc << 1) & ~1;
      if (shiftcrc & 0x00000100)
	shiftcrc ^= 0x00000031;
    }
  return (unsigned char) (shiftcrc & 0x000000FF);
}

/* Retrieve the ring information; will check up to five times to cope
   with read errors (CRC == 0xFF) */
ringinfo getringinfo (void)
{
  ringinfo ri;
  int i, j;
  unsigned char buf [8];

  /* If there is a ring (this also resets it) ... */
  if (checkring ())
    {
      j = 0;
      ri.crc = 0xFF;

      /* Check up to five times */
      while (j < 5 && ri.crc == 0xFF)
	{
	  /* Send read command */
	  ringwritebyte (0x0F);
	  /* Get product code */
	  ri.product = ringreadbyte (1);
	  /* Get iButton ID */
	  for (i = 5; i >= 0; i--)
	    ri.id [i] = ringreadbyte (1);
	  /* Get CRC */
	  ri.crc = ringreadbyte (0);
	  j++;
	};
    }
  return ri;
}
