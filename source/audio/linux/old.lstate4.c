/*  Linux audio system.
  $Id:
  $Log

Compile with :
gcc -I/home/sysd7/source/include/ -O2 -o linuxtest lstate4.c ../audio.c ~/source/transport/linux/interrupt.c
 */
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <audio.h>
#include <stdio.h>
#include <stdlib.h>
#include <serial.h>
#include <data.h>
#include <control.h>
#include <signal.h>


/*****
  Bizarrely enough I have these the wrong way round, that is PLAY... pertains to
  the size of the stuff sampled by the PC, and SAMPLE... is stuff received via
  the serial to be played on the PC.
  */
#define PLAY_BUF_SIZE 4096
#define SAMPLE_BUF_SIZE 4096
#define PLAY_FRAG 12
#define SAMPLE_FRAG 12

/*
  Uncomment this line to make the software automatically alter the mixer settings.
#define MIXER_TWEAKS 
*/


int audio_fd,len;
int mixer_fd;
unsigned char TransitionTo=0;
int Curstate;
struct sigaction signals;


/**********
   Buffers for Tx Data 
*/
unsigned char audioIn_Buf[SAMPLE_BUF_SIZE];
unsigned char serialOut_Buf[SAMPLE_BUF_SIZE/2];
int Tx_Data_count=0;


/***********
   Buffers for Rx Data 
*/
unsigned char audioOut_Buf[PLAY_BUF_SIZE];
unsigned char serialIn_Buf[PLAY_BUF_SIZE/2];
int Rx_Data_count=0;



/***************************************
*  Soundcard initialisation routine.   *
****************************************/

int init_audio (int Mode)
{
  int mono;
  /* Open the soundcard on /dev/dsp */
  while ((audio_fd = open ("/dev/dsp", Mode, 0)) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_soundcard] : couldn't open /dev/dsp\nTrying again...\n");
    sleep(5);
    /*    exit (EXIT_FAILURE);*/
  }
  

  /*
    Set the buffered fragment size so that there is less delay.
  */
  if (Mode==O_RDONLY)
    /*mono=0x7fff0000&SAMPLE_FRAG;*/
    mono=0x7FFF000C;
  else
    mono=0x7FFF000C;
    /*mono=0x7fff0000&PLAY_FRAG;*/
  if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &mono) == -1){
    fprintf(stderr,"Arsebandits!  The git doesn't wan't to play!\n");
    return 2;
  }
  fprintf(stderr,"Returned : %X\n",mono);

  
  /*
    Set to mono operation.
  */
  mono=0;
  if (ioctl (audio_fd, SNDCTL_DSP_STEREO, &mono) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_soundcard] : problem setting mono operation?!?!");
    return 3;
  }

  /*
    set it to 8-bit unsigned sampling.
  */
  mono = AFMT_U8;
  if (ioctl (audio_fd, SNDCTL_DSP_SETFMT, &mono) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/init_soundcard] : problem setting unsigned 8-bit mode.");
    return 4;
  }
  
  /*
    set it to 8kHz sampling rate.
  */
  mono = 6250;
  mono = 8000;
  if (ioctl (audio_fd, SNDCTL_DSP_SPEED, &mono) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/init_soundcard] : problem setting sampling rate.");
    return 5;
  }

  /*
    Initialise the audio subsystem...
  */
  TransitionTo=0;
  return 0;
}


/*
  It does exactly what it says on the tin!
*/
void close_audio(void)
{
  close (audio_fd);
}

#ifdef MIXER_TWEAKS
/*************************************************************************************
 Mixer Functions .......
*************************************************************************************/
void init_mixer(void)
{
  int mixer;
  /* Open the soundcard on /dev/mixer */
  if ((mixer_fd = open ("/dev/mixer", O_WRONLY, 0)) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_mixer] : couldn't open /dev/mixer");
    exit (EXIT_FAILURE);
  }
}

void mixer_volume(int volume) 
{
  /*  Set to mono operation. */
  if (ioctl (mixer_fd, SOUND_MIXER_VOLUME, &volume) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/mixer_volume] : problem setting volume");
  }
}

void mixer_mic_level(int level)
{
  if (ioctl (mixer_fd, SOUND_MIXER_MIC, &level) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/mixer_mic_level] : problem setting level");
  }
}

void close_mixer(void)
{
  close (mixer_fd);
}

