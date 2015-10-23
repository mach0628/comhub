// Chennel.cpp: implementation of the CChennel class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Chennel.h"

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
#ifdef __OS_DOS
//#define send        send
  inline int GetLastError(void){ return errno;}
#endif

#ifdef __OS_LINUX
 #define ioctlsocket ioctl
 #define closesocket close
 #define  GETSOCKETERROR()   errno
#endif

#ifdef  OS_AIX
 #define ioctlsocket ioctl
 #define closesocket close
 #define  GETSOCKETERROR()   errno
 #define  bzero(__s,__n)      memset(__s,0,__n)
#endif

#ifdef  WIN32
 #define  GETSOCKETERROR()  WSAGetLastError()
 #define  EWOULDBLOCK     WSAEWOULDBLOCK
 #define  SHUT_RDWR       SD_BOTH
 #define  SHUT_WR         SD_SEND 
#endif
#ifndef WIN32
 #define SOCKET_ERROR -1
 #define INVALID_SOCKET -1
#endif
#define mprintf printf

//##ModelId=43586653006F
CChennel::CChennel()
{
#ifdef WIN32
	WORD wVersion;
	WSADATA WSAData;
	wVersion=MAKEWORD(2,0);
	if( WSAStartup(wVersion,&WSAData)!=0 )  return;
#endif
	mAddr[0]=0;
	mPort = 0;
	mHandle = INVALID_SOCKET;
//	mData = NULL;
//	if(nSize>0) mData = new char[nSize];
}
//##ModelId=435866530071
CChennel::CChennel(char * sAddr,int port)
{
 #ifdef WIN32
	WORD wVersion;
	WSADATA WSAData;
	wVersion=MAKEWORD(2,0);
	if( WSAStartup(wVersion,&WSAData)!=0 )  return;
#endif
  strncpy(mAddr,sAddr,23);
   mPort = port;
//	mData = NULL;
//	if(nSize>0) mData = new char[nSize];
}
//##ModelId=435866530075
CChennel::~CChennel()
{
  CloseChennel();
}

void CChennel::Attach(CChennel & chennel){
        CloseChennel();
        mHandle = chennel.mHandle;
        mPort = chennel.mPort;
        strcpy(mAddr,chennel.mAddr);
};
void CChennel::Attach(int handle,sockaddr_in in){
        CloseChennel();
        mHandle = handle;
        //stLclAddr = in;
        mPort = in.sin_port;
        strcpy(mAddr,inet_ntoa(in.sin_addr));
};
void CChennel::Attach(int handle,const char * sIP,int port){
        CloseChennel();
        mHandle = handle;
        strncpy(mAddr,sIP,23);
        mPort = port;
};
void CChennel::Attach(const char * sIP,int port){
        CloseChennel();
        strncpy(mAddr,sIP,23);
        mPort = port;
}
//##ModelId=43586653005E
int CChennel::OpenChennel()
{
	    /* Get a TCP socket */
  struct sockaddr_in	stLclAddr;      /* Sockets Address Structure */

#ifdef SOCKET_TRACE
	mprintf("Open Connection [%s:%d]",mPorts[index].sIp,mPorts[index].iHPort);
#endif
	if (mHandle !=INVALID_SOCKET ){
		shutdown(mHandle,2);
		closesocket (mHandle);
		mHandle=INVALID_SOCKET;
	}

    memset(&stLclAddr,0,sizeof(stLclAddr));
     /* Get IP Address from a hostname (or IP address string in dot-notation) */

  mHandle = socket (AF_INET, SOCK_STREAM, 0);
  if (mHandle ==INVALID_SOCKET)  {
	mprintf("Open Socket Failure\n");
	return -1;
  }
   /* Initialize the Sockets Internet Address (sockaddr_in) structure */
   stLclAddr.sin_addr.s_addr =inet_addr(mAddr);
   stLclAddr.sin_family      = AF_INET;      /* Internet Address Family */
   stLclAddr.sin_port        = htons(mPort);  /* TCP Echo Port */

   if (connect(mHandle,(struct sockaddr*)&stLclAddr,sizeof(stLclAddr)) < 0) {
       mprintf("fail Connect to %s:%u\n",mAddr,mPort );
	   closesocket (mHandle);
	   mHandle=INVALID_SOCKET;
	   return -1;
   }
   u_long ch = 1;
   if( ioctlsocket ( mHandle,FIONBIO,&ch)<0){
      mprintf("Set Socket to nonblocking failure\n");
      CloseChennel();
      return -1;
   }

#ifdef SOCKET_TRACE
   BTRACE1("Connect OK socket is %d!!",mPorts[index].wTcpSock);
#endif   
	RESET_TIME_INTERVAL(mTP);
   return 1;

}

