#include "stdafx.h"
#include "HubCmd.h"
#include "SLexer.h"
#include "HubCenter.h"
#include "ScriptUser.h"

#define ERR_OK    1
#define ERR_LEXER -1
#define ERR_SYNTAX -2
#define ERR_CMD   -3
#define ERR_EXEC   -4
#define GETONE( )  if( (type=lexer.Scan())<0 ) {  return Error(ERR_LEXER,lexer.pos(),"%s",lexer.error()); }

HubCmd::HubCmd()
{
	mCenter = NULL;
}

HubCmd::~HubCmd()
{
}

int HubCmd::Error(int err,int pos,const char * fmt,...)
{
	switch(err){
	case 0:
		msg[0]=0;
		return 0;
	case ERR_OK:
		sprintf(msg,"command ok: ");
		break;
	case ERR_LEXER:
	case ERR_SYNTAX:
		sprintf(msg,"bad syntax[%d]: ",pos);
		break;
	case ERR_EXEC:
		sprintf(msg,"failure[%d]: ",pos);
		break;
	default:
		sprintf(msg,"bad command[%d]: ",pos);
		break;
	}
	int len = (int)strlen(msg);
	va_list argList;
	va_start(argList, fmt);
	vsnprintf(msg+len,sizeof(msg)-1-len,fmt,argList);
	va_end(argList);
	mCurrent->Show(NULL,msg);
	return err;
}

