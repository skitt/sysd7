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
      /*
      Data = serialreceive();
      */
      if ((Data=getchar())==EOF)
	return;
      decompress((unsigned char)Data,I);
      /*I[0]=decompressI(Data);*/
      fprintf(stderr,"%d %d %d ",(unsigned char)Data,I[0],I[1]);
    }
    putchar(I[v]);
    v=1-v;
  }
    
}
