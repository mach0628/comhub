// ServerChennel.cpp: implementation of the CServerChennel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chennel.h"
#include "ServerChennel.h"

#ifdef __OS_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#endif

#ifdef WIN32
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#pragma comment(lib, "ws2_32.lib")
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __OS_LINUX
 #define ioctlsocket ioctl
 #define closesocket close
 #define  GETSOCKETERROR()   errno
#endif
#ifdef  WIN32
//##ModelId=43586653007E
 typedef int socklen_t;
 #define  GETSOCKETERROR()  WSAGetLastError()
 #define  EWOULDBLOCK     WSAEWOULDBLOCK
// #define  EINTR           WSAEINTR
#define   ECONNRESET    WSAECONNRESET
 #define  SHUT_RDWR       SD_BOTH
 #define  SHUT_WR         SD_SEND 
#endif

//##ModelId=4358665203A9
CServerChennel::CServerChennel(int nPort)
{
    mPort = nPort;
    mHandle = -1;
#ifdef WIN32
	WORD wVersion;
	WSADATA WSAData;
	wVersion=MAKEWORD(2,0);
	if( WSAStartup(wVersion,&WSAData)!=0 )  return;
#endif
}

//##ModelId=4358665203AB
CServerChennel::~CServerChennel()
{
	CloseChennel();
}
//##ModelId=4358665203AD
int CServerChennel::CloseChennel()
{
	if( mHandle<0 ) return 1;
	Trace::traceEvent(TRACE_NORMAL,"listen %d is down",mPort);
	closesocket (mHandle);
	mHandle=-1;
	return 1;
}
//##ModelId=4358665203AF
int CServerChennel::CanRead()
{
	if( mHandle<0 ) return -1;
	struct timeval timeout = { 0, 0 };
	fd_set readSocketSet;

	FD_ZERO( &readSocketSet );
	FD_SET( mHandle, &readSocketSet );
	switch(select( mHandle+1, &readSocketSet, NULL, NULL, &timeout )){
	case 1: return 1;
	case 0: return -1;
	default:
		CloseChennel(); 
	}
	return -1;
}
//##ModelId=4358665203AE
int CServerChennel::OpenChennel()
{
	int  nRet,optval,nRetryTimes;
	struct linger m_linger;
	if  ( (mHandle = socket( AF_INET,SOCK_STREAM,0 ) )< 0 )	{
		return -1;
	}

	m_linger.l_onoff=1;
	m_linger.l_linger=0;
	if ( setsockopt(mHandle,SOL_SOCKET,SO_LINGER,(char*)&m_linger,
						sizeof(m_linger))<0 ){
		CloseChennel();
		return -1;
	}

	optval = 1;		/* set socket re-use local address*/
	if(setsockopt(mHandle,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof(optval))<0)	{
		CloseChennel();
		return -1;
	}

	memset( ( char*) &svr_addr,0,sizeof( svr_addr ) );
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	svr_addr.sin_port = htons( mPort );

	/* binding */
	nRetryTimes = 0;
	nRet=bind(mHandle,(struct sockaddr *)&svr_addr,sizeof(svr_addr));
	while( nRet<0 && nRetryTimes<10 )	{
		nRetryTimes++;
#ifdef WIN32
		::Sleep(1000);
#else
		sleep(1);
#endif
		nRet=bind(mHandle,(struct sockaddr *)&svr_addr,sizeof(svr_addr));
	}
	if ( nRet<0 )	{
		CloseChennel();
		return -1;
	}

   u_long ch = 1;
   if( ioctlsocket ( mHandle,FIONBIO,&ch)<0){
      CloseChennel();
      return -1;
   }

	if  ( listen( mHandle,10) < 0 )	{
		CloseChennel();
		return -1;
	}
	return mHandle;
}
//##ModelId=4358665203B0
CChennel * CServerChennel::ReadSocket()
{
	int  nFd;
	socklen_t nLen;
   	sockaddr_in	client_addr;
	struct linger m_linger;

	nLen = sizeof( client_addr );
	nFd=accept(mHandle,(struct sockaddr*)&client_addr,&nLen);
	if  ( nFd<0 ){
		int nErr = GETSOCKETERROR();
		if ( nErr != EWOULDBLOCK && nErr != EINTR && nErr!=ECONNRESET )	{
			CloseChennel();
		}
		
		return NULL;
	}

   u_long ch = 1;
   if( ioctlsocket ( nFd,FIONBIO,&ch)<0){
      closesocket(nFd);
      return NULL;
   }

	m_linger.l_onoff=1;
	m_linger.l_linger=0;
	if ( setsockopt(nFd,SOL_SOCKET,SO_LINGER,
		(char*)&m_linger,sizeof(m_linger))<0 ){
		closesocket(nFd);
		return NULL;
	}
	CChennel * p = new CChennel();
	p->Attach(nFd,client_addr);
	Trace::traceEvent(TRACE_DEBUG,"client %s:%d up",p->mAddr,p->mPort);
	return p;
}


