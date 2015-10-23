/////////////////////////////////////////////////////////
//SocketUser
//
#include "stdafx.h"
#include "SocketUser.h"
#include "Chennel.h"

SocketUser::SocketUser(CChennel * cs,HubCmd * cm):HubUser(cm)
{
	mSocket = cs;
	if( mSocket ) sprintf(mName,"%s-%d",mSocket->mAddr,mSocket->mPort);
}

SocketUser::~SocketUser()
{
}


void SocketUser::Close()
{
	unLock();
	if( mSocket ) {
		mSocket->CloseChennel();
		delete mSocket;
		HubUser::Close();
	}
	mSocket = NULL;
}

char * SocketUser::ReadData()
{
	if( mSocket==NULL ) return 0;
	int len = mSocket->CanRead();
	if( len<0 ){
		Trace::traceEvent(TRACE_DEBUG,"client %s read error",mName);
		Close();
		return NULL;
	}
	if( len>0 ){
		char * p = GetWPtr(len);
		mSocket->ReadData(p,len);		
		return p;
	} 
	return NULL;
}

void SocketUser::Show(CSerial * src,const char *msg)
{
	if( mSocket ) mSocket->WriteData(msg,(int)strlen(msg));
}
static char clscode[] = {0x1B, 0x5B, 0x48, 0x1B, 0x5B, 0x4A};
void SocketUser::Cls()
{
	mSocket->WriteData(clscode,sizeof(clscode));
}

