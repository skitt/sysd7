#include <m68kboard.h>
#include <support.h>
#include <board.h>

volatile struct mc68230fm * spit = 0x05000000;

/* Initialise the board */
void boardinit (void)
{
  /* Initialise the counter preload */
  spit->cph = 0;
  spit->cpm = 0;
  spit->cpl = 0x10;
  /* Set up timer - PC3/TOUT carries TOUT, CLK and the prescaler are
     used, and the timer is enabled */
  spit->tcr = 0x43;
  /* Set up port A */
  spit->ddra = 0xE0;
  spit->dra = 0xE0;
}

/* Send a sample to the DAC */
void setsample (unsigned char sample)
{
  /* Initialise DAC */
  nop ();
  spit->dra = 0xE0;
  nop ();
  spit->ddrb = 0xFF;

  /* Send the sample */
  nop ();
  spit->drb = sample;

  /* Set the DAC */
  nop ();
  spit->dra = 0xA0;
}

/* Get a sample from the ADC */
unsigned char getsample (void)
{
  /* Initialise ADC */
  nop ();
  spit->ddrb = 0;
  nop ();
  spit->dra = 0x60;

  /* Return the sample on the bus */
  nop ();
  return spit->arb;
}
