/* Linux interrupt-driven serial code
   $Id$
   $Log$
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

int serialfd;

packethandler handlers [256];

#define SERIALINSIZE 32768
#define SERIALOUTSIZE 65536
#define SERIALOUTDELAY 10
unsigned char inbuffer [SERIALINSIZE];
volatile unsigned int inbl;
packet pkt;
unsigned char pktbuf [SERIALINSIZE];
unsigned char outbuffer [SERIALOUTSIZE];
volatile unsigned int outbl, outbr, outbw;

void defaulthandler (packet * in)
{
}

void serialevent (void);

void iosighandler (int arg)
{
  serialevent ();
}

void serialevent (void)
{
  fd_set rfds, wfds;
  struct timeval tv;
  int i;
  
  /* Use select to determine whether we should read or write */
  FD_ZERO (&rfds);
  FD_SET (serialfd, &rfds);
  FD_ZERO (&wfds);
  FD_SET (serialfd, &wfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select (serialfd + 1, &rfds, &wfds, NULL, &tv);
  if (FD_ISSET (serialfd, &rfds))
    {
      inbl += read (serialfd, &(inbuffer [inbl]), SERIALINSIZE -
		    inbl);
      if (inbl > 4 && pkt.length == 0)
	{
	  pkt.datatype = inbuffer [0];
	  printf ("Read datatype: %d\n", (int) pkt.datatype);
	  printf ("Reading length: ");
	  pkt.length = 0;
	  for (i = 1; i < 5; i++)
	    {
	      printf ("%2X ", inbuffer [i]);
	      pkt.length = (pkt.length << 8) + inbuffer [i];
	    }
	  printf ("= %d\n", pkt.length);
	}
      else if (inbl > 4 + pkt.length)
	{
	  puts ("Read data");
	  for (i = 0; i < pkt.length; i++)
	    pktbuf [i] = inbuffer [i + 5];
	  for (i = pkt.length; i < inbl; i++)
	    inbuffer [i - pkt.length] = inbuffer [i];
	  inbl -= pkt.length + 5;
	  pkt.data = pktbuf;
	  if (handlers [pkt.datatype] != NULL)
	    {
	      puts ("Calling data handler");
	      handlers [pkt.datatype] (&pkt);
	    }
	  pkt.length = 0;
	}
    }
  if ((FD_ISSET (serialfd, &wfds)) && outbl > 0)
    {
      puts ("Writing");
      write (serialfd, &(outbuffer [outbr]), 1);
      outbr = (outbr + 1) % SERIALOUTSIZE;
      outbl--;
    }
}

int serialinit (void)
{
  int i;
  struct termios termios;
  struct sigaction sa;

  inbl = 0;
  
  sa.sa_handler = iosighandler;
  sa.sa_mask = 0;
  sa.sa_flags = 0;
  sa.sa_restorer = NULL;
  sigaction (SIGIO, &sa, NULL);
  serialfd = open ("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NONBLOCK);
  fcntl (serialfd, F_SETOWN, getpid ());
  fcntl (serialfd, F_SETFL, FASYNC);
  termios.c_cflag = B38400 | CS8 | CREAD | CLOCAL;
  termios.c_iflag = IGNPAR;
  termios.c_oflag = 0;
  termios.c_lflag = 0;
  termios.c_cc[VINTR] = 0;
  termios.c_cc[VQUIT] = 0;
  termios.c_cc[VERASE] = 0;
  termios.c_cc[VKILL] = 0;
  termios.c_cc[VEOF] = 4;
  termios.c_cc[VTIME] = 0;
  termios.c_cc[VMIN] = 0;
  termios.c_cc[VSWTC] = 0;
  termios.c_cc[VSTART] = 0;
  termios.c_cc[VSTOP] = 0;
  termios.c_cc[VSUSP] = 0;
  termios.c_cc[VEOL] = 0;
  termios.c_cc[VREPRINT] = 0;
  termios.c_cc[VDISCARD] = 0;
  termios.c_cc[VWERASE] = 0;
  termios.c_cc[VLNEXT] = 0;
  termios.c_cc[VEOL2] = 0;
  tcflush (serialfd, TCIFLUSH);
  tcflush (serialfd, TCOFLUSH);
  tcsetattr (serialfd, TCSANOW, &termios);

  handlers [0] = defaulthandler;
  for (i = 1; i < 256; i++)
    handlers [i] = NULL;

  return 0;
}

int serialclose (void)
{
  close (serialfd);
  
  return 0;
}

int senddata (packet * pkt)
{
  int i;
  unsigned char c;

  write (serialfd, &(pkt->datatype), 1);
  for (i = 24; i >= 0; i -= 8)
    {
      c = (pkt->length & (0x000000FF << i)) >> i;
      usleep (SERIALOUTDELAY);
      write (serialfd, &c, 1);
    }
  for (i = 0; i < pkt->length; i++)
    {
      usleep (SERIALOUTDELAY);
      write (serialfd, &(pkt->data [i]), 1);
    }
  
#if 0
  while (outbl == SERIALOUTSIZE);
  outbuffer [outbw] = pkt->datatype;
  outbw = (outbw + 1) % SERIALOUTSIZE;
  outbl++;
  for (i = 24; i >= 0; i -= 8)
    {
      while (outbl == SERIALOUTSIZE);
      outbuffer [outbw] = (pkt->length & (0x000000FF << i)) >> i;
      outbw = (outbw + 1) % SERIALOUTSIZE;
      outbl++;
    }
  for (i = 0; i < pkt->length; i++)
    {
      while (outbl == SERIALOUTSIZE);
      outbuffer [outbw] = pkt->data [i];
      outbw = (outbw + 1) % SERIALOUTSIZE;
      outbl++;
    }
  serialevent ();
#endif

  return 0;
}

int registerhandler (unsigned char datatype, packethandler handler)
{
  handlers [datatype] = handler;

  return 0;
}

int unregisterhandler (unsigned char datatype)
{
  if (datatype > 1)
    handlers [datatype] = NULL;
  else
    handlers [datatype] = defaulthandler;

  return 0;
}
