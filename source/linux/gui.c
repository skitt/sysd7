/* gui.c */
/* Edward Knowelden */

/* Signals to audio process
SIGUSER1 : Linux to Motorola (TALK)
SIGUSER2 : Motorola to Linux (LISTEN)
SIGUSER1/2 sent while its corresponding direction is active stops audio.
*/

#include <gtk/gtk.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <serial.h>
#include <control.h>
#include <data.h>
#include "xpm.h"

void init_gui(int argc, char *argv[]);
void get_new_image( GtkWidget *widget, gpointer *data);
extern int itos(int num,char *result,int mlength);
void handle_control(packet *p);
void show_buttons();
void hide_buttons();

/* globals */

extern int audio_pid;
int audio_state;

/* flag to stop 'auto' toggle button clicks */
int no_clicks = TRUE;

GtkWidget *window, *pixmapwid, *button1, *button2, *rc_button,
  *cl_button, *cf_button, *au_button, *tk_button, *ls_button,*lbox,
  *button_box, *rbox, *box, *bellwin, *separator, *radiobox, *button,
  *group,*rbutton1,*rbutton2,*status_bar,*master_box;

GdkPixmap *pixmap;
GdkBitmap *mask;
GtkStyle *style;
GtkTooltips *cf_tips,*au_tips,*dn_tips,*rc_tips, *tk_tips;
int context_id;

/* audio_state: can be 1 or 2 corresponding to audio states (see
   comment at top of file. */
int audio_state = 0;

/* when invoked (via signal delete_event), terminates the application.
 */
void close_application( GtkWidget *widget, gpointer *data ) {
  free_mem();
  if(kill(audio_pid,SIGINT) == 0) printf("Killed Audio process\n");
  gtk_main_quit();
}

void ctrl_c_handler(int sig)
{
  close_application(NULL,NULL);
}

/* is invoked when the button is clicked.  It just prints a message.
 */
void button_clicked( GtkWidget *widget, gpointer *data ) {
  int rval;
  if((audio_state == 2) || (audio_state == 1)) no_clicks = FALSE;
  printf( "button clicked\n" );
  /* listen button */
  if(widget ==ls_button){
    gtk_statusbar_pop(status_bar,context_id);
    gtk_statusbar_push(status_bar,context_id,"Listen mode");
    
    /* send signal to audio process */
    if(audio_state == 1) {
      kill(audio_pid,SIGUSR1);
      audio_state = 0;
      sleep (2);
      while(serialinit());
      serialtransmit(10);
      serialclose ();
      kill (audio_pid, SIGUSR2);
      audio_state = 2;
    }
    else if (audio_state == 0) {
      changestate(10);
      serialclose();
      kill(audio_pid,SIGUSR2);
      audio_state = 2;
    }
    no_clicks = TRUE;
    printf("Signalled audio process SIGUSR2\n");
    return;
  }
  
  /* Talk button */
  if(widget == tk_button){
    gtk_statusbar_pop(status_bar,context_id);
    gtk_statusbar_push(status_bar,context_id,"Talk mode");
    
    /* send signal to audio process */
    if(audio_state == 2) {
      kill(audio_pid,SIGUSR2);
      audio_state = 0;
      sleep (2);
      while(serialinit());
      serialtransmit(4);
      serialclose ();
      kill (audio_pid, SIGUSR1);
      audio_state = 1;
    }
    else if (audio_state == 0)
      {
	changestate(4);
	serialclose();
	kill(audio_pid,SIGUSR1);
	audio_state = 1;
      }
    no_clicks = TRUE;
    printf("Signalled audio process SIGUSR1\n");
    return;
  }
  
  
  /* config button */
  if(widget == cf_button){
      if(fork() == 0) rval = execl("/home/sysd7/source/X/config",
				   "/home/sysd7/source/X/config",NULL);
      return;
  }
  
  /* open door */
  if(widget == button1) {
    if(audio_state == 1){
      kill(audio_pid,SIGUSR1);
      audio_state = 0;
      sleep (2);
      while(serialinit());
      serialtransmit(8);
      registerhandler(CONTROL,handle_control);
      initVideo ();
      enablepackets ();
    } else if(audio_state == 2){
      kill(audio_pid,SIGUSR2);
      audio_state = 0;
      sleep (2);
      while(serialinit());
      serialtransmit(8);
      registerhandler(CONTROL,handle_control);
      initVideo ();
      enablepackets ();
    } else changestate(8);
    
    hide_buttons();
    return;
  }

  /* refuse entry */
  if(widget == button2) {
    if(audio_state == 1){
      kill(audio_pid,SIGUSR1);
      audio_state = 0;
      sleep (2);
      while(serialinit());
      serialtransmit(7);
      registerhandler(CONTROL,handle_control);
      initVideo ();
      enablepackets ();
    } else if(audio_state == 2){
      kill(audio_pid,SIGUSR2);
      audio_state = 0;
      sleep (2);
      while(serialinit());
      serialtransmit(7);
      registerhandler(CONTROL,handle_control);
      initVideo ();
      enablepackets ();
    } else changestate(7);
    
    hide_buttons();
    return;
  }
  /* recapture */
  if(widget == rc_button) {
    gtk_statusbar_pop(status_bar,context_id);
    gtk_statusbar_push(status_bar,context_id,"Video mode");
    if (audio_state)
      {
	switch (audio_state)
	  {
	  case 1:
	    kill (audio_pid, SIGUSR1);
	    break;
	  case 2:
	    kill (audio_pid, SIGUSR2);
	    break;
	  }
	audio_state = 0;
	sleep (5);
	while (serialinit ());
	serialtransmit (3);
	registerhandler (CONTROL, handle_control);
	initVideo ();
	enablepackets ();
      }
    else
      {
	changestate(2);
      }
    return;
  }
  /* audio control */
  if(widget == au_button){
    if(fork() == 0) rval = execl("/usr/X386/bin/xmixer",
				 "/usr/X386/bin/xmixer",NULL);
  }
}  

