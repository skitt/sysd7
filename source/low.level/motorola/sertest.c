#include <m68kboard.h>

struct mc68681wfm * uartw = (struct mc68681wfm *) 0x00B00003;
struct mc68681rfm * uartr = (struct mc68681rfm *) 0x00B00003;

void puts (char * string)
{
  while (*string)
    {
      mon_putc (*string);
      string++;
    }
}

void realhex (unsigned char value)
{
  if (value < 10)
    mon_putc (value + '0');
  else
    mon_putc (value + 'A' - 10);
}

void hex (unsigned char value)
{
  unsigned char high, low;
  high = (value & 0xF0) >> 4;
  low = value & 0x0F;
  realhex (high);
  realhex (low);
}

void main (void)
{
  unsigned char c;

  puts ("Initialising transmitter\n\r");
  /* Reset errors */
  uartw->crb = cr_reseterr;
  /* Reset TX */
  uartw->crb = cr_resettx;
  /* Reset RX */
  uartw->crb = cr_resetrcv;
  /* Reset MR pointer */
  uartw->crb = cr_resetmr;
#if 0
  /* Init IVR */
  uartw->ivr = 0xF0;
  /* Init IMR (no interrupts) */
  uartw->imr = 0x00;
  /* Init ACR */
  uartw->acr = 0x00;
  /* Init CTUR */
  uartw->ctur = 0x00;
  /* Init CTLR */
  uartw->ctlr = 0x02;
  /* Init OPCR */
  uartw->opcr = 0x00;
  /* Init CTS */
  uartw->opr_set = 0x01;
#endif
  /* Init CSR (9600bps) */
  uartw->csrb = 0xBB;
  /* Init MR1 (8N) */
  uartw->mr1b = 0x13;
  /* Init MR2 (1) */
  uartw->mr2b = 0x07;
  /* Enable port */
  uartw->crb = cr_txen | cr_rxen;
#if 0
  c = 'A';
  do
    {
      if (uartr->srb & 0x01)
	{
	  puts ("Received: ");
	  mon_putc (uartr->rbb);
	  puts ("\n\r");
	}
      if (uartr->srb & 0x04)
	{
	  uartw->tbb = c;
	  c++;
	}
    }
  while (c <= 'Z');
#endif
  /* Reset MR pointer */
  uartw->crb = cr_resetmr;
  /* Print diagnostics */
  puts ("MR1B: ");
  hex (uartr->mr1b);
  puts ("\n\rMR2B: ");
  hex (uartr->mr2b);
  puts ("\n\rSRB: ");
  hex (uartr->srb);
}
