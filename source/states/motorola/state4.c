/* State 4 */
#include <serial.h>
#include <m68kboard.h>
#include <support.h>
#include <board.h>
#include <audio.h>

/******************************************
  State 4  Linux PC -> M68k board
  Compile with :
gcc68s /home/sysd7/source/states/motorola/state4.c /home/sysd7/source/audio/audio.c /home/sysd7/source/transport/motorola/polling.c /home/sysd7/source/low.level/motorola/board.c

NOTE : -- 
  This is just a DUMMY routine although I
have included all the stuff that I have 
been using to check how well it all
integrates with the rest.
******************************************/

#define SAMPLE_DELAY 39
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
  int otimer=10000000;

  /* PlayBuf[]  -  incoming data is decompressed into this array and then played. */
  unsigned char PlayBuf[2];

  /* Holds the position of the next sample to be played.  When this is 0, the routine
     will attempt to read a byte from the serial port and decompress it into PlayBuf.*/
  char BufPos=0;

  /* Serial data is read into this byte before being decoded. */
  unsigned char Data_next;

/**********************************************************************
  Dummy routine just returns 10.
  
  return 10;
  */

  /* Initialise the board settings, the serial driver and the decompression routines.*/
  initAudio();
  serialinit();
  initDecompr();
  
  /* Set the direction bits of data register A to 11100000. */
  tpit->ddra=0xE0;
  /* Set data register A to the same thing. */
  tpit->dra=0xE0;

  /* Send a synchronisation byte to the Linux side. */
  puts("Say hello to PC.\n\r");
#if 0
  serialtransmit('A');
#endif
  puts("Waiting for PC to be socialble...");
  serialreceive();
  puts("Let's have a chat...\n\r");

  starttimer(SAMPLE_DELAY);
  Data_next = serialreceive();
  while (1) {
    if (BufPos) {
      Data_next = serialreceive();
      if (Data_next == 0xFF) {
	Data_next = serialreceive();
	if (Data_next != 0xFF) {
#if 0
	  serialtransmit('B');
#endif
	  return (int) Data_next;
	}
      }
    } else {
      decompress(Data_next, PlayBuf);
    }
    /*    while ( otimer > (otimer=gettimer()) );
	  Essentially the same but faster?
     */
    while ( otimer > (otimer=gettimer()));
    setsample (PlayBuf[BufPos]);
    BufPos = 1 - BufPos;
  }

}


