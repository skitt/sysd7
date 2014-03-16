/*
  Compression / Decompression Testbed
  Ian Clarke (iic)
*/

#include<stdio.h>

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
  int i, bit;
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

/*
  src is a pointer to an array of unsigned chars which is the output
  of RLEencode.  dst is a pointer to an empty buffer of size maxL.
  sLen is the number of compressed **bits** in src as given by
  the RLEencode function.
  RLEdecode will decompress the data into dst, and will return the
  size of the data in dst in bytes, or it will return 0 if this size
  exceeded maxL and RLEdecode aborted the decompression.
 */
  
int RLEdecode(unsigned char * src, unsigned char * dst, int sLen, int maxL)
{
  int sPtr=0, dPtr=0, x;
  unsigned char count, c;
  while((sPtr<sLen)&&(dPtr<=maxL))
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
	  for(x=dPtr;x<dPtr+count+2;x++)
	    {
	      dst[x]=c;
	    }
	  dPtr=x;
	}
    }
  if (dPtr>maxL)
    dPtr=0;
  return dPtr;
}

/* Length of test data */
#define len 10

void main()
{
  int x,c;
  unsigned char in[]={1,9,2,8,3,7,5,5,5,6};
  unsigned char compressed[len];
  unsigned char out[len];
  
  c=RLEencode(in,len,compressed);
  printf("Compressed Length:%d\n",c);
  for(x=0;x<c;x++)
    {
      printf("%d",getbit(compressed,x));
    }
    printf("\n");
  printf("Decompressing...\n");
  c=RLEdecode(compressed,out,c,len);
  for(x=0;x<c;x++)
    {
      printf("%d",(unsigned int) out[x]);
    }
  printf("\n");
}
