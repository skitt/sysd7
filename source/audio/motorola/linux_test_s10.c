/*
  Takes compressed sound from motorola and plays it on stdout.
  */
#include <stdio.h>
#include <audio.h>
#include <serial.h>

void main(void)
{
  unsigned char I[2];
  int Data;
  char v=0;
  serialinit();
  initDecompr();

  while(1){
    if (!v) {
      Data = serialreceive();
      fprintf(stderr,"%d ",Data);
      decompress((unsigned char)Data,I);
    }
    putchar(I[v]);
    v=1-v;
  }
    
}
