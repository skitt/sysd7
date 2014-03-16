#include <stdio.h>
#include <string.h>

#define NAME_SIZE 30

int swap_names(char *name) 
{
  char sname[NAME_SIZE];
  char fname[NAME_SIZE];
  char *comma;
  int i=0,j=0;
  printf("name: %s\n",name);

  if(comma = strchr(name,',')){
    while(name+i != comma){
      sname[i] = name[i];
      i++;
    }
    sname[i]='\0';
    i++;
    while(name[i] != '\0'){
      fname[j] = name[i];
      i++;j++;
    }
    fname[i]='\0';
    sprintf(name,"%s %s",fname,sname);
  }
  
}

void main()
{
  char name[] = "Mouse,Kimberly-Davis";

  swap_names(name);
  printf("%s\n",name);
}
