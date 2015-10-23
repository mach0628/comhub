//
#pragma once

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0501	
#endif						

#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#ifdef WIN32
#include "hubwin32.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dirent.h>

typedef int                 BOOL;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define _ASSERT( exp) 

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef int                 INT;
typedef unsigned int        UINT;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;

typedef int HANDLE;
#ifndef CONST
#define CONST               const
#endif

#endif




#ifndef MAX_PATH
#define MAX_PATH                256
#endif

#ifdef WIN32
#define mkdir_s(a, b) _mkdir(a)
#define CONST_PATH_SEP                    '\\'
#define SLEEP( a ) Sleep(a)
#else
#define mkdir_s(a, b) mkdir(a, b)
#define CONST_PATH_SEP                    '/'
#define SLEEP( a )   usleep(a *1000)
#endif

#ifdef __GNUG__
typedef  int SOCKET;
typedef   CONST   CHAR   *LPCSTR,   *PCSTR;   
typedef   LPCSTR   LPCTSTR;  
#define strcmpi strcasecmp
#else // not gcc

#endif


#ifndef min
#define min(a,b) ((a < b) ? a : b)
#endif

#ifndef max
#define max(a,b) ((a > b) ? a : b)
#endif

#include "new_debug.h"
#include "Trace.h"
// \r 0x0d
// \n 0x0a
#define CR   0x0d
#define LF   0x0a
#define CRLF "\r\n"



