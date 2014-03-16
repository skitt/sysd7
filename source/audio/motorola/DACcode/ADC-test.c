#include <m68kboard.h>
#include <support.h>
/*
less /usr/lib/gcc-lib/m68k-68board-blob/include/m68kboard.h 

cph, cpm =0
cpl =20

tcr=43;   - set b0 to 0 to stop ADC.

ddrb = 0x0  - input
       0xFF - output

drb - to write?
arb - to read.

ddra - 11100000

dra
10100000  - ADC enable data out.

Writing to DAC:
01100000 
00100000 


dra  - to write to A, to control ADC/DAC.


Reading ADC,
  To initialise, set data B direction to input.
  set ddra to 11100000.
  set dra to 00100000.
  */

volatile struct mc68230fm * pit = (void *)0x05000000;

void initADC (void)
{
  pit->cph=0;
  asm("nop");

  pit->cpm=0;
  asm("nop");

  pit->cpl=0x20;

  pit->tcr=0x43;
  asm("nop");
  
  /*should set the ADC hardware to read onto the bus.*/
  pit->ddrb=0x00;
  asm("nop");

  pit->ddra=0xE0;
  asm("nop");

  pit->dra=0x60;
  asm("nop");

}
  
unsigned char readADC(void)
{
  unsigned char t;
  asm("nop");
  t= pit->arb;
  asm("nop");
  return t;
}

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
  char vuMeter[80];
  unsigned char d,t,o=0;
  int i;
  vuMeter[79]='\0';
  initADC();
  while(1){
    d=readADC();
    /*if (d!=o) {*/
    o=d;
    t=d>>2;
    for (i=0;i<t;i++)
      vuMeter[i]=' ';
    vuMeter[i++]=':';
    /*    vuMeter[i++]='\n';
    vuMeter[i++]='\r';*/
    vuMeter[i]='\0';
    puts(vuMeter);
    hex (d);
      mon_putc ('\n'); mon_putc ('\r');
    /* }*/
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
