/*
  Motorola Video Compression and Transmission Code
  $Id: video.c,v 1.10 1998/05/07 14:06:28 iic Exp iic $
  $Log: video.c,v $
  Revision 1.10  1998/05/07 14:06:28  iic
  Removed CRC checking, seemed unnescessary and over fussy

  Revision 1.9  1998/04/30 10:04:50  iic
  Fixed incorrectly used Enum

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

/* Define buffers */

#define frameBufferSz 19200

unsigned char in[2][frameBufferSz];
unsigned char diff[frameBufferSz];
unsigned char compressed[frameBufferSz+1];
