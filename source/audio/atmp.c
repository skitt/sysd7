#include <stdio.h>
#include <stdlib.h>

/*
.---------------------------------------------------------------------------.
|  This is a lookup table of coded deltas.                                  |
|  i.e. nib_lookup[sample_delta] gives you the relevant 4-bit encoding.     |
|---------------------------------------------------------------------------|*/
unsigned char nib_lookup[]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,4,4,
  4,4,4,4,5,5,5,6,7,7,8,9,9,10,10,10,11,11,11,11,11,11,12,12,12,
  12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,
  13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,
  14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
  14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
  15,15,15,15,15,15,15};
/*                                                                          |
`---------------------------------------------------------------------------'*/



/*
.---------------------------------------------------------------------------.
|  This is the decoding table.  It performs the inverse mapping.            |
|  ie unnib_lookup[4_bit_code] returns the estimated delta for the sample.  |
|---------------------------------------------------------------------------|*/
char unnib_lookup[]={-128,-64,-32,-16,-8,-4,-2,-1,1,2,4,8,16,32,64,127};
/*                                                                          |
`---------------------------------------------------------------------------'*/


/***************************************************************************
 * Previous sample values for encoding and decoding respectively           *
 *                                                                         */
unsigned char prev_val_c, prev_val_d;
/***************************************************************************/


/* 
   Initialise compression
*/
void initCompr()
{
  prev_val_c=0;
}

/*
  Initialise decompression
*/

void initDecompr()
{
  prev_val_d=0;
}

/*
  Takes a byte and compresses it, which is returned
*/

unsigned char compressI(unsigned char sample)
{
  unsigned char ret;
  ret=nib_lookup[128+sample-prev_val_c];
  prev_val_c+=unnib_lookup[ret];
  fprintf(stderr,"%d>%d>%d\n",sample,ret,prev_val_c);
  return ret;
}

/* Takes two unsigned 8-bit samples and compresses them into 1 byte,
   which is returned


unsigned char compress(unsigned char sample1, unsigned char sample2)
{
  return compressI(sample1)|(compressI(sample2)<<4);
}
*/
unsigned char compress(unsigned char *src)
{
  return compressI(src[0])|(compressI(src[1])<<4);
}


/* Takes a byte s and returns it's decompressed version */

unsigned char decompressI(unsigned char s)
{
  prev_val_d+=unnib_lookup[s];
  return prev_val_d;
}

/* Takes a byte s and a pointer to a two byte buffer to which
   s will be decompressed */

void decompress(unsigned char s, unsigned char *dst)
{
  dst[0]=decompressI(s & 0x0F);
  dst[1]=decompressI((s & 0xF0) >> 4);
}

/* Test code

void main(void)
{
  int i;
  unsigned char testin[] = {120,100,60,30,-20,-100,-120,-123};
  unsigned char testout[8];
  unsigned char compressed[4];
  initDecompr();
  printf("Compressed:");
  for(i=0;i<4;i++)
    {
      compressed[i]=compress(testin[i*2],testin[i*2+1]);
      printf("%d ",compressed[i]);
    }
  printf("Uncompressed:");
  for(i=0;i<4;i++)
    {
      decompress(compressed[i],testout+i*2);
    }
  for(i=0;i<8;i++)
    {
      printf("%d ",testout[i]);
    }
  printf("\n");
}

*/