//##ModelId=43586653005D
int CChennel::CloseChennel()
{
#ifdef SOCKET_TRACE
    printf("Close Socket [%d]",index);
#endif
    if( mHandle !=INVALID_SOCKET ){
		Trace::traceEvent(TRACE_DEBUG,"client %s:%d is down",mAddr,mPort);
		shutdown(mHandle,0);
		closesocket (mHandle);
		mHandle=INVALID_SOCKET;
    }
	return 1;
}
//##ModelId=43586653006D
int CChennel::IsClose()
{
	if( mHandle==INVALID_SOCKET ) return 1;
	int bConnDropped = FALSE;
	int iRet = 0;
	int bOK = TRUE;

	struct timeval timeout = { 0, 0 };
	fd_set readSocketSet;

	FD_ZERO( &readSocketSet );
	FD_SET( mHandle, &readSocketSet );

	iRet = select( mHandle+1, &readSocketSet, NULL, NULL, &timeout );
	bOK = ( iRet > 0 );

	if( bOK )
	{
		bOK = FD_ISSET( mHandle, &readSocketSet );
		char szBuffer[1] = "";
		iRet = recv( mHandle, szBuffer, 1, MSG_PEEK );
		bOK = ( iRet > 0 );
		if( !bOK )
		{
			int iError = GETSOCKETERROR();
			bConnDropped = ( (iError!=EWOULDBLOCK)
#ifdef __OS_LINUX
				&&(iError!=EAGAIN) 
#endif
				);
				/*( ( iError == WSAENETRESET ) ||
				( iError == WSAECONNABORTED ) ||
				( iError == WSAECONNRESET ) ||
				( iError == WSAEINVAL ) ||
				( iRet == 0 ) ); //Graceful disconnect from other side.
				*/
		}

	}
	    return( bConnDropped );
}
//##ModelId=435866530065
int CChennel::CanRead()
{
	struct timeval timeout = { 0, 0 };
	fd_set readSocketSet;
	if( mHandle == INVALID_SOCKET ) return -1;
	FD_ZERO( &readSocketSet );
	FD_SET( mHandle, &readSocketSet );
	int i=select( mHandle+1, &readSocketSet, NULL, NULL, &timeout );
    if( i>0){
		unsigned long templen=0;
		if( ioctlsocket (mHandle,FIONREAD,&templen)<0 ){
			CloseChennel();
			return -1;
		}
		return templen;
	}else if( i<0 ) CloseChennel();
	return i;
}
int  CChennel::ReadData(char * buff,int* length,int nMillSec)
{
  int i;
  timeval mSocketTime;
  unsigned long templen=0;
  int total = *length;
  if( *length<1 ) return 0;
  if( buff==NULL ) return 0;
  if( mHandle==INVALID_SOCKET ) return -1;
  fd_set rdfds;
  DECLARE_TIME_INTERVAL(tmp);
  RESET_TIME_INTERVAL(tmp);
  while(GET_NOW_INTERVAL(tmp)<nMillSec ){
	  FD_ZERO(&rdfds);
	  FD_SET(mHandle, &rdfds);
	  mSocketTime.tv_sec = 0L;
	  mSocketTime.tv_usec = 5000L;
	  i = select( mHandle+1, &rdfds, NULL, NULL,&mSocketTime);
	  if( i <0 ) {
	  	  CloseChennel();
		   return -1;
	  }
	  if( i==0 ){ 
		  SLEEP( 5 );
		  continue;
	  }
	  if( i!=1 ) return 0;
	  i=ioctlsocket (mHandle,FIONREAD,&templen);
	  if(i==-1){
		 CloseChennel();
		 return -1;
	  }
	  if( templen == 0 ) continue;

	  if( (long)templen>total ) templen = total;
	  RESET_TIME_INTERVAL(mTP);
	  i = recv (mHandle,buff,templen,0);
	  if( i< 0 ){
		if( GETSOCKETERROR() == EWOULDBLOCK ) return 0;
		CloseChennel();
		return -1;
	  } 
	  total -=templen;
	  if( total<=0 ) return 1;
  }
  *length-= total;
  return 0;
}
//##ModelId=43586653005F
int  CChennel::ReadData(char * buff,int length)
{
  int i;
  timeval mSocketTime;
  unsigned long templen=0;
  if( mHandle==INVALID_SOCKET ) return -1;
	  fd_set rdfds;
	  FD_ZERO(&rdfds);
	  FD_SET(mHandle, &rdfds);
	  mSocketTime.tv_sec = 0L;
	  mSocketTime.tv_usec = 5000L;
	  i = select( mHandle+1, &rdfds, NULL, NULL,&mSocketTime);
	  if( i<0 ) {
	  	  CloseChennel();
		   return -1;
	  }
	  if( i!=1 ) return 0;
	  i=ioctlsocket (mHandle,FIONREAD,&templen);
	  if(i==-1){
		 CloseChennel();
		 return -1;
	  }
	  if( templen == 0 ) return 0;
	  if((length<0)||(buff==NULL)) return 0;
	  if( (long)templen<length ) length=templen;
	  RESET_TIME_INTERVAL(mTP);
	  i = recv (mHandle,buff,length,0);
	  if( i< 0 ){
		if( GETSOCKETERROR() == EWOULDBLOCK ) return 0;
		CloseChennel();
		return -1;
	  } 
	  return i;
}
//##ModelId=435866530062
int  CChennel::WriteData(const char * buff,int length)
{
    if( mHandle==INVALID_SOCKET) return -1;
#ifdef SOCKET_TRACE
		BTRACE2("send [%d] use %d.",length,sock);
#endif
		fd_set rdfds;
		FD_ZERO(&rdfds);
       	FD_SET(mHandle, &rdfds);
		timeval mSocketTime;
       	mSocketTime.tv_sec = 1L;
       	mSocketTime.tv_usec = 1000L;
       	int i = select( mHandle+1,NULL, &rdfds,NULL,&mSocketTime);
       	if( i != 1) {
          //  CloseChennel();
	       	return -1;
        }
		if( send (mHandle,buff,length,0) == SOCKET_ERROR ){
			if( GETSOCKETERROR() == EWOULDBLOCK ) return 0;
			//CloseChennel();
			return -1;
		}
		RESET_TIME_INTERVAL(mTP);
		return 1;
}
