int key=1;

int getkeypad()
{
  return key++;
}

void puts(char * string){
  printf("%s",string);
}

void mon_putc(char c)
{
  putchar(c);
}
