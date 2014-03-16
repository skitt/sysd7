#include <stdio.h>

#include "crc.h"

/* Assumes AL = CRC, BL = byte, returns AL = CRC */
char crc8bytedef[] = { 0xB9, 0x08, 0x00,        /* MOV CX, 8 */
                       0xD0, 0xE3,              /* SHL BL, 1 */
                       0xD0, 0xD0,              /* RCL AL, 1 */
                       0x73, 0x02,              /* JNC +2 */
                       0x34, 0x07,              /* XOR AL, 7 */
                       0xE2, 0xF6,              /* LOOP -10 */
                       0xCB };                  /* RETF */

unsigned char crc8byte (unsigned char crc, unsigned char byte)
{
  void (* crc8asm) ();

  crc8asm = crc8bytedef;
  _BL = byte;
  _AL = crc;
  crc8asm ();
  return _AL;
}

unsigned char crc8 (void * data, unsigned long length, unsigned char end)
{
  unsigned char crc;
  unsigned long i;
  unsigned char * ptr;

  crc = 0;
  ptr = (unsigned char *) data;
  for (i = 0; i < length; i++)
    crc = crc8byte (crc, ptr [i]);
  return crc8byte (crc, end);
}

char crc16bytedef[] = { 0xB9, 0x08, 0x00,       /* MOV CX, 8 */
                        0xD0, 0xE3,             /* SHL BL, 1 */
                        0xD1, 0xD0,             /* RCL AX, 1 */
                        0x73, 0x03,             /* JNC +3 */
                        0x35, 0x21, 0x10,       /* XOR AX, 0x1021 */
                        0xE2, 0xF5,             /* LOOP -11 */
                        0xCB };                 /* RETF */

unsigned int crc16byte (unsigned int crc, unsigned char byte)
{
  void (* crc16asm) ();

  crc16asm = crc16bytedef;
  _BL = byte;
  _AX = crc;
  crc16asm ();
  return _AX;
}

char crc16word16def[] = { 0xB9, 0x10, 0x00,     /* MOV CX, 16 */
                          0xD1, 0xE3,           /* SHL BX, 1 */
                          0xD1, 0xD0,           /* RCL AX, 1 */
                          0x73, 0x03,           /* JNC +3 */
                          0x35, 0x21, 0x10,     /* XOR AX, 0x1021 */
                          0xE2, 0xF5,           /* LOOP -11 */
                          0xCB };               /* RETF */

unsigned int crc16word16 (unsigned int crc, unsigned int word16)
{
  void (* crc16asm) ();

  crc16asm = crc16word16def;
  _BX = word16;
  _AX = crc;
  crc16asm ();
  return _AX;
}

unsigned int crc16 (void * data, unsigned long length, unsigned int end)
{
  unsigned int crc;
  unsigned long i;
  unsigned char * ptr;

  crc = 0;
  ptr = (unsigned char *) data;
  for (i = 0; i < length; i++)
    crc = crc16byte (crc, ptr [i]);
  return crc16word16 (crc, end);
}

char crc32bytedef[] = { 0xB9, 0x08, 0x00,           /* MOV CX, 8 */
                        0xD0, 0xE3,                 /* SHL BL, 1 */
                        0xD1, 0xD0,                 /* RCL AX, 1 */
                        0xD1, 0xD2,                 /* RCL DX, 1 */
                        0x73, 0x07,                 /* JNC +7 */
                        0x35, 0xB7, 0x1D,           /* XOR AX, 0x1DB7 */
                        0x81, 0xF2, 0x81, 0x04,     /* XOR DX, 0x0481 */
                        0xE2, 0xEF,                 /* LOOP -17 */
                        0xCB };                     /* RETF */

unsigned long crc32byte (unsigned long crc, unsigned char byte)
{
  void (* crc32asm) ();
  unsigned int ax, dx;

  crc32asm = crc32bytedef;
  ax = (unsigned int) (crc >> 16);
  dx = (unsigned int) crc;
  _BL = byte;
  _AX = ax;
  _DX = dx;
  crc32asm ();
  dx = _DX;
  ax = _AX;
  return ((unsigned long) dx << 16) + ax;
}

char crc32word16def[] = { 0xB9, 0x10, 0x00,         /* MOV CX, 16 */
                          0xD1, 0xE3,               /* SHL BX, 1 */
                          0xD1, 0xD0,               /* RCL AX, 1 */
                          0xD1, 0xD2,               /* RCL DX, 1 */
                          0x73, 0x07,               /* JNC +7 */
                          0x35, 0xB7, 0x1D,         /* XOR AX, 0x1DB7 */
                          0x81, 0xF2, 0x81, 0x04,   /* XOR DX, 0x0481 */
                          0xE2, 0XEF,               /* LOOP -17 */
                          0xCB };                   /* RETF */

unsigned long crc32word32 (unsigned long crc, unsigned long word32)
{
  void (* crc32asm) ();
  unsigned int ax, dx, high16, low16;

  crc32asm = crc32word16def;
  dx = (unsigned int) (crc >> 16);
  ax = (unsigned int) crc;
  high16 = (unsigned int) (word32 >> 16);
  low16 = (unsigned int) word32;
  _AX = ax;
  _DX = dx;
  _BX = high16;
  crc32asm ();
  _BX = low16;
  crc32asm ();
  dx = _DX;
  ax = _AX;
  return ((unsigned long) dx << 16) + ax;
}

unsigned long crc32 (void * data, unsigned long length, unsigned long end)
{
  unsigned long crc;
  unsigned long i;
  unsigned char * ptr;

  crc = 0;
  ptr = (unsigned char *) data;
  for (i = 0; i < length; i++)
    crc = crc32byte (crc, ptr [i]);
  crc = crc32word32 (crc, end);
  return crc;
}

