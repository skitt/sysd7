/* test program for downloading database to Motorola */

#include "../dbm_defs.h"
extern int dbdl_done;

/* declare the database memory */
local_db_item db_local_store[MAX_USERS];
extern void handle_control_data(packet *p);

void main()
{
  serialinit();
  printf("Initialised serial.\n");
  printf("Listening for motorola...");
  if(serialreceive() == 0xFF){
    printf("connected.\n");
    serialtransmit(0xFF);
    registerhandler(DATABASE,handle_control_data);
    while(!dbdl_done);
    printf("Send database. Exiting.\n");
  }
  serialclose();
}
  
