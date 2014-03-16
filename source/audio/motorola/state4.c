/*
  Code for implementing states 4 & 10 on the motorola board.  Includes state4a(), which
  is used to test the serial interface by just grabbing away merrily until it receives
  a state change signal.

  State changes are indicated by a 0xFF byte being transmitted, followed by a byte
  containing the state to change to.  If the sound data being transmitted actually is
  an FF, the second byte will also be an FF and no state transition should occur.


  Compile with :
gcc68s -o state4test state4.c ../audio.c ~/source/transport/motorola/polling.c ~/source/low.level/motorola/board.c

  
*/
#include <serial.h>
#include <m68kboard.h>
#include <support.h>
#include <control.h>
#include <board.h>
#include <audio.h>

/*
  Actual delay calculated as :
     32                     1
------------  X Delay  =  ------
 10,000,000                8000

hence:
           10,000       1250       625
Delay  =  --------  =  ------  =  -----  =  39 1/16
           32 x 8        32        16

16

#define SAMPLE_DELAY 39
*/

int SAMPLE_DELAY = 39;

/*
  Parallel interface timer interface.  Memory-mapped into 0x05000000.
  Registers are dereferenced thus :
  Data = tpit->dra;

*/
volatile struct mc68230fm * tpit = (void *)0x05000000;

/* 
   A buffer used for testing the code.  Holds a record of all incoming data.
*/
unsigned char Incoming[16384];

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


int state4a(void)
{
  unsigned char PlayBuf[2];
  char v=0;
  unsigned char t;
  unsigned int j=0;
  unsigned char Data_next;
  
  serialinit();
  boardinit();
  initDecompr();

  tpit->ddra=0xE0;
  tpit->dra=0xE0;

  for (j=0;j<100;j++){
    for (t=0;t<255;t++)
      setsample(t);
    for (t=255;t>0;t--)
      setsample(t);
  }
  
  j=0; 
  puts("Starting sound reproduction...\n\r");
  serialtransmit('A');
  puts("synched..\n\r");  

  Incoming[j++] = (unsigned char) serialreceive ();
  /*  mon_putc('.');*/
  Incoming[j++] = (unsigned char) serialreceive ();
  /*  mon_putc('.');*/
  while(!((Incoming[j-2] == 0xFF) && (Incoming[j-1] != 0xFF))){
    /*    if (!(j % 70))
      puts ("\n\r");    mon_putc('.');*/
    Incoming[j++]=(unsigned char)serialreceive();
    /*    if ((Incoming[j-1]==0xFF) && (Incoming[j++]!=0xFF)) break;*/
  }
  /*return 1;*/
  puts("\n\rSending data acknowledge...");
  serialtransmit('B');

  puts("Received ");
  putn(j,10,5);
  puts ("samples OK.\n\rPlaying...\n\r");

  j=0;

  while (1) {
    starttimer (10000000);
    if (!v) {
      Data_next=Incoming[j++];
      /*Data_next = serialreceive();*/
      /*      hex (Data_next); 
	      puts("R\n\r");*/
      if (Data_next == 0xFF) {
	/*puts ("Waiting for new state\n\r");*/
	Data_next = serialreceive();
	if (Data_next != 0xFF) 
	  return (int) Data_next;
      }
      /*      Incoming[j++]=Data_next;*/
      decompress(Data_next, PlayBuf);
    }
    setsample (PlayBuf[v]);
    while ( gettimer() > 10000000 - SAMPLE_DELAY );
    v = 1 - v;
  }

}



void initAudio(void)
{
  tpit->cph=0x0;
  
  tpit->cpm=0x0;

  tpit->cpl=0x10;

  tpit->tcr=0x43;

  tpit->ddra=0xE0;

  tpit->dra=0xE0;
}


void setsamplen(unsigned char sample)
{
  /*-- init DAC --*/
  tpit->dra=0xE0;

  tpit->ddrb=0xFF;

  /*-- Send sample --*/
  tpit->drb=sample;

  /*-- Set DAC --*/
  tpit->dra=0xA0;

  tpit->dra=0xE0;
}




/******************************************
  State 4  Linux PC -> M68k board
******************************************/

int state4(void)
{
  int otime=1000,ctime;

  /* PlayBuf[]  -  incoming data is decompressed into this array and then played. */
  unsigned char PlayBuf[2];

  /* Holds the position of the next sample to be played.  When this is 0, the routine
     will attempt to read a byte from the serial port and decompress it into PlayBuf.*/
  char BufPos=0;

  /* Serial data is read into this byte before being decoded. */
  unsigned char Data_next;
  
  /* Initialise the board settings, the serial driver and the decompression routines.*/
  serialinit();
  initAudio();
  initDecompr();

  /* Send a synchronisation byte to the Linux side. */
  puts("Say hello to PC.\n\r");
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
	  return (int) Data_next;
	}
      }
    } else {
      decompress(Data_next, PlayBuf);
    }
    /*    while ( otimer > (otimer=gettimer()) );
	  Essentially the same but faster?
     */
  ctime=gettimer();
  while ( otime > ctime ) {
    otime=ctime;
    ctime=gettimer();
  }
  /*    while ( otimer > (otimer=gettimer();));*/
    setsamplen (PlayBuf[BufPos]);
    BufPos = 1 - BufPos;
  }

}

int state10(void)
{
  unsigned char RecBuf[2];
  char v=1;
  int otime;
  unsigned char Data_next;
  int Control=0;
  serialinit();
  initAudio();
  initCompr();
  
  /* Synch with PC... */
  serialtransmit('A');
  otime=10000000;
  starttimer(SAMPLE_DELAY);
  while ( otime > (otime=gettimer()) );
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
      if (Control) 
	return Control;
    }
    v=1-v;
  }
}


int s4test(void)
{
  int i;
  serialinit();
  puts("Synching...\n\r");
  serialtransmit('1');
  for (i=0;i<800;i++) {
    Incoming[i] = serialreceive();
  }
  i = serialreceive();
  puts("synching...\n\r");
  serialtransmit('2');
  return i;
}


void main(void)
{
  int j;
  /*  putn ((unsigned char) state10 (), 16, 2);*/
  state4();

  for (j=0;j<800;j++) {
    if (!(j % 25))
      puts ("\n\r");
    /*putn (Incoming [j], 16, 2);*/
    hex(Incoming[j]);
    mon_putc (' ');
  }
  serialclose();
}