#endif

/***********************************
  Audio Tx routines...
***********************************/

void getsample (unsigned char *buffer)
{
  int tmp=SAMPLE_BUF_SIZE;
  if ((len = read (audio_fd, buffer, tmp)) == -1)
    fprintf(stderr,"error [(Duncan) lstate4.c/get_sample] : Sample failed!\n");
  if (len != SAMPLE_BUF_SIZE)
    fprintf(stderr,"error [(Duncan) lstate4.c/get_sample] : incorrect sample!\n");
    
  /*  for (tmp=0;tmp<SAMPLE_BUF_SIZE;tmp++)
   *(buffer++)=(unsigned char)getchar();*/
}

void compress_sample(unsigned char *InBuf, unsigned char *OutBuf)
{
  int j=SAMPLE_BUF_SIZE;
  Tx_Data_count=0;
  while (j--) {
    *(OutBuf)=compress(*(InBuf),*(InBuf+1));
    OutBuf++;
    InBuf+=2;
    /*    if ((*(OutBuf++)=compress(*(InBuf++),*(InBuf++))) == 0xFF) {
	  Tx_Data_count++;
	  *(OutBuf++)=0xFF;
	  }
	  Tx_Data_count++;*/
  }
  
}

void aserialtransmit(unsigned char c)
{
  putchar (c);
}

void sendSample(void)
{
  int i=SAMPLE_BUF_SIZE/2;
  unsigned char t;
  unsigned char *SBuf=serialOut_Buf;

  fprintf(stderr,"Sampling...\n");
  getsample(audioIn_Buf);

  fprintf(stderr,"Compressing...\n");
  compress_sample(audioIn_Buf,serialOut_Buf);

  fprintf(stderr,"Sending...\n");
  while(i--){
    if (*SBuf==0xFF){
      serialtransmit(0xFF);
      fprintf(stderr,"Doubling FF\n");
    }
    serialtransmit(*(SBuf++));
  }

  /*    putchar (serialOut_Buf[i]);*/
}



/*
  Audio Rx routines...
*/
/* Play a sample. */
void setsample(unsigned char *buffer)
{
  int i;
  /*for (i=0;i<PLAY_BUF_SIZE;i++) {
    Rx_Data_count++;
    putchar(*(buffer++));
  }*/
 
  if ((len = write (audio_fd, buffer, PLAY_BUF_SIZE)) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/set_sample] : Sample failed!\n");
  }
}

/* Decompress a sample. */
void decompress_sample(unsigned char *InBuf, unsigned char *OutBuf)
{
  int j=PLAY_BUF_SIZE/2;
  while (j--) {
    decompress(*(InBuf++),OutBuf);
    OutBuf+=2;
  }
}

unsigned char aserialreceive()
{
  return getchar();
}

/* ... */
void receiveSample(void)
{
  int i;
  /* We know that there will be precisely half as many compressed samples as there are
     actual data samples.  Since there is no control info being sent to the PC. */
  /*  for (i=0;i<PLAY_BUF_SIZE/2;i++)
    serialIn_Buf[i]=serialreceive();
    */
  decompress_sample(serialIn_Buf,audioOut_Buf);
  setsample(audioOut_Buf);
}


/**************************************************************************************
* Right... that should take care of most of it, the next bit is mostly guesswork.  That
* is to say that it is correct as far as it goes (I think!) but I'm guessing as to how
* it's going to tie in with the rest of the system.
***************************************************************************************
  In fact I think these are utter tosh.  The l_stateN()s won't happen along with the
  GUI, consequently they will have to be in a separate process, but then the GUI cannot
  call audio_MakeTransition() in order to stop the audio...  AAArrrrrgggghhhH!!!!
**************************************************************************************/


void l_state4(void)
{
  sleep (2);
  serialinit();
  /*
    init_audio(O_RDONLY);
    initCompr();
    initDecompr();
    #ifdef MIXER_TWEAKS
    mixer_volume(0);
    mixer_mic_level(255);
    #endif
    fprintf(stderr,"Waiting for Motorola board...\n");
    serialreceive();
    fprintf(stderr,"Motorola board is asking for a chat.\nWaiting...\n");
    sleep(2);
    fprintf(stderr,"Sending acknowledge.\n");
    serialtransmit('B');
    fprintf(stderr,"Starting...\n");
    while (!TransitionTo)
    sendSample();
  */
  serialtransmit (0xFF);
  while (Curstate==4)
    sleep (10);
  serialtransmit (0xFF);
  serialtransmit (0xFF);
  /*  serialtransmit(0xFF);*/
  /*  serialtransmit(TransitionTo);
  fprintf(stderr,"Waiting for EOT\n");
  serialreceive();*/
  close_audio();
  serialclose();
}

