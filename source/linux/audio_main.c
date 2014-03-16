/* audio_main.c
   -- Linux-side audio system.
   
   Compile with :
   gcc -I/home/sysd7/source/include/ -O2 -o linuxtest lstate4.c ../audio.c ~/source/transport/linux/interrupt.c
   
   A shed-load of includes...
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

/*
  Define the buffer sizes for playing and recording sound.
*/
#define PLAY_BUF_SIZE 4096
#define SAMPLE_BUF_SIZE 4096
#define PLAY_FRAG 12
#define SAMPLE_FRAG 12

/*
  Uncomment this define to make the software automatically alter the mixer
  settings.  This is to be used to stop feed-back loops on the PC since the
  microphone's input is played directly to the speaker.
*/
/*#define MIXER_TWEAKS */

#define FULL_AUDIO

/* Some global variables for accessing the OSS audio */
int audio_fd,len;
#ifdef MIXER_ TWEAKS
int mixer_fd;
#endif 

/* Variables for indicating state changes */
/* I don't think that TranstionTo is actually used anymore but it's still
   here just in case. */
unsigned char TransitionTo=0;
/*Curstate is set to the current audio state with respect to the IDP state-
  machine paradigm.  The signal handler sets it to another state if it want
  the audio to switch states.
*/
int Curstate;

/* Structure for setting up the signal handler for IPC */
struct sigaction signals;


/**********
	   Buffers for Tx Data 
	   Fairly self-explanatory...
*/
unsigned char audioIn_Buf[SAMPLE_BUF_SIZE];
unsigned char serialOut_Buf[SAMPLE_BUF_SIZE/2];
int Tx_Data_count=0;


/***********
	    Buffers for Rx Data 
	    Ditto...
*/
unsigned char audioOut_Buf[PLAY_BUF_SIZE];
unsigned char serialIn_Buf[PLAY_BUF_SIZE/2];
int Rx_Data_count=0;



/***************************************
*  Soundcard initialisation routine.   *
****************************************/

int init_audio (int Mode)
{
  int retry = 10;
  int mono;
  /* Open the soundcard on /dev/dsp */
  while (retry-- && (audio_fd = open ("/dev/dsp", Mode, 0)) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_soundcard] : couldn't open /dev/dsp\nTrying again...\n");
    sleep(5);
    /*    exit (EXIT_FAILURE);*/
  }
  if (audio_fd == -1)
    return -1;
  
  /*
    Set the buffered fragment size so that there is less delay.
    This was supposed to be configurable from the defines above, but it
    didn't seem to work, so I've hard-coded it.
  */
  if (Mode==O_RDONLY)
    /*mono=0x7fff0000&SAMPLE_FRAG;*/
    mono=0x7FFF000C;
  else
    mono=0x7FFF000C;
  /*mono=0x7fff0000&PLAY_FRAG;*/
  if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &mono) == -1){
    fprintf(stderr,"error [(Duncan) lstate4.c/init_audio() : Couldn't set the fragment size properly, this may result in glitchy sound.\n");
    fprintf(stderr,"ioctl call Returned : %X\n",mono);
    return 2;
  }
  
  /*
    Set to mono operation.
  */
  mono=0;
  if (ioctl (audio_fd, SNDCTL_DSP_STEREO, &mono) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_audio()] : problem setting mono operation?!?!");
    return 3;
  }
  
  /*
    set it to 8-bit unsigned sampling.
  */
  mono = AFMT_U8;
  if (ioctl (audio_fd, SNDCTL_DSP_SETFMT, &mono) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/init_audio()] : problem setting unsigned 8-bit mode.");
    return 4;
  }
  
  /*
    set it to 8kHz sampling rate.
    This assignment is a hangover from testing, I've decided to leave it in
    this comment for the time being.
    mono = 6250;
  */
  mono = 8000;
  if (ioctl (audio_fd, SNDCTL_DSP_SPEED, &mono) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/init_audio()] : problem setting sampling rate.");
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
/****************************************************************************
   --=[ Mixer Functions ]=--
   only active if MIXER_TWEAKS is #defined.
*****************************************************************************/
/* opens the mixer device */
int init_mixer(void)
{
  int mixer;
  /* Open the audio mixer on /dev/mixer */
  if ((mixer_fd = open ("/dev/mixer", O_WRONLY, 0)) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_mixer] : couldn't open /dev/mixer");
    return -1;
  }
  return 0;
}

/* Set the volume of the speakers */
int mixer_volume(int volume) 
{
  if (ioctl (mixer_fd, SOUND_MIXER_VOLUME, &volume) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/mixer_volume] : problem setting volume");
    return -1;
  }
  return 0;
}

/* Set the recording level of the microphone */
int mixer_mic_level(int level)
{
  if (ioctl (mixer_fd, SOUND_MIXER_MIC, &level) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/mixer_mic_level] : problem setting level");
    return -1;
  }
  return 0;
}


