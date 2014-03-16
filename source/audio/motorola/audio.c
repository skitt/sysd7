/*
  Code for integration of M68k audio into M68k system.
  Includes dummy routine for state 4 which simply returns straight away,
  calling state 10.  This can be changed to something else in the main()
  function if required.

  Compile with :
gcc68s /home/sysd7/source/audio/motorola/audio.c /home/sysd7/source/audio/audio.c /home/sysd7/source/transport/motorola/polling.c /home/sysd7/source/low.level/motorola/board.c

  -- Duncan.
  */

#include <serial.h>
#include <m68kboard.h>
#include <support.h>
#include <board.h>
#include <audio.h>


#define SAMPLE_DELAY 39;

volatile struct mc68230fm * tpit = (void *)0x05000000;

void initAudio(void)
{
  tpit->cph=0x0;
  
  tpit->cpm=0x0;

  tpit->cpl=0x10;

  tpit->tcr=0x43;

  tpit->ddra=0xE0;

  tpit->dra=0xE0;
}



int state4(void)
{
  return 10;
}



int state10(void)
{
  unsigned char RecBuf[2];
  char v=1;
  int otime,ctime;
  unsigned char Data_next;
  int Control=0;
  serialinit();
  initAudio();
  initCompr();
  
  /* Synch with PC... */
  serialtransmit('A');
  otime=10000000;
  starttimer(SAMPLE_DELAY);
  ctime=gettimer();
  while ( otime > ctime ) {
    otime=ctime;
    ctime=gettimer();
  }
  RecBuf[0] = (getsample());
  otime=1000;
  while (1) {
    /*starttimer(SAMPLE_DELAY);*/
    while ( otime > (otime= gettimer() > 0) );
    RecBuf[v] = (getsample());
    if (v) {
      Control = compress(RecBuf[0],RecBuf[1]);
    } else {
      Control = serialtransmit(Control);
      if (Control) {
	serialclose();
	return Control;
      }
    }
    v=1-v;
  }
}
