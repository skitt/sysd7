/* State 4 */
#include <serial.h>
#include <m68kboard.h>
#include <support.h>
#include <board.h>
#include <audio.h>

/***************************************************
  state4.c
  -- Part of the motorola state machine. Handles audio 
     from Linux PC -> M68k board.

  Compile with :
gcc68s /home/sysd7/source/states/motorola/state4.c /home/sysd7/source/audio/audio.c /home/sysd7/source/transport/motorola/polling.c /home/sysd7/source/low.level/motorola/board.c

***************************************************/
/*
  This is the value for the 68230's count register which controls the
  timing of the audio.  i.e. it syncs it to roughly 8 KHz. 
*/
#define SAMPLE_DELAY 39

/*
  This is the structure that represents the 68230's registers.  It is 
  memory mapped to 0x05000000.
*/
volatile struct mc68230fm * tpit = (void *)0x05000000;


/* 
   Routine to initialise the audio hardware on the motorola's bus.
*/
void initAudio(void)
{
  /*
    Set up the counter preload registers cp?
  */
  tpit->cph=0x0;
  
  tpit->cpm=0x0;

  tpit->cpl=0x10;
  /*Start the timer going */
  tpit->tcr=0x43;
  
  /* Set up data register A for our use. */
  tpit->ddra=0xE0;

  tpit->dra=0xE0;
}


/*
  The main code for state 4 of the IDP state machine.
*/
int state4(void)
{
  int otimer=10000000;

  /* PlayBuf[]  -  incoming data is decompressed into this array and then
     played. */
  unsigned char PlayBuf[2];

  /* Holds the position of the next sample to be played.  When this is 0,
     the routine will attempt to read a byte from the serial port and
     decompress it into PlayBuf.*/
  char BufPos=0;

  /* Serial data is read into this byte before being decoded. */
  unsigned char Data_next;


  /* Initialise the board settings, the serial driver and the decompression
     routines.*/
  initAudio();
  serialinit();
  initDecompr();
  
  /* Set the direction bits of data register A to 11100000. */
  tpit->ddra=0xE0;
  /* Set data register A to the same thing. */
  tpit->dra=0xE0;

  /* Wait for a byte from PC to indicate start up. */
  serialreceive();
  
  /* Initialise the sample timer */
  starttimer(SAMPLE_DELAY);
  /* Get the first data byte from the PC.
     This has to be here in order to even out the processing requirements.
  */
  Data_next = serialreceive();
  while (1) {
    if (BufPos) {
      /* If we are about to play the second sample in the buffer, fetch the
	 next byte of data ready for decompression */
      Data_next = serialreceive();
      if (Data_next == 0xFF) {
	/* This could be the PC trying to indicate a state change, so get
	   the next item of data to see what to do. */
	Data_next = serialreceive();
	if (Data_next != 0xFF) {
	  /* The data wasn't another FF, so quit and return the state to 
	     change into. */
	  return (int) Data_next;
	}
      }
    } else {
      /* If we are about to have to play the first sample in the buffer,
	 we had better decompress the samples into it so that they can be
	 played. */
      decompress(Data_next, PlayBuf);
    }
    /* Wait for the audio timer...*/
    while ( otimer > (otimer=gettimer()));
    /* Play the next sample throught the DAC. */
    setsample (PlayBuf[BufPos]);
    /* Change to the next sample in the buffer and loop. */
    BufPos = 1 - BufPos;
  }

}


