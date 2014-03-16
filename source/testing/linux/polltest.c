#include <serial.h>
#include <stdio.h>

/* External declaration for temporary testing function */
void serialloop (void);

/* Handler for datatype 1 - the puts is necessary to flush stdout in
   this case */
void handler (packet * in)
{
  int i;
  printf ("Handler called; data received: ");
  for (i = 0; i < in->length; i++)
    putchar (in->data [i]);
  puts ("");
}

void main (void)
{
  /* Initialise serial */
  serialinit ();
  /* Register handler */
  registerhandler (1, handler);
  /* Loop */
  serialloop ();
  /* Close serial (never reached :-) */
  serialclose ();
}
