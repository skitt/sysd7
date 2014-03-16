/* Support functions */

/* itos : converts an integer to a string. Returns length or 0 for error.
 * NB: Will need to write abs() for use on motorola.
 * Args: num - number to convert, 
         result - where to place the result (pointer)
	 mlength - max length of string */

int itos(int num,char *result,int mlength) 
{ 

  int mag = 1,digits = 0,str_pos=0,c;
  
  /* find the magnitude of number */
  while (num/mag > 0) mag *= 10;

  mag = mag/10;
  /* go back down, adding to string */
  while(num > 10){
    c = abs(num/mag);
    result[str_pos] = '0'+c;
    str_pos++;
    if (str_pos > mlength) break;
    num -= mag*c;
    mag = mag / 10;
  }
  result[str_pos] = num+'0';
  result[str_pos+1] = '\0';
}

