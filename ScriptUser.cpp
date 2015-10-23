#include "stdafx.h"
#include "ScriptUser.h"
#include "Utils.h"
#include "HubCmd.h"

#define CMD_EXEC 1
#define CMD_WAIT 2
#define CMD_WAITIO 3
#define CMD_WAITIO1 4

ScriptUser::ScriptUser(const char * file,HubCmd * cm):HubUser(cm)
{
	strcpy(mName,"ScriptUser");
	mStatus =0;
	mFileName = file;
	mModel = MOD_SILENT|MOD_QUICKW;
	mRemain[0] =0;
}

ScriptUser::~ScriptUser()
{
	if( mCmds.GetSize()>0 ){
		Trace::traceEvent(TRACE_NORMAL,"buffer not explicit free");
		FreeCommand();
	}
}
BOOL ScriptUser::canShow(CSerial * src)
{
	if( src==NULL ) return FALSE;
	if( src==mLocker ) return TRUE;
	return FALSE;
}
BOOL ScriptUser::LoadCommand(const char * file)
{
	FILE * in;
	char data[512];
	int len;
	in = fopen(file,"r+t");
	if( in == NULL ){
		printf("command file not exist!\n");
		return -1;
	}
   while(feof(in)==0){
	  memset(data,0,sizeof(data));
	  if(fgets(data,511,in)==NULL) break;
	  len = (int)strlen(data);
	  if( data[len-1]=='\n' || data[len-1]=='\r' ) {
		 len--;
		 if( data[len-1]=='\n' || data[len-1]=='\r' )len--;
	  }
	  AddCmd(data);	 
   }
	fclose(in);
	return TRUE;
}
void ScriptUser::onConnected()
{
	HubUser::onConnected();
	LoadCommand(mFileName);
	mStatus = CMD_EXEC;
	mStep = 0;
}
void ScriptUser::FreeCommand()
{
	for(int i=0;i<mCmds.GetSize();i++){
		delete (char *)mCmds.GetAt(i);
	}
	mCmds.RemoveAll();
}
void ScriptUser::Close()
{
	unLock(); 
	FreeCommand();
	mStatus = 0;
	mStep = 0;
	HubUser::Close();
}
void ScriptUser::AddCmd(char *cm)
{
	  char * tmp = Utils::Trim(cm);
	  if( tmp[0]==0 ) return;//empty line
	  if( tmp[0]=='#' ) return;//comment
	  char * tmp2 = new char[strlen(tmp)+1];
	  strcpy(tmp2,tmp);
	  mCmds.SetAt(mCmds.GetSize(),tmp2);
}
void ScriptUser::Start()
{
	mStatus = CMD_EXEC;
	mStep = 0;
}
void ScriptUser::Stop()
{
	mStatus = 0;
}
void ScriptUser::RmCmd()
{
	mStatus = 0;
	mStep = 0;
	FreeCommand();
}
void ScriptUser::Dump(int index,char * buffer)
{
	HubUser::Dump(index,buffer);
	int k = (int)strlen(buffer);
	sprintf(buffer+k,"Other:  status %d ,step %d-%d , wait key '%s' wait time %ld\r\n",
		mStatus,mStep,mCmds.GetSize(),mWaitKey, mStatus==CMD_WAIT?GetTimeSnap():0);
}

void ScriptUser::Show(CSerial * src,const char * msg)
{
	printf("%s",msg);
	if( mStatus==CMD_WAIT ){
		int l1 = (int)strlen(mWaitKey);
		int l2 = (int)strlen(msg);
		int l3 = (int)strlen(mRemain);

		if( (l2+l3)<(int)sizeof(mRemain) ){
			strcpy(mRemain+l3,msg);
			if(strstr(mRemain,mWaitKey)){
				mRemain[0]=0;
				mStatus = CMD_EXEC;
				return;
			}
			if( (l3+l2)>l1 ){
				memmove(mRemain,mRemain+l2+l3-l1,l1);
				mRemain[l1]=0;
			}
		} else {
			if(strstr(msg,mWaitKey)){
				mRemain[0]=0;
				mStatus = CMD_EXEC;
				return;
			}
			const char * tp = msg;
			int i=l3;
			while(i<l1*2){
				mRemain[i]=tp[0];
				tp++;i++;
				if( tp[0]==0 ) break;
			}
			mRemain[i]=0;
			if(strstr(mRemain,mWaitKey)){
				mRemain[0]=0;
				mStatus = CMD_EXEC;
				return;
			}
			memmove(mRemain,msg+l2-l1,l1);
			mRemain[l1]=0;
		}
	} else if( mStatus==CMD_WAITIO ){
		if( (msg[0]!=0)&&(msg[0]!=-1) ){
			SetTimeSnap();
			mStatus=CMD_WAITIO1;
		}
	} else if( mStatus==CMD_WAITIO1 ){
		SetTimeSnap();
	}
}

char *  ScriptUser::ReadData()
{
	char cm[256];
	 long t;
	switch( mStatus){
	case CMD_EXEC:
		if( mStep<mCmds.GetSize() ){
			char * tp = (char * )mCmds.GetAt(mStep);
			mStep++;
			int k = Utils::GetWord(tp,cm,256);
			if( strcmpi(cm,"wait")==0 ){
				if( Utils::GetCString(tp+k+1,mWaitKey,sizeof(mWaitKey))>0 ){
					SetTimeSnap();
					mRemain[0]=0;
					mStatus = CMD_WAIT;
				}
				return NULL;
			} else if( strcmpi(cm,"waitio")==0 ){
				if( mTime2 <100 ) mTime2 = 100;//Fix Time2 , set to mini 100
				SetTimeSnap();
				mStatus = CMD_WAITIO;
			} else {
				strcpy(mData,tp);
				mWritePtr = (int)strlen(tp);
				return mData;
			}
		}
	break;
	case  CMD_WAIT:
	case  CMD_WAITIO:
		t = GetTimeSnap();
		if( t>mTime1 ){
			printf("\ntime out,command stop!\n");
			mStatus =0;
			mStep = 0;
		}
		break;
	case CMD_WAITIO1:
		t = GetTimeSnap();
		if( t>mTime2 ) mStatus =CMD_EXEC;		
		break;
	}
	return NULL;
}


