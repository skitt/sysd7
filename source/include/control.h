/*
  control.h defines enums for various messages which are sent by the
  control module
  $Id: control.h,v 1.7 1998/05/09 14:50:23 iic Exp $
  $Log: control.h,v $
  Revision 1.7  1998/05/09 14:50:23  iic
  Added BUZZER_PRESSED

  Revision 1.6  1998/05/07 19:22:02  dwi
  Added MAKE_TRANSITION to enum.

  Revision 1.5  1998/05/07 19:20:35  skk
  Added comments.

  Revision 1.4  1998/05/04 11:14:45  iic
  *** empty log message ***

  Revision 1.3  1998/05/04 09:39:37  skk
  Changed controltag to controltag_t.

*/

/* Control packets (CONTROL datatype) start with a tag byte which
   takes a value from the following enum; each control handler should
   compare the first byte of a received control packet with the tags
   it's expecting. Any data after that is defined by the control
   handler's author; so the video control handler needn't know about
   the database control handler. Any unrecognised packets should be
   ignored, because we assume out-of-bounds packets will never be sent 
   (this also eliminates a slew of DoS attacks).
*/

typedef enum
{
  /* Reset Motorola board */
  RESET_MOTOROLA,
  RESTART_VIDEO,
  MAKE_TRANSITION,
  BUZZER_PRESSED
} controltag_t;
