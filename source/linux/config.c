/* The configuration GUI */

#include <gtk/gtk.h>
#include <stdio.h>
#include "../control/dbm_defs.h"
#include <string.h>
#include <unistd.h>
/* need to compile with db_download.c */

/* this is our data identification string to store
 * data in list items
 */
const   gchar   *list_item_data_key="list_item_data";

/* database location */
extern db_item *database;

/* GUI globals */

GtkWidget *separator,*window,*pinwin,*lvbox,*rvbox,*bbox,
  *scrolled_window, *frame, *gtklist, *cl_button, *add_button,
  *del_button, *fname_entry, *sname_entry, *uid_entry, *last_entry,
  *list_item, *fnm_label, *snm_label, *uid_label, *last_label,
  *master_hbox, *fnamebox, *snamebox, *uidbox, *lastbox, *pinbox,
  *pin_button, *log_button, *pixmapwid, *deptbox, *dept_entry,
  *dp_label, *pvbox, *lok_button,*pin_ok_button, *status_bar,
  *master_vbox, *pin_entry1, *pin_entry2, *pin_vbox, *pin_label,
  *pinbuttonbox, *pin_c_button;

GdkPixmap *pixmap;
GdkBitmap *mask;
GtkStyle *style;

int context_id,prev_error;

/* a database record for adding a user */
db_item add_user_item,final_add_item;
/* a BOOL flag for adding a user */
int adding_user = FALSE; 
char pin_1[5];
char pin_2[5];

/* function protypes */
void user_selected (GtkWidget *gtklist,gpointer func_data);
void add_user( GtkWidget *widget, gpointer *data );
void enter_callback(GtkWidget *widget, GtkWidget *entry);
void enter_pin();
void finished_pin(GtkWidget *widget, gpointer *data );
void clear_add_item();
void create_new_user();

/* default click handler */
void button_clicked( GtkWidget *widget, gpointer *data ) {
  printf( "button clicked\n" );
}

/* frees memory and exits */
void clean_exit( GtkWidget *widget, gpointer *data ) {
  free_database();
  gtk_main_quit();
}

