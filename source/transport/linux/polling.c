/* Linux polling serial code
   $Id: polling.c,v 1.1 1998/04/30 08:56:14 skk Exp skk $
   $Log: polling.c,v $
   Revision 1.1  1998/04/30 08:56:14  skk
   Initial revision

*/

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include <serial.h>

int serialfd;

packethandler handlers [256];

void defaulthandler (packet * in)
{
}

int serialinit (void)
{
  int i;
  struct termios termios;

  serialfd = open ("/dev/ttyS1", O_RDWR | O_NOCTTY);
  tcgetattr (serialfd, &termios);
  termios.c_cflag = CS8 | CREAD | PARENB | CLOCAL;
  cfsetospeed (&termios, B38400);
  cfsetispeed (&termios, B38400);
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

int serialreceive (void)
{
  unsigned char c;
  read (serialfd, &c, 1);
  return c;
}

void serialtransmit (unsigned char value)
{
  write (serialfd, &value, 1);
}

int registerhandler (unsigned char datatype, packethandler handler)
{
  handlers [datatype] = handler;

  return 0;
}

int unregisterhandler (unsigned char datatype)
{
  if (datatype)
    handlers [datatype] = NULL;
  else
    handlers [datatype] = defaulthandler;

  return 0;
}

void serialloop (void)
{
  unsigned char buffer [65536];
  packet in;
  int i;

  in.data = buffer;
  while (1)
    {
      in.datatype = (unsigned char) serialreceive ();
      for (i = 0; i < 4; i++)
	in.length = (in.length << 8) + (unsigned char) serialreceive
	  ();
      for (i = 0; i < in.length; i++)
	buffer [i] = serialreceive ();
      if (handlers [in.datatype] != NULL)
	handlers [in.datatype] (&in);
    };
}
