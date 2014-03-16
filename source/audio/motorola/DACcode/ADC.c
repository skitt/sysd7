#include <m68kboard.h>
#include <support.h>
/*#include <board.h>*/
#include <audio.h>

#define Blah

unsigned char PacketBuffer[510];

/*
less /usr/lib/gcc-lib/m68k-68board-blob/include/m68kboard.h 

cph, cpm =0
cpl =20

tcr=43;   - set b0 to 0 to stop ADC.

-------------------------------------------------

Writing to DAC:
1110 0000`
1010 0000 

Set dra b7,6 to 1.   ie dra = 11100000
Set ddrb to 0xFF (output).
Output sample on drb.
set dra b6 to 0, dra=10100000
 maybe a couple of nops, or set it to 0 again.
then back to 1. ie 11100000

--------------------------------------------------


===================================================

Start audio :
  set ddra to 11100000.
  Set dra b7,6 to 1.   ie dra = 11100000
WriteDac,
  assume dra=11100000
  Set ddrb to 0xFF (output).
  Output sample on drb.
  set dra b6 to 0, dra=10100000
  maybe a couple of nops, or set it to 0 again.
  then back to 1. ie 11100000
ReadAdc,
  assume dra=11100000
  set ddrb=0x00,             
  set dra = 01100000
  do a couple o' nops.
  read arb to get sample.
  set dra back to 11100000

===================================================
  */

volatile struct mc68230fm * pit = (void *)0x05000000;

void initAudio(void)
{
  pit->cph=0x0;
  
  pit->cpm=0x0;

  pit->cpl=0x10;

  pit->tcr=0x43;

  pit->ddra=0xE0;

  pit->dra=0xE0;
}

/*  Unnecessary ----
void initDAC(void)
{
  pit->dra=E0;
  pit->ddrb=0xFF;
}
*/

#ifdef Blah
void setsample(unsigned char sample)
{
  /*-- init DAC --*/
  pit->dra=0xE0;

  pit->ddrb=0xFF;

  /*-- Send sample --*/
  pit->drb=sample;

  /*-- Set DAC --*/
  pit->dra=0xA0;

  pit->dra=0xE0;
}

/*  Unnecessary -----
void initADC (void)
{
  /*should set the ADC hardware to read onto the bus.*/
/*  pit->ddrb=0x00;
  /*  asm("nop");*/
/*
  pit->dra=0x60;
  asm("nop");
  asm("nop");

}
*/  

unsigned char getsample(void)
{
  unsigned char t;
  /*-- init ADC --*/
  pit->ddrb=0x00;
 
  pit->dra=0x60;

  /*-- return the sample on the bus --*/
  t=pit->arb;

  return t;
}

#endif

void realhex (unsigned char c)
{
  if (c > 9)
    mon_putc (c + 'A' - 10);
  else
    mon_putc (c + '0');
}

void hex (unsigned char c)
{
  realhex ((c & 0xF0) >> 4);
  realhex (c & 0x0F);
}


void main(void)
{
  int i,t[2];
  unsigned char buffer[2];
  /* Set up initial state for audio system */
  initAudio();
  /*boardinit();*/
  initCompr();
  initDecompr();
  asm("nop");
  while (1){
    /*starttimer(10000000);*/
 
    setsample(decompressI(compressI(getsample())));
    /*    while(gettimer()>10000000-39) ;*/
    /*    getsample()-128;*/
    
    /*    for (i=0;i<256;i+=20) 
	  setsample(i);
	  for (i=255;i>=0;i-=10)
	  setsample(i);*/
    /*  }*/
    
  }
}
#ifdef ABC
/*
Old main functions...
*/
void main(void)
{
  char vuMeter[80];
  unsigned char d,t,o=0;
  int i;
  vuMeter[79]='\0';
  initAudio();
  mon_putc('A');
  while(1){
    d=getsample();
    /*    if (d!=o) {*/
      o=d;
      t=d>>2;
      for (i=0;i<t;i++)
	vuMeter[i]=' ';
      vuMeter[i++]=':';
      vuMeter[i]='\0';
      puts(vuMeter);
      hex (d);
      mon_putc ('\n'); mon_putc ('\r');
      setsample(d);
  }
}


/*
void main (void)
{
  int a;
  unsigned char t[2],o=0;
  t[1]='\0';
  initADC();
  o=t[0]=readADC();
  puts(t);
  for (a=0;a<800;a++) {
    while (t[0] == o)
      t[0]=readADC();
    puts(t);
    o=t[0];
    
  }
}
*/
#endif
