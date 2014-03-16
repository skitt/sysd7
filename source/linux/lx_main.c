/* Linux Main */

#include <signal.h>

int audio_pid;
extern void close_application(int);

/* forks off a child process for audio and saves its uid in a global */

int main( int argc, char *argv[] )
{
  audio_pid = fork();
  if(audio_pid == 0) audio_main();
  else {
    printf("forked Audio with pid %i\n",audio_pid);
    /* set up ctrl-c handler */
    signal(SIGINT,close_application);
    gui_main(argc,argv);
  }
}
