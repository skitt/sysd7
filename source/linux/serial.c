/* Linux interrupt-driven serial code
   Stephen Kitt
*/

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#include <serial.h>

/* Serial file descriptor */
int serialfd;

/* Packet handling flag */
int packets;

/* Packet handlers */
packethandler handlers [256];

/* Serial input buffer information - size, buffer, length, current
   read and write indices, lock */
#define SERIALINSIZE 32768
unsigned char inbuffer [SERIALINSIZE];
volatile unsigned int inbl, inbr, inbw;
volatile inblock;

/* Temporary packet storage */
packet pkt;
int pktlen;
unsigned char pktbuf [SERIALINSIZE];

/* Ping indicator */
int __ping;

/* Default handler - handles pings */
void defaulthandler (packet * in)
{
#ifdef DEBUG
  printf ("Default handler called, data type %d\n", (int)
	  in->datatype);
#endif
  /* If we receive a ping, acknowledge it immediately */
  if (in->datatype == 1)
    {
#ifdef DEBUG
      puts ("Received ping, acknowledging");
#endif
      serialtransmit (2);
#ifdef DEBUG
      puts ("Ping acknowledged");
#endif
    }
  /* If we receive a ping acknowledgement, store that fact */
  if (in->datatype == 2)
    {
#ifdef DEBUG
      puts ("Serial: received ping acknowledgement");
#endif
      __ping = 1;
    }
}

