#ifndef _motorola_video_h_
#define _motorola_video_h_

/*
  Motorola Video Interface
*/

/* 

   /* RLE Encoding function:
 `src' is a pointer to an array of unsigned chars of length sLen
   where only the 4 least significant bits are to be compressed.
  `dst' is a pointer to a buffer which should be of at least
   size (sLen*8)/5+1.
  This buffer will be filled with a RLE encoded version of src
  and the function will return the compressed length in bits.
                                                        ^^^^
							*/

int RLEencode(unsigned char *, int, unsigned char *);


#endif