int HubCmd::Execute(const char * cmd, HubUser* user)
{
	mCurrent = user;
	SLexer lexer(cmd);
	int type;
	GETONE();
	if( type==SLexer::LX_NULL ) return 0;
	if( type=='?' ) return Help();
	if( type!=SLexer::LX_ID ) return Error(ERR_SYNTAX,lexer.pos(),"expect command name");
	if( strcmpi("quit",lexer.txt())==0 || strcmpi("exit",lexer.txt())==0|| strcmpi("bye",lexer.txt())==0 ){
		Error(ERR_OK,0,"session will close");
		if( user ) user->Close();
		return ERR_OK;
	}else if( strcmpi("sysdown",lexer.txt())==0 ){
		Error(ERR_OK,0,"program will down");
		mCenter->Quit();
		return  ERR_OK;
	}else if( strcmpi("help",lexer.txt())==0 ){
		return Help();
	} else if( strcmpi("cls",lexer.txt())==0 ){
		mCurrent->Cls();
		return  ERR_OK;
	}
	int k =0;
	if( strcmpi("list",lexer.txt())==0) k=1;
	else if( strcmpi("dir",lexer.txt())==0 ) k = 1;
	else if( strcmpi("ls",lexer.txt())==0 ) k = 1;
	else if( strcmpi("set",lexer.txt())==0 ) k = 2;
	else if( strcmpi("select",lexer.txt())==0) k=3;
	else if( strcmpi("cat",lexer.txt())==0 ) k=3;
	else if( strcmpi("out",lexer.txt())==0) k=4;
	else if( strcmpi("write",lexer.txt())==0) k=4;
	else if( strcmpi("sh",lexer.txt())==0) k=5;
	if( k==0 ) return Error(ERR_CMD,lexer.pos(),"unrecognized command '%s'",lexer.txt() );
	GETONE();
	if( type==SLexer::LX_NULL ){
		if( k==1 ) return (ListObject(2,0,-1),0);
		return Error(ERR_SYNTAX,lexer.pos(),"unexpect end");
	}
	switch(k){
	case 1:
		return ListCommand(lexer,type);
	case 2: 
		return SetCommand(lexer,type);
	case 3:
		return SelectCommand(lexer,type);
	case 4:
		return WriteCommand(lexer,type);
	case 5:
		return ScriptCommand(lexer,type);
	}	
	return 0;
}
int HubCmd::Help()
{
	Error(ERR_OK,0,"\r\n");
	mCurrent->Show(NULL,
"quit/bye/exit                         exit current user\r\n\
list/ls/dir io/user/sh [start] [end]  list io or user from start to end\r\n\
set mode +/- character                set user mode , '+' indicate set character , '-' indicate clear character\r\n\
                                        character can be:\r\n\
                                         silent: only receive msg from catched serial\r\n\
                                         quick:  quick write mode, receive from user,and sent to catched serial directly\r\n\
                                              use \"$\" can  quit\r\
                                         cr:     cr at line end on quick mode\r\n\
                                         lf:     lf at line end on quick mode\r\n\
set unlock io/user index              clear lock object of io/user\r\n\
set timeout  number number            set user two timeout value,if number less zero,ingored\r\n\
select index/name                     set currnet io object , index is number in io list, name is string of io object\r\n\
cat index/name                        set currnet io object , index is number in io list, name is string of io object\r\n\
write/out string                      send string to current io\r\n\
write/out break                       send break to current io\r\n\
write/out return                      send CRLF to current io\r\n\
sh stop                               stop script running\r\n\
sh start                              start script\r\n\
sh clear                              clear script content\r\n\
sh add string                         add command to script\r\n\
sh list                               list command in script\r\n\
string:\r\n\
\\r  CR\r\n\
\\n  LF\r\n\
\\t  TAB\r\n\
\\c[           CTRL+]\r\n\
\\c/           CTRL+/\r\n\
\\c]           CTRL+]\r\n\
\\c^           CTRL+^\r\n\
\\c_           CTRL+ '_'\r\n\
\\cA -- \\cZ    CTRL+ A ---- CTRL+Z\r\n" );
	return ERR_OK;
}
void HubCmd::ListObject(int c,int x,int y)
{
	ScriptUser * sc = mCenter->GetScriptor();
	int k=0;
	switch(c){
	case 1:
		 k=mCenter->mIO.Length();
		break;
	case 2:
		k = mCenter->mUsers.Length();
		break;
	case 3:
		k = sc->GetSize();
		break;
	default:
		return ;
	}
	if( y<0 ) y = k;
	Error(ERR_OK,0,"total %d\r\n",k);
	if( c==1 ){
		CSerial::DumpHead(msg);
		mCurrent->Show(NULL,msg);
		for(int i=x;i<y;i++){
			CSerial * t = mCenter->mIO.Get(i);
			t->Dump(i,msg);
			mCurrent->Show(NULL,msg);
		}
	} else if( c==2) {
		HubUser::DumpHead(msg);
		mCurrent->Show(NULL,msg);
		for(int i=0;i<y;i++){
			HubUser * t = mCenter->mUsers.GetAt(i);
			if( t ) t->Dump(i,msg);				
			else sprintf(msg,"%d\r\n",i);
			mCurrent->Show(NULL,msg);
		}
	} else {
		for(int i=0;i<sc->GetSize();i++){
			sprintf(msg,"%d    %s\r\n",i,sc->GetAt(i));
			mCurrent->Show(NULL,msg);
		}
	}
}
int HubCmd::ListCommand(SLexer & lexer,int type)
{
	//list io/user [start] [end]
	if( type!=SLexer::LX_ID ) return Error(ERR_SYNTAX,lexer.pos(),"expect io/user/sh,but '%s'",lexer.txt());
	int c=0;
	if( strcmpi("io",lexer.txt() )==0 ) c=1;
	else if( strcmpi("user",lexer.txt())==0 ) c=2;
	else if( strcmpi("sh",lexer.txt())==0 ) c=3;

	if( c==0 ) return Error(ERR_CMD,lexer.pos(),"expect io/user/sh,but '%s'",lexer.txt());
	int x=0;int y=-1;
	GETONE();
	if( type!=0 ){
		if( type!=SLexer::LX_INT ) return Error(ERR_SYNTAX,lexer.pos(),"expect int here");
		x = lexer.mValueInt;
		GETONE();
		if( type!=0 ) {
			if( type!=SLexer::LX_INT ) return Error(ERR_SYNTAX,lexer.pos(),"expect int here");
			y = lexer.mValueInt;
		}
	}
	GETONE();
	if( type!=0 ) return Error(ERR_SYNTAX,lexer.pos(),"too more parameter");
	ListObject(c,x,y);
	return ERR_OK;
}
int HubCmd::UnLock(SLexer & lexer)
{
	int type;
	GETONE();
	if( type==SLexer::LX_NULL ) return Error(ERR_SYNTAX,lexer.pos(),"unexpect end");
	if( type!=SLexer::LX_ID ) return Error(ERR_SYNTAX,lexer.pos(),"expect io/user,but '%s'",lexer.txt());
	int x=0,y=0;
	if( strcmpi("io",lexer.txt())==0 ) x=1;
	 else if( strcmpi("user",lexer.txt())==0 ) x=2;
	if( x==0 )	return Error(ERR_CMD,lexer.pos(),"expect io/user,but '%s'",lexer.txt());

	GETONE();
	if( type==SLexer::LX_NULL ) return Error(ERR_SYNTAX,lexer.pos(),"unexpect end");
	if( type!=SLexer::LX_INT ) return Error(ERR_SYNTAX,lexer.pos(),"expect number,but '%s'",lexer.txt());
	y = lexer.mValueInt;
	if( x==1 ){
		if( y<0 || y>= mCenter->mIO.Length() ) return Error(ERR_EXEC,lexer.pos(),"index out of range");
		mCenter->mIO[y]->SetUser(NULL);
	} else{
		if( y<0 || y>= mCenter->mUsers.Length() ) return Error(ERR_EXEC,lexer.pos(),"index out of range");
		if( mCenter->mUsers[y] ) mCenter->mUsers[y]->unLock();
	}
	return Error(ERR_OK,lexer.pos(),"unlock ok");
}
int HubCmd::SetTimeOut(SLexer & lexer)
{
	int type;
	GETONE();
	if( type==SLexer::LX_NULL ) return Error(ERR_SYNTAX,lexer.pos(),"unexpect end");
	if( type!=SLexer::LX_INT ) return Error(ERR_SYNTAX,lexer.pos(),"expect number,but '%s'",lexer.txt());
	int x = lexer.mValueInt;

	GETONE();
	int y=-1;
	if( type!=SLexer::LX_NULL ){
		if( type!=SLexer::LX_INT ) return Error(ERR_SYNTAX,lexer.pos(),"expect number,but '%s'",lexer.txt());
		y = lexer.mValueInt;
	}
	mCurrent->SetTimeOut(x,y);
	return Error(ERR_OK,lexer.pos(),"ok");
}
int HubCmd::SetCommand(SLexer &lexer,int type)
{
	// set mode +/- character[silent|quick|CR|LF]
	// set unlock io/user index
	// set timeout tim1 time2
	if( type!=SLexer::LX_ID ) return Error(ERR_SYNTAX,lexer.pos(),"expect mode/unlock/timeout,but '%s'",lexer.txt());
	type =0;
	if( strcmpi("mode",lexer.txt())==0 ) type=1;
	else if( strcmpi("unlock",lexer.txt())==0 ) return UnLock(lexer);
	else if( strcmpi("timeout",lexer.txt())==0 ) return SetTimeOut(lexer);
	if( type==0 ) return Error(ERR_CMD,lexer.pos(),"expect mode/unlock/timeout,but '%s'",lexer.txt());
		GETONE();
		if( type==SLexer::LX_NULL ) return Error(ERR_SYNTAX,lexer.pos(),"unexpect end");
		int k =0 ;
		if( type=='+' )  k=1;
		if( type=='-' ) k=2;
		if(k==0 )  return Error(ERR_SYNTAX,lexer.pos(),"expect +/- here, but '%s'",lexer.txt());

		int cb=0;
		type =1;
		while( type!=SLexer::LX_NULL ){
			GETONE();
			if( type==SLexer::LX_NULL )  break;
			if( type!=SLexer::LX_ID ) return Error(ERR_SYNTAX,lexer.pos(),"bad character '%s'",lexer.txt());
			if( strcmpi("silent",lexer.txt())==0 ) cb |= HubUser::MOD_SILENT;
			 else if( strcmpi("quick",lexer.txt())==0 ) cb |= HubUser::MOD_QUICKW;
			   else if( strcmpi("cr",lexer.txt())==0 ) cb |= HubUser::MOD_CR;
			     else if( strcmpi("lf",lexer.txt())==0 ) cb |= HubUser::MOD_LF;
				 else return Error(ERR_CMD,lexer.pos(),"unknown character '%s'",lexer.txt()); 
		}
		if( cb==0  ) return Error(ERR_SYNTAX,lexer.pos(),"no character");
		if( k==1 ) mCurrent->SetModel(cb);
		else mCurrent->ResetModel(cb);
		return Error(ERR_OK,lexer.pos(),"ok");
}

