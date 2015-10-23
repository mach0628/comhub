
#include "stdafx.h"
#include "LogUser.h"
#include "HubCenter.h"

LogUser::LogUser(HubCenter * center):HubUser(NULL)
{
	strcpy(mName,"LogUser");
	mCenter = center;
	mModel = 0;
	mTime1 = 2000;
	mTime2 = 30000;
	mStatus =0;
}

LogUser::~LogUser(void)
{
}
void LogUser::Dump(int index,char * buffer)
{
	HubUser::Dump(index,buffer);
	int k = (int)strlen(buffer);
	sprintf(buffer+k,"Other:  status %d ,current %d,wait time %ld\r\n",
		mStatus,mIndex,GetTimeSnap());
}

void LogUser::Show(CSerial * src,const char * msg)
{
	if( (mLocker!=NULL)&&(src==mLocker) ){
		if( mStatus!=0 ){
			switch(mStatus){
			case 2:
				if( (msg[0]!=0)&&(msg[0]!=-1) ){
					mStatus = 3;
					SetTimeSnap();
				}
				break;
			case 3:
				SetTimeSnap();
				break;
			} 
			return;
		}
	}
	Trace::traceEventRaw("%s",msg);
}
char * LogUser::ReadData()
{
	long t;
	switch( mStatus ){
	case 0:
		 t = GetTimeSnap();
		if( mTime1>0 && t>mTime1 ) {
			SetTimeSnap();
			mStatus =1;
			mIndex = 0;
		}
		break;
	case 1:
		for(int i=0;i<mCenter->mIO.Length();i++ ){
			if( mCenter->mIO[i]->GetTimeSnap()>60000 ){
				mLocker =  mCenter->mIO[i];
				mLocker->Write("\n",1);
				SetTimeSnap();
				mIndex=i;
				mStatus =2 ;
				break;
			}
		} 
		if( mStatus==1 ) mStatus = 0;
		break;
	case 2://wait io out start
		break;
	case 3://wait io out stop
		t = GetTimeSnap();
		if( t>mTime2 ) {
			mStatus =1;	
			mLocker = NULL;
		}
		break;
	}
	return NULL; 
}

