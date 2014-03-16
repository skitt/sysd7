/* Dummy routines for testing the database (with everything running on Linux)
/* Edward Knowelden (ejk) */

/* dummy linux senddata function */

#include "dbm_defs.h"

int lnx_senddata(packet p)
{
  int count;
  printf("Datatype: %i\n",p.datatype);
  printf("Data sent:\n");
  for(count=0;count<p.length;count++) putchar(*(p.data + count));
  /* pretend message reached destination */
  handle_db_data(p);

  return TRUE;
}

/* Dummy motorola senddata function */
void mtr_senddata(packet p)
{
  /* pretend message reached destination...*/
  if((p.datatype = DATABASE) && (atoi(p.data) == REQUEST_DBDL)){
    printf("Lnx Control received REQUEST_DBDL. Sending data\n");
  }
  db_download();
}

/* main - simulates a motorola DB request */

void main()
{
  /* send a request for the database */
  request_db();

  printf("Sending uid...\n");
  send_DR_value("111111111111");
  send_keypad_digit(1);
  send_keypad_digit(3);
  send_keypad_digit(3);
  send_keypad_digit(4);

  send_DR_value("111111111111");
  send_keypad_digit(1);
  send_keypad_digit(3);
  send_keypad_digit(3);
  send_keypad_digit(4);

  send_DR_value("010101010101");
  send_keypad_digit(1);
  send_keypad_digit(1);
  send_keypad_digit(2);
  send_keypad_digit(3);

  printf("Finished.\n");
}

/* dummy unlock door function */
void unlock_door()
{
  printf("Door is now open!\n");
  exit(1);
}

