#include <stdio.h>

unsigned char uid[] = { 0x00, 0x00, 0x01, 0x88, 0x81, 0xA4 };

char uid2[] = { 0x00, 0x00, 0x01, 0x88, 0x75, 0x6E };
char uid_no[13];

void conv_hchar (unsigned long number, char *buffer);

void main()
{
  int i;
  for(i=0;i<6;i++){
    
    printf("C:%2.2X \n",(unsigned int)uid[i]);
    conv_hchar((unsigned long)uid[i],&(uid_no[(i*2)]));
  }
  uid_no[12] = '\0';

  printf("Converted string: ");
  printf("%s\n",uid_no);
}

void conv_hchar (unsigned long number, char *buffer)
{
  unsigned long magnitude;
  unsigned char digit;
  int digits = 2;
  int base =16;
  int i=0;

  magnitude = 1;
  while (digits > 1)
    {
      magnitude *= base;
      digits--;
    };
  while (magnitude > 0)
    {
      digit = number / magnitude;
      if (digit < 10)
	buffer[i] = ('0' + digit);
      else
	buffer[i] = ('A' + digit - 10);
      number %= magnitude;
      magnitude /= base;
      i++;

    }
  buffer[i] = '\0';
  printf("Converted:%s\n",buffer);
    
}
