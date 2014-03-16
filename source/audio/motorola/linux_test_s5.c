/*
  Reads sound from stdin, compresses it and sends it to the motorola.
  */
#include <stdio.h>
#include <audio.h>
#include <serial.h>

void main(void)
{
  unsigned char I[2];
  char v=0;
  int i=100;
  int t;
  unsigned char t2;
  serialinit();
  initCompr();
  
  while(i--){
    I[v] = getchar();
    if (v) {
      t2=compress(I[0],I[1]);
      /*      t2=compressI(I[0]);*/
      fprintf(stderr,"%d %d %d ",t2,I[0],I[1]);
      putchar(t2);
      /*
	if (t=serialtransmit(t2))
	fprintf (stderr, "Error :- %d\n", t);
      */   
    }
    v=1-v;
  }
  fprintf (stderr, "Finished.");
  serialtransmit(0xFF);
  serialtransmit(1);  
}