/* Go on... have a guess. */
void close_mixer(void)
{
  close (mixer_fd);
}

#endif



/***********************************
  Audio Tx routines...
***********************************/

/* Samples the microphone into the buffer pointer at by buffer. */
int getsample (unsigned char *buffer)
{
  int tmp=SAMPLE_BUF_SIZE;
  if ((len = read (audio_fd, buffer, tmp)) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/get_sample] : Sample failed!\n");
    return -1;
  }
  if (len != SAMPLE_BUF_SIZE) {
    fprintf(stderr,"error [(Duncan) lstate4.c/get_sample] : incorrect sample!\n");
    return -1;
  }
  return 0;
}

/* Takes a sample buffer and compresses it ready for sending. */
void compress_sample(unsigned char *InBuf, unsigned char *OutBuf)
{
  /* Set j to the size of the sample then loop until j becomes zero. */
  int j=SAMPLE_BUF_SIZE;
  while (j--) {
    /* Run through the Buffers, compressing the sound. */
    *(OutBuf++)=compress(*(InBuf),*(InBuf+1));
    InBuf+=2;
  }  
}


/* Grabs a sample of length SAMPLE_BUF_SIZE into audioIn_Buf,
   compresses it into serialOut_Buf, then sends the contents
   of serialOut_Buf a byte ata a time to the IDP board.
*/
void sendSample(void)
{
  int i=SAMPLE_BUF_SIZE/2;
  unsigned char t;
  unsigned char *SBuf=serialOut_Buf;
  
#ifdef DEBUG
  fprintf(stderr,"Sampling...\n");
#endif
  getsample(audioIn_Buf);
  
#ifdef DEBUG
  fprintf(stderr,"Compressing...\n");
#endif
  compress_sample(audioIn_Buf,serialOut_Buf);
  
#ifdef DEBUG
  fprintf(stderr,"Sending...\n");
#endif
  
}



/*
  Audio Rx routines...
*/
/* Play a sample of length PLAY_BUF_SIZE from buffer. */
int setsample(unsigned char *buffer)
{
  int i;
  if ((len = write (audio_fd, buffer, PLAY_BUF_SIZE)) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/set_sample] : Sample failed!\n");
    return -1;
  }
  return 0;
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


/* ... */
void receiveSample(void)
{
  int i;
  /* We know that there will be precisely half as many compressed samples as
     there are actual data samples.  Since there is no control info being sent
     to the PC. */
  /* 
     This for-loop has been moved out to l_state10() in order to allow rapid
     state changes.
     
     for (i=0;i<PLAY_BUF_SIZE/2;i++)
     serialIn_Buf[i]=serialreceive();
  */
  /* decompress the sample from serialIn_Buf into audioOut_Buf */
  decompress_sample(serialIn_Buf,audioOut_Buf);
  /* Now, play the sample. */
  setsample(audioOut_Buf);
}


/***************************************************************************/


void l_state4(void)
{
  sleep (2);
  serialinit();
  /* The audio code to send from the PC to the motorola is a bit broken at
     the moment, consequently this state just waits to be signalled to stop.
     It should be noted that this function's counterpart on the motorola is
     actually fully operational, and that this state just waits for an
     instruction to stop then sends the necessary data to initiate a change-
     state on the motorola.
  */
#ifdef FULL_AUDIO
  /* Initialise the soundcard and compression routines. */
  init_audio(O_RDONLY);
  initCompr();
  
#ifdef MIXER_TWEAKS
  /*Turn the speakers off and the mic. up. */
  mixer_volume(0);
  mixer_mic_level(64);
#endif
#ifdef DEBUG
  fprintf(stderr,"lstate4 message : Starting to send audio...\n");
#endif
  /*Send a start transmission byte to the motorola. */
  serialtransmit(0xFE);
  while (Curstate==4) {
    sendSample();
    for (i=0;i<SAMPLE_BUF_SIZE/2;i++) {
      if (Curstate!=4) break;
      /* enumerates the contents of serialOut_Buf.  If an FF is found in the
	 compressed data, then it is doubled to allow for state changes to be
	 sent.
      */
      if (*SBuf==0xFF){
	serialtransmit(0xFF);
#ifdef DEBUG
  	fprintf(stderr,"Doubling FF\n");
#endif
      }
      serialtransmit(*(SBuf++));
    }
  }
#else
  
  serialtransmit (0xFF);
  while (Curstate==4)
    sleep (10);
  serialtransmit (0xFF);
#endif
  serialtransmit (0xFF);
  
  close_audio();
  serialclose();
}



