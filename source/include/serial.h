#ifndef __SERIAL_H__
#define __SERIAL_H__

/* Serial interface definitions
   Stephen Kitt

   Note that this requires SDPMotorola to be defined if used in
   Motorola code
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

/* Transmits a single byte of data */
int serialtransmit (unsigned char);

/* Received a single byte of data, or -1 in case of timeout */
int serialreceive (void);

/* Sends a change state message */
#ifndef SDPMotorola
void changestate (unsigned char);
#endif

/* Receives a packet */
#ifdef SDPMotorola
packet * receivepacket (void);
#endif

/* Enables packets */
#ifndef SDPMotorola
void enablepackets (void);
#endif

/* Disables packets */
#ifndef SDPMotorola
void disablepackets (void);
#endif

#endif /* ndef __SERIAL_H__ */
