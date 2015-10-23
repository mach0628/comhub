#ifndef __HUBCMD_H__
#define __HUBCMD_H__
#include "SLexer.h"

class HubUser;
class HubCenter;
class HubCmd
{
	HubUser * mCurrent;
	HubCenter * mCenter;
	char msg[1024];

public:
	HubCmd();
	~HubCmd();
	int Execute(const char * cmd,HubUser* user);
	void SetCenter(HubCenter *p){ mCenter = p; };
protected:
	int Error(int err,int pos,const char * fmt,...);
	int Help();
	int ListCommand(SLexer & lexer,int type);
	int SetCommand(SLexer & lexer,int type);
	int SelectCommand(SLexer & lexer,int type);
	int WriteCommand(SLexer & lexer,int type);
	int ScriptCommand(SLexer & lexer,int type);

	int UnLock(SLexer & lexer);
	int SetTimeOut(SLexer & lexer);
	void ListObject(int c,int x,int y);
};


#endif


