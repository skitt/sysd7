/* Database User Validation functions 
 * Edward Knowelden
 */
/* Needs libsupport.a library for puts */
/* Needs m_support.c for string functions */
/* NB. most global database variables can be local to the state
   (except database iteself */

#include <database.h>

extern local_db_item db_local_store[];
extern int pin_pos;
extern char uid_no[13];
extern int got_uid;
extern int pin;

int send_keypad_digit(int digit)
{
  int pin_int, result;

  if(got_uid){
    pin = pin * 10 + digit;
    pin_pos++;
    if(pin_pos == PIN_SIZE){
      return TRUE;
    }
  }
  return FALSE;
}

/* validate user */

int validate_user(char *uid, int pin)
{
  int count = 0,i;
  /* simple search for uid */
  while(strcomp(uid,db_local_store[count].uid) != 0) {
    if(db_local_store[count].uid[0] == 'X') break;
    else count++;
  }
  if(db_local_store[count].uid[0] == 'X') {
    return FALSE;
  }

  if (pin == db_local_store [count].pin)
    return 1;
  else return 0;
}

/* converts a 6 digit string from dallas ring into 12-digit hex serial number */
void conv_hchar (unsigned long number, char *buffer)
{
  unsigned long magnitude;
  unsigned char digit;
  int digits = 2;
  int base =16;
  int i=0;

  magnitude = 1;
  while (digits > 1)
    {
      magnitude *= base;
      digits--;
    };
  while (magnitude > 0){
    digit = number / magnitude;
    if (digit < 10)
      buffer[i] = ('0' + digit);
    else
      buffer[i] = ('A' - 10 + digit);
    number %= magnitude;
    magnitude /= base;
    i++;
    buffer[i] = '\0';
  }
}

