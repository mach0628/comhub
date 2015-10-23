#ifndef __SCRIPT_USER_H__
#define __SCRIPT_USER_H__
#include "HubUser.h"
#include "SPtrArray.h"


class ScriptUser:public HubUser
{

	int mStatus;
	CSPtrArray mCmds;
	const char * mFileName;
	char mWaitKey[64];
	char mRemain[128];
	int mStep;
	DECLARE_TIME_INTERVAL(mTimeWait);
public:
	ScriptUser(const char * file,HubCmd * cm);
	virtual ~ScriptUser();
public:
	virtual BOOL canShow(CSerial * src);
	virtual void Close();
	virtual void onConnected();
	virtual void Cls(){};

	void Start();
	void Stop();
	void RmCmd();
	void AddCmd(char * cm);
	virtual void Dump(int index,char * buffer);
	int GetSize(){ return mCmds.GetSize(); }
	const char * GetAt(int index){ return (const char *)mCmds.GetAt(index); }
	const char * operator[](int index){ return (const char *)mCmds.GetAt(index); }
protected:
	virtual void Show(CSerial * src,const char * msg);
	virtual char * ReadData();

	BOOL LoadCommand(const char * file);
	void FreeCommand();

};

#endif

