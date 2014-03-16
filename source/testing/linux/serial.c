#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int serialfd;

unsigned char serialreceive (void)
{
  unsigned char c;
  read (serialfd, &c, 1);
  return c;
}

void serialtransmit (unsigned char value)
{
  write (serialfd, &value, 1);
}

void main (void)
{
  int c;
  struct termios termios;

  serialfd = open ("/dev/ttyS1", O_RDWR | O_NOCTTY);
  termios.c_cflag = CS8 | CREAD | PARENB | CLOCAL;
  cfsetospeed (&termios, B38400);
  cfsetispeed (&termios, B38400);
  tcsetattr (serialfd, TCSANOW, &termios);
  serialtransmit ('H');
  for (c = 0; c < 19200; c++)
    putchar (serialreceive ());
  close (serialfd);
}
