/* State 5 
 * Authenticate 
 * Compile with: 
 * ~/source/control/motorola/validate.c
 * ~/source/control/motorola/m_support.c
 * ~/source/control/support.c
 * ~/source/low.level/board.c
 */

#include <database.h>
#include <board.h>

/* globals */

int pin_pos = 0;
char uid_no[13];
int got_uid=FALSE;
int pin;

int state5()
{
  ringinfo dring;
  unsigned char digit,c;
  int i, t;

  /* read the Dallas ring */
  while(!checkring());
  dring = getringinfo();
  
  /* bleep twice */
  starttimer (10000000);
  buzzerled (BUZZER);
  while (gettimer () > 10000000 - 30000);
  buzzerled (ALLOFF);
  while (gettimer () > 10000000 - 40000);
  buzzerled (BUZZER);
  while (gettimer () > 10000000 - 70000);
  buzzerled (ALLOFF);
  stoptimer ();

  /* convert serial no. to 12-digit hex */
  for(i=0;i<6;i++) conv_hchar((unsigned long)dring.id[i],&(uid_no[(i*2)]));
  uid_no[12] = '\0';

  /* set some states */
  got_uid = TRUE;
  pin_pos = 0;
  pin = 0;

  /* timeout */
  starttimer(5010000);
  
  /* get 4 keypad digits and bleep after each one */
  for(i=0;i<PIN_SIZE;i++){
    while(!checkkeypad() && gettimer () > 10000);
    if (gettimer () < 10000)
      return 2;
    digit = getkeypad();
    t = gettimer ();
    buzzerled (BUZZER);
    while (gettimer () > t - 30000);
    buzzerled (ALLOFF);
    /* register digit */
    if(send_keypad_digit(digit)) break;
  }
  /* validate user and move to a different state on outcome */
  if(validate_user(uid_no,pin))
    {
      pin = 0;
      pin_pos = 0;
      return 8;
    }
  else
    {
      pin = 0;
      pin_pos = 0;
      return 7;
    }
}


  
  
