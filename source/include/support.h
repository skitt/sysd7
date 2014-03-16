#ifndef __SUPPORT_H__
#define __SUPPORT_H__

/* General support routines, supplied by libsupport
   Stephen Kitt
*/

#ifdef SDPMotorola
/* Motorola section */

/* puts outputs the string given as argument on the console (either
   Minicom on channel A (ttyS0) or stdout)
*/
void puts (char *);

#else
/* Linux section */

#include <stdio.h>

#endif /* SDPMotorola */

/* Common section */

/* putn outputs the number given as argument in the specified base
   with the specified number of digits on the console
*/
void putn (unsigned long, int, int);

#endif /* __SUPPORT_H__ */
