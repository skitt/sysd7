/* code to convert 6-byte string to 12-byte serial code */

  int cpos = 0,upos = 0;
  
  /* convert to 12 digit hex */
  while(cpos < 6 ){
    if(c[cpos] > '9') 
      uid_no[upos] = (c[cpos]-'A'+10) << 4;
    else
      uid_no[upos] = (c[cpos] - '0') << 4;
    if(c[cpos+1] > '9')
      uid_no[upos] += (c[cpos+1] - 'A' + 10);
    else
      uid_no[upos] += c[cpos] - '0';
    cpos+=2;
    upos++;
  }

