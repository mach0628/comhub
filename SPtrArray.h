// Copyright (C) 1991 - 1999 Rational Software Corporation

#ifndef _INC_CSPTRARRAY_4049F5AF0162_INCLUDED
#define _INC_CSPTRARRAY_4049F5AF0162_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//##ModelId=4049F5AF0162
class CSPtrArray 
{
public:
	void InsertAt(int nIndex, void * newElement, int nCount /*=1*/);
	//##ModelId=4060357101C5
	int SetAt(int nIndex,void * newElement);
	//##ModelId=404C958903D9
	void RemoveAll();

	//##ModelId=4049F71402AF
	CSPtrArray();

	//##ModelId=4049F738001C
	virtual ~CSPtrArray();

	//##ModelId=4049F7D20349
	void* GetAt(int nIndex) const;
	int GetSize() { return m_nSize;};
	//##ModelId=404C95C1002C
	void RemoveAt(int nIndex,int nCount=1);
protected:

	//##ModelId=4049F8490372
	virtual void DeletePtr(void ** ptr,int nLength);

	//##ModelId=4049F67902F3
	void** m_pData;

	//##ModelId=4049F6DA00B7
	int m_nSize;

	//##ModelId=4049F6ED0014
	int m_nMaxSize;

	//##ModelId=4049F6FF0291
	int m_nGrowBy;

	//##ModelId=4049F7440286
	void SetSize(int nNewSize, int nGrowBy = -1);

	//##ModelId=4049F7A3032D
	void*& ElementAt(int nIndex);


};
class CSPtrStack : public CSPtrArray  
{
public:
	//##ModelId=4048ACE603B4
	void * Push(void* newElement);

	//##ModelId=4048ACFF0071
	void* Pop();

	//##ModelId=4048AD230113
	void* Head();

	//##ModelId=4048AD30039C
	void* Tail();

	//##ModelId=4049DD330245
	void* operator[](int nIndex) const;

	//##ModelId=4049DD3302D1
	void* GetAt(int nIndex) const;
public:
	CSPtrStack();
	//##ModelId=4060357100F4
	virtual ~CSPtrStack();
protected:
	int sp;
};
#endif /* _INC_CSPTRARRAY_4049F5AF0162_INCLUDED */
