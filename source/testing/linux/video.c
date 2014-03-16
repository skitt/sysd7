#include <unistd.h>
#include <stdio.h>

#include <data.h>
#include <serial.h>

extern void handleVideo (packet *);

void main (int argc, char **argv)
{
  puts ("Initialising serial");
  serialinit ();
  puts ("Registering handler");
  registerhandler (VIDEO, handleVideo);
  init_gui(argc,argv);
  puts ("Closing serial");
  serialclose ();

}
