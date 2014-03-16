#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <stdio.h>

#define BUF_SIZE 8192

int audio_fd,len;
unsigned char audio[BUF_SIZE];

void print_bin(int n)
{
  fprintf(stderr,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n",(n>>15)&1,(n>>14)&1,(n>>13)&1,(n>>12)&1,(n>>11)&1,(n>>10)&1,(n>>9)&1,(n>>8)&1,(n>>7)&1,(n>>6)&1,(n>>5)&1,(n>>4)&1,(n>>3)&1,(n>>2)&1,(n>>1)&1,n&1);
}

void main(void)
{
  int tmp;
  int len;
  if ((audio_fd = open("/dev/dsp", O_RDONLY, 0)) == -1) {
    fprintf(stderr,"error [(Duncan) audio.c/init_soundcard] : couldn't open /dev/dsp");
    exit(EXIT_FAILURE);
  }
  len = 0xMMMMSSS8;

                      if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &arg)) error();
  tmp=0;
  /*  if (ioctl(audio_fd, SNDCTL_DSP_SETDUPLEX, &tmp)==-1)
    fprintf(stderr, "Dagnammit!!!\n");
  if (ioctl(audio_fd, SNDCTL_DSP_GETCAPS,&len)==-1)
    fprintf(stderr, "Buggeration!\n");
  print_bin(len);
  print_bin(DSP_CAP_REVISION);
  print_bin(-32768);
  if (!(len & DSP_CAP_DUPLEX)) {
    fprintf(stderr, "Aaarrgh!  Half-Duplex ONLY!!!!\n");
  }*/

  /* set it to 8-bit unsigned sampling. */
  tmp=AFMT_U8;
  if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &tmp)==-1) {
    fprintf(stderr,"error [(Duncan) audio.c/init_soundcard] : problem setting unsigned 8-bit mode.");
  }
  
  /* set it to 8kHz sampling rate. */
  tmp=8000;
  if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &tmp)==-1) {
    fprintf(stderr,"error [(Duncan) audio.c/init_soundcard] : problem setting sampling rate.");
  }
  while(1) {
    len=read(audio_fd, audio, 32);
    for (tmp=0;tmp<len;tmp++) 
      putchar(audio[tmp]);
  }
	      /*  write(audio_fd,audio,32);
		  }*/
       
}