/* Serial event handler */
void serialevent (void)
{
  fd_set rfds;
  struct timeval tv;
  int i;

  /* If the input buffer is currently locked, indicate we need it
     eventually, otherwise lock it */
  if (inblock == 1)
    {
      inblock = 2;
      return;
    }
  else
    inblock == 1;
  
  /* Use select to determine if we can read */
  FD_ZERO (&rfds);
  FD_SET (serialfd, &rfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select (serialfd + 1, &rfds, NULL, NULL, &tv);

  /* If we can read... */
  if (FD_ISSET (serialfd, &rfds))
    {
      /* Read as many bytes as we can */
      i = read (serialfd, &(inbuffer [inbw]), SERIALINSIZE - inbw);

      /* If this didn't fail, update the write pointer and length */
      if (i != -1)
	{
	  inbw = (inbw + i) % SERIALINSIZE;
	  inbl += i;
	}

      /* If we're handling packets... */
      if (packets)
	{
	  /* If we've got more than five characters, we've got a packet */
	  if (inbl >= 5 && pktlen == -1)
	    {
#ifdef DEBUG
	      printf ("Serial: ");
	      for (i = inbr; i < inbr + 5; i++)
		printf ("%2.2X ", inbuffer [i % SERIALINSIZE]);
	      printf ("\n\r");
	      printf ("Serial: inbl = %d, inbr = %d, inbw = %d\n",
		      inbl, inbr, inbw);
#endif
	      /* Store the datatype */
	      pkt.datatype = inbuffer [inbr];
#ifdef DEBUG
	      printf ("Serial: read datatype: %d\n", (int) pkt.datatype);
	      printf ("Serial: reading length: ");
#endif
	      /* Clear the packet length, and read it in */
	      pktlen = 0;
	      for (i = inbr + 1; i < inbr + 5; i++)
		{
#ifdef DEBUG
		  printf ("%2.2X ", inbuffer [i % SERIALINSIZE]);
#endif
		  pktlen = (pktlen << 8) + inbuffer [i % SERIALINSIZE];
		}
#ifdef DEBUG
	      printf ("= %d\n", pktlen);
#endif	  
	    }
	  /* If we've got at least as many bytes as the packet length
	     plus five, we've got a packet */
	  if (inbl >= pktlen + 5 && pktlen != -1)
	    {
	      /* Update the read pointer and length */
	      inbr = (inbr + 5) % SERIALINSIZE;
	      inbl -= 5;
#ifdef DEBUG
	      printf
("Serial: read data; inbl = %d, inbr = %d, inbw = %d, pktlen = %d\n",
 inbl, inbr, inbw, pktlen);
#endif
	      /* Copy the data into the temporary packet */
	      for (i = 0; i < pktlen; i++)
		{
		  pktbuf [i] = inbuffer [(inbr + i) % SERIALINSIZE];
		}
	      
	      /* Update the read pointer and length */
	      inbr = (inbr + pktlen) % SERIALINSIZE;
	      inbl -= pktlen;
	      
	      /* Store the packet information */
	      pkt.data = pktbuf;
	      pkt.length = pktlen;
	      
	      /* Call the handler */
	      if (handlers [pkt.datatype] != NULL)
		{
#ifdef DEBUG
		  puts ("Serial: calling data handler");
#endif
		  handlers [pkt.datatype] (&pkt);
		}
	      
	      /* Clear out the packet length */
	      pktlen = -1;
	      
	      /* If there's still something left, we've either made a
		 mistake or we've got two packets close together... */
	      if (inbl > 0)
		puts ("Serial: possible problem");
#ifdef DEBUG
	      printf ("Serial: inbl = %d, inbr = %d, pktlen = %d\n",
		      inbl, inbr, pktlen);
#endif
	    }
	}
    }

  /* Release the lock */
  inblock = 0;
}

/* IOSIG handler */
void iosighandler (int arg)
{
  serialevent ();
}

/* Serial initialisation function */
int serialinit (void)
{
  int i;
  struct termios termios;
  struct sigaction sa;

  /* Clear the ping, pointers, lock and packet length */
  __ping = 0;
  inbl = inbr = inbw = 0;
  inblock = 0;
  pktlen = -1;

  /* Install our signal handler */
  sa.sa_handler = iosighandler;
  sa.sa_mask = 0;
  sa.sa_flags = 0;
  sa.sa_restorer = NULL;
  sigaction (SIGIO, &sa, NULL);

  /* Open the tty */
  serialfd = open ("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (serialfd == -1)
    return -1;
  
  /* Change the tty so we get SIGIO */
  fcntl (serialfd, F_SETOWN, getpid ());
  fcntl (serialfd, F_SETFL, FASYNC);

  /* Setup 38400bps, 8 bit characters, no modem stuff, ignore parity
   */
  termios.c_cflag = B38400 | CS8 | CREAD | CLOCAL;
  termios.c_iflag = IGNPAR;
  termios.c_oflag = 0;
  termios.c_lflag = 0;

  /* Totally non-blocking I/O */
  termios.c_cc[VTIME] = 0;
  termios.c_cc[VMIN] = 0;

  /* Flush */
  tcflush (serialfd, TCIFLUSH);
  tcflush (serialfd, TCOFLUSH);

  /* Store attributes */
  tcsetattr (serialfd, TCSANOW, &termios);

  /* Initialise the handlers */
  handlers [0] = defaulthandler;
  handlers [1] = defaulthandler;
  handlers [2] = defaulthandler;
  for (i = 3; i < 256; i++)
    handlers [i] = NULL;

  /* Disable packets */
  packets = 0;

  /* Success */
  return 0;
}

/* Close the serial */
int serialclose (void)
{
  /* Wait for transactions to finish */
  while (inbl > 0);

  /* Release tty */
  close (serialfd);

  /* Success */
  return 0;
}

/* Send a data packet */
int senddata (packet * pkt)
{
  int i;
  unsigned char c;

#ifdef DEBUG
  printf ("Serial: sending a packet, type %d, length %d\n",
	  pkt->datatype, pkt->length);
#endif
  /* Write the datatype */
  write (serialfd, &(pkt->datatype), 1);

  /* Write the length */
  for (i = 24; i >= 0; i -= 8)
    {
      c = (pkt->length & (0x000000FF << i)) >> i;
      write (serialfd, &c, 1);
    }

  /* Write the data */
  for (i = 0; i < pkt->length; i++)
    write (serialfd, &(pkt->data [i]), 1);

#ifdef DEBUG
  puts ("Serial: packet sent");
#endif

  return 0;
}

/* Send a single character */
int serialtransmit (unsigned char c)
{
  write (serialfd, &c, 1);

  return 0;
}

/* Receive a single character */
int serialreceive (void)
{
  unsigned char r;
  
  /* As long as we're not getting anything, sleep a while and try to
     force the issue - timeout is 5 seconds */
  while (inbl == 0)
    {
      if (sleep (5) == 0)
	return -1;
      serialevent ();
    }

  /* Lock the buffer */
  inblock = 1;

  /* Reduce the length, extract the character, and update the read
     pointer */
  inbl--;
  r = inbuffer [inbr];
  inbr = (inbr + 1) % SERIALINSIZE;

  /* If serialevent requested a call, call it */
  if (inblock == 2)
    serialevent ();
  
  return (int) r;
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
  int i;
  if (datatype > 2)
    handlers [datatype] = NULL;
  else
    handlers [datatype] = defaulthandler;

  return 0;
}

/* Send a ping */
int ping (void)
{
  int sr;

  /* Clear the ping indicator */
  __ping = 0;

  /* Send the ping */
#ifdef DEBUG
  puts ("Serial: pinging");
#endif
  serialtransmit (1);

  /* Wait for the ping indicator to change */
#ifdef DEBUG
  puts ("Serial: waiting for acknowledgement");
#endif
  while (!__ping)
    if (sleep (10) == 0)
      return -1;

#ifdef DEBUG
  puts ("Serial: received acknowledgement");
#endif
  return 1;
}

/* Send a change state message */
void changestate (unsigned char state)
{
#ifdef DEBUG
  printf ("Serial: changing state to %d\n", (int) state);
#endif
  /* Send a ping */
  ping ();
#ifdef DEBUG
  puts ("Serial: transmitting new state");
#endif
  /* Send the new state */
  serialtransmit (state);
#ifdef DEBUG
  puts ("Serial: sent new state");
#endif
}

/* Enable packets */
void enablepackets (void)
{
  packets = 1;
}

/* Disable packets */
void disablepackets (void)
{
  packets = 0;
}
