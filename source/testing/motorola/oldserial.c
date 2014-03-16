#include <m68kboard.h>
#include <support.h>
#include <quickcam.h>

volatile struct mc68681wfm * uartw = 0x00B00000;
volatile struct mc68681rfm * uartr = 0x00B00000;

unsigned char buffer [19200];

void serialinit (void)
{
  puts ("Initialising serial channel B...\n\r");
  /* Reset MRB pointer */
  puts ("Resetting MRB pointer.\n\r");
  uartw->crb = 0x10;
  /* Setup even parity, 8-bits per character, IRQ on RxRDY */
  puts ("Setting even parity, 8bpc, IRQ on RxRDY\n\r");
  uartw->mr1b = 0x03;
  /* One stop bit */
  puts ("Setting one stop bit\n\r");
  uartw->mr2b = 0x07;
  /* 38.4kbps both ways */
  puts ("Setting 38400bps\n\r");
  uartw->csrb = 0xCC;
  /* Reset receiver, transmitter and error status */
  puts ("Resetting receiver\n\r");
  uartw->crb = 0x20;
  puts ("Resetting transmitter\n\r");
  uartw->crb = 0x30;
  puts ("Resetting error status\n\r");
  uartw->crb = 0x40;
  /* Enable transmitter and receiver */
  puts ("Enabling transmitter and receiver\n\r");
  uartw->crb = 0x05;
  puts ("Channel B initialisation done.\n\r");
}

void serialtransmit (unsigned char value)
{
  /* Wait for TxRDY */
  while (!(uartr->srb & 0x04));
  /* Transmit character */
  uartw->tbb = value;
}

unsigned char serialreceive (void)
{
  /* Wait for RxRDY */
  while (!(uartr->srb & 0x01));
  /* Receive character */
  return uartr->rbb;
}

void main (void)
{
  int i;

  serialinit ();

  puts ("Waiting for a character... ");
  mon_putc (serialreceive ());
  puts ("\n\r");
  puts ("Initialising camera...");
  QC_init ();
  puts (" done.\n\rGrabbing frame...");
  QC_grabframe (buffer);
  puts (" done.\n\r");
  puts ("Transmitting frame...");
  for (i = 0; i < 19200; i++)
    serialtransmit (buffer [i] + '0');
  puts (" done.\n\r");
}
