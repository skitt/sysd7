/* Motorola state 8 - open the door
   Stephen Kitt
*/

#include <board.h>

int state8 (void)
{
  /* Open the door and wait for it to be opened or 15 seconds to
     elapse before closing it again */
  starttimer (3010000);
  buzzerled (LEDGREEN);
  while (gettimer () > 10000 && getdoorstatus ());
  stoptimer ();
  buzzerled (ALLOFF);
  return 2;
}
