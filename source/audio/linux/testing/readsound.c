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

#define SAMPLE_BUF_SIZE 4096

int audio_fd,len;
int mixer_fd;
unsigned char TransitionTo=0;
int Curstate;
struct sigaction signals;

unsigned char audioIn_Buf[SAMPLE_BUF_SIZE];
unsigned char serialOut_Buf[SAMPLE_BUF_SIZE/2];
int Tx_Data_count=0;

int init_audio (int Mode)
{
  int mono;
  /* Open the soundcard on /dev/dsp */
  if ((audio_fd = open ("/dev/dsp", Mode, 0)) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_soundcard] : couldn't open /dev/dsp");
    return 1;
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
  FILE *fp;
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
      aserialtransmit(0xFF);
      fprintf(stderr,"Doubling FF\n");
    }
    aserialtransmit(*(SBuf++));
  }

  fp=fopen("raw.out","w");
  for (i=0;i<SAMPLE_BUF_SIZE;i++)
    fputc(audioIn_Buf[i],fp);
  fclose(fp);
  /*    putchar (serialOut_Buf[i]);*/
}

void main()
{
  init_audio(O_RDONLY);
  initCompr();
  sendSample();
}
