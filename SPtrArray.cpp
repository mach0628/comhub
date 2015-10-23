// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "SPtrArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//##ModelId=4049F71402AF
CSPtrArray::CSPtrArray()
{
	// TODO: Add your specialized code here.
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

//##ModelId=4049F738001C
CSPtrArray::~CSPtrArray()
{
	// TODO: Add your specialized code here.
	if (m_pData != NULL)
	{
		DeletePtr(m_pData, m_nSize);
		delete[] (BYTE*)m_pData;
	}
}

//##ModelId=4049F7D20349
void* CSPtrArray::GetAt(int nIndex) const
{
	// TODO: Add your specialized code here.
	ASSERT(nIndex<m_nSize);
	return m_pData[nIndex];
}
void CSPtrArray::InsertAt(int nIndex, void * newElement, int nCount /*=1*/)
{
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		DeletePtr(&m_pData[nOldSize], nCount);
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(void *));

		// re-init slots we copied from
		//memset(&m_pData[nIndex],0, nCount);
	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

//##ModelId=4049F7440286
void CSPtrArray::SetSize(int nNewSize, int nGrowBy)
{
	// TODO: Add your specialized code here.
	ASSERT(nNewSize >= 0);
	if (nGrowBy != -1)	m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			DeletePtr(m_pData, m_nSize);
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
//		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(void));    // no overflow
#endif
		m_pData = (void**) new BYTE[nNewSize * sizeof(void *)];
		memset(m_pData,0,nNewSize * sizeof(void *));
		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			memset(&m_pData[m_nSize],0, (nNewSize-m_nSize)*sizeof(void*));
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			DeletePtr(&m_pData[nNewSize],m_nSize-nNewSize);
			memset(&m_pData[nNewSize],0, (m_nSize-nNewSize)*sizeof(void*));
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
//		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		void** pNewData = (void **) new BYTE[nNewMax * sizeof(void*)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(void *));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);
		memset(&pNewData[m_nSize], 0,(nNewSize-m_nSize)*sizeof(void*));

		// get rid of old stuff (note: no destructors called)
		delete[] m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}

}

//##ModelId=4049F7A3032D
void*& CSPtrArray::ElementAt(int nIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	ASSERT(nIndex<m_nSize);
	return m_pData[nIndex];
}



//##ModelId=404C958903D9
void CSPtrArray::RemoveAll()
{
	// TODO: Add your specialized code here.
	SetSize(0);
}

//##ModelId=404C95C1002C
void CSPtrArray::RemoveAt(int nIndex,int nCount)
{
	// TODO: Add your specialized code here.
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);
	DeletePtr(&m_pData[nIndex], nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(void *));
	m_nSize -= nCount;

}


//##ModelId=4060357101C5
int CSPtrArray::SetAt(int nIndex,void * newElement)
{
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize) SetSize(nIndex+1);
	m_pData[nIndex] = newElement;

	return nIndex;
}


void CSPtrArray::DeletePtr(void **ptr,int nLength)
{
}


CSPtrStack::CSPtrStack():sp(-1)
{
   SetSize(10);
}

//##ModelId=4060357100F4
CSPtrStack::~CSPtrStack()
{

}

void* CSPtrStack::Push(void* newElement)
{
	SetAt(++sp,newElement);
	return m_pData[sp];
}

//##ModelId=4048ACFF0071
void* CSPtrStack::Pop()
{
	// TODO: Add your specialized code here.
	if( sp>=0 ){
		void * tmp =m_pData[sp+1];
		RemoveAt(sp--);
		return tmp;
	}
	return NULL;
}

//##ModelId=4048AD230113
void* CSPtrStack::Head()
{
	// TODO: Add your specialized code here.
	if( sp<0 )	return 0;
	return m_pData[sp];
}

//##ModelId=4048AD30039C
void* CSPtrStack::Tail()
{
	// TODO: Add your specialized code here.
	if( sp<0 ) return 0;
	return m_pData[0];
}

//##ModelId=4049DD330245
void* CSPtrStack::operator[](int nIndex) const
{
	// TODO: Add your specialized code here.
	ASSERT(nIndex<=sp);
	return GetAt(nIndex);
}

//##ModelId=4049DD3302D1
void* CSPtrStack::GetAt(int nIndex) const
{
	// TODO: Add your specialized code here.'
	ASSERT(nIndex<=sp);
	return CSPtrArray::GetAt(nIndex);
}
