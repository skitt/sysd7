#include <serial.h>
#include <support.h>

void main()
{
  serialinit();
  serialtransmit('a');
  serialreceive();
  mon_putc('A');
  while(1)
    serialtransmit(serialreceive());
}
