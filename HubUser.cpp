#include "stdafx.h"
#include "HubUser.h"
#include "Serial.h"
#include "nvt.h"
#include "HubCmd.h"

////////////////////////////////////////////////////////////
//HubUser
//

HubUser::HubUser(HubCmd * cm)
{
	mWritePtr = 0;
	memset(mName,0,sizeof(mName));
	mExecuter = cm;
	mLocker = NULL;
	mModel = HubUser::MOD_QUICKW|HubUser::MOD_CR;
	mTime1 = 0;
	mTime2 = 0;
	RESET_TIME_INTERVAL(mLastTime);
}
HubUser::~HubUser()
{
}
#define MAX_MSG_LEN 1024*3
void HubUser::ShowV(CSerial * src,const char * fmt,...)
{
	char tmp[MAX_MSG_LEN];
	va_list argList;
	va_start(argList, fmt);
	vsnprintf(tmp,MAX_MSG_LEN,fmt,argList);
	va_end(argList);
	Show(src,tmp);
}
char * HubUser::GetWPtr(int length)
{
	if( (mWritePtr+length)>=(int)sizeof(mData) ) return NULL;
	char * p = mData+mWritePtr;
	mWritePtr += length;
	mData[mWritePtr]=0;
	return p;
}

void HubUser::Lock(CSerial * p)
{
		mLocker = p;
		p->SetUser(mName);
}
void HubUser::unLock()
{
	if( mLocker==NULL ) return;
	mLocker->SetUser(NULL);
	mLocker = NULL;
}
long HubUser::GetTimeSnap()
{
	STAMP_TIME_INTERVAL(mLastTime);
	return GET_TIME_INTERVAL(mLastTime);
}
void HubUser::SetTimeSnap()
{
	RESET_TIME_INTERVAL(mLastTime);
}
void  HubUser::DumpHead(char * buffer)
{
	sprintf(buffer," Index \tName                    \tLocker          \t Model  \t Time1 \tTime2\r\n");
}
void HubUser::Dump(int index,char * buffer)
{
	sprintf(buffer," %d \t%-24s  \t%-16s \t%-8s\t%-7d%-7d\r\n",index,mName,isLocked()?mLocker->Name():"(null)",
			GetModelChar(),mTime1,mTime2);
}
int HubUser::Run()
{
	char * p = ReadData();
	if( !p ) return 0;
	if( ((BYTE)p[0])>=NVT_BASE ){
            //
			switch(p[0]){
				case NVT_EOF:
				case NVT_BRK:
				case NVT_ABORT:
					if( isLocked() ){
						ShowV(NULL,"out of io %s",mLocker->Name());
						unLock();
					}
					break;
			}
			ResetPtr();
            return 1;
     }
	mData[mWritePtr]=0;
	if( mLocker && IsModel(HubUser::MOD_QUICKW) ){
		if( mData[0]=='$' && (mData[1]==CR||mData[1]==LF) ){
			ShowV(NULL,"out of io %s",mLocker->Name());
			Show(NULL,Prop());
			unLock();
			ResetPtr();
			return 1;
		}
		if( mData[mWritePtr-1]==CR || mData[mWritePtr-1]==LF ){
			mWritePtr--;
			if( mData[mWritePtr-1]==CR || mData[mWritePtr-1]==LF )mWritePtr--;
			if( mModel& MOD_CR ) mData[mWritePtr++]=CR; 
			if( mModel& MOD_LF ) mData[mWritePtr++]=LF; 
			mData[mWritePtr]=0;
		}
		mLocker->Write(p);
		ResetPtr();
		return 1;
	}
	if( strstr(p,"\r")||strstr(p,"\n") ){
		//execute command
		if( mExecuter ) {
			mExecuter->Execute(GetRPtr(),this);
			Show(NULL,Prop());
		}
		ResetPtr();
	}
	return 1;
}
void HubUser::SetTimeOut(int time1,int time2)
{
	if( time1>=0 ) mTime1 = time1;
	if( time2>=0 ) mTime2 = time2;
}
const char * HubUser::Prop()
{
	return "\r\n>";
}
static char ModelChar[6];
char * HubUser::GetModelChar()
{
   if( mModel & MOD_SILENT ) ModelChar[0]= 'S';
   else ModelChar[0]='*';
   if( mModel & MOD_QUICKW ) ModelChar[1]= 'Q';
   else ModelChar[1]='*';
   if( mModel & MOD_CR ) ModelChar[2]= 'R';
   else ModelChar[2]='*';
   if( mModel & MOD_LF ) ModelChar[3]= 'F';
   else ModelChar[3]='*';
	ModelChar[4]=0;
	return ModelChar;
}
static char * welcome =
"******************************************************************************\r\n\
* Copyright (c) 198-2015 atdmliao. Co., Ltd. All rights reserved.            *\r\n\
* IO Hub  is present,   version 2.1.0.3                                      *\r\n\
******************************************************************************\r\n"; 

void HubUser::onConnected()
{
	Trace::traceEvent(TRACE_NORMAL,"client %s up",mName);
	Show(NULL,welcome);
	Show(NULL,Prop());
}
void HubUser::Close()
{
	Trace::traceEvent(TRACE_NORMAL,"client %s down",mName);
}
BOOL HubUser::canShow(CSerial * src)
{
	if( src==NULL ) return TRUE;
	if( !IsModel(MOD_SILENT) ) return TRUE;
	if( src==mLocker ) return TRUE;
	return FALSE;
}
//////////////////////////////////////////////////
//HubUserList
//
HubUserList::HubUserList()
{
}
HubUserList::~HubUserList()
{
}

int HubUserList::Add(HubUser *usr)
{
	return mData.SetAt(mData.GetSize(),usr);
}
void HubUserList::RemoveAll()
{
	HubUser * p;
	for(int i=0;i<mData.GetSize();i++){
		p = GetAt(i);
		if( p!=NULL ){
			p->Close();
			delete p;
		}
	}
	mData.RemoveAll();
}
void HubUserList::Remove(HubUser * usr)
{
	for(int i=0;i<mData.GetSize();i++){
		if( GetAt(i)==usr ) {
			mData.RemoveAt(i);
			delete usr;
			return;
		}
	}
}
void HubUserList::Remove(int index)
{
	if( index>=0 && index< mData.GetSize() ){
		HubUser * p =  GetAt(index);
		mData.RemoveAt(index);
		delete p;
	}
}
int HubUserList::IndexOf(const char * p)
{
	for(int i=0;i<mData.GetSize();i++){
		if( mData.GetAt(i) && strcmp(p,GetAt(i)->Name())==0 ) return i;
	}
	return -1;
}

void HubUserList::IoDown(CSerial * p)
{
	const char * tmp = p->User();
	if( tmp[0]!=0 ){
		int k = IndexOf(tmp);
		if( k>=0 ){
			GetAt(k)->unLock();
		}
		char d[128];
		sprintf(d,"%s is down",p->Name());
		ShowMsg(p,d);
	}
}

void HubUserList::ShowMsg(CSerial * src,const char * msg)
{
	for(int i=0;i<mData.GetSize();i++){
		HubUser * p = GetAt(i);
		if( p ){
			if( p->canShow(src) )  p->Show(src,msg);
		}
	}
}

int HubUserList::Run()
{
	int k=0;
	for(int i=0;i<mData.GetSize();i++){
		HubUser * p = GetAt(i);
		if( p ){
			k+=p->Run();
		}
	}
	return k;
}

