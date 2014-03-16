#include <support.h>

unsigned char crc8 (unsigned char, unsigned char);

void main (void)
{
  unsigned char buf[] = { 0x01, 0x00, 0x00, 0x01, 0x88, 0x81, 0xA4 };
  unsigned char crc = 0;
  int i;
  for (i = 0; i < 7; i++)
    crc = crc8 (crc, buf [i]);
  putn (crc, 2, 8);
}

