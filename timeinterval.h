#ifndef __TIMEINTERVAL_H__
#define __TIMEINTERVAL_H__
#include <time.h>

#ifdef WIN32
#include <sys\timeb.h>
  #define DECLARE_TIME_INTERVAL(name) _timeb name##start, name##end
  #define RESET_TIME_INTERVAL(name) _ftime(&name##start)
  #define STAMP_TIME_INTERVAL(name)  _ftime(&name##end)
  #define GET_TIME_INTERVAL(name)  (1000*(name##end.time-name##start.time)+ name##end.millitm-name##start.millitm)
  #define GET_NOW_INTERVAL(name)  (_ftime(&name##end),GET_TIME_INTERVAL(name))
  #define PRINT_TIME_INTERVAL(name) ACE_DEBUG((LM_DEBUG,ACE_TEXT("second:%ld,usecond:%ld\n"),name.time,name.millitm))

#else
#include <sys/time.h>
  #define DECLARE_TIME_INTERVAL(name) timeval name##start, name##end
  #define RESET_TIME_INTERVAL(name) gettimeofday(&name##start,NULL)
  #define STAMP_TIME_INTERVAL(name)  gettimeofday(&name##end,NULL)
  #define GET_TIME_INTERVAL(name)  (1000*(name##end.tv_sec-name##start.tv_sec)+ (name##end.tv_usec-name##start.tv_usec)/1000)
  #define GET_NOW_INTERVAL(name)  (gettimeofday(&name##end,NULL),GET_TIME_INTERVAL(name))
  #define PRINT_TIME_INTERVAL(name) ACE_DEBUG((LM_DEBUG,ACE_TEXT("second:%ld,usecond:%ld\n"),name##.tv_sec,name##.tv_usec))

#endif
void GetNow(tm& nowtime);
#endif
