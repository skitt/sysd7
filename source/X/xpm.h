/* xpm.h */
/* Edward Knowelden */
/* Definitions for conversion to xpm & doubling */

#define XPM_WIDTH 320
#define XPM_HEIGHT 240
#define XPM_LINES 256
#define IMAGE_DATA_SIZE 19200
#define XPM_DATA_OFFSET 17

/* function prototypes */

void get_xpm_info(char *info_line, int *w, int *h, int *c);
void clean_exit(char *);
void init_xpm();
void free_mem();
void output_xpm();

/* globals */

int auto_col; /* increments greyscale for testing */
char **xpm; /* base pointer for xpm */
