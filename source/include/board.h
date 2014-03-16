#ifndef __BOARD_H__
#define __BOARD_H__

/* Low-level board functions
   Stephen Kitt

   Note that not all these functions are actually implemented...
*/

/* Timer functions */
void starttimer (int);
int stoptimer (void);
inline int gettimer (void);

/* Checks for the presence of a ring in the Dallas ring reader;
   returns 1 if there is a ring, 0 otherwise */
int checkring (void);

/* Ring information structure */
typedef struct
{
  unsigned char product;
  unsigned char id [6];
  unsigned char crc;
} ringinfo;

/* Returns the ring information as a pointer to a ringinfo structure
 */
ringinfo getringinfo (void);

/* Checks for the presence of a digit on the keypad; returns 1 if
   there is data, 0 otherwise.
   Note that to get a PIN, we need to wait for this to change to 1,
   get the digit, wait for this to change back to 0, and loop. */
int checkkeypad (void);

/* Returns the digit currently on the keypad */
#define KP_STAR 0x0A
#define KP_HASH 0x0B
#define KP_DOORBELL 0x0C
#define KP_UNDEF 0xFF
unsigned char getkeypad (void);

/* Returns the current sample from the ADC */
unsigned char getsample (void);

/* Sends a new sample to the DAC */
void setsample (unsigned char);

/* RTC clock structure */
typedef struct
{
  unsigned char seconds;
  unsigned char minutes;
  unsigned char hours;
  unsigned char day;
  unsigned char date;
  unsigned char month;
  unsigned char year;
} rtctime;

/* Get the current RTC time */
rtctime getrtctime (void);

/* Set the current RTC time */
void setrtctime (rtctime);

/* Display a character on the LED display; use -1 to blank */
void leddisplay (unsigned char);

/* Get data from the NVRAM - returns a pointer to a list of bytes
   extracted from the NVRAM, from the first argument to the last
   inclusive; these can range from 0 to 759 */
unsigned char * getnvram (int, int);

/* Set data in the NVRAM */
void setnvram (unsigned char *, int, int);

/* Returns the door's status - 1 is close, 0 is open */
int getdoorstatus (void);

/* Opens the door - returns 0 in case of success, -1 otherwise */
int opendoor (void);

/* Refuse entry */
int refuseentry (void);

/* Executes a no-op */
#define nop() asm ("nop")

/* Activates the buzzer or the LEDs */
enum { ALLOFF, BUZZER, LEDGREEN, LEDRED };
int buzzerled (int);

#endif /* ndef __BOARD_H__ */
