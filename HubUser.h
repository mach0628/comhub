#ifndef __HUBUSER_H__
#define __HUBUSER_H__
#include "SPtrArray.h"
#include "TimeInterval.h"

class CSerial;
class HubCmd;
class HubUser
{
protected:
	char mName[32];
	char mData[512];
	int mWritePtr;
	HubCmd * mExecuter;
	int mModel;
	int mTime1;
	int mTime2;
	DECLARE_TIME_INTERVAL(mLastTime);
public:
	CSerial * mLocker;
public:
	HubUser(HubCmd * cm);
	virtual ~HubUser();
public:
	static const int MOD_SILENT = 0x0001;
	static const int MOD_QUICKW = 0x0002;
	static const int MOD_CR  = 0x0004;
	static const int MOD_LF  = 0x0008;
	const char * Name() const { return mName;};
	const char * GetRPtr(){ return mData; };
	BOOL isLocked(){ return mLocker!=NULL; };
	virtual BOOL isClosed(){ return FALSE; };

	int Run();
	virtual void onConnected();
	virtual void Close();

	virtual BOOL canShow(CSerial * src);
	virtual void Show(CSerial * src,const char * msg)=0;
	virtual void Cls()=0;
	void ShowV(CSerial * src,const char * fmt,...);
	static void  DumpHead(char * buffer);
	virtual void Dump(int index,char * buffer);

	void Lock(CSerial * p);
	void unLock();
	int GetModel(){ return mModel; }
	char * GetModelChar();
	BOOL IsModel(int cb){ return mModel & cb; }
	void SetModel(int cb){ mModel |=cb; };
	void ResetModel(int cb) { mModel &= ~cb; }
	void SetTimeOut(int time1,int time2);
	long GetTimeSnap();
	void SetTimeSnap();
protected:
	virtual char * ReadData()=0;
	char * GetWPtr(int length);
	void ResetPtr(){ mWritePtr=0; };
	const char * Prop();
};
class HubUserList 
{
	CSPtrArray mData;
public:
	HubUserList();
	~HubUserList();
public:
	HubUser * GetAt(int nIndex) const;
	HubUser * operator[](int nIndex) const;
	int Length(){ return mData.GetSize(); }
	int Add(HubUser * usr);
	void Remove(HubUser * usr);
	void Remove(int index);
	void RemoveAll();
	void ShowMsg(CSerial * src,const char * msg);
	int IndexOf(const char * p);
	int Run();
public:
	void IoDown(CSerial * p);
	
};

inline HubUser * HubUserList::GetAt(int nIndex) const
{ return (HubUser * )mData.GetAt(nIndex); }
inline HubUser * HubUserList::operator[](int nIndex) const
{ return (HubUser *)mData.GetAt(nIndex); }


#endif

