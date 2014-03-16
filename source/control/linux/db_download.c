/* Functions for database download (Linux) via serial line.
 * Edward Knowelden 
 */

#include <stdlib.h>
#include <stdio.h>
#include "../dbm_defs.h"

extern int itos(int,char *,int);
extern db_item *database;
int dbdl_done = FALSE,next_flag = FALSE;

/* Control message structure:
   [XXX][\0][DDDDD.....D]
   where: XXX is message identifier (see below), \0 is null char
          DDD is unspecified data */

/* database buffer */
extern db_item db_store[MAX_USERS];

int db_download()
{
  packet dload,*next;
  int load_result, success;
  db_item *curr;
  local_db_item send_item;
  int sent = 0; 
  char eof = 0xFF;
  
  printf("Starting DB download\n");
  load_result = load_database();
  if(load_result == 0) {
    printf("Download problem");
    exit(1);
  }
  curr = database;
  
  dload.datatype = (datatype_t)DATABASE;
  dload.length = sizeof(local_db_item);
  
  while(curr != NULL){
    strcpy(send_item.uid,curr->uid);
    strcpy(send_item.pin,curr->pin);
    
    dload.data = (char *)&send_item;
    success = senddata(&dload);
    printf("Sent %s. Size %i\n",send_item.uid,dload.length);
    curr = curr->next;
    sent += sizeof(db_item);
    
    /* wait for sync signal */
    printf("Waiting for next flag\n");
    while(!next_flag)
      sleep(5);
    next_flag = FALSE;
    printf("Got next flag\n");
  }    
    /* send an EOF packet */
    dload.length = sizeof(char);
    dload.data = &eof;
    senddata(&dload);
    printf("sent EOF\n");
    
    printf("Finished. Sent %i bytes\n",sent);
}

  

/* Handle CONTROL packets received by SERIAL_TRANSPORT */
void handle_control_data(packet *p)
{
  int message_type;
  /* set 'done' flag (for testing) */
  dbdl_done = TRUE;

  printf("Called handle_control_data\n");
  /* find type of message */
  message_type = atoi(p->data);
  
  if(message_type == REQUEST_NEXT) {
    printf("next flag = %i\n",next_flag);
    next_flag = TRUE;
  }
  if(message_type == REQUEST_DBDL) db_download();
} 