/* main function to set up the user interface */
gint main (int argc, gchar *argv[])
{                                  

    GList           *dlist;
    guint           i;
    gchar          buffer[64];
    db_item *curr;
    GdkBitmap *mask;
    GtkStyle *style;
    
    /* initialize gtk+ (and subsequently gdk) */

    gtk_init(&argc, &argv);
    
    /* load user database */
    load_database();
    curr = database;

    /* create a window to put all the widgets in
     * connect gtk_main_quit() to the "destroy" event of
     * the window to handle window manager close-window-events
     */
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "User Database");
    gtk_signal_connect(GTK_OBJECT(window),
		       "destroy",
		       GTK_SIGNAL_FUNC(clean_exit),
		       NULL);
    /* a master vbox for whole window and status bar */
    master_vbox = gtk_vbox_new(FALSE,5);
    gtk_container_border_width(GTK_CONTAINER(master_vbox), 5);
    gtk_container_add(GTK_CONTAINER(window), master_vbox);    
    gtk_widget_show(master_vbox);


    /* a master hbox for whole work area */
    master_hbox = gtk_hbox_new(FALSE,5);
    gtk_container_border_width(GTK_CONTAINER(master_hbox), 5);
    gtk_box_pack_start (GTK_BOX (master_vbox), master_hbox, TRUE, TRUE, TRUE);
    gtk_widget_show(master_hbox);

    /* create a separator
     */
    separator=gtk_hseparator_new();
    gtk_box_pack_start (GTK_BOX (master_vbox), separator, TRUE, TRUE, TRUE);
    gtk_widget_show(separator);

    /**** Left vertical box Widgets ****/    

    /* this is the scolled window to put the GtkList widget inside */
    scrolled_window=gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_usize(scrolled_window, 250, 150);
    gtk_container_add(GTK_CONTAINER(master_hbox), scrolled_window);
    gtk_widget_show(scrolled_window);
    
    /* create the GtkList widget
     * connect the sigh_print_selection() signal handler
     * function to the "selection_changed" signal of the GtkList
     * to print out the selected items each time the selection
     * has changed */
    gtklist=gtk_list_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), gtklist);
    gtk_widget_show(gtklist);
    gtk_signal_connect(GTK_OBJECT(gtklist),
		       "selection_changed",
		       GTK_SIGNAL_FUNC(user_selected),
		       NULL);
    
    /* now we create 5 list items, each having it´s own
     * label and add them to the GtkList using gtk_container_add()
     * also we query the text string from the label and
     * associate it with the list_item_data_key for each list item
     */

    while(curr){
      GtkWidget       *label;
      gchar           *string;
      char name[60];

      sprintf(name,"%s %s",curr->fname,curr->sname);
      label=gtk_label_new(name);
      list_item=gtk_list_item_new();
      gtk_container_add(GTK_CONTAINER(list_item), label);
      gtk_widget_show(label);
      gtk_container_add(GTK_CONTAINER(gtklist), list_item);
      gtk_widget_show(list_item);
      gtk_label_get(GTK_LABEL(label), &string);
      gtk_object_set_data(GTK_OBJECT(list_item),
			    list_item_data_key,
			    string);
      curr = curr->next;
    }

    /* Operation buttons */
    bbox=gtk_hbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(bbox), 5);
    gtk_box_pack_start (GTK_BOX (master_vbox), bbox, TRUE, TRUE, TRUE);
    gtk_widget_show(bbox);


    add_button=gtk_button_new_with_label("Add");
    gtk_widget_show(add_button);
    gtk_box_pack_start (GTK_BOX (bbox), add_button, TRUE, TRUE, TRUE);
    gtk_signal_connect_object(GTK_OBJECT(add_button),
			      "clicked",
			      GTK_SIGNAL_FUNC(add_user),
			      GTK_OBJECT(window));

    del_button=gtk_button_new_with_label("Delete");
    gtk_box_pack_start (GTK_BOX (bbox), del_button, TRUE, TRUE, TRUE);
    gtk_widget_show(del_button);

    /* pin button */

    pin_button=gtk_button_new_with_label("Change PIN");
    gtk_box_pack_start (GTK_BOX (bbox), pin_button, TRUE, TRUE, TRUE);
    gtk_widget_show(pin_button);

    /*gtk_signal_connect_object(GTK_OBJECT(pin_button),
			      "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(window));*/

    /*log_button=gtk_button_new_with_label("View Log");
    gtk_widget_show(log_button);
    gtk_box_pack_start (GTK_BOX (bbox), log_button, TRUE, TRUE, TRUE);
    gtk_signal_connect_object(GTK_OBJECT(log_button),
			      "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(window));*/

      /* finally create a button and connect it´s "clicked" signal
     * to the destroyment of the window
     */
    cl_button=gtk_button_new_with_label("Close");
    gtk_box_pack_start (GTK_BOX (bbox), cl_button, TRUE, TRUE, TRUE);
    gtk_widget_show(cl_button);
    gtk_signal_connect_object(GTK_OBJECT(cl_button),
			      "clicked",
			      GTK_SIGNAL_FUNC(clean_exit),
			      GTK_OBJECT(window));

    /**** Right hand side ****/

    rvbox=gtk_vbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(rvbox), 5);
    gtk_box_pack_start (GTK_BOX (master_hbox), rvbox, TRUE, TRUE, TRUE);
    gtk_widget_show(rvbox);


    /** SName box **/

    snamebox=gtk_hbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(snamebox), 5);
    gtk_box_pack_start (GTK_BOX (rvbox), snamebox, TRUE, TRUE, TRUE);
    gtk_widget_show(snamebox);


    /* entry 2 */
    snm_label = gtk_label_new ("Surname");
    gtk_box_pack_start (GTK_BOX (snamebox), snm_label, TRUE, TRUE, 0);
    gtk_widget_show(snm_label);

    /* entry */
    sname_entry = gtk_entry_new_with_max_length (50);
    gtk_signal_connect(GTK_OBJECT(sname_entry), "changed",
		       GTK_SIGNAL_FUNC(enter_callback),
		       sname_entry);
    gtk_box_pack_start (GTK_BOX (snamebox), sname_entry, TRUE, TRUE, 0);
    gtk_entry_set_editable (sname_entry,FALSE);
    gtk_widget_show (sname_entry);
    /** FName box **/

    fnamebox=gtk_hbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(fnamebox), 5);
    gtk_box_pack_start (GTK_BOX (rvbox), fnamebox, TRUE, TRUE, TRUE);
    gtk_widget_show(fnamebox);

    /* label */

    fnm_label = gtk_label_new ("Forename");
    gtk_box_pack_start (GTK_BOX (fnamebox), fnm_label, TRUE, TRUE, 0);
    gtk_widget_show(fnm_label);

    /* entry */
    fname_entry = gtk_entry_new_with_max_length (50);
    gtk_signal_connect(GTK_OBJECT(fname_entry), "changed",
		       GTK_SIGNAL_FUNC(enter_callback),
		       fname_entry);
    gtk_box_pack_start (GTK_BOX (fnamebox), fname_entry, TRUE, TRUE, 0);
    gtk_entry_set_editable (fname_entry,FALSE);
    gtk_widget_show (fname_entry);
    

    /** Department box **/

    deptbox=gtk_hbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(deptbox), 5);
    gtk_box_pack_start (GTK_BOX (rvbox), deptbox, TRUE, TRUE, TRUE);
    gtk_widget_show(deptbox);

    /* label */

    dp_label = gtk_label_new ("Department");
    gtk_box_pack_start (GTK_BOX (deptbox), dp_label, TRUE, TRUE, 0);
    gtk_widget_show(dp_label);

    /* entry */
    dept_entry = gtk_entry_new_with_max_length (50);
    gtk_signal_connect(GTK_OBJECT(dept_entry), "changed",
		       GTK_SIGNAL_FUNC(enter_callback),
		       dept_entry);
    gtk_box_pack_start (GTK_BOX (deptbox), dept_entry, TRUE, TRUE, 0);
    gtk_entry_set_editable (dept_entry,FALSE);
    gtk_widget_show (dept_entry);
    
    /** UID box **/

    uidbox=gtk_hbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(uidbox), 5);
    gtk_box_pack_start (GTK_BOX (rvbox), uidbox, TRUE, TRUE, TRUE);
    gtk_widget_show(uidbox);

    /* label */

    uid_label = gtk_label_new ("ID Number");
    gtk_box_pack_start (GTK_BOX (uidbox), uid_label, TRUE, TRUE, 0);
    gtk_widget_show(uid_label);

    /* entry */
    uid_entry = gtk_entry_new_with_max_length (12);
    gtk_signal_connect(GTK_OBJECT(uid_entry), "changed",
		       GTK_SIGNAL_FUNC(enter_callback),
		       uid_entry);
    gtk_box_pack_start (GTK_BOX (uidbox), uid_entry, TRUE, TRUE, 0);
    gtk_entry_set_editable (uid_entry,FALSE);    
    gtk_widget_show (uid_entry);
    
    /** last entry box **/

    lastbox=gtk_hbox_new(FALSE, 5);
    gtk_container_border_width(GTK_CONTAINER(lastbox), 5);
    gtk_box_pack_start (GTK_BOX (rvbox), lastbox, TRUE, TRUE, TRUE);
    gtk_widget_show(lastbox);

    /* label */

    last_label = gtk_label_new ("Last Entry");
    gtk_box_pack_start (GTK_BOX (lastbox), last_label, TRUE, TRUE, 0);
    gtk_widget_show(last_label);

    /* entry */
    last_entry = gtk_entry_new_with_max_length (50);
    gtk_box_pack_start (GTK_BOX (lastbox), last_entry, TRUE, TRUE, 0);
    gtk_entry_set_editable (last_entry,FALSE);
    gtk_widget_show (last_entry); 

    /* the lockout check button */

    lok_button=gtk_check_button_new_with_label("User lockout");
    gtk_box_pack_start (GTK_BOX (rvbox), lok_button, TRUE, TRUE, TRUE);
    gtk_widget_show(lok_button);
    gtk_signal_connect_object(GTK_OBJECT(lok_button),
			      "clicked",
			      GTK_SIGNAL_FUNC(button_clicked),
			      GTK_OBJECT(window));

    /* a status bar */
    status_bar = gtk_statusbar_new();      
    gtk_box_pack_start (GTK_BOX (master_vbox), status_bar, TRUE, TRUE, 0);
    gtk_widget_show (status_bar);

    context_id = gtk_statusbar_get_context_id(
	GTK_STATUSBAR(status_bar), "Normal");
        gtk_statusbar_push(status_bar,context_id,
			   "Click a user to see details.");

	/* finaly we want to see the window */
    gtk_widget_show(window);
    
    /* fire up the main event loop of gtk
     */
    gtk_main();
    
    /* we get here after gtk_main_quit() has been called which
     * happens if the main window gets destroyed
     */
    return 0;
}


