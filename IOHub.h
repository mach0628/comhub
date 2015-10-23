#ifndef __IOHUB_H__
#define __IOHUB_H__

#include "SPtrArray.h"
#include "Serial.h"
class HubUser;
class CommArray: public CSPtrArray
{
protected:
	virtual void DeletePtr(void ** ptr,int nLength);
public:
	CommArray();
	virtual ~CommArray();
public:
	CSerial * GetAt(int nIndex) const;
	CSerial * operator[](int nIndex) const;
	int Add(CSerial * comm);
	void Remove(CSerial * comm);
	void Remove(int index);
	int IndexOf(const char * name);
};

inline CSerial * CommArray::GetAt(int nIndex) const
{ return (CSerial * )CSPtrArray::GetAt(nIndex); }
inline CSerial * CommArray::operator[](int nIndex) const
{ return (CSerial *)CSPtrArray::GetAt(nIndex); }

class HubUserList;

class IOHub
{
protected:
	HubUserList * mUser;
	CommArray mIo;
	char data[1024];
public:
	IOHub();
	~IOHub();
	void Build(HubUserList* u);
public:
	CSerial * Get(int index) const { return mIo.GetAt(index); };
	CSerial * operator[](int index) const{ return mIo.GetAt(index); };

	int Add(const char * name );
	int Length() { return mIo.GetSize(); }
	void Remove(int index);
	void RemoveAll();
	int Run();
	// -1 参数错误， -2 com不存在，-3 已经被占用
	int Regist(HubUser * p,const char * name);
	void unRegist(const char * name);
};



#endif


