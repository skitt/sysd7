#include <m68kboard.h>
#include <support.h>
#include "sampler.h"

void initAudio(void)
{
  pit->cph=0x0;
  asm("nop");
  asm("nop");

  pit->cpm=0x0;
  asm("nop");
  asm("nop");
  
  pit->cpl=0x10;
  asm("nop");
  asm("nop");

  pit->tcr=0x43;
  asm("nop");
  asm("nop");

  pit->ddra=0xE0;
  asm("nop");
  asm("nop");

  pit->dra=0xE0;
  asm("nop");
  asm("nop");
}


void setsample(unsigned char sample)
{
  /*-- init DAC --*/
  pit->dra=0xE0;
  asm("nop");

  pit->ddrb=0xFF;
  asm("nop");

  /*-- Send sample --*/
  pit->drb=sample;
  asm("nop");

  /*-- Set DAC --*/
  pit->dra=0xA0;
  asm("nop");
}


unsigned char getsample(void)
{
  unsigned char t;
  /*-- init ADC --*/
  pit->ddrb=0x00;
  asm("nop");
 
  pit->dra=0x60;
  asm("nop");

  /*-- return the sample on the bus --*/
  t=pit->arb;
  asm("nop");

  return t;
}


/*Simple main function*/

void main(void)
{
  int i;
  /* Set up initial state for audio system */
  initAudio();
  asm("nop");
  while (1){
    /*  This will make the board act as an amplifier... */
    /*-- INactive ...
    setsample(getsample());
    */
    

    /*  This will make the board play a saw-tooth waveform... */
    /*-- Active ... */
    for (i=0;i<256;i+=20) 
      setsample(i);
    for (i=255;i>=0;i-=10)
      setsample(i);
    /**/
  }
  
}
