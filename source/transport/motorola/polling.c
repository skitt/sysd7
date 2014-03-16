/* Motorola serial code, polling version
   Stephen Kitt
*/

#include <m68kboard.h>
#include <serial.h>

/* UART access */
volatile struct mc68681wfm * uartw = (struct mc68681wfm *) 0x00B00000; 
volatile struct mc68681rfm * uartr = (struct mc68681rfm *) 0x00B00000;

/* Flags identifying need to acknowledge packet, and packet being sent 
 */
int __ack, __sendingpacket;

/* Initialise the DUART channel B, for polled serial */
int serialinit (void)
{
  __ack = 0;
  __sendingpacket = 0;
  
  /* Reset ACR */
  uartw->acr = 0;
  /* Reset MRB pointer */
  uartw->crb = 0x10;
  /* Setup no parity, 8-bits per character, IRQ on RxRDY */
  uartw->mr1b = 0x33;
  /* One stop bit */
  uartw->mr2b = 0x07;
  /* 38.4kbps both ways */
  uartw->csrb = 0xCC;
  /* Reset receiver, transmitter and error status */
  uartw->crb = 0x20;
  uartw->crb = 0x30;
  uartw->crb = 0x40;
  /* Enable transmitter and receiver */
  uartw->crb = 0x05;

  return 0;
}

/* Shut the serial down */
int serialclose (void)
{
  return 0;
}

/* Ping the PC */
int ping (void)
{
  int sr;

  /* Prepare packet */
  packet pkt;
  pkt.datatype = 1;
  pkt.length = 0;
  pkt.data = 0;

  /* Send it */
#ifdef DEBUG
  puts ("Serial: sending ping packet, type 1, length 0, data 0\n\r");
#endif
  senddata (&pkt);

  /* Wait for response; swallowing everything is obviously not
     ideal... */
#ifdef DEBUG
  puts ("Serial: waiting for response\n\r");
#endif
  do
    sr = serialreceive ();
  while (sr != 2 && sr != -1);

  if (sr == 2)
    {
#ifdef DEBUG
      puts ("Serial: response received\n\r");
#endif
      return 0;
    }
  else
    {
#ifdef DEBUG
      puts ("Serial: ping timed out\n\r");
#endif
      return -1;
    }
}

/* Acknowledge a ping and wait for new state */
int pingack (void)
{
  int i;
  
#ifdef DEBUG
  puts ("Serial: ping received, noting need to acknowledge\n\r");
#endif
  /* If we're currently sending a packet, we should wait until we've
     finished */
  if (__sendingpacket)
    {
      __ack = 1;
      return 0;
    }
  /* Otherwise, send it straight away */
  else
    {
#ifdef DEBUG
      puts ("Serial: acknowledging ping\n\r");
#endif
      serialtransmit (2);
      for (i = 0; i < 4; i++)
	serialtransmit (0);
      __ack = 0;
#ifdef DEBUG
      puts ("Serial: acknowledged, waiting for new state\n\r");
#endif
      return serialreceive ();
    }
}

/* Transmits a single byte of data */
int serialtransmit (unsigned char value)
{
  int r = 0;
  /* Wait for TxRDY; if RxRDY and ping happens, get byte and return
     corresponding value */
  while (!(uartr->srb & 0x04));
  /* Transmit character */
  uartw->tbb = value;
  if ((uartr->srb & 0x01) && uartr->rbb == 1)
    r = pingack ();

  return r;
}

/* Receives a single byte of data */
int serialreceive (void)
{
  unsigned char r;
  /* Wait for RxRDY */
  while (!(uartr->srb & 0x01));
  /* Receive character */
  r = uartr->rbb;
  return r;
}

/* Sends a packet of data; doesn't return until the packet's been
   transmitted */
int senddata (packet * out)
{
  int i;

  /* Say we're sending a packet */
  __sendingpacket = 1;

  /* Send the datatype */
  serialtransmit (out->datatype);
#ifdef DEBUG
  puts ("Serial: sending packet, length ");
  putn (out->length, 10, 6);
  puts ("\n\r");
#endif
  /* Send the length */
  for (i = 24; i >= 0; i -= 8)
    serialtransmit ((out->length >> i) & 0xFF);
  /* Send the data */
  for (i = 0; i < out->length; i++)
    serialtransmit (out->data [i]);

  /* We're no longer sending a packet */
  __sendingpacket = 0;

  /* If we need to acknowledge a ping, do so, and wait for the new
     state */
  if (__ack)
    {
#ifdef DEBUG
      puts ("Serial: acknowledging ping\n\r");
#endif
      serialtransmit (2);
      for (i = 0; i < 4; i++)
	serialtransmit (0);
      __ack = 0;
#ifdef DEBUG
      puts ("Serial: acknowledged, waiting for new state\n\r");
#endif
      return serialreceive ();
    }
  else
    return 0;
}

/* Storage for the packet */
packet pkt;
unsigned char pktbuf [32768];

/* Received a packet; everything must be copied before calling
   receivepacket again */
packet * receivepacket (void)
{
  int i;

#ifdef DEBUG
  puts ("Serial: receiving a packet");
#endif

  /* Get the datatype */
  pkt.datatype = serialreceive ();
#ifdef DEBUG
  puts ("; datatype ");
  putn (pkt.datatype, 10, 3);
#endif

  /* Get the length */
  pkt.length = 0;
  for (i = 0; i < 4; i++)
    pkt.length = (pkt.length << 8) + serialreceive ();
#ifdef DEBUG
  puts ("; length ");
  putn (pkt.length, 10, 3);
#endif

  /* Get the data */
  for (i = 0; i < pkt.length; i++)
    pktbuf [i] = serialreceive ();
#ifdef DEBUG
  puts ("; done\n\r");
#endif

  /* Store and return */
  pkt.data = pktbuf;
  return &pkt;
}
