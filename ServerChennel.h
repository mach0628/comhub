// ServerChennel.h: interface for the CServerChennel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SERVERCHENNEL_H__)
#define __SERVERCHENNEL_H__

class CChennel;
class CServerChennel  
{
protected:
	SOCKET mHandle;
public:
	int mPort;
    struct sockaddr_in	svr_addr;
	
	CServerChennel(int nPort=8080);
	virtual ~CServerChennel();
	void Attach(int nPort){ mPort = nPort; };
	int CloseChennel();
	int OpenChennel();
	int CanRead();
	int IsClosed(){ return mHandle==-1? 1:0; };
	CChennel * ReadSocket();
};

#endif 

