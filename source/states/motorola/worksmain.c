/* 
  State transition glue code
*/

#include <board.h>
#include <support.h>

/* Prototypes for states 1-9 */

int state1 (void);
int state2 (void);
int state3 (void);
int state4 (void);
int state5 (void);
int state6 (void);
int state7 (void);
int state8 (void);
int state9 (void);
int state10 (void);

void main (void)
{
  int cs = 1;
  while (1)
    {
      /* Display current state on LED */

      if (cs == 10)
	leddisplay ('0');
      else
	leddisplay ('0' + cs);
      switch (cs)
	{
	case 1:
	  cs = state1 ();
	  break;
	case 2:
	  cs = state2 ();
	  break;
	case 3:
	  cs = state3 ();
	  break;
	case 4:
	  cs = state4 ();
	  break;
	case 5:
	  cs = state5 ();
	  break;
#if 0
	case 6:
	  cs = state6 ();
	  break;
#endif
	case 7:
	  cs = state7 ();
	  break;
	case 8:
	  cs = state8 ();
	  break;
#if 0
	case 9:
	  cs = state9 ();
	  break;
#endif
	case 10:
	  cs = state10 ();
	  break;
	default:
	  cs = 1;
	  break;
	}
    }
}
