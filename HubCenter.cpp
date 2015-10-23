#include "stdafx.h"
#include "HubCenter.h"
#include "Prefs.h"
#include "ServerChennel.h"
#include "ScriptUser.h"
#include "KeyboardUser.h"
#include "SocketUser.h"
#include "LogUser.h"

HubCenter::HubCenter(void)
{
	RunSignal =0;
	mControl = NULL;
	mLogIndex =0;
}

HubCenter::~HubCenter(void)
{
}
ScriptUser * HubCenter::GetScriptor()
{
	return (ScriptUser *)mUsers[0];
}
LogUser * HubCenter::GetLogger()
{
	if( mLogIndex>0 ) return (LogUser *)mUsers[mLogIndex];
	return NULL;
}
void HubCenter::Build()
{
	mExecute.SetCenter(this);
	mIO.Build(&mUsers);

	if( gPrefs->mListenPort>0 ) {
		Trace::traceEvent(TRACE_NORMAL,"controll port set to %d",gPrefs->mListenPort);
		mControl = new CServerChennel(gPrefs->mListenPort);
		if( mControl->OpenChennel()<0 ){
			Trace::traceEvent(TRACE_WARNING,"controll setup failure");
			exit(1);
		}
	} else Trace::traceEvent(TRACE_NORMAL,"controll is disabled");

	mUsers.Add(new ScriptUser(gPrefs->ScriptFile(),&mExecute));

	if( gPrefs->mKbEnable ){
		 Trace::traceEvent(TRACE_NORMAL,"keyboard is enable...");
		mUsers.Add(new KeyboardUser(&mExecute));
		mUsers[1]->onConnected();
	} else{
		Trace::traceEvent(TRACE_NORMAL,"keyboard is disabled");
		mUsers.Add(new LogUser(this));
		mUsers[1]->SetTimeOut(gPrefs->mKaTime1,gPrefs->mKaTime2);
		mUsers[1]->onConnected();
	}

}
void HubCenter::Quit()
{
	if( mControl ) mControl->CloseChennel();
	RunSignal = 0;
}
void HubCenter::Run()
{
	RunSignal = 1;
	Trace::traceEvent(TRACE_NORMAL,"service run.....");
	while(RunSignal){
		if( (mIO.Run()+mUsers.Run())<1 ){
			for( int i=0;i<mUsers.Length();i++){
				if( mUsers[i]->isClosed() ){
					mUsers.Remove(i);
					break;
				}
			}
			SLEEP(50);
		}
		if( mControl ){
			CChennel * p = mControl->ReadSocket();
			if( p ){
				SocketUser * u = new SocketUser(p,&mExecute);
				mUsers.Add( u);
				u->onConnected();
			}
		}
	}
	Trace::traceEvent(TRACE_NORMAL,"service down completed.");	
}
void HubCenter::Cleanup()
{
	mUsers.RemoveAll();
	mIO.RemoveAll();
	if( mControl ) delete mControl;
	mControl = NULL;
}



