/* System Design Group 7
 * control_rt_040.h -- Runtime control for motorola board
 * EXTERNAL INTERFACE SPECIFICATION
 * Edward Knowelden (ejk)
 */

#ifndef __CTRL_RT_040
#define __CTRL_RT_040

#include "serial.h"

void send_keypad_digit( int digit );
/* Called by LOW_LEVEL upon recieving a digit from the keypad. */

void send_DR_value( int val ); 
/* Called by LOW_LEVEL upon recieving a value from the Dallas Ring
 * reader. (an array may be used if UID is too long for int) */

void doorbell();
/* Called by LOW_LEVEL if the doorbell is pressed. Will initiate
 * audio/video transmission. Subsequent calls have no effect until GUI
 * unlocks door or refuses entry. */

BOOL handle_ctrldata_040( packet_t packet );
/* Called by SERIAL_TRANSPORT_040 when it has a packet of data for the
 * control module. The packet_t datatype is defined by the
 * SERIAL_TRANSPORT module as indicated by the #include. Used to
 * communicate (for example) "unlock door" from remote GUI. In
 * practice, an int may used instead of packet_t as no large bits of
 * data need to be sent; (program download will be handled by
 * CONTROL_SU_040). BOOL is for error check.  */

#endif