/* Called if a user is selected. Opens a user properties window */

void user_selected (GtkWidget *gtklist,gpointer func_data)
{
  GtkObject       *list_item;
  gchar           *item_data_string;
  GList   *dlist;
  db_item *item;

  if(!adding_user){
    /* fetch the doubly linked list of selected items
     * of the GtkList, remember to treat this as read-only!
     */
    dlist=GTK_LIST(gtklist)->selection;
    
    /* if there are no selected items there is nothing more
     * to do than just telling the user so
     */
    if (!dlist) {
      g_print("Selection cleared\n");
      return;
    }
    /* ok, we got a selection and so we print it
     */
    
    /* get the list item from the doubly linked list
     * and then query the data associated with list_item_data_key
     * we then just print it
   */
    list_item=GTK_OBJECT(dlist->data);
    item_data_string=gtk_object_get_data(list_item,
				       list_item_data_key);
    item = get_db_item(item_data_string);
    
    if(item){
      gtk_entry_set_text (GTK_ENTRY (sname_entry),item->sname);
      gtk_entry_set_text (GTK_ENTRY (fname_entry),item->fname);
      gtk_entry_set_text (GTK_ENTRY (dept_entry),item->dept);
      gtk_entry_set_text (GTK_ENTRY (uid_entry),item->uid);
      if(item->lock == 'L') gtk_toggle_button_set_state (lok_button,TRUE);
      else gtk_toggle_button_set_state(lok_button,FALSE);
      
      printf("%s\n",item->photo);
  }
  else printf("Data record not found\n");
  }
}  

