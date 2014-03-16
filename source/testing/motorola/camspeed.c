#include <quickcam.h>
#include <support.h>

unsigned char buffer [19200];

void main (void)
{
  int i;
  
  QC_init ();
  QC_grabframe (buffer);
  puts ("Hit a key to grab 30 frames...\n\r");
  mon_getc ();
  for (i = 0; i < 30; i++)
    QC_grabframe (buffer);
  puts ("Done.\n\r");
}
