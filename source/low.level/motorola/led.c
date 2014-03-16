#include <m68kboard.h>

#define LEDCODES 37

unsigned char ledcodes [] =
{
  '0', 0xC0,
  '1', 0xF9,
  '2', 0xA4,
  '3', 0xB0,
  '4', 0x99,
  '5', 0x92,
  '6', 0x82,
  '7', 0xF8,
  '8', 0x80,
  '9', 0x98,
  ' ', 0xFF,
  'a', 0xDF,
  'b', 0xFC,
  'c', 0xD8,
  'd', 0xDE,
  'e', 0xFB,
  'f', 0xF1,
  'g', 0xEF,
  'h', 0xF4,
  'i', 0x90,
  'j', 0x8C,
  'k', 0xF6,
  'l', 0xB0,
  'm', 0xD4,
  'n', 0xD4,
  'o', 0xDC,
  'p', 0xF3,
  'q', 0xE7,
  'r', 0xD0,
  's', 0xED,
  't', 0xF8,
  'u', 0x9C,
  'v', 0x9C,
  'w', 0x9C,
  'x', 0xF6,
  'y', 0xEE,
  'z', 0xA4
};
  
void led (unsigned char c)
{
  unsigned char * ledreg;
  int i;

  ledreg = (unsigned char *) 0x00D00003;
  for (i = 0; i < LEDCODES; i++)
    if (ledcodes [2 * i] == c)
      *ledreg = ledcodes [2 * i + 1];
}

void main (void)
{
  led ('A');
}
