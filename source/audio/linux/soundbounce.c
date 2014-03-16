#include <serial.h>
#include <stdio.h>

void main()
{
  serialinit();
  while(1)
    putchar(serialreceive());
}