void add_user( GtkWidget *widget, gpointer *data ) {

  if(adding_user == FALSE) adding_user = TRUE;
  else if(strlen(add_user_item.uid) == 12){ 
    /* copy the text entries into "final" buffer */
    strcpy(final_add_item.sname,add_user_item.sname);
    strcpy(final_add_item.fname,add_user_item.fname);
    strcpy(final_add_item.uid,add_user_item.uid);
    strcpy(final_add_item.dept,add_user_item.dept);
    /* open an enter pin requester */
    enter_pin();
  }
  else {
      gtk_statusbar_push(status_bar,context_id,
	"Serial number must be 12 digits, as shown on your iButton");
      prev_error = TRUE;
      return;
  }

  gtk_window_set_title(window,"User Database - Add User");
  
  gtk_entry_set_text (GTK_ENTRY (sname_entry), "");
  gtk_entry_set_text (GTK_ENTRY (fname_entry), "");
  gtk_entry_set_text (GTK_ENTRY (uid_entry), "");
  gtk_entry_set_text (GTK_ENTRY (dept_entry), "");
  gtk_entry_set_text (GTK_ENTRY (last_entry), "--/--/--");
  
  gtk_entry_set_editable (sname_entry,TRUE);
  gtk_entry_set_editable (fname_entry,TRUE);
  gtk_entry_set_editable (uid_entry,TRUE);
  gtk_entry_set_editable (dept_entry,TRUE);

  gtk_statusbar_push(status_bar,context_id,
		      "Fill in the text fields. Click Add again to finish");
}

