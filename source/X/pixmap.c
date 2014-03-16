/* pixmap.c */

#include <gtk/gtk.h>


/* XPM data of Open-File icon */
static const char * xpm_data[] = {
  "16 16 3 1",
  "       c None",
  ".      c #000000000000",
  "X      c #FFFFFFFFFFFF",
  "                ",
  "   ......       ",
  "   .XXX.X.      ",
  "   .XXX.XX.     ",
  "   .XXX.XXX.    ",
  "   .XXX.....    ",
  "   .XXXXXXX.    ",
  "   .XXXXXXX.    ",
  "   .XXXXXXX.    ",
  "   .XXXXXXX.    ",
  "   .XXXXXXX.    ",
  "   .XXXXXXX.    ",
  "   .XXXXXXX.    ",
  "   .........    ",
  "                ",
  "                "};


/* when invoked (via signal delete_event), terminates the application.
 */
void close_application( GtkWidget *widget, gpointer *data ) {
  gtk_main_quit();
}

/* is invoked when the button is clicked.  It just prints a message.
 
void button_clicked( GtkWidget *widget, gpointer *data ) {
  printf( "button clicked\n" );
}
*/
/* void get_new_image( GtkWidget *widget, gpointer *data ) {
  printf("Getting new image\n");
}
*/

int main( int argc, char *argv[] )
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window, *pixmapwid, *button1, *button2, *rc_button,
    *cl_button, *cf_button, *au_button, *lbox, *button_box, *rbox, *box;
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  GtkStyle *style;
  
  
  /* create the main window, and attach delete_event signal to terminating
     the application */
  gtk_init( &argc, &argv );
  window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

  gtk_window_set_title (GTK_WINDOW (window), "Door Entry System");

  gtk_signal_connect( GTK_OBJECT (window), "delete_event",
		      GTK_SIGNAL_FUNC (close_application), NULL );
  gtk_container_border_width( GTK_CONTAINER (window), 10 );

  /* create new boxes */
  box = gtk_hbox_new (FALSE, 1);
  button_box = gtk_hbox_new(TRUE,1);
  lbox = gtk_vbox_new(FALSE,1);
  rbox = gtk_vbox_new(TRUE,1);

  /* now for the pixmap from gdk */
  style = gtk_widget_get_style( window );
  pixmap = gdk_pixmap_create_from_xpm( window->window,  &mask,
				       &style->bg[GTK_STATE_NORMAL],
				       "/home/sysd7/images/james_2x.xpm");
  /* a pixmap widget to contain the pixmap */
  pixmapwid = gtk_pixmap_new( pixmap, mask );
  gtk_box_pack_start (GTK_BOX (lbox), pixmapwid, TRUE, TRUE, TRUE);

  printf("Created pixmap widget\n");
  /* define the buttons and pack into boxes */
  button1 = gtk_button_new_with_label("Unlock");
  gtk_signal_connect( GTK_OBJECT(button1), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), NULL );
  gtk_box_pack_start (GTK_BOX (button_box), button1, TRUE, TRUE, TRUE);
  gtk_widget_show( button1 );

  button2 = gtk_button_new_with_label("Refuse");
  gtk_signal_connect( GTK_OBJECT(button2), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), NULL );
  gtk_box_pack_start (GTK_BOX (button_box), button2, TRUE, TRUE, TRUE);
  gtk_widget_show( button2 );

  rc_button = gtk_button_new_with_label("Recapture");
  gtk_signal_connect( GTK_OBJECT(rc_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked ), NULL );
  gtk_box_pack_start (GTK_BOX (rbox), rc_button, TRUE, TRUE, TRUE);
  gtk_widget_show( rc_button );

  au_button = gtk_button_new_with_label("Audio");
  gtk_signal_connect( GTK_OBJECT(au_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), NULL );
  gtk_box_pack_start (GTK_BOX (rbox), au_button, TRUE, TRUE, TRUE);
  gtk_widget_show( au_button );

  cf_button = gtk_button_new_with_label("Configure");
  gtk_signal_connect( GTK_OBJECT(cf_button), "clicked",
		      GTK_SIGNAL_FUNC(button_clicked), NULL );
  gtk_box_pack_start (GTK_BOX (rbox), cf_button, TRUE, TRUE, TRUE);
  gtk_widget_show( cf_button );

  cl_button = gtk_button_new_with_label("Close");
  gtk_signal_connect( GTK_OBJECT(cl_button), "clicked",
		      GTK_SIGNAL_FUNC (close_application), NULL );
  gtk_box_pack_start (GTK_BOX (rbox), cl_button, TRUE, TRUE, TRUE);
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
  printf("Packed main box\n");
  gtk_widget_show(box);

  gtk_container_add( GTK_CONTAINER(window), box );
  
  printf("Done button and added box to window\n");
  /* show the window */
  gtk_widget_show( window );
  gtk_main ();
  
  return 0;
}
