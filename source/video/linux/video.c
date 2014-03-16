/*
  Linux video reception and decompression code
  $Id: video.c,v 1.11 1998/05/07 14:04:44 iic Exp iic $
  $Log: video.c,v $
  Revision 1.11  1998/05/07 14:04:44  iic
  Added restartVideo - Untested

  Revision 1.10  1998/05/07 13:40:09  iic
  Waaaaaaaaaaaaaaaahhhhhhhhhhhhhhoooooooooooooo!  IT WORKS!

  Revision 1.9  1998/05/07 13:01:16  iic
  Works, but still has loads of debugging stuff (incl CRC)

  Revision 1.8  1998/05/06 15:20:25  iic
  Added CRC checking to Video - not yet tested it though

  Revision 1.7  1998/05/06 14:58:47  skk
  Initial changes to merge with serial.

  Revision 1.6  1998/05/04 12:16:31  iic
  Now ready for testing, will output image to STDOUT as text

  Revision 1.5  1998/04/30 10:06:07  iic
  Fixed ENUM bug.

  Revision 1.4  1998/04/29 13:41:38  iic
  Used data.VIDEO definition in data.h

  Revision 1.3  1998/04/28 13:25:32  iic
  Minor commenting changes.

*/

#include <serial.h>
#include <linux.video.h>
#include <data.h>
#include <control.h>

/*
  Some bit manipulation utilities
*/

int getbit (unsigned char * ptr, unsigned long off)
{
  unsigned int idx, bit;
  idx = (unsigned int) (off / 8);
  bit = 7 - ((unsigned int) (off % 8));
  return (ptr [idx] & (1 << bit)) >> bit;
}

int getval (unsigned char * ptr, unsigned long off, int bits)
{
  int result, i;
  result = 0;
  for (i = 0; i < bits; i++)
    result |= getbit (ptr, off + i) << (bits - i - 1);
  return result;
}

/*
  src is a pointer to an array of unsigned chars which is the output
  of RLEencode.  dst is a pointer to an empty buffer of size dLen.
  RLEdecode will decompress the data into dst, stopping short if it
  is about to exceed the length of the buffer.  If the buffer is too
  long the remaining space may be filled with junk.
 */
  
void RLEdecode(unsigned char * src, unsigned char * dst, int dLen)
{
  int sPtr=0, dPtr=0, x;
  unsigned char count, c;
  while(dPtr<dLen)
    {
      if (getbit(src,sPtr)==0)
	{
	  sPtr++;
	  dst[dPtr]=getval(src,sPtr,4);
	  dPtr++;
	  sPtr+=4;
	}
      else
	{
	  sPtr++;
	  c=getval(src,sPtr,4);
	  sPtr+=4;
	  count=getval(src,sPtr,5);
	  sPtr+=5;
	  for(x=dPtr;(x<dPtr+count+2)&&(x<dLen);x++)
	    {
	      dst[x]=c;
	    }
	  dPtr=x;
	}
    }
}

/* Video Init routine.  Registers video handler with serial interface */

void handleVideo(packet * p);

void initVideo()
{
  registerhandler(VIDEO, (packethandler) handleVideo);
}

#define frameBufferSz 19200

/* Declare buffer for incoming data */

unsigned char incoming[frameBufferSz+1];

/* Declare buffer for XORing data */

unsigned char out[2][frameBufferSz];

/* Video handler */

void handleVideo(packet * p)
{
  int x, y, *a, *b, *c;
  static int current=0, last=1;
  RLEdecode(&(p->data[1]), incoming, frameBufferSz);

  /* If this is the first frame in a sequence then reset the previous frame */
  if (p->data[0]==1)
    {
      printf("**First frame in image\n");
      for(x=0;x<19200;x++)
	out[last][x]=0;
    }

  /* XOR uncompressed data with previous frame to obtain new image */

  for (x = 0; x < 19200; x++)
    out [current][x] = incoming [x] ^ out [last][x];
  display_image(out[current]);
  
  /* Reverse what is current and what is last */
  current=1-current;
  last=1-last;
}
