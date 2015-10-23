#ifndef _HUB_WIN32_H_
#define _HUB_WIN32_H_

 

#include <windows.h>

#include <io.h>
#include <stdio.h>

#include <direct.h>  /* mkdir */


/* Values for the second argument to access. These may be OR'd together.  */
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */

#define access _access
#define ftruncate _chsize



/* getopt.h */
#define __GNU_LIBRARY__ 1

#ifndef __GNUC__
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   uint;
typedef unsigned long  u_long;
#endif

typedef u_char  u_int8_t;
typedef u_short u_int16_t;
typedef uint   u_int32_t;
typedef int   int32_t;
typedef unsigned __int64 u_int64_t;
typedef __int64 int64_t;


#define localtime_r(a, b) localtime_s(b,a)
#define strcmpi _strcmpi


#if _MSC_VER 
#define snprintf _snprintf 
#endif


#endif /* _NTOP_WIN32_H_ */

