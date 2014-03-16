/* Test program for User Validation System on motorola */
/* No keypad/DR version */
/* Edward Knowelden */

/*#include <m68kboard.h>*/
#include <board.h>
#include "../dbm_defs.h"

void puts ( char * string);
/* Database is included as we don't have comms */

db_item db_local_store[] = {
  {"111111111111","1234"},
  {"111111111112","1000"},
  {"101010101010","3410"},
  {"010101010101","1123"}
};

void main()
{
  ringinfo *dring;
  unsigned char digit;
  int i;

  send_DR_value("111111111111");
  puts("Sent uid");
  send_keypad_digit(1);
  puts("Sent digit");
  send_keypad_digit(2);
  puts("Sent digit");
  send_keypad_digit(3);
  puts("Sent digit");
  send_keypad_digit(4);
}

void unlock_door()
{
  puts("The door is unlocked");
}

/* dummy puts */

void puts(char * string){
  printf("%s",string);
}

