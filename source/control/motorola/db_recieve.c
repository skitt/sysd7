/* Functions for database receive (Motorola end) via serial line */
/* Edward Knowelden */

#include <m68kboard.h>
#include "../dbm_defs.h"

extern local_db_item db_local_store[MAX_USERS];

/* request_db  
   Called on powerup of motorola board. Asks Linux box to
   send database. */

int request_db()
{
  char data[4];
  packet p,next,*rcp;
  datatype_t datatp;
  int record = 0;

  puts("Requesting DB\n\r");
  /* send request message */
  itos(REQUEST_DBDL,data,4);
  datatp = DATABASE;
  p.datatype = datatp;
  p.length = 4;
  p.data = data;
  senddata(&p);
  
  /* build "next" packet */
  next.datatype = DATABASE;
  next.length = 4;
  itos(REQUEST_NEXT,data,4);
  next.data = data;

  /* now receive the data. At present, database is split into
     separate packets for each record */
  while (1) {
#if 0
    puts("Waiting for data packet\n\r");
#endif
    rcp = receivepacket();
    if(rcp->data[0] == 0xFF) break;
    /* copy data out of packet */
    strcopy(db_local_store[record].uid,rcp->data);
    strcopy(db_local_store[record].pin,(rcp->data)+sizeof(char[UID_SIZE+1]));
    puts("Recieved"); 
    puts(db_local_store[record].uid);
    puts(" ");
    puts(db_local_store[record].pin);
    puts("\n\r");
    record++;
    /* send a signal that we are ready for the next packet */
    senddata(&next);
  }
  
  /* add eof record */
  strcopy(db_local_store[record].uid,"X");
}

/* Called by SERIAL_TRANSPORT when it has some data for the database */
void handle_db_data(packet p)
{
  int i,j,rsize;

  /* calculate record size (+2 for 2 NULL chars)*/
  rsize = UID_SIZE+PIN_SIZE+2;

  puts("\nExtracting packet...");
    /*...(%i records)\n",(p.length/sizeof(db_item)));*/
  /* extract data from packet into memory */
  for(i=0; i < (p.length/sizeof(db_item)); i++){
    /* first 12 bytes are uid */
    for(j=0;j<UID_SIZE+1;j++){
      db_local_store[i].uid[j] = *(p.data+(rsize*i)+j);
    }
    puts("uid: "); 
    puts(db_local_store[i].uid);

    /* next 4 are PIN */
    for(j=0;j<PIN_SIZE+1;j++){
      db_local_store[i].pin[j] = *(p.data+(rsize*i)+UID_SIZE+1+j);
    }
    puts("pin: "); 
    puts(db_local_store[i].pin);
  }
}


