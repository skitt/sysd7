#include <stdio.h>
#include "../dbm_defs.h"

/* database pointer */
db_item *database;

/* function prototypes */
db_item *make_llist(FILE *file);
void free_database();
db_item *get_db_index(char *name);

int users;

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

  printf("Database:\n");

  /* try to open 'users' file */
  file = fopen(DB_FILENAME,"r");
  if(file == NULL) {
    printf("File problem!\n");
    return FALSE;
  }

  database = make_llist(file);

  printf("Database loaded - %i users\n",users);
  fclose(file);
  return users;
}

/* Recursive function to build up the linked list of database items */

db_item *make_llist(FILE *file)
{
  db_item *head;
  char get_uid[UID_SIZE+1];
  char get_pin[PIN_SIZE+1];
  char get_sname[NAME_SIZE];
  char get_fname[NAME_SIZE];
  char get_dept[NAME_SIZE];
  char get_photo[NAME_SIZE];
  char lock;

  /* file format is:
     UID PIN FNAME SNAME XPM_FILENAME|% DEPT L|N
  */
                         
  

  if(fscanf(file,"%s %s %s %s %s %s %c\n",
	 &get_uid,&get_pin,&get_fname,&get_sname,&get_photo,&get_dept,&lock) 
	 == EOF ) return NULL;
  else {
    head = (db_item *)malloc(sizeof(db_item));

    printf("Allocated memory for db_item\n");
    strcpy(head->uid,get_uid);
    strcpy(head->pin,get_pin);
    strcpy(head->fname,get_fname);
    strcpy(head->sname,get_sname);
    strcpy(head->dept,get_dept);
    strcpy(head->photo,get_photo);
    head->lock = lock;
    printf("Loading %s %s %s\n",get_fname,get_sname,get_uid);
    head->next = make_llist(file);
    users++;
    return head;
  }
}

void free_database()
{
  db_item *curr=database,*next;

  while(curr){
    printf("Freeing %s\n",curr->sname);
    next = curr->next;
    free(curr);
    curr = next;

  }
}

/* Returns a pointer to a user's record structure when given a text
 * string of the users name (fname+sname).
 */
db_item *get_db_item(char *name)
{
  char fname[NAME_SIZE];
  char sname[NAME_SIZE];
  db_item *curr=database;
  printf("get_db_item: string = %s\n",name);
  
  printf("%s\n",name);  
  /* first, separate name string */
  sscanf(name,"%s %s",fname,sname);

  while(curr){
    if(strcmp(curr->sname,sname)==0 &&
       strcmp(curr->fname,fname)==0) return curr;
    printf("get_db_item: searched %s\n",curr->sname);
    curr = curr->next;
  }
  return NULL;
}
