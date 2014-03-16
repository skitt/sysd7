#ifndef __DATA_H__
#define __DATA_H__

/* Datatype definitions 
   $Id: data.h,v 1.9 1998/05/08 15:18:58 skk Exp skk $

*/

typedef enum
{
  DEFAULT,
  PING,         /* Ping, bidirectional */
  PING_ACK,     /* Ping acknowledge, bidirectional */
  CONTROL,      /* Control information */
  DATABASE,     /* Database information */
  AUDIO,        /* A compressed audio packet, bidirectional */
  VIDEO         /* A compressed video packet, board -> PC only */
} datatype_t;

#endif /* ndef __DATA_H__ */
