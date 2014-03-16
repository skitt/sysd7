/* Motorola state 7 - refuse door entry
   Stephen Kitt
*/

#include <board.h>

int state7 (void)
{
  int i;

  /* Flash the red LED on and off five times */
  for (i = 0; i < 5; i++)
    {
      buzzerled (LEDRED);
      starttimer (210000);
      while (gettimer () > 110000);
      buzzerled (ALLOFF);
      while (gettimer () > 10000);
      stoptimer ();
    }
  return 2;
}
