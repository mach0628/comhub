#pragma once
#include "HubUser.h"
#include "IOHub.h"
#include "HubCmd.h"
class CServerChennel;
class ScriptUser;
class LogUser;
class HubCenter
{
	int mLogIndex;
public:
	int RunSignal;
	HubCmd mExecute;
	HubUserList mUsers;
	IOHub mIO;
	CServerChennel * mControl;
public:
	HubCenter(void);
	~HubCenter(void);
	ScriptUser * GetScriptor();
	LogUser * GetLogger();
	void Build();
	void Run();
	void Cleanup();
	void Quit();
};
