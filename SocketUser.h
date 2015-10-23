#ifndef __SOCKET_USER_H__
#define __SOCKET_USER_H__

#include "HubUser.h"

class CChennel;

class SocketUser:public HubUser
{
	CChennel * mSocket;
public:
	SocketUser(CChennel * cs,HubCmd * cm);
	virtual ~SocketUser();
public:
	virtual void Close();
	virtual BOOL isClosed(){ return mSocket==NULL; };
	virtual void Cls();
protected:
	virtual void Show(CSerial * src,const char * msg);
	virtual char * ReadData();
};

#endif


