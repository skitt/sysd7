#include <stdio.h>
#include <stdlib.h>
#include <serial.h>

unsigned char Incoming[24000];

void main()
{
  int i;
  serialinit();
  for (i=0; i<24000;i++) {
    Incoming[i]=serialreceive();
  }
  for (i=0;i<24000;i++) {
    putchar(Incoming[i]);
  }

}
