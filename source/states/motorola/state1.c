/* State 1 
 * Initialisation 
 * Compile with:
 * source/control/motorola/db_receive.c
 * 
 */

#include <database.h>

/* hardcoded replacement for database download (!) */
local_db_item db_local_store[] = {
  { "0000018881A4",1234}, 
  { "00000188756E",1000}, 
  { "000001888321",1111}, 
  { "0000018874EC",5436}, 
  { "000001888A56",1184}, 
  { "000001887E6E",4321},
  { "X",0}}
;

int state1()
{
  boardinit();  
  serialinit();


  /* database download. Not working yet! */
#if 0
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
#endif  
  return 2;
}
