#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_BUF_SIZE 4096

int audio_fd,len;
int mixer_fd;
unsigned char TransitionTo=0;

unsigned char audioIn_Buf[SAMPLE_BUF_SIZE];

void init_audio (int Mode)
{
  int mono;
  fprintf(stderr,"Audio initialisation...\n");
  /* Open the soundcard on /dev/dsp */
  if ((audio_fd = open ("/dev/dsp", Mode, 0)) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_soundcard] : couldn't open /dev/dsp");
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
  if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &mono) == -1)
    fprintf(stderr,"Arsebandits!  The git doesn't wan't to play!\n");
  fprintf(stderr,"Returned : %d\n",mono);

  
  /*
    Set to mono operation.
  */
  mono=0;
  if (ioctl (audio_fd, SNDCTL_DSP_STEREO, &mono) == -1) {
    fprintf (stderr, "error [(Duncan) lstate4.c/init_soundcard] : problem setting mono operation?!?!");
  }

  /*
    set it to 8-bit unsigned sampling.
  */
  mono = AFMT_U8;
  if (ioctl (audio_fd, SNDCTL_DSP_SETFMT, &mono) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/init_soundcard] : problem setting unsigned 8-bit mode.");
  }
  
  /*
    set it to 8kHz sampling rate.
  */
  mono = 6250;
  mono = 8000;
  if (ioctl (audio_fd, SNDCTL_DSP_SPEED, &mono) == -1) {
    fprintf(stderr,"error [(Duncan) lstate4.c/init_soundcard] : problem setting sampling rate.");
  }

  /*
    Initialise the audio subsystem...
  */
  fprintf(stderr,"Audio init complete.\n");
}

void close_audio(void)
{
  close (audio_fd);
}

void getsample (unsigned char *buffer)
{
  int tmp=SAMPLE_BUF_SIZE;
  if (((len = read (audio_fd, buffer, tmp)) == -1)) {
    fprintf(stderr,"error [(Duncan) lstate4.c/get_sample] : Sample failed!\n");
  }
}

void main()
{
  int i;
  init_audio(O_RDONLY);
  while (1) {
    getsample(audioIn_Buf);
    for (i=0;i<SAMPLE_BUF_SIZE;i++)
      putchar(audioIn_Buf[i]);
  }
}
