#include <m68kboard.h>
#include <board.h>

volatile struct mc68230fm * spit = 0x05000000;

unsigned char kptable[] = { 1, 2, 3, KP_UNDEF,
			    4, 5, 6, KP_UNDEF,
			    7, 8, 9, KP_UNDEF,
			    KP_STAR, 0, KP_HASH, KP_UNDEF };

unsigned char getkeypad (void)
{
  unsigned char c, key;
  spit->ddra = 0xC0;
  do
    {
      c = spit->ara;
      asm ("nop");
    }
  while (!(c & 0x10));
  do
    {
      c = spit->ara;
      asm ("nop");
    }
  while (c & 0x10);
  key = spit->ara & 0x0F;
  return kptable [key];
}

int checkkeypad (void)
{
  spit->ddra = 0xC0;
  return (spit->ara & 0x10);
}
