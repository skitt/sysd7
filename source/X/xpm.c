/* xpm.c */
/* Functions for conversion to xpm & doubling */

#include <stdlib.h>
#include "xpm.h"

/* Frees any allocated memory */
void free_mem()
{
  int i;

  for(i=0;i<XPM_LINES;i++){
    if(xpm[i]) free(xpm[i]);
  }
  if(xpm) free(xpm);
}

void clean_exit(char *text)
{
  free_mem();
  printf("%s\n",text);
  exit(1);
}

/* init_xpm() : Allocates memory for xpm data. Must be called *once*
 * before double_xpm
 */
void init_xpm()
{
  int i;
  /* allocate mem for array of str pointers */
  xpm = calloc(XPM_LINES,sizeof(char *));
  if(!xpm) clean_exit("malloc problem");
  
  /* allocate each string */
  for(i=0;i<XPM_LINES;i++){
    xpm[i] = (char *)calloc(XPM_WIDTH,sizeof(char));
    if(!xpm[i]) clean_exit("malloc problem");
  }
  printf("Allocated xpm memory\n");

  /* copy colourmap/infoline  - this never changes */
  strcpy(xpm[0],"320 239 16 1");
  strcpy(xpm[1],"0 c #000000");
  strcpy(xpm[2],"1 c #111111");
  strcpy(xpm[3],"2 c #222222");
  strcpy(xpm[4],"3 c #333333");
  strcpy(xpm[5],"4 c #444444");
  strcpy(xpm[6],"5 c #555555");
  strcpy(xpm[7],"6 c #666666");
  strcpy(xpm[8],"7 c #777777");
  strcpy(xpm[9],"8 c #888888");
  strcpy(xpm[10],"9 c #999999");
  strcpy(xpm[11],"A c #aaaaaa");
  strcpy(xpm[12],"B c #bbbbbb");
  strcpy(xpm[13],"C c #cccccc");
  strcpy(xpm[14],"D c #dddddd");
  strcpy(xpm[15],"E c #eeeeee");
  strcpy(xpm[16],"F c #ffffff");
  auto_col = 0;
}

/* Double_xpm : Expands xpm to double its size and returns a pointer
   to the new xpm */

void double_xpm(char *image_data)
{

  char temp_char[XPM_WIDTH];
  int yindex=0,xindex=0,data_count,c;
  char temp[] = " ";  /* for transferring a char to a string */

  auto_col++;
  /* copy pixel data but doubling width and height */
  for(data_count = 0;data_count < IMAGE_DATA_SIZE;data_count++){
    /* on end of line... */
    if(xindex == XPM_WIDTH){
      xindex = 0;
      /* double vertically */
      strcpy(xpm[XPM_DATA_OFFSET+yindex+1],xpm[XPM_DATA_OFFSET+yindex]);
      yindex+=2;
    }
    
    /* display a blank image if we have no image pointer */
    if(image_data) c = image_data[data_count]; 
    else c =  auto_col;
    
    /* convert to HEX */
    if (c < 10)
      c = c + '0';
    else
      c = c + 'A' - 10;

    xpm[yindex+XPM_DATA_OFFSET][xindex] = c;
    xpm[yindex+XPM_DATA_OFFSET][xindex+1] = c;
    xindex+=2;
  }
}

void output_xpm()
{
  int i;
  for(i=0;i<XPM_LINES;i++){
    printf("\"%s\",\n",xpm[i]);
  }
  exit(1);
}