void l_state10(void)
{
  int i;
  printf("S10: starting audio\n");
  init_audio(O_WRONLY);
  printf ("S10: Audio initialised.\nStarting serial...\n");
  while(serialinit())
    printf("Truying serial again...\n");
  initCompr();
  initDecompr();
  printf("lstate10\n");
#ifdef MIXER_TWEAKS
  mixer_volume(255);
  mixer_mic_level(0);
#endif
  serialreceive();
  fprintf (stderr, "Let's go..\n");
  while (Curstate==10) {
    for (i=0;i<PLAY_BUF_SIZE/2;i++) {
      if (Curstate!=10) break;
      
      serialIn_Buf[i]=serialreceive();
    }
    if (Curstate==10) receiveSample();
  }
  serialtransmit(1);
  do
    {
      if (serialreceive () != 2)
	continue;
      for (i = 0; i < 4; i++)
	if (serialreceive () != 0)
	  break;
      if (i == 4)
	break;
    }
  while (1);
  
  /*  serialtransmit(TransitionTo);*/
  /*  serialreceive();*/
  close_audio();
  serialclose();
  printf("Done state 10..\n");
  fflush (stdout);
}

void audio_MakeTransition(unsigned char State)
{
  TransitionTo=State;
}


void play_sample(char *fname) {
  FILE *SFile;
  int BufPos=0;
  int next;
  fprintf(stderr, "Starting audio...\n");
  while (init_audio(O_WRONLY)) {
    sleep (5);
  }
  fprintf(stderr, "Opening file '%s'...\n",fname);
  if ((SFile = fopen(fname, "r"))==NULL)
    fprintf(stderr, "Couldn't open file!\n");

  fprintf(stderr, "Playing audio...");
  while ((next=fgetc(SFile))!=EOF) {
    serialIn_Buf[BufPos++]=(unsigned char) next;
    if (!(BufPos % PLAY_BUF_SIZE)) {
      fprintf(stderr, ".");
      setsample(audioOut_Buf);
      BufPos=0;
    }
  }
  for (BufPos--; BufPos<PLAY_BUF_SIZE; BufPos++)
    audioOut_Buf[BufPos]=127;
  setsample(audioOut_Buf);
  fprintf(stderr, ".\nShutting down...\n");
  close_audio();
}


void sighandle(int s)
{
  printf("Signal handler called\n");
  switch (s) {
  case SIGUSR1: 
    switch (Curstate) {
    case 10:
    case 0: 
      Curstate = 4;
      break;
    case 4:
      Curstate = 0;
      break;
    }
    break;
  case SIGUSR2:
    switch (Curstate) {
    case 4:
    case 0: 
      Curstate = 10;
      printf("Set state = 10\n");
      break;
    case 10: 
      Curstate = 0;
      printf("Set state off\n");
      break;
    }
    break;
  case SIGINT:
    Curstate=-1;
    break;
    
  }
}


void audio_main (void)
{
  struct sigaction oa;
  signals.sa_handler=sighandle;
  signals.sa_mask=signals.sa_flags=0;
  signals.sa_restorer=NULL;
  
  sigaction(SIGUSR1, &signals, &oa);
  sigaction(SIGUSR2, &signals, &oa);
  sigaction(SIGINT, &signals, &oa);
  Curstate=0;

   while (Curstate != -1)
     {
       switch (Curstate) {
       case 4:
	 printf ("Entering state 4\n");
	 l_state4();
	 break;
       case 10:
	 printf ("Entering state 10\n");
	 l_state10();
	 break;
       case 0:
	 printf ("Audio is going off for a bit o' kip!\n");
	 sleep(60);
	 printf("Wakey wakey!\n");
	 break;
       }
     };
  
  /*
#ifdef MIXER_TWEAKS
  init_mixer();
#endif
  l_state4();
#ifdef MIXER_TWEAKS
  close_mixer();
#endif
*/
}

