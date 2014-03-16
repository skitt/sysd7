/* Motorola state 2 - video
   
   This state performs 4 functions

   * it polls the video camera, it captures an image and uses a combination
     of frame-difference and RLE encoding to compress the image before 
     sending it via serial to the Linux client machine;

   * it polls the Dallas Ring to detect whether a ring is present in the
     detector;

   * it checks for a signal from the client;

   * it polls the buzzer (linked to the keypad).

   Transitions
  
   The following states can be entered from state 2:

   * state 1 --- a signal is received from the client instructing the
     board to enter the initialisation state;

   * state 3 --- the buzzer is pressed;

   * state 5 --- a Dallas ring is detected.

   Otherwise state 2 will loop.

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

/*
  This function will grab a frame but will abort the grab if it detects
  that the buzzer is pressed or a dallas ring is present returning
  the state to change to.  0 is returned if the grab completes.
 */

int grabframe(char *buffer)
{
  int x, ret=0;
  for(x=0;x<20;x++)
    {
      QC_grabdata(buffer+(x*(19200/20)),19200/20);
      if (checkkeypad())
	if(getkeypad()==KP_DOORBELL)
	  {
	    ret=3;
	    break;
	  }
      if (checkring())
	{
	  ret=5;
	  break;
	}
    }
  return ret;
}


/* Makes a noise and informs the PC that the doorbell has been pressed */

void informdoorbell (void)
{
  packet pkt;
  char buzzer_pressed=BUZZER_PRESSED;

  /* Make a noise */
  starttimer(40000);
  buzzerled(BUZZER);
  while(gettimer()>10000);
  buzzerled(ALLOFF);
  stoptimer();

  /* Send packet */
  pkt.datatype=CONTROL;
  pkt.data=&(buzzer_pressed);
  pkt.length=1;
  senddata(&pkt);
}

int state2 (void)
{
  packet pkt;
  int errno, current=0, last=1, csz, x, y, *a, *b, *c,doorbell=0, ns;
  /* Set retransmit flag to 1 for first transmission */
  int retransmitFlag=1, changeState=0;
  /* Init Camera */
  QC_init ();
  while(changeState==0)
    {
      /* Check doorbell */
      if (checkkeypad())
	if (getkeypad()==KP_DOORBELL)
	  {
	    informdoorbell ();
	    return 3;
	  }

      /* Check Dallas Ring */
      if (checkring())
	return 5;

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

      if ((changeState = grabframe (in[current])) == 0)
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
	    changeState=ns;
	  
	  /* Reverse what is current and what is previous */
	  current=1-current;
	  last=1-last;
	}
    };
  if (changeState == 3)
    informdoorbell ();
  return changeState;
}
