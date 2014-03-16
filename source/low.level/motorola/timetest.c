#include <board.h>
#include <support.h>

void main (void)
{
  boardinit ();
  puts ("Going\n\r");
  starttimer (10010000);
  while (gettimer () > 10000);
  puts ("Gone\n\r");
  stoptimer ();
}
