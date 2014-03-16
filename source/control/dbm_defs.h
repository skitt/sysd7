/* database.h 
 * Definitions for database module (used with both platforms at
 * present).
 * Edward Knowelden 
 */

#include <serial.h>
#include <data.h>

#define DB_FILENAME "/home/sysd7/source/control/users"
#define UID_SIZE 12
#define PIN_SIZE 4
#define NAME_SIZE 30
#define MAX_USERS 10
#define ENTRY_ATTEMPTS 3
#define TRUE 1
#define FALSE 0

/* CONTROL message identifiers */
#define REQUEST_DBDL  111  /* request database download */
#define REQUEST_NEXT  222  /* request the next record */

/*typedef enum {CONTROL, DATABASE} data_t;*/

/* a Linux database item definition - stored as a linked list */
struct db_struct {
  char uid[UID_SIZE+1];
  char pin[PIN_SIZE+1];
  char fname[NAME_SIZE+1];
  char sname[NAME_SIZE+1];
  char dept[NAME_SIZE+1];
  char photo[NAME_SIZE];
  char lock;
  struct db_struct *next;
};

typedef struct db_struct db_item;

/* a locally stored database item. */
typedef struct {
  char uid[UID_SIZE+1];
  char pin[PIN_SIZE+1];
} local_db_item;
  

/* Control message structure:
   [XXX][\0][DDDDD.....D]
   where: XXX is message identifier (see below), \0 is null char
          DDD is unspecified data
	  */

/* Global variables for getting pin/uid*/
/*static char uid_no[13];
static int got_uid;
static char pin[PIN_SIZE+1];*/

/* Database storage
   A bit inefficient to allocate mem for all possible users, but
   alternative is malloc, which would have to be written from scratch */
/* a global array to store database in */
/*db_item db_local_store[MAX_USERS];*/

/* motorola function prototypes */

int request_db();
void handle_db_data(packet p);
void send_DR_value(char *string);
void send_keypad_digit(int digit);
int validate_user(char *uid, char *pin);

/* linux function prototypes */
int db_download();
int load_database();
void handle_control_data(packet *p);
db_item *get_db_item(char *name);

/* support functions */
int itos(int,char *,int);
int strcomp(char *string1,char *string2);
void strcopy(char *to, char *from);

/* test functions */
void mtr_senddata(packet p);
int lnx_senddata(packet p);
void unlock_door();
