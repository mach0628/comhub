#include "stdafx.h"
#include "IOHub.h"
#include "Prefs.h"
#include "HubUser.h"

////////////////////////////////////////////////////
//CommArray
//
CommArray::CommArray()
{
}
CommArray::~CommArray()
{
}
void CommArray::DeletePtr(void **ptr, int nLength)
{
	for(int i=0;i<nLength;i++){
		CSerial * p = ((CSerial *)ptr[i]);
		if( p!=NULL ){
			p->Close();
			delete p;
		}
	}
}
int CommArray::Add(CSerial * comm)
{
	return SetAt(m_nSize,comm);
}

void CommArray::Remove(CSerial * comm)
{
	for(int i=0;i<GetSize();i++){
		if( GetAt(i)==comm ) {
			SetAt(i,NULL);
			comm->Close();
			delete comm;
			return;
		}
	}
}
void CommArray::Remove(int index)
{
	if( index< m_nSize ){
		CSerial * comm = GetAt(index);
		if( comm!=NULL ){
			comm->Close();
			delete comm;
			SetAt(index,NULL);
		}
	}
}
int CommArray::IndexOf(const char * name)
{
	for(int i=0;i<GetSize();i++){
		if( GetAt(i)!=NULL && strcmpi(GetAt(i)->Name(),name)==0 ) return i;	
	}
	return -1;
}
////////////////////////////////////////////////////////////////////////
// IOHub
//
IOHub::IOHub()
{
	mUser = NULL;
}

IOHub::~IOHub()
{
}

int IOHub::Add(const char * name)
{
	char tmp[24];
	CSerial * p = new CSerial();

	if( p->Open(name) ){
		p->Setup(gPrefs->mBaudRate,gPrefs->mDataBtis,gPrefs->mParity,gPrefs->mStopBits);
		int k = mIo.Add(p);
		sprintf(tmp,"com%d",k);
		p->SetName(tmp);
		Trace::traceEvent(TRACE_NORMAL,"add serial %s to %d, name is %s",name,k,tmp);
		return 1;
	}
	Trace::traceEvent(TRACE_NORMAL,"add serial %s failure",name);
	delete p;
	return -1;
}
void IOHub::Remove(int index)
{
	Trace::traceEvent(TRACE_NORMAL,"remove serial ,index = %d",index);
	mIo.Remove(index);
}
void IOHub::RemoveAll()
{
	mIo.RemoveAll();
}
void IOHub::Build(HubUserList* u)
{
	mUser = u;
	Trace::traceEvent(TRACE_NORMAL,"setup serial....");
	for(int i=0;i<Prefs::MAX_COMM;i++){
	  if( gPrefs->mComms[i]!=NULL ){
		  Add(gPrefs->mComms[i]);
	  } else break;
	}
}
int IOHub::Run()
{
	int dwRead;
	int ok = 0;
	for(int i=0;i<mIo.GetSize();i++){
		if( mIo[i]!=NULL ) {
			if( mIo[i]->Read(data,1024,&dwRead) ){
				if( dwRead>0 ){
					data[dwRead]=0;
					if( mUser ) mUser->ShowMsg(mIo[i],data);
					ok ++;
				}
			}else{
				if( mUser ) mUser->IoDown(mIo[i]);
				mIo[i]->SetUser(NULL);
				mIo[i]->Close();
			}
		} else break;
	}
	return ok;
}
// -1 参数错误， -2 com不存在，-3 已经被占用
int IOHub::Regist(HubUser * p,const char * name)
{
	if( p==NULL ) return -1;
	if( name==NULL) return -1;
	if( strlen(name)<2) return -1;
	int k = mIo.IndexOf(name);
	if(k<0 ) return -2;
	if( mIo[k]->SetUser(p->Name()) ){
		p->Lock(mIo[k]);
		return k;
	}
	return -3;
}
void IOHub::unRegist(const char * name)
{
	int k = mIo.IndexOf(name);
	if(k>=0){
		mIo[k]->SetUser(NULL);
	}
}



