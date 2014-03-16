#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main (void)
{
  int s0, s1;
  struct termios termios;

  s0 = open ("/dev/ttyS0", O_NOCTTY | O_RDWR);
  s1 = open ("/dev/ttyS1", O_NOCTTY | O_RDWR);
  tcgetattr (s0, &termios);
  tcsetattr (s1, TCSANOW, &termios);
  close (s1);
  close (s0);
}
