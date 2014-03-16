#include <m68kboard.h>
#include <quickcam.h>
#include <support.h>

volatile struct mc68681wfm * uartw = 0x00B00000;
volatile struct mc68681rfm * uartr = 0x00B00000;
volatile struct mc68230fm * pit = 0x00C00000;
volatile void ** irqvect = 0x00000000;

unsigned char buffer [19200];

#define SERIALINSIZE 32768
#define SERIALOUTSIZE 65536
volatile unsigned char serialin [SERIALINSIZE];
volatile unsigned int serialinwriteidx, serialinreadidx, serialinlen;
volatile unsigned char serialout [SERIALOUTSIZE];
volatile unsigned int serialoutwriteidx, serialoutreadidx,
  serialoutlen;

extern void serialirq (void);

void realhex (unsigned char c)
{
  if (c > 9)
    mon_putc ('A' - 10 + c);
  else
    mon_putc ('0' + c);
}

void hex (unsigned char c)
{
  realhex ((c & 0xF0) >> 4);
  realhex (c & 0x0F);
}

/* This function is called by the serial IRQ handler. It has to
   retrieve or send any data, and manage the buffers.

   Note that to get round the edge- vs level-triggering problem (the
   UART and CPU assume level, the PIT uses edge), we clear the
   interrupt mask register to stop IRQ activity then restart it.
*/
void serialevent (void)
{
  int i;
  unsigned char srbs;

  /* Retrieve current value of channel B's status register */
  srbs = uartr->srb;
  /* Clear all IRQ activity from the UART */
  uartw->imr = 0;
  /* Check if we can transmit */
  if ((srbs & 0x04) && serialoutlen > 0)
    {
      uartw->tbb = serialout [serialoutreadidx];
      serialoutreadidx = (serialoutreadidx + 1) % SERIALOUTSIZE;
      serialoutlen--;
    }
  /* Check if there's a byte waiting to be received */
  while ((srbs & 0x01) && serialinlen < SERIALINSIZE)
    {
      serialin [serialinwriteidx] = uartr->rbb;
      serialinwriteidx = (serialinwriteidx + 1) % SERIALINSIZE;
      serialinlen++;
      srbs = uartr->rbb;
    }
  /* If we needn't transmit, disable IRQs on TxRDY */
  if (serialoutlen == 0)
    uartw->imr = 0x20;
  else
    uartw->imr = 0x30;
}

void pitinit (void)
{
  /* Store vector */
  irqvect [66] = serialirq;
  /* Set mode 0 */
  pit->gcr = 0;
  /* Setup PIRQ and PIACK */
  pit->srr = 0x18;
  /* Submode 1x, H3 interrupt enabled */
  pit->crb = 0x82;
  /* Setup PIVR */
  pit->pivr = 64;
  /* Turn on the ports (H1-H4) */
  pit->gcr = 0x30;
}

void serialinit (void)
{
  puts ("Initialising data buffers...");
  serialinwriteidx = serialinreadidx = serialinlen = serialoutwriteidx
    = serialoutreadidx = serialoutlen = 0;
  puts (" done\n\r");
  pitinit ();
  puts ("Initialising serial channel B...");
  /* Reset MRB pointer */
  uartw->crb = 0x10;
  /* Setup even parity, 8-bits per character, IRQ on RxRDY */
  uartw->mr1b = 0x83;
  /* One stop bit */
  uartw->mr2b = 0x07;
  /* 38.4kbps both ways */
  uartw->csrb = 0xCC;
  /* Interrupts on RxRDY and TxRDY */
  uartw->imr = 0x30;
  /* Reset receiver, transmitter and error status */
  uartw->crb = 0x20;
  uartw->crb = 0x30;
  uartw->crb = 0x40;
  /* Enable transmitter and receiver */
  uartw->crb = 0x05;
  puts (" done.\n\r");
  /* Enable interrupts */
  puts ("Enabling interrupts...");
  asm ("andi #0xF8FF, sr");
  puts (" done.\n\r");
}

void serialtransmit (unsigned char * buf, int length)
{
  int i;

  for (i = 0; i < length; i++)
    {
      while (serialoutlen == SERIALOUTSIZE);
      serialout [serialoutwriteidx] = buf [i];
      serialoutwriteidx = (serialoutwriteidx + 1) % SERIALOUTSIZE;
      serialoutlen++;
    }
  serialevent ();
}

unsigned char serialreceive (void)
{
  unsigned char c;
  while (serialinlen == 0);
  c = serialin [serialinreadidx];
  serialinreadidx = (serialinreadidx + 1) % SERIALINSIZE;
  serialinlen--;
  return c;
}

void main (void)
{
  unsigned char c;
  int i;

  serialinit ();

  puts ("Grabbing frame\n\r");
  QC_init ();
  QC_grabframe (buffer);
  puts ("Transmitting\n\r");
  pitinit ();
  serialtransmit (buffer, 19200);
  while (serialoutlen > 0);

  puts ("Waiting for transmit to finish...");
  while (serialoutlen > 0);
  puts (" done\n\r");
}
