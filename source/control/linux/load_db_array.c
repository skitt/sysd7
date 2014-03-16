#include <stdio.h>
#include "../dbm_defs.h"

/* database buffer */
db_item db_store[MAX_USERS];

/* returns number of items read. Need to load into single buffer,
ready for transmission. ie. all user x's details next to each
other. structure? */

int load_database()
{
  FILE *file;
  char get_uid[UID_SIZE+1];
  char get_pin[PIN_SIZE+1];
  char get_sname[NAME_SIZE];
  char get_fname[NAME_SIZE];
  char comment;
  int i,count = 0;

  printf("Database:\n");

  /* try to open 'users' file */
  file = fopen(DB_FILENAME,"r");
  if(file == NULL) {
    printf("File problem!\n");
    return FALSE;
  }

  /* file format is:
     UID PIN FNAME SNAME "DEPT" XPM_PHOTOFILENAME|% for no photo */
  while( fscanf(file,"%s %s %s %s \"%s\" %s\n",
		&get_uid,&get_pin,&get_fname,&get_sname,&get_dept,&get_photo) 
	 != EOF ){
    strcpy(db_store[count].uid,get_uid);
    strcpy(db_store[count].pin,get_pin);
    strcpy(db_store[count].fname,get_fname);
    strcpy(db_store[count].sname,get_sname);
    strcpy(db_store[count].dept,get_dept);
    strcpy(db_store[count].photo,get_photo);
    printf("Loading %s\n",get_sname);
    count++;
  }

  /* add a special EOF uid starting with 'X' */
  db_store[count].uid[0] = 'X';

  printf("Database loaded - %i users\n",count);
  fclose(file);
  return count;
}

