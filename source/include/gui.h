/* System Design Group 7
 * gui.h -- Linux GUI
 * EXTERNAL INTERFACE SPECIFICATION
 * Edward Knowelden (ejk)
 */

#ifndef __GUI_H__
#define __GUI_H__

#include "linux.video.h"

void gui_display_frame(frame_t *frame);
/* Called by VIDEO_LNX when it has decoded a frame. Copies frame from
 * specified location and displays it in a window. The frame type is
 * specified in linux.video.h. */

void gui_display_uid(int uid);
/* Called by CONTROL_LNX if it receives a uid from motorola board and
 * the GUI is active. ie. if someone uses a DR to gain access. */

#endif
