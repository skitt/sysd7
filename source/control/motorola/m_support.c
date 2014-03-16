/* Support functions for motorola board */

/* Simple strcmp function for motorola */
/* Not the same as one in string.h - still returns 0 if strings are the
 * same and 1 otherwise. 
 */


int strcomp(char *string1,char *string2) 
{
  int index=0;

  while( *string1 != '\0' && *string2 != '\0') {
    if(*string1++ != *string2++) break;
  }
  if(*string1 == '\0' && *string2 == '\0') return 0; else return 1;
}


/* String copy function for motorola */
void strcopy(char *to, char *from)
{
  int index=0;
  
  while(from[index] != '\0'){
    to[index] = from[index];
    index++;
  }
  to[index] = '\0';
}
