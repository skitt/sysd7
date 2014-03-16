#ifndef __SERIAL_H__
#define __SERIAL_H__

/* Serial interface definitions
   $Id: serial.h,v 1.7 1998/04/30 08:28:20 skk Exp skk $
   $Log: serial.h,v $
   Revision 1.7  1998/04/30 08:28:20  skk
   Added init/shutdown functions.

   Revision 1.6  1998/04/29 14:53:57  skk
   Added ping function.

 */

/* External packet structure */
typedef struct
{
  unsigned char datatype;
  unsigned int length;
  unsigned char * data;
} packet;

/* Packet handler - this is a pointer to a function which takes a
   pointer to a packet and returns nothing */
typedef void (* packethandler) (packet *);

/* Initialises the serial stuff */
int serialinit (void);

/* Shuts the serial down */
int serialclose (void);

/* Queues a packet to be sent across the serial line */
/* Input: pointer to a packet */
/* Output: 0 for success, errno-type code otherwise (include
   <errno.h>) */
int senddata (packet *);

/* Registers a data handler for a given data type */
/* Input: datatype to be handled (can be DEFAULT), pointer to a
   function taking a packet as input */
/* Output: 0 for success */
int registerhandler (unsigned char, packethandler);

/* Removes the data handler for the given type; note that a default
   handler is always available (it discards everything) and will be
   reinstated if the default is removed */
/* Input: datatype to be removed */
int unregisterhandler (unsigned char);

/* Returns 1 if the opposite communicator is alive, 0 if it isn't */
int ping (void);

/* Grabs a frame from the camera; this ensures we don't suffer from
   the camera v serial conflict. */
int grabframe (unsigned char *);

#endif /* ndef __SERIAL_H__ */
