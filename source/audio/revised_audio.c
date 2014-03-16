/* audio.c
   -- contains the definitions for the audio codec routines.
   -- I have revised the encoding lookup table to allow deltas of
      +/-255. I think that this may cure some of our audio problems!
*/
#include <audio.h>

/*
.---------------------------------------------------------------------------.
|  This is a lookup table of coded deltas.                                  |
|  i.e. nib_lookup[sample_delta] gives you the relevant 4-bit encoding.     |
|---------------------------------------------------------------------------|*/
unsigned char nib_lookup[]={
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 7,

  7, 8, 9, 9, 10,10,10,11,11,11,11,11,11,12,12,12,
  12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,
  13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
  13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
  14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
  14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};

  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};

/*                                                                          |
`---------------------------------------------------------------------------'*/



/*
.---------------------------------------------------------------------------.
|  This is the decoding table.  It performs the inverse mapping.            |
|  ie unnib_lookup[4_bit_code] returns the estimated delta for the sample.  |
|---------------------------------------------------------------------------|*/
char unnib_lookup[]={-128,-64,-32,-16,-8 ,-4 ,-2 ,-1, 1 , 2 , 4 , 8 ,16 ,32 ,64 ,127};
/*                                                                          |
`---------------------------------------------------------------------------'*/


/***************************************************************************
 * Previous sample values for encoding and decoding respectively           *
 *                                                                         */
unsigned char prev_val_c, prev_val_d;
/***************************************************************************/


/* 
   Initialise compression
   set the previous sample value to 128 which is the normal zero value for 
   sampling.
*/
void initCompr()
{
  prev_val_c=128;
}

/*
  Initialise decompression
  Same as above.
*/

void initDecompr()
{
  prev_val_d=128;
}

/**************************************************
      --=[  Compression routines.   ]=--
**************************************************/
/*
  compressI() Takes an unsigned 8-bit sample and compresses it,
  returns the 4-bit encoding in the lower 4 bits of the byte.
*/

unsigned char compressI(unsigned char sample)
{
  unsigned char ret;
  /* Find the estimated delta encoding */
  ret=nib_lookup[256+sample-prev_val_c];
  /* modify the prev_val to reflect the decompressed estimation */
  prev_val_c+=unnib_lookup[ret];
  /* Stop and return the compressed value. */
  return ret;
}

/*
  Takes two unsigned 8-bit samples and compresses them into 1 byte,
  which is returned with the first sample's encoding in the lower
  half of the byte and the second one's in the high half.
*/

unsigned char compress(unsigned char sample1, unsigned char sample2)
{
  unsigned char t1,t2;
  /* Compress the first sample and store it temporarily. */
  t1=compressI(sample1);
  /* Compress the second sample, bit shift it 4 places up the byte
     and append the encoding of the first sample */
  t2=(compressI(sample2)<<4)|t1;
  /* Stop and return the two encodings packed as one byte. */
  return t2;
}


/**************************************************
      --=[ Decompression routines. ]=--
**************************************************/

/*
  Takes a byte, s, containing the right-justified 4-bit encoding of the
  sample delta and returns the estimation of the next sample.
*/

unsigned char decompressI(unsigned char s)
{
  /* Find the estimated delta for the encoding and modify the previous
     sample accordingly. */
  prev_val_d+=unnib_lookup[s];
  /* Stop and return the estimation of the next sample. */
  return prev_val_d;
}

/*
  Takes a byte, s, containing two, 4-bit delta encodings,and a pointer
  to a two byte buffer to which s will be decompressed this buffer will
  then contain the next two samples to be played with the first byte in
  the buffer representing the first of the two samples.
*/

void decompress(unsigned char s, unsigned char *dst)
{
  /* extract the lower half of the byte and use it to produce the estimated
     sample to be played first. */
  dst[0]=decompressI(s & 0x0F);
  /* extract the high half of the byte and shift it into the lower half,
     then use it to create the second sample of the pair. */
  dst[1]=decompressI(((s & 0xF0)>>4)&0x0F);
}
