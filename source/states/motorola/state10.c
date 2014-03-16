/******************************************
  State 10  M68k board -> Linux PC
  Compile with :
gcc68s /home/sysd7/source/states/motorola/state10.c /home/sysd7/source/audio/audio.c /home/sysd7/source/transport/motorola/polling.c /home/sysd7/source/low.level/motorola/board.c
******************************************/
#include <serial.h>
#include <m68kboard.h>
#include <support.h>
#include <board.h>
#include <audio.h>

#define SAMPLE_DELAY 39


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
  otime=1000;
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
