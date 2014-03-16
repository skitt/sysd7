#include <serial.h>
#include <m68kboard.h>
#include <board.h>

#define SAMPLE_DELAY 38

volatile struct mc68230fm * tpit = (void *)0x05000000;

unsigned char Outgoing[24000];


void initAudio(void)
{
  tpit->cph=0x0;
  
  tpit->cpm=0x0;

  tpit->cpl=0x10;

  tpit->tcr=0x43;

  tpit->ddra=0xE0;

  tpit->dra=0xE0;
}


unsigned char getsamp(void)
{
  unsigned char t;
  /*-- init ADC --*/
  tpit->dra = 0xE0;
  tpit->ddrb=0x00;
 
  tpit->dra=0x60;
  asm ("nop"); asm ("nop");

  /*-- return the sample on the bus --*/
  t=tpit->arb;

  tpit->dra = 0xE0;

  return t;
}


void main(void)
{
  int i;
  unsigned char s;
  serialinit();
  initAudio();
  for (i=0; i<24000;i++) {
    starttimer(28);
    while (gettimer() > 0);
    Outgoing[i]=getsamp();
  }
  for (i=0;i<24000;i++) {
    serialtransmit(Outgoing[i]);
  }
}
