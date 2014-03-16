/* Test program for User Validation System on motorola */
/* Edward Knowelden */

#include <m68kboard.h>
#include <board.h>
#include "../dbm_defs.h"

int pin_pos = 0;
char uid_no[13];
int got_uid=FALSE;
char pin[PIN_SIZE+1];

/* Database is included as we don't have comms */
/* maybe store pin as ints ?*/

/*local_db_item db_local_store[] = {
  { "0000018881A4","1234"}, 
  { "00000188756E","1000"}, 
  { "000001888321","1111"}, 
  { "0000018874EC","5436"}, 
  { "000001888A56","1184"}, 
  { "X",""}
};*/

/* declare storage space */
local_db_item db_local_store[MAX_USERS];

void main()
{
  ringinfo dring;
  unsigned char digit,c;
  int i;

  boardinit();  
  serialinit();

  puts("Waiting for connection...");
  /* transmit 'ping' character */
  serialtransmit(0xFF);
  puts("sent ping\n\r");
  /* wait for return ping */
  if(serialreceive() == 0xFF){
      puts("connected\n\r");
      request_db(); 
      puts("Closing serial\n\r");
  }
  serialclose ();
  
  /* Check DR and keypad in infinite loop */
  /* poll DR until we get input (unsupported by lowlevel) */

  puts("\n\r");
  puts("Ready for iButton:");

  while(!checkring());
  dring = getringinfo();

  starttimer (10000000);
  buzzerled (BUZZER);
  while (gettimer () > 10000000 - 30000);
  buzzerled (ALLOFF);
  while (gettimer () > 10000000 - 40000);
  buzzerled (BUZZER);
  while (gettimer () > 10000000 - 70000);
  buzzerled (ALLOFF);
  stoptimer ();

  /*send_DR_value(dring.id); */
  
  for(i=0;i<6;i++){
    
    /* printf("C:%2.2X \n",(unsigned int)dring.uid[i]);*/
    conv_hchar((unsigned long)dring.id[i],&(uid_no[(i*2)]));
  }
  uid_no[12] = '\0';
  
  puts("Converted string: ");
  puts(uid_no);

  got_uid = TRUE;
  pin_pos = 0;

  puts("Enter PIN: ");
 
  for(i=0;i<PIN_SIZE;i++){
    
    while(!checkkeypad());
    digit = getkeypad();
    starttimer (10000000);
    buzzerled (BUZZER);
    while (gettimer () > 10000000 - 30000);
    buzzerled (ALLOFF);
    
    send_keypad_digit(digit);


    while(checkkeypad());
  }

  
}

/* these are different states in real life */
void refuse_entry()
{
  int i;
  for (i = 0; i < 5; i++)
    {
      buzzerled (LEDRED);
      starttimer (210000);
      while (gettimer () > 110000);
      buzzerled (ALLOFF);
      while (gettimer () > 10000);
      stoptimer ();
    }
  puts("The door is unlocked");
}

void unlock_door()
{
  starttimer (3010000);
  buzzerled (LEDGREEN);
  while (gettimer () > 2010000);
  while (gettimer () > 10000 && !getdoorstatus ());
  stoptimer ();
  buzzerled (ALLOFF);
}