void l_state10(void)
{
  int i;
#ifdef DEBUG
  fprintf(stderr, "S10: starting audio\n");
#endif
  
  /* open soundcard for writing. */
  init_audio(O_WRONLY);
  
#ifdef DEBUG
  printf ("S10: Audio initialised.\nStarting serial...\n");
#endif
  
  /* Wait for serial to become free, and grab it */
  while(serialinit())
#ifdef DEBUG    
    printf("Trying serial again...\n");
#endif
  
  /* Start the decompression up */
  initDecompr();
  
#ifdef MIXER_TWEAKS
  /* Turn on the speakers */
  mixer_volume(255);
  /*Turn off the mic. */
  mixer_mic_level(0);
#endif
  
  /* Synch with motorola */
  serialreceive();
  
#ifdef DEBUG
  fprintf (stderr, "Let's go..\n");
#endif
  while (Curstate==10) {
    for (i=0;i<PLAY_BUF_SIZE/2;i++) {
      if (Curstate!=10) break;
      
      serialIn_Buf[i]=serialreceive();
    }
    if (Curstate==10) receiveSample();
  }
  /* Simulate a ping ... */
  serialtransmit(1);
  /* Wait for a ping ack */
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
  
  /* Shut everything down and leave */
  close_audio();
  serialclose();
#ifdef DEBUG
  fprintf(stderr, "Done state 10..\n");
  fflush (stderr);
#endif
}


/*
  Opens file fname and plays it to /dev/dsp
*/

int play_sample(char *fname) {
  FILE *SFile;
  int BufPos=0;
  int next;
#ifdef DEBUG
  fprintf(stderr, "Starting audio...\n");
#endif
  /* open soundcard */
  init_audio(O_WRONLY);

  /*open the file */
#ifdef DEBUG
  fprintf(stderr, "Opening file '%s'...\n",fname);
#endif
  if ((SFile = fopen(fname, "r"))==NULL) {
#ifdef DEBUG
    fprintf(stderr, "Couldn't open file!\n");
#endif
    return -1;
  }
#ifdef DEBUG
  fprintf(stderr, "Playing audio...");
#endif
  while ((next=fgetc(SFile))!=EOF) {
    serialIn_Buf[BufPos++]=(unsigned char) next;
    if (!(BufPos % PLAY_BUF_SIZE)) {
#ifdef DEBUG
      fprintf(stderr, ".");
#endif
      setsample(audioOut_Buf);
      BufPos=0;
    }
  }
  for (BufPos--; BufPos<PLAY_BUF_SIZE; BufPos++)
    audioOut_Buf[BufPos]=127;
  setsample(audioOut_Buf);
#ifdef DEBUG
  fprintf(stderr, ".\nShutting down...\n");
#endif
  close_audio();
}


/* Signal handler primarily for communications between GUI and audio
   processes, but since there is no UI to the audio, it also handles a
   SIGINT - sent by the GUI - which closes the program down.
*/
void sighandle(int s)
{
#ifdef DEBUG
  fprintf(stderr, "Signal handler called\n");
#endif
  /*Decide what to do...*/
  switch (s) {
    /*SIGUSR1 has been trapped.
      This indicates that state 4 is being requested */
  case SIGUSR1: 
    switch (Curstate) {
    case 10:
    case 0: 
      /*Toggle the state to 4 */
      Curstate = 4;
      break;
    case 4:
      /* Audio is already in state 4 so stop. */
      Curstate = 0;
      break;
    }
    break;
  case SIGUSR2:
    /*SIGUSR2 has been trapped.
      This indicates that state 10 is being requested */
    switch (Curstate) {
    case 4:
    case 0: 
      /* Toggle state to 4 */
      Curstate = 10;
      break;
    case 10: 
      /* Audio is already in state 10, so just stop. */
      Curstate = 0;
      break;
    }
    break;
  case SIGINT:
    /* The process is being killed off so exit nicely. */
    Curstate=-1;
    break;
    
  }
}


void audio_main (void)
{
  struct sigaction oa;
  /* Set up the signal handler. */
  signals.sa_handler=sighandle;
  signals.sa_mask=signals.sa_flags=0;
  signals.sa_restorer=NULL;
  
  /*Register the handler for the 3 signals */
  sigaction(SIGUSR1, &signals, &oa);
  sigaction(SIGUSR2, &signals, &oa);
  sigaction(SIGINT, &signals, &oa);
  /* When the process starts, the audio should be off. */
  Curstate=0;
  
  while (Curstate != -1) {
    switch (Curstate) {
    case 4:
      /* The Current state is 4 so call l_state4() */
      l_state4();
      break;
    case 10:
      /* The current state is 10 so call l_state10() */
      l_state10();
      break;
    case 0:
      /* The audio is off so have a little snooze so that the CPU isn't 
	 bothered by the process.  If a signal is generated by the gui,
	 this sleep will exit prematurely and the program will loop again.
	 If the audio is not turned on, the program will continue to go to
	 sleep for 60 seconds, wake up, go back to sleep...*/
      sleep(60);
      break;
    }
  };  
}

