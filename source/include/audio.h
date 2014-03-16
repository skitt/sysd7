#ifndef __AUDIO_H__
#define __AUDIO_H__

/*
  Audio header file for audio compression utilities

  $Id: audio.h,v 1.2 1998/05/07 18:52:02 dwi Exp $
  $Log: audio.h,v $
  Revision 1.2  1998/05/07 18:52:02  dwi
  Just added '#ifndef's.

  Revision 1.1  1998/05/05 15:38:41  iic
  Initial revision

*/

/* Initialise compression */

void initCompr();

/* Initialise decompression */

void initDecompr();


/* Takes two bytes and compresses them into 1 which is returned */

unsigned char compress(unsigned char, unsigned char);

/* Takes a byte s and a pointer to a two byte buffer to which
   s will be decompressed */

void decompress(unsigned char, unsigned char *);

#endif /* ndef __AUDIO_H__ */
