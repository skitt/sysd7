/*
  Motorola Video Compression and Transmission Code
  $Id: video.c,v 1.8 1998/04/29 13:42:01 iic Exp iic $
  $Log: video.c,v $
  Revision 1.8  1998/04/29 13:42:01  iic
  Used data.VIDEO definition in data.h

  Revision 1.7  1998/04/28 14:59:44  iic
  Minor changes

  Revision 1.6  1998/04/28 12:34:23  iic
  Fixed some bugs found during testing, but still not tested with
  serial code.

  Revision 1.5  1998/04/27 16:25:50  iic
  Completed transmission code.  Now it *should* work but can't test
  until serial code is up and running

  Revision 1.4  1998/04/27 15:18:12  iic
  Made XOR code about 4 times faster by treating char array as an int array

  Revision 1.3  1998/04/27 15:05:17  iic
  Fixed some minor bugs so that it now syntax checks

  Revision 1.2  1998/04/27 14:41:38  iic
  Created runVideo() function, init camera, XOR and compress, but not
  yet transmission

*/

#include <motorola.video.h>
#include <serial.h>
#include <m68kboard.h>
#include <quickcam.h>
#include <support.h>
#include <data.h>

/*
  Some bit manipulation utility functions, designed by Ian Clarke
  (iic@dcs.ed.ac.uk), this implementation due to Stephen Kitt
  (skk@dcs.ed.ac.uk).
*/

void setbit (unsigned char * ptr, unsigned long off, int val)
{
  unsigned int idx, bit;
  idx = (unsigned int) (off / 8);
  bit = 7 - ((unsigned int) (off % 8));
  if (val)
    ptr [idx] |= 1 << bit;
  else
    ptr [idx] &= ~(1 << bit);
}

unsigned char bitset[] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F,
			   0x7F, 0xFF };

/* Maximum bits set: 8 */
/* This works by splitting the changes over at most two bytes, zeroing
   out the changed sections, and OR'ing the changes.
   For example, say we have the following two bytes:
           01100000 00000000
   and we want to store the value 011111 at offset 3; this splits into
   two bytes, 00001111 and 1000000. We zero out the changed area
   (nothing changes) and OR to get:
           01101111 10000000
   which is the desired result. */
void setval (unsigned char * ptr, unsigned long off, int val, int
	     bits)
{
  int bit;
  unsigned char * realptr;
  int coff;
  realptr = ptr + (off / 8);
  bit = off % 8;
  coff = bits + bit - 8;
  if (coff > 0)
    {
      realptr [0] &= ~(bitset [bits] >> coff);
      realptr [1] &= ~(bitset [bits] << (8 - coff));
      realptr [0] |= val >> coff;
      realptr [1] |= val << (8 - coff);
    }
  else
    {
      coff = -coff;
      *realptr &= ~(bitset [bits] << coff);
      *realptr |= val << coff;
    }
}

/*
  `src' is a pointer to an array of unsigned chars of length sLen
   where only the 4 least significant bits are to be compressed.
  `dst' is a pointer to a buffer which should be of at least
   size (sLen*8)/5+1.
  This buffer will be filled with a RLE encoded version of src
  and the function will return the compressed length in bits.
                                                        ^^^^
 */

int RLEencode(unsigned char * src, int sLen, unsigned char * dst)
{
  int sPtr, dPtr=0 , count=1;
  for(sPtr=1;sPtr<sLen;sPtr++)
    {
      if ((src[sPtr]==src[sPtr-1])&&(count!=33))
	{
	  count++;
	}
      else
	{
	  if (count==1)
	    {
	      setbit(dst,dPtr,0); dPtr++;
	      setval(dst,dPtr,src[sPtr-1],4);
	      dPtr+=4;
	    }
	  else
	    {
	      setbit(dst,dPtr,1); dPtr++;
	      setval(dst,dPtr,src[sPtr-1],4);
	      dPtr+=4;
	      setval(dst,dPtr,count-2,5);
	      dPtr+=5;
	    }
	  count=1;
	}
    }
  if (count==1)
    {
      setbit(dst,dPtr,0); dPtr++;
      setval(dst,dPtr,src[sPtr-1],4);
      dPtr+=4;
    }
  else
    {
      setbit(dst,dPtr,1); dPtr++;
      setval(dst,dPtr,src[sPtr-1],4);
      dPtr+=4;
      setval(dst,dPtr,count-2,5);
      dPtr+=5;
    }
  return dPtr;
}

#define frameBufferSz 19200
#define SMOOTHING

unsigned char in[2][frameBufferSz];
unsigned char diff[frameBufferSz];
unsigned char compressed[frameBufferSz];
packet pkt;

void main()
{
  int errno, current=0, last=1, csz, x, rt;
  int *a,*b,*c;
  retransmit=1; /* Ensure last frame is clear to avoid confusion */
  /* Initialise the camera */
  serialinit();
  if((errno=QC_init())==QC_SUCCESS)
    {

	  rt=retransmit;
	  retransmit=0;
	  /* Test for retransmit flag */
	  if(rt==1)
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
#ifdef SMOOTHING
		   if(in[current][x-1]==in[current][x+1])
		     in[current][x]=in[current][x-1];
#endif
		}
	      /* XOR frame with previous to obtain difference
	         Here I exploit the fact that the 68040 is 32 bit
	         so by treating the bytes as ints I can XOR
	         4 bytes at a time.... clever huh?
	      */
	      a=(int *) in[current];
	      b=(int *) in[last];
	      c=(int *) diff;
	      for(x=0;x<(frameBufferSz/4);x++)
		c[x] = a[x] ^ b[x];

	      /* Use RLE to compress the difference 
	       (We increment compressed by 1 to make room for
	        a control byte) */
	      csz=RLEencode(diff,frameBufferSz,compressed+1);
	      
	      /* Tell the receiver whether this is the first frame in
	         a sequence (compressed[0]=1) or not (compressed[0]=0 */
	      compressed[0]=rt;
	      
	      /* Construct Packet */

	      pkt.datatype=5;
	      pkt.length=csz/8+2;
	      pkt.data=compressed;

	      /* Place it in serial Queue */

   
		if( senddata(&pkt) != 0)
		  retransmit=1;
	      
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
    }
  else
    {
      /* Handle Failed Init */
      switch (errno)
	{
	case QC_BAD_RESPONSE:
	  puts ("Init: Bad response from camera\n\r");
	  break;
	case QC_TIMEOUT:
	  puts ("Init: Camera timed out\n\r");
	  break;
	}
    }
}
