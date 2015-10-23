// comhub.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Utils.h"
#include "Prefs.h"
#include "HubCenter.h"
HubCenter * gCenter = NULL;
#ifndef WIN32
#define _tzset tzset
#endif

static void sigproc(int sig) {
  static int called = 0;
	Trace::traceEvent(TRACE_NORMAL, "Receive signal %d.",sig);
  if(called) {
	  Trace::traceEvent(TRACE_NORMAL, "Ok I am leaving now");
    exit(0);
  } else {
    Trace::traceEvent(TRACE_NORMAL, "Shutting down...");
	if( gCenter ) gCenter->Quit();
    called = 1;
  }
  if( gCenter ) gCenter->Quit();

  //sleep(1); /* Wait until all threads know that we're shutting down... */

#ifndef WIN32
  if(gPrefs->IsDaemonize()){
	  if(gPrefs->PidPath() != NULL) {
		int rc = unlink(gPrefs->PidPath());
		Trace::traceEvent(TRACE_NORMAL, "Deleted PID %s [rc: %d]", 
					 gPrefs->PidPath(), rc);
	  }
  }
#endif

  Trace::traceEvent(TRACE_NORMAL, "Ok signal finish now");
}


static void Daemonize() {

	Trace::traceEvent(TRACE_NORMAL,"com hub set to daemonize");
#ifndef WIN32
  int childpid;

  signal(SIGHUP, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);

  if((childpid = fork()) < 0)
    Trace::traceEvent(TRACE_ERROR, "Occurred while daemonizing (errno=%d)",
				 errno);
  else {
    if(!childpid) { /* child */
      int rc;

      //Trace::traceEvent(TRACE_NORMAL, "Bye bye: I'm becoming a daemon...");

#if 1
      rc = chdir("/");
      if(rc != 0)
	Trace::traceEvent(TRACE_ERROR, "Error while moving to / directory");

      setsid();  /* detach from the terminal */

      fclose(stdin);
      fclose(stdout);
      /* fclose(stderr); */

      /*
       * clear any inherited file mode creation mask
       */
      umask(0);

      /*
       * Use line buffered stdout
       */
      /* setlinebuf (stdout); */
      setvbuf(stdout, (char *)NULL, _IOLBF, 0);
#endif
    } else { /* father */
      Trace::traceEvent(TRACE_NORMAL,
				   "Parent process is exiting (this is normal)");
      exit(0);
    }
  }
#endif
}

int main(int argc, char *argv[])
{
  
	_tzset();
	if((gPrefs = new Prefs(argv[0])) == NULL)  exit(0);
	if( gPrefs->LoadFromCLI(argc, argv) < 0) return(-1);
	gCenter = new HubCenter();
	gCenter->Build();
  	if(gPrefs->IsDaemonize()) {  
	  Daemonize();
#ifndef WIN32
	  if(gPrefs->PidPath() != NULL) {
    		FILE *fd = fopen(gPrefs->PidPath(), "w");
    		if(fd != NULL) {
      			fprintf(fd, "%u\n", getpid());
      			fclose(fd);
      			chmod(gPrefs->PidPath(), 0777);
     			Trace::traceEvent(TRACE_NORMAL, "PID stored in file %s", gPrefs->PidPath());
    		} else
      		Trace::traceEvent(TRACE_ERROR, "Unable to store PID in file %s", gPrefs->PidPath());
 	  }
#endif
  	}
  	signal(SIGINT, sigproc);
  	signal(SIGTERM, sigproc);
  	signal(SIGINT, sigproc);
	gCenter->Run();
	gCenter->Cleanup();
	delete gCenter;
	delete gPrefs;
	gPrefs = 0;
	Trace::traceEvent(TRACE_NORMAL, "main finish");
}

