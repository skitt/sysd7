#include <m68kboard.h>
#include <quickcam.h>
#include <support.h>
#include <serial.h>

/* Interrupt-driven serial code for the Motorola IDP board */

/* Hardware access pointers */
volatile struct mc68681wfm * uartw = 0x00B00000;
volatile struct mc68681rfm * uartr = 0x00B00000;
volatile struct mc68230fm * bpit = 0x00C00000;
volatile void ** irqvect = 0x00000000;

void realhex (unsigned char c)
{
  if (c > 9)
    mon_putc ('A' - 10 + c);
  else
    mon_putc ('0' + c);
}

void hexchar (unsigned char c)
{
  realhex ((c & 0xF0) >> 4);
  realhex (c & 0x0F);
}

void hexint (unsigned int v)
{
  hexchar ((v & 0xFF000000) >> 24);
  hexchar ((v & 0x00FF0000) >> 16);
  hexchar ((v & 0x0000FF00) >> 8);
  hexchar (v & 0x000000FF);
}

void number (unsigned int v)
{
  int i;
  for (i = 1000000; i > 0; i /= 10)
    {
      mon_putc (v / i + '0');
      v %= i;
    }
}

#define NULL 0L

/* Data buffers */
#define SERIALINSIZE 32768
#define SERIALOUTSIZE 65536
volatile unsigned char serialin [SERIALINSIZE];
volatile unsigned int serialinidx;
volatile unsigned char serialout [SERIALOUTSIZE];
volatile unsigned int serialoutwriteidx, serialoutreadidx,
  serialoutlen;

/* Packet being constructed */
packet pkt;
unsigned char pktbuf [32768];

/* Packet handlers */
packethandler handlers [256];

/* Input state flag */
enum { HEADER, DATA };
int serialinstate;

/* Serial activity flag */
int serialactive;

/* Serial IRQ handler, defined in serialirq.S */
extern void serialirq (void);

/* Default data handler */
void defaulthandler (packet * pkt)
{
}

/* This function is called by the serial IRQ handler. It has to
   retrieve or send any data, and manage the buffers.

   Note that to get round the edge- vs level-triggering problem (the
   UART and CPU assume level, the PIT uses edge), we clear the
   interrupt mask register to stop IRQ activity then restart it.
*/
void serialevent (void)
{
  unsigned char srbs;
  int i;

  if (!serialactive)
    return;

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
  if ((srbs & 0x01) && serialinidx < SERIALINSIZE)
    {
      serialin [serialinidx] = uartr->rbb;
      serialinidx++;
    }
  /* If we've got the header, store it and move to data */
  if (serialinstate == HEADER && serialinidx > 4)
    {
      pkt.datatype = serialin [0];
      pkt.length = 0;
      for (i = 1; i < 5; i++)
	pkt.length = (pkt.length << 8) + serialin [i];
      for (i = 5; i < serialinidx; i++)
	serialin [i - 5] = serialin [i];
      serialinidx -= 5;
      serialinstate = DATA;
    }
  /* If we've got the data, handle it and move back to header */
  if (serialinstate == DATA && serialinidx >= pkt.length)
    {
      for (i = 0; i < pkt.length; i++)
	pktbuf [i] = serialin [i];
      for (i = pkt.length; i < serialinidx; i++)
	serialin [i - pkt.length] = serialin [i];
      serialinidx -= pkt.length;
      pkt.data = pktbuf;
      if (handlers [pkt.datatype] != NULL)
	handlers [pkt.datatype] (&pkt);
      serialinstate = HEADER;
    }
  /* If we needn't transmit, disable IRQs on TxRDY */
  if (serialoutlen == 0)
    uartw->imr = 0x20;
  else
    uartw->imr = 0x30;
}