int HubCmd::SelectCommand(SLexer &lexer,int type)
{
	//select index/name
	if( (type!=SLexer::LX_STRING )&&(type!=SLexer::LX_INT) )  return Error(ERR_CMD,lexer.pos(),"'%s' is not a io",lexer.txt());
	if( type==SLexer::LX_STRING) {
		for( int i=0;i<mCenter->mIO.Length();i++)
			if( strcmpi(lexer.txt(),mCenter->mIO[i]->Name())==0 ){
				if( mCenter->mIO[i]->User() ) return Error(ERR_EXEC,lexer.pos(),"IO is already locked");
				mCurrent->Lock(mCenter->mIO[i]);
				return Error(ERR_OK,lexer.pos(),"Locked");
			}
			return Error(ERR_EXEC,lexer.pos(),"can not find '%s'",lexer.txt());
	}
	type = lexer.mValueInt;
	if( type<0 || type>=mCenter->mIO.Length() ) return Error(ERR_EXEC,lexer.pos(),"IO index out of range");
	if( mCenter->mIO[type]->User() ) return Error(ERR_EXEC,lexer.pos(),"IO is already locked");
	mCurrent->Lock(mCenter->mIO[type]);
	return Error(ERR_OK,lexer.pos(),"Locked");
}

int HubCmd::WriteCommand(SLexer & lexer,int type)
{
	//write string
	if( !mCurrent->isLocked() ) return Error(ERR_EXEC,lexer.pos(),"no io");

	if( type==SLexer::LX_ID ){
		if( strcmpi("break",lexer.txt())==0 ) mCurrent->mLocker->Break();
		else if( strcmpi("return",lexer.txt())==0 ) mCurrent->mLocker->Write("\r\n",2);
		else return Error(ERR_CMD,lexer.pos(),"unknow content %s",lexer.txt());
		return Error(ERR_OK,lexer.pos(),"ok");
	}
	if( type!=SLexer::LX_STRING  )  return Error(ERR_SYNTAX,lexer.pos(),"expect string");
	if( mCurrent->mLocker->Write(lexer.txt()) ){
		//mCurrent->mLocker->Write("\r\r\n");
		return Error(ERR_OK,lexer.pos(),"ok");
	}
	return Error(ERR_EXEC,lexer.pos(),"write error");
}
int HubCmd::ScriptCommand(SLexer & lexer,int type)
{
	//script stop
	//script start
	//script clear
	//script add string
	//script list
	if( type==SLexer::LX_ID ){	
		ScriptUser * sc = mCenter->GetScriptor();
		if( strcmpi("start",lexer.txt())==0 ){
			sc->Start();
			return Error(ERR_OK,lexer.pos(),"script start");
		} else if( strcmpi("stop",lexer.txt())==0 ){
			sc->Stop();
			return Error(ERR_OK,lexer.pos(),"script stop");
		} else if( strcmpi("clear",lexer.txt())==0 ){
			sc->RmCmd();
			return Error(ERR_OK,lexer.pos(),"script command clear");
		} else if( strcmpi("add",lexer.txt())==0 ){
			GETONE();
			if( type==SLexer::LX_ID || type==SLexer::LX_STRING ) {
				if( strlen(lexer.txt())<3 ) return Error(ERR_CMD,lexer.pos(),"script command too short");
				sc->AddCmd((char *)lexer.txt());
				return Error(ERR_OK,lexer.pos(),"add script command ok");
			}
			return Error(ERR_CMD,lexer.pos(),"bad script command");
		} else if( strcmpi("list",lexer.txt())==0 )	return (ListObject(3,0,-1),ERR_OK);
		
	}
	return Error(ERR_SYNTAX,lexer.pos(),"expect stop/start/clear/add/list,but '%s'",lexer.txt());
}

