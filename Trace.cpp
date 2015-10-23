#include "stdafx.h"
#include "Trace.h"
#include "Prefs.h"
#include "Utils.h"
/* ******************************* */
u_int8_t Trace::traceLevel=TRACE_LEVEL_NORMAL;


/* ******************************* */

void Trace::set_trace_level(u_int8_t id) {
  if(id > MAX_TRACE_LEVEL) id = MAX_TRACE_LEVEL;

  traceLevel = id;
}

/* ******************************* */

void Trace::traceEvent(int eventTraceLevel, const char* _file,
		       const int line, const char * format, ...) {
  va_list va_ap;

   if(eventTraceLevel <= traceLevel) {
    char buf[4096], out_buf[4096];
    char theDate[32], *file = (char*)_file;
    const char *extra_msg = "";
    time_t theTime = time(NULL);


#ifdef WIN32
    char filebuf[MAX_PATH];
    const char *backslash = strrchr(_file, '\\');

    if(backslash != NULL) {
      snprintf(filebuf, sizeof(filebuf), "%s", &backslash[1]);
      file = (char*)filebuf;
    } 
#endif

    va_start (va_ap, format);

    /* We have two paths - one if we're logging, one if we aren't
     *   Note that the no-log case is those systems which don't support it (WIN32),
     *                                those without the headers !defined(USE_SYSLOG)
     *                                those where it's parametrically off...
     */

    memset(buf, 0, sizeof(buf));
	memset(theDate, 0, sizeof(theDate));
	Utils::formattime(theDate,theTime);
    //strftime(theDate, 32, "%d/%b/%Y %H:%M:%S", localtime_r(&theTime, &result));

    vsnprintf(buf, sizeof(buf)-1, format, va_ap);

    if(eventTraceLevel == 0 /* TRACE_ERROR */)
      extra_msg = "ERROR: ";
    else if(eventTraceLevel == 1 /* TRACE_WARNING */)
      extra_msg = "WARNING: ";

    while(buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = '\0';

    // trace_mutex.lock();
    snprintf(out_buf, sizeof(out_buf), "%s [%s:%d] %s%s", theDate, file, line, extra_msg, buf);

    if(gPrefs && gPrefs->LogFd()) {
      fprintf(gPrefs->LogFd(), "%s\n", out_buf);
      fflush(gPrefs->LogFd());
    }

    printf("%s\n", out_buf);
    fflush(stdout);

    // trace_mutex.unlock();

#ifndef WIN32

#ifdef USE_SYSLOG
    char *syslogMsg;
    syslogMsg = &out_buf[strlen(theDate)+1];
    if(eventTraceLevel == 0 /* TRACE_ERROR */)
      syslog(LOG_ERR, "%s", syslogMsg);
    else if(eventTraceLevel == 1 /* TRACE_WARNING */)
      syslog(LOG_WARNING, "%s", syslogMsg);
#endif

#endif

    va_end(va_ap);
  }
}

void Trace::traceEventN(int eventTraceLevel, const char* _file,
		       const int line, const char * format, ...) {
  va_list va_ap;

   if(eventTraceLevel <= traceLevel) {
    char buf[4096], out_buf[4096];
    char theDate[32], *file = (char*)_file;
    const char *extra_msg = "";
    time_t theTime = time(NULL);


#ifdef WIN32
    char filebuf[MAX_PATH];
    const char *backslash = strrchr(_file, '\\');

    if(backslash != NULL) {
      snprintf(filebuf, sizeof(filebuf), "%s", &backslash[1]);
      file = (char*)filebuf;
    } 
#endif

    va_start (va_ap, format);

    /* We have two paths - one if we're logging, one if we aren't
     *   Note that the no-log case is those systems which don't support it (WIN32),
     *                                those without the headers !defined(USE_SYSLOG)
     *                                those where it's parametrically off...
     */

    memset(buf, 0, sizeof(buf));
	memset(theDate, 0, sizeof(theDate));
	Utils::formattime(theDate,theTime);
    //strftime(theDate, 32, "%d/%b/%Y %H:%M:%S", localtime_r(&theTime, &result));

    vsnprintf(buf, sizeof(buf)-1, format, va_ap);

    if(eventTraceLevel == 0 /* TRACE_ERROR */)
      extra_msg = "ERROR: ";
    else if(eventTraceLevel == 1 /* TRACE_WARNING */)
      extra_msg = "WARNING: ";

    

    // trace_mutex.lock();
    snprintf(out_buf, sizeof(out_buf), "%s [%s:%d] %s%s", theDate, file, line, extra_msg, buf);

    if(gPrefs && gPrefs->LogFd()) {
      fprintf(gPrefs->LogFd(), "%s", out_buf);
      fflush(gPrefs->LogFd());
    }

    printf("%s", out_buf);
    fflush(stdout);

    // trace_mutex.unlock();

#ifndef WIN32

#ifdef USE_SYSLOG
    char *syslogMsg;
    syslogMsg = &out_buf[strlen(theDate)+1];
    if(eventTraceLevel == 0 /* TRACE_ERROR */)
      syslog(LOG_ERR, "%s", syslogMsg);
    else if(eventTraceLevel == 1 /* TRACE_WARNING */)
      syslog(LOG_WARNING, "%s", syslogMsg);
#endif

#endif

    va_end(va_ap);
  }
}

void Trace::traceEventRaw(const char * format, ...) {
  va_list va_ap;

    char buf[4096];

    va_start (va_ap, format);

    memset(buf, 0, sizeof(buf));
    vsnprintf(buf, sizeof(buf)-1, format, va_ap);  

    if(gPrefs && gPrefs->LogFd()) {
      fprintf(gPrefs->LogFd(), "%s", buf);
      fflush(gPrefs->LogFd());
    }

    printf("%s", buf);
    fflush(stdout);


    va_end(va_ap);
}