/* Initialises the board's PIT */
void pitinit (void)
{
  /* Store vector */
  irqvect [66] = serialirq;
  /* Set mode 0 */
  bpit->gcr = 0;
  /* Setup PIRQ and PIACK */
  bpit->srr = 0x18;
  /* Submode 1x, H3 interrupt enabled */
  bpit->crb = 0x82;
  /* Setup PIVR */
  bpit->pivr = 64;
  /* Turn on the ports (H1-H4) */
  bpit->gcr = 0x30;
}

/* Initialises the serial */
int serialinit (void)
{
  int i;
  puts ("Testing buffers...\n\r");
  puts ("  serialin");
  for (i = 0; i < SERIALINSIZE; i++)
    serialin [i] = 0;
  puts (" OK\n\r  serialout");
  for (i = 0; i < SERIALOUTSIZE; i++)
    serialout [i] = 0;
  puts (" OK\n\r  pktbuf");
  for (i = 0; i < 32768; i++)
    pktbuf [i] = 0;
  puts (" OK\n\r");
  /* Initialise variables */
  serialinidx = serialoutwriteidx = serialoutreadidx = serialoutlen =
    0;
  serialinstate = HEADER;
  /* Initialise PIT */
  pitinit ();
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
  /* Activate serial */
  serialactive = 1;
  /* Reset receiver, transmitter and error status */
  uartw->crb = 0x20;
  uartw->crb = 0x30;
  uartw->crb = 0x40;
  /* Enable transmitter and receiver */
  uartw->crb = 0x05;
  /* Enable interrupts */
  asm ("andi #0xF8FF, sr");

  return 0;
}

/* Shuts the serial down */
int serialclose (void)
{
  /* Wait for serial communications to cease */
  while (serialinidx > 0 || serialoutlen > 0);
  /* Stop UART IRQ */
  uartw->imr = 0;
  /* Stop serial activity */
  serialactive = 0;
}

/* Sends a packet of data */
int senddata (packet * pkt)
{
  int i;

  hexint ((unsigned int) pkt);
  mon_putc (' ');
  hexint ((unsigned int) pkt->data);
  puts ("\n\r");
  puts ("Sending packet, size ");
  number (pkt->length);
  puts ("\n\r");
  while (serialoutlen == SERIALOUTSIZE);
  serialout [serialoutwriteidx] = pkt->datatype;
  serialoutwriteidx = (serialoutwriteidx + 1) % SERIALOUTSIZE;
  serialoutlen++;
  for (i = 24; i >= 0; i -= 8)
    {
      while (serialoutlen == SERIALOUTSIZE);
      serialout [serialoutwriteidx] = (pkt->length & (0x000000FF <<
						      i)) >> i;
      serialoutwriteidx = (serialoutwriteidx + 1) % SERIALOUTSIZE;
      serialoutlen++;
    }
  for (i = 0; i < pkt->length; i++)
    {
      while (serialoutlen == SERIALOUTSIZE);
      serialout [serialoutwriteidx] = pkt->data [i];
      serialoutwriteidx = (serialoutwriteidx + 1) % SERIALOUTSIZE;
      serialoutlen++;
    }
  serialevent ();

  return 0;
}

/* Register a handler */
int registerhandler (unsigned char datatype, packethandler handler)
{
  handlers [datatype] = handler;

  return 0;
}

/* Unregister a handler */
int unregisterhandler (unsigned char datatype)
{
  if (datatype > 1)
    handlers [datatype] = NULL;
  else
    handlers [datatype] = defaulthandler;
}

/* Grab a frame, handling the conflict between camera and serial */
int grabframe (unsigned char * buffer)
{
  int qcerr;
  /* Shut the serial down */
  puts ("Closing serial\n\r");
  serialclose ();
  /* Initialise the camera */
  puts ("Initialising camera\n\r");
  if ((qcerr = QC_init ()) == QC_SUCCESS)
    {
      /* Grab the frame */
      puts ("Grabbing frame\n\r");
      qcerr = QC_grabframe (buffer);
    }
  /* Initialise the serial */
  puts ("Initialising serial\n\r");
  serialinit ();

  return qcerr;
}
