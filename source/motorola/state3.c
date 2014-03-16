/* Motorola state 3 - Ian Clarke

   Functionality as State 2 but without iButton and Buzzer
   detection.  Times out after 10 video transmissions and returns
   to State 2
*/

#include <motorola.video.h>
#include <serial.h>
#include <m68kboard.h>
#include <quickcam.h>
#include <support.h>
#include <control.h>
#include <board.h>
#include <data.h>

#define frameBufferSz 19200

/* Refer to buffers defined in video/motorola/video.c */

extern unsigned char in[2][frameBufferSz];
extern unsigned char diff[frameBufferSz];
extern unsigned char compressed[frameBufferSz+1];

int state3 (void)
{
  packet pkt;
  int errno, current=0, last=1, csz, x, y, *a, *b, *c,doorbell=0, ns;
  int timeout=10;

  /* Set retransmit flag to 1 for first transmission */
  int retransmitFlag=1, changeState=0;

  /* Reinit Camera */

  QC_init();

  /* Inform client that buzzer has been pressed */
  
  while((changeState==0)&&(timeout!=0))
    {
      timeout--;
      /* Test for retransmit flag */
      if(retransmitFlag==1)
	{
	  /* Reset last frame so that when it is XORd a full picture is
	     transmitted */
	      for(x=0;x<frameBufferSz;x++)
		{
		  in[last][x]=0;
		}
	}

      /* Attempt to grab a frame */
      if ((errno = QC_grabframe (in[current])) == QC_SUCCESS)
	{
	  /* Correction and smoothing */
	  for(x=0;x<frameBufferSz;x++)
	    {
	      if (in[current][x] != 0)
		in[current][x]=16-in[current][x];
	    }

	  /* XOR frame with previous to obtain difference
	     Here I exploit the fact that the 68040 is 32 bit
	     so by treating the bytes as ints I can XOR
	     4 bytes at a time.... clever huh?
	  */
	  a=(int *) in[current];
	  b=(int *) in[last];
	  c=(int *) diff;
	  for (x = 0; x < (frameBufferSz / 4); x++)
	    c[x] = a[x] ^ b[x];

	  /* Use RLE to compress the difference 
	     (We increment compressed by 1 to make room for
	     a control byte) */
	  csz=RLEencode(diff,frameBufferSz,compressed+1);
	  /* Tell the receiver whether this is the first frame in
	     a sequence (compressed[0]=1) or not (compressed[0]=0) */
	  compressed[0]=retransmitFlag;
	  retransmitFlag=0;
	  /* Construct Packet */

	  pkt.datatype=VIDEO;
	  pkt.length=csz/8+2;
	  pkt.data=compressed;
	  
	  /* Place it in serial Queue and check for change of state
	     instruction from client */

	  ns=senddata(&pkt);
	  if(ns > 0)
	    {
	      changeState=ns;
	    }
	  /* Reverse what is current and what is previous */
	  current=1-current;
	  last=1-last;
	}
      else
	{
	  /* Handle Failed Framegrab */
	  switch (errno)
	    {
	    case QC_BAD_RESPONSE:
	      puts ("Framegrab: Bad response from camera\n\r");
	      break;
	    case QC_TIMEOUT:
	      puts ("Framegrab: Camera timed out\n\r");
	      break;
	    }
	}
    };
  if(timeout==0)
    changeState=2;
  return changeState;
}
