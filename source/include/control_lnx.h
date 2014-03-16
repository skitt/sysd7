/* System Design Group 7
 * control_lnx.h -- Control module for Linux box
 * EXTERNAL INTERFACE SPECIFICATION
 * Edward Knowelden (ejk)
 */

#ifndef __CTRL_LNX_H__
#define __CTRL_LNX_H__

#include "serial.h"

BOOL handle_ctrldata_lnx( packet_t packet);
/* Called by SERIAL_TRANSPORT_LNX when it has a packet of data for the
 * control module. The packet_t datatype is defined by the
 * SERIAL_TRANSPORT module as indicated by the #include. Return val
 * used for error check. Rx communications include: 
 * - Start A/V processing (on doorbell press) [CONTROL_RT_040]
 * - Log this automatic entry [CONTROL_RT_040]
 * - Download program/database [CONTROL_SU_040] */

void stop_av();
/* Called by GUI when a "caller session" is ended. ie. caller is let
 * in or refused entry. Stops audio and video processing (although
 * this could be done directly). */

#endif
