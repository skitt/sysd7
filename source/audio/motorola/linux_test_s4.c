/*
  Reads sound from stdin, compresses it and sends it to the motorola.

compile with :
gcc -I/home/sysd7/source/include -O2 -o linux_c linux_test_s4.c ../audio.c ~/source/transport/linux/interrupt.c
  */
#include <stdio.h>
#include <audio.h>
#include <serial.h>


#ifdef BLAH
void main(void)
{
  unsigned char I[2];
  char v=0;
  int i=1610;
  int j=0;
  unsigned char Outgoing[40000];
  int t;
  unsigned char t2;
  serialinit();
  initCompr();
  
  fprintf (stderr, "Received :%c.  Starting sound capture...\n",serialreceive());
  
  while(i){
    if ((t = getchar())==EOF) break;
    I[v]=t;
    if (v) {
      t2=compress(I[0],I[1]);
      Outgoing[j++]=t2;
      if (t2==0xFF) {serialtransmit(0xFF);/*usleep(10);*/}
      serialtransmit(t2);
      /*            usleep(5);*/
	
    }
    v=1-v;
  }
  /*  
  for (j=0;j<800;j++) {
    serialtransmit((unsigned char)0x00);
  }
  */

  fprintf (stderr, "Finished : %d samples.", j);
  for (j=0;j<800;j++)
    fprintf(stderr, "%X ", Outgoing[j]);
  serialtransmit(0xFF);
  /*  usleep(10);*/
  serialtransmit(1);
  fprintf(stderr,"Waiting...");
  serialreceive();
  fprintf(stderr,"done.\nClosing serial...");
  serialclose();
  fprintf(stderr,"done.\n");
}
#else
void main (void)
{
  int i;
  serialinit();
  fprintf(stderr,"synching...\n");
  serialreceive();
  for (i=0;i<800;i++)
    serialtransmit((unsigned char)(i%256));
  fprintf(stderr,"Tx'd 800 characters.");
  serialtransmit(1);
  serialreceive();
  serialclose();
}

#endif
