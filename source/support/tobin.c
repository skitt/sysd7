#include <stdio.h>

void main(int argc,char **argv)
{
  long int l;
  int x;
  if(argc!=2)
    exit(1);
  l=strtoul(argv[1]);
  for(x=0;x<16;x++)
    {
      if(l&(1<<15))
	putchar('1');
      else
	putchar('0');
      l<<=1;
    }
  putchar('\n');
}