/* callback function for all entry widgets */
void enter_callback(GtkWidget *widget, GtkWidget *entry)
{
  gchar *entry_text;
  entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

  if(prev_error){   
    gtk_statusbar_pop(status_bar,context_id);
    prev_error = FALSE;
  }
  /* surname */
  if(widget == sname_entry) {
    strcpy(add_user_item.sname,entry_text);
  }
  /* forename */
  if(widget == fname_entry) strcpy(add_user_item.fname,entry_text);
  if(widget == uid_entry) {
    strcpy(add_user_item.uid,entry_text);
  }
  if(widget == dept_entry) strcpy(add_user_item.dept,entry_text);
  if(widget == pin_entry1) strcpy(pin_1,entry_text);
  if(widget == pin_entry2) strcpy(pin_2,entry_text);
}

/* PIN entry dialog box */
void enter_pin()
{
  pinwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(pinwin), "PIN Entry");

  gtk_widget_show(pinwin);

  pin_vbox=gtk_vbox_new(FALSE, 5);
  gtk_container_border_width(GTK_CONTAINER(pin_vbox), 5);
  gtk_container_add(GTK_CONTAINER(pinwin), pin_vbox);    
  gtk_widget_show(pin_vbox);

  /* Text */
  pin_label = gtk_label_new ("Enter your 4-digit pin in each box");
  gtk_box_pack_start (GTK_BOX (pin_vbox), pin_label, TRUE, TRUE, 0);
  gtk_widget_show(pin_label);

  /* entry */
  pin_entry1 = gtk_entry_new_with_max_length (4);
  gtk_signal_connect(GTK_OBJECT(pin_entry1), "changed",
		     GTK_SIGNAL_FUNC(enter_callback),
		       pin_entry1);
  gtk_box_pack_start (GTK_BOX (pin_vbox), pin_entry1, TRUE, TRUE, 0);
  gtk_entry_set_visibility (pin_entry1,FALSE);
  gtk_widget_show (pin_entry1); 
  
  /* entry */
  pin_entry2 = gtk_entry_new_with_max_length (4);
  gtk_signal_connect(GTK_OBJECT(pin_entry2), "changed",
		     GTK_SIGNAL_FUNC(enter_callback),
		       pin_entry2);
  gtk_box_pack_start (GTK_BOX (pin_vbox), pin_entry2, TRUE, TRUE, 0);
  gtk_entry_set_visibility (pin_entry2,FALSE);
  gtk_widget_show (pin_entry2); 

  /* hbox */
  pinbuttonbox=gtk_hbox_new(FALSE, 5); 
  gtk_container_border_width(GTK_CONTAINER(pinbuttonbox), 5);
  gtk_box_pack_start (GTK_BOX (pin_vbox), pinbuttonbox, FALSE, FALSE, TRUE);
  gtk_widget_show(pinbuttonbox);

  /* OK button */
  
  pin_ok_button=gtk_button_new_with_label("OK");
  gtk_box_pack_start (GTK_BOX (pinbuttonbox), pin_ok_button, TRUE, TRUE, TRUE);
  gtk_widget_show(pin_ok_button);
  gtk_signal_connect_object(GTK_OBJECT(pin_ok_button),
			    "clicked",
			    GTK_SIGNAL_FUNC(finished_pin),
			    GTK_OBJECT(pin_ok_button));
  gtk_widget_show (pin_ok_button); 
  
 
  /* Cancel button */
  
  pin_c_button=gtk_button_new_with_label("Cancel");
  gtk_box_pack_start (GTK_BOX (pinbuttonbox), pin_c_button, TRUE, TRUE, TRUE);
  gtk_widget_show(pin_c_button);
  gtk_signal_connect_object(GTK_OBJECT(pin_c_button),
			    "clicked",
			    GTK_SIGNAL_FUNC(finished_pin),
			    GTK_OBJECT(pin_c_button));
  gtk_widget_show (pin_c_button); 
  
}

