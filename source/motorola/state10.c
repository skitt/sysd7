/******************************************
  state10.c
  -- handles audio from M68k board -> Linux PC

  Compile with :
gcc68s /home/sysd7/source/states/motorola/state10.c /home/sysd7/source/audio/audio.c /home/sysd7/source/transport/motorola/polling.c /home/sysd7/source/low.level/motorola/board.c
******************************************/
#include <serial.h>
#include <m68kboard.h>
#include <support.h>
#include <board.h>
#include <audio.h>

/* the delay for the sample timer again  - see state4c.*/
#define SAMPLE_DELAY 39

/*
  The main code for implementing state 10 of the IDP state machine.
*/
int state10(void)
{
  /* A Buffer in which to store incoming samples before they are compressed */
  unsigned char RecBuf[2];
  /* Variable which indicates where the next sample will be stored. */
  char BufPos=1;
  /* timing variable to allow for the sample rate delay. */
  int otime;
  /* Holds the compressed data ready for sending on the next loop. */
  unsigned char Data_next;
  /* This holds the return value of serialtransmit.  If it is non-zero,
     a state change has been requested. */
  int Control=0;

  /* start up the serial driver, initialise the hardware and the compressio
     software */
  serialinit();
  initAudio();
  initCompr();
  
  /* Synch with PC... */
  serialtransmit('A');

  /* Initialise the old timer value to something big! */
  otime=1000;

  /* Set the timer going then wait for it to reset to acheive the correct
     sampling rate */
  starttimer(SAMPLE_DELAY);
  while ( otime > (otime = gettimer()));
  /* Grab the first sample */
  RecBuf[0] = (getsample());
  while (1) {
    /* wait for the timer to reset again. */
    while ( otime > (otime = gettimer() > 0) );
    /*grab a sample */
    RecBuf[v] = (getsample());
    if (BufPos) {
      /* If we have buffered two samples, then compress them ready for
	 transmission on the next loop */
      Data_next = compress(RecBuf[0],RecBuf[1]);
    } else {
      /* We have only buffered one sample so far, so we should send the
	 previously compressed pair on this loop. */
      Control = serialtransmit(Data_next);
      /* Check for a state change being signalled */
      if (Control) {
	/* If we are changing state, then shut down the serial and return. */
	serialclose();
	return Control;
      }
    }
    /* Alternate buffer position and loop. */
    BufPos=1-BufPos;
  }
}