void get_new_image( GtkWidget *widget, gpointer *data) 
{
  GtkStyle *style;
  GdkBitmap *mask;

  /* get image and resize */
  double_xpm(NULL);

  /* free last pixmap */
  gdk_pixmap_unref(pixmap);

  /* create pixmap */
  style = gtk_widget_get_style( window );
  pixmap = gdk_pixmap_create_from_xpm_d( window->window,  &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (char **)xpm);
  gtk_pixmap_set( (GtkPixmap *)pixmapwid,pixmap,mask);
}

int gui_main( int argc, char *argv[] )
{
  int i;
  /* start serial */
  serialinit();

  /* request state change */
  serialtransmit (1);
#ifdef DEBUG
  printf ("Waiting for ping\n");
#endif
  do
    {
      if (serialreceive () != 2)
	continue;
      for (i = 0; i < 4; i++)
	if (serialreceive () != 0)
	  break;
      if (i == 4)
	break;
    }
  while (1);

  serialtransmit (2);

  enablepackets ();

  initVideo();
  init_gui(argc,argv);
  return 1;
}

void init_gui(int argc, char *argv[])
{
  /* create the main window, and attach delete_event signal to terminating
     the application */
  gtk_init( &argc, &argv );

  /* allocate mem for xpm */
  init_xpm();
  
  /* create window */
  window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

  gtk_window_set_title (GTK_WINDOW (window), "Door Entry System");

  gtk_signal_connect( GTK_OBJECT (window), "delete_event",
		      GTK_SIGNAL_FUNC (close_application), NULL );
  gtk_container_border_width( GTK_CONTAINER (window), 10 );

  /* register the handler */
  registerhandler(CONTROL,handle_control);
  
  /* create new boxes */
  master_box = gtk_vbox_new(FALSE,1);
  box = gtk_hbox_new (FALSE, 1);
  button_box = gtk_hbox_new(TRUE,1);
  lbox = gtk_vbox_new(FALSE,1);
  rbox = gtk_vbox_new(TRUE,1);

  /* now for the pixmap */

  style = gtk_widget_get_style( window );
  printf("Resizing...");
  double_xpm(NULL);
  printf("done\n");
#ifdef DEBUG
  output_xpm();
#endif
  pixmap = gdk_pixmap_create_from_xpm_d( window->window,  &mask,
				       &style->bg[GTK_STATE_NORMAL],
				       (char **)xpm);

  /* a pixmap widget to contain the pixmap */
  pixmapwid = gtk_pixmap_new( pixmap, mask );
  gtk_box_pack_start (GTK_BOX (lbox), pixmapwid, TRUE, TRUE, TRUE);

  /* define the buttons and pack into boxes */
  /*NB: show functions commented so buttons only appear later */

  cf_tips = gtk_tooltips_new();
  rc_tips = gtk_tooltips_new();
  dn_tips = gtk_tooltips_new();
  au_tips = gtk_tooltips_new();
  button1 = gtk_button_new_with_label("Unlock");
  gtk_signal_connect( GTK_OBJECT(button1), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), button1 );
  gtk_box_pack_start (GTK_BOX (button_box), button1, TRUE, TRUE, TRUE);
  /*gtk_widget_show( button1 );*/

  button2 = gtk_button_new_with_label("Refuse");
  gtk_signal_connect( GTK_OBJECT(button2), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), button2 );
  gtk_box_pack_start (GTK_BOX (button_box), button2, TRUE, TRUE, TRUE);
  /*gtk_widget_show( button2 );*/


  tk_button = gtk_button_new_with_label("Talk");
  gtk_signal_connect( GTK_OBJECT(tk_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), tk_button );
  gtk_box_pack_start (GTK_BOX (rbox), tk_button, TRUE, TRUE, TRUE);
  gtk_tooltips_set_tip(GTK_OBJECT(dn_tips),GTK_OBJECT(tk_button),
			"Talk to visitor",NULL);
  /*gtk_widget_show( tk_button );*/

    ls_button = gtk_button_new_with_label("Listen");
  gtk_signal_connect( GTK_OBJECT(ls_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), ls_button );
  gtk_box_pack_start (GTK_BOX (rbox), ls_button, TRUE, TRUE, TRUE);
  gtk_tooltips_set_tip(GTK_OBJECT(dn_tips),GTK_OBJECT(ls_button),
			"Let the visitor talk to you",NULL);
  /*gtk_widget_show( ls_button );*/


  rc_button = gtk_button_new_with_label("Video");
  gtk_signal_connect( GTK_OBJECT(rc_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked ), rc_button );
  gtk_box_pack_start (GTK_BOX (rbox), rc_button, FALSE, TRUE, TRUE);
  gtk_tooltips_set_tip(GTK_OBJECT(rc_tips),GTK_OBJECT(rc_button),
			"Grab frames from camera",NULL);
  gtk_widget_show( rc_button );

  /* create a separator
   */
  separator=gtk_hseparator_new();
  gtk_box_pack_start (GTK_BOX (rbox), separator, TRUE, TRUE, TRUE);
  /*gtk_widget_show(separator);*/
  
  au_button = gtk_button_new_with_label("Audio Control");
  gtk_signal_connect( GTK_OBJECT(au_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), au_button );
  gtk_box_pack_start (GTK_BOX (rbox), au_button, FALSE, TRUE, TRUE);
  gtk_tooltips_set_tip(GTK_OBJECT(au_tips),GTK_OBJECT(au_button),
			"Change mic/speaker volumes",NULL);
  gtk_widget_show( au_button );

  cf_button = gtk_button_new_with_label("Configure");
  gtk_signal_connect( GTK_OBJECT(cf_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), cf_button );
  gtk_tooltips_set_tip(GTK_OBJECT(cf_tips),GTK_OBJECT(cf_button),
			"Add/delete users and change options",NULL);
  gtk_box_pack_start (GTK_BOX (rbox), cf_button, FALSE, TRUE, TRUE);
  gtk_widget_show( cf_button );

  cl_button = gtk_button_new_with_label("Close");
  gtk_signal_connect( GTK_OBJECT(cl_button), "clicked",
		      GTK_SIGNAL_FUNC (close_application), NULL );
  gtk_box_pack_start (GTK_BOX (rbox), cl_button, FALSE, TRUE, 0);
  gtk_widget_show( cl_button );

  gtk_widget_show( button_box );
  gtk_widget_show( rbox );

  /* pack button box into main box */
  gtk_box_pack_start (GTK_BOX (lbox), button_box, TRUE, TRUE, TRUE);
  gtk_widget_show( pixmapwid );  
  gtk_widget_show( lbox );
  printf("showed pixmap and lbox\n");

  /* pack left and right boxes into main box */

  gtk_box_pack_start (GTK_BOX (box), lbox, TRUE, TRUE, TRUE);
  gtk_box_pack_start (GTK_BOX (box), rbox, TRUE, TRUE, TRUE);

  gtk_widget_show(box);

  gtk_box_pack_start (GTK_BOX (master_box), box, TRUE, TRUE, TRUE);
  
  /* a status bar */
  status_bar = gtk_statusbar_new();      
  gtk_box_pack_start (GTK_BOX (master_box), status_bar, TRUE, TRUE, 0);
  gtk_widget_show (status_bar);
  context_id = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar), "Normal");
  gtk_statusbar_push(status_bar,context_id,"Video mode");

  gtk_container_add( GTK_CONTAINER(window), master_box );
  
  /* show the window */
  gtk_widget_show( master_box);
  gtk_widget_show( window );

  gtk_main ();
}