/* validates PIN */
void finished_pin(GtkWidget *widget, gpointer *data )
{
  char status_text[100];
  printf("Entered finished pin. %s\n",add_user_item.sname);
  /* To Do: check whether PIN entries are equal and valid. Save in
     global struct, and add user to linked list in memory, in a sorted
     position. Flag a changed database. The whole thing is resaved on
     exit of program */

  /* If cancel is pressed, clear input vars etc */
  if(widget == pin_c_button){
    gtk_statusbar_push(status_bar,context_id,"Add user cancelled");
    clear_add_item();

    adding_user = FALSE;
    gtk_object_destroy(pinwin);
  }
  else if(widget == pin_ok_button){
    /* check that pins are the same and 4 chars long */
    if(strcmp(pin_1,pin_2) == 0 &&
       strlen(pin_1) == 4 && strlen(pin_2) == 4){
      /* PIN OK */
      printf("%s %s\n",final_add_item.fname,add_user_item.sname);
      adding_user = FALSE;
      sprintf(status_text,"%s %s has been added to the user database.",
	      final_add_item.fname,final_add_item.sname);
      gtk_statusbar_push(status_bar,context_id,status_text);
      gtk_object_destroy(pinwin);      
      gtk_window_set_title(window,"User Database");
      create_new_user();
    }
    else {
      /* PIN entries different */
      gtk_statusbar_push(status_bar,context_id,
			 "You must enter the same 4-digit PIN twice");
      gtk_entry_set_text (GTK_ENTRY (pin_entry1),"");
      gtk_entry_set_text (GTK_ENTRY (pin_entry2),"");
    }
  }
} 
      
/* resets entry widgets to no string */
void clear_add_item()
{
  add_user_item.sname[0] = '\0';
  add_user_item.fname[0] = '\0';
  add_user_item.dept[0] = '\0';
  add_user_item.uid[0] = '\0';
  gtk_entry_set_text (GTK_ENTRY (sname_entry),"");
  gtk_entry_set_text (GTK_ENTRY (fname_entry),"");
  gtk_entry_set_text (GTK_ENTRY (uid_entry),"");
  gtk_entry_set_text (GTK_ENTRY (dept_entry),"");
  gtk_entry_set_text (GTK_ENTRY (pin_entry1),"");
  gtk_entry_set_text (GTK_ENTRY (pin_entry2),"");
}

/* Allocates memory for user record, copies data and links it into the
 * list. Also adds it to the GUI scrolled list.
 */

void create_new_user()
{
  db_item *new_item,*prev, *curr = database, *temp;
  int listpos=0;
  GtkObject *list_item;
  gchar *item_data_string;
  GList *dlist;
  char name[60];
  GtkWidget       *label;

  new_item = (db_item *)malloc(sizeof(db_item));

  strcpy(new_item->uid,final_add_item.uid);
  strcpy(new_item->pin,pin_1);
  strcpy(new_item->fname,final_add_item.fname);
  strcpy(new_item->sname,final_add_item.sname);
  strcpy(new_item->dept,final_add_item.dept);
  strcpy(new_item->photo,"%");
  new_item->lock = 'N';
  new_item->next = NULL;
  printf("Created new user %s\n",new_item->sname);
  
  curr = database;
  printf("first name: %s\n",database->sname);

  /* find insertion point */
  while(curr!=NULL){ 
    int r;
    r = strcmp(new_item->sname,curr->sname);
    if(r < 0) break;
    else {
      printf("compared %s: %i\n",curr->sname,r);
      prev = curr;
      curr = curr->next;
      listpos++; /* so we know were to insert in GUI list */
    }
  }
  printf("Inserting in position %i\n",listpos);
  
  /* when we find it, insert it! */
  if(listpos == 0){
    temp = database;
    database = new_item;
    new_item->next = temp;
  } else {
    temp = prev -> next;
    prev->next = new_item;
    new_item->next = temp;
  }
  
  
  /* print out list */
  curr = database;
  while(curr){
    printf("%s %s %s\n",curr->fname,curr->sname,curr->uid);
    curr = curr->next;
  }
  
  /* add into GUI list */
  
  sprintf(name,"%s %s",new_item->fname,new_item->sname);
  label=gtk_label_new(name);
  list_item=gtk_list_item_new();
  gtk_container_add(GTK_CONTAINER(list_item), label);
  gtk_widget_show(label);
  gtk_container_add(GTK_CONTAINER(gtklist), list_item);
  gtk_widget_show(list_item);
  gtk_label_get(GTK_LABEL(label), name);
  gtk_object_set_data(GTK_OBJECT(list_item),
		      list_item_data_key,
		      name);
}