/* called by video module when a new video frame arrives */
void display_image(char *image)
{
  GtkStyle *style;
  GdkBitmap *mask;

  double_xpm(image);

  gdk_pixmap_unref(pixmap);

  style = gtk_widget_get_style( window );
  pixmap = gdk_pixmap_create_from_xpm_d( window->window,  &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (char **)xpm);
  gtk_pixmap_set( (GtkPixmap *)pixmapwid,pixmap,mask);
  /* turn on clicks */
  no_clicks = FALSE;
}

/* handle control packets from board */
void handle_control(packet *p)
{
  GtkWidget *label,*picwid,*vbox;
  GdkPixmap *pxmp;
  GtkStyle *style;
  GdkBitmap *mask;
  int x = TRUE;

  /* Code for a Buzzer press */
  
  /*  if(*(p->data) == BUZZER_PRESSED){*/
  if(x){
    /* pop the gui up if not already up. Make a sound and display
       requester */
    bellwin=gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_window_set_title(GTK_WINDOW(bellwin), "Doorbell");
    gtk_signal_connect( GTK_OBJECT (bellwin), "delete_event",
		      GTK_SIGNAL_FUNC (gtk_object_destroy), bellwin );


    vbox = gtk_vbox_new(FALSE,1);
    gtk_container_add(GTK_CONTAINER(bellwin),vbox);
    gtk_widget_show(vbox);
    
    style = gtk_widget_get_style( bellwin );
    pxmp = gdk_pixmap_create_from_xpm( bellwin->window,  &mask,
				       &style->bg[GTK_STATE_NORMAL],
				       "/home/sysd7/bell.xpm");

    picwid = gtk_pixmap_new( pxmp, mask );
    gtk_box_pack_start(GTK_BOX(vbox),picwid,TRUE,TRUE,TRUE);
    gtk_widget_show(picwid);
    
    label = gtk_label_new("Someone is at the door!");
    gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,TRUE);
    gtk_widget_show(label);
    
    gtk_widget_show(bellwin);    

    /* play sample */
    if(fork()==0) execl("/home/sysd7/sndscript","/home/sysd7/sndscript",NULL);
    show_buttons();
  }
}

void hide_buttons()
{
    gtk_widget_hide( button1 );
    gtk_widget_hide( button2 );
    
    gtk_widget_hide( tk_button);
    gtk_widget_hide( ls_button);
    gtk_widget_hide(separator);
    gtk_widget_destroy(bellwin);
    /* update statusbar */
    gtk_statusbar_push(status_bar,context_id,"Video mode");
}
  
void show_buttons()
{
    gtk_widget_show( button1 );
    gtk_widget_show( button2 );
    gtk_widget_show( tk_button );
    gtk_widget_show( ls_button );
    gtk_widget_show(separator);
}
