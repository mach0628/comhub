// ByteArray.h: interface for the CByteArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BYTEARRAY_H__7A0D8168_8418_49F4_8589_A0142C1725CF__INCLUDED_)
#define AFX_BYTEARRAY_H__7A0D8168_8418_49F4_8589_A0142C1725CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CByteArray
{
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	BYTE GetAt(int nIndex) const;
	void SetAt(int nIndex, BYTE newElement);

	BYTE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const BYTE* GetData() const;
	BYTE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, BYTE newElement);

	int Add(BYTE newElement);

	int Append(const CByteArray& src);
	int Append(BYTE * newElement,int length);
	void Copy(const CByteArray& src);

	// overloaded operator helpers
	BYTE operator[](int nIndex) const;
	BYTE& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, BYTE newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CByteArray* pNewArray);

	BYTE * GetWritePtr(int length);
// Implementation
protected:
	BYTE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:
	CByteArray();
	~CByteArray();
};

class CWordArray 
{

public:

// Construction
	CWordArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	WORD GetAt(int nIndex) const;
	void SetAt(int nIndex, WORD newElement);

	WORD& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const WORD* GetData() const;
	WORD* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, WORD newElement);

	int Add(WORD newElement);

	int Append(const CWordArray& src);
	void Copy(const CWordArray& src);

	// overloaded operator helpers
	WORD operator[](int nIndex) const;
	WORD& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, WORD newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CWordArray* pNewArray);

// Implementation
protected:
	WORD* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount


public:
	~CWordArray();

};


////////////////////////////////////////////////////////////////////////////

class CDWordArray
{

	//DECLARE_SERIAL(CDWordArray)
public:

// Construction
	CDWordArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	DWORD GetAt(int nIndex) const;
	void SetAt(int nIndex, DWORD newElement);

	DWORD& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const DWORD* GetData() const;
	DWORD* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, DWORD newElement);

	int Add(DWORD newElement);

	int Append(const CDWordArray& src);
	void Copy(const CDWordArray& src);

	// overloaded operator helpers
	DWORD operator[](int nIndex) const;
	DWORD& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, DWORD newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CDWordArray* pNewArray);

// Implementation
protected:
	DWORD* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount


public:
	~CDWordArray();

protected:
	// local typedefs for class templates
	typedef DWORD BASE_TYPE;
	typedef DWORD BASE_ARG_TYPE;
};

class CUIntArray
{

	//DECLARE_DYNAMIC(CUIntArray)
public:

// Construction
	CUIntArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	UINT GetAt(int nIndex) const;
	void SetAt(int nIndex, UINT newElement);

	UINT& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const UINT* GetData() const;
	UINT* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, UINT newElement);

	int Add(UINT newElement);

	int Append(const CUIntArray& src);
	void Copy(const CUIntArray& src);

	// overloaded operator helpers
	UINT operator[](int nIndex) const;
	UINT& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, UINT newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CUIntArray* pNewArray);

// Implementation
protected:
	UINT* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount


public:
	~CUIntArray();

};
inline int CByteArray::GetSize() const
	{ return m_nSize; }
inline int CByteArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CByteArray::RemoveAll()
	{ SetSize(0); }
inline BYTE CByteArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline void CByteArray::SetAt(int nIndex, BYTE newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }

inline BYTE& CByteArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline const BYTE* CByteArray::GetData() const
	{ return (const BYTE*)m_pData; }
inline BYTE* CByteArray::GetData()
	{ return (BYTE*)m_pData; }
inline int CByteArray::Add(BYTE newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline BYTE CByteArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline BYTE& CByteArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline int CWordArray::GetSize() const
	{ return m_nSize; }
inline int CWordArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CWordArray::RemoveAll()
	{ SetSize(0); }
inline WORD CWordArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline void CWordArray::SetAt(int nIndex, WORD newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }

inline WORD& CWordArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline const WORD* CWordArray::GetData() const
	{ return (const WORD*)m_pData; }
inline WORD* CWordArray::GetData()
	{ return (WORD*)m_pData; }
inline int CWordArray::Add(WORD newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline WORD CWordArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline WORD& CWordArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline int CDWordArray::GetSize() const
	{ return m_nSize; }
inline int CDWordArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CDWordArray::RemoveAll()
	{ SetSize(0); }
inline DWORD CDWordArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline void CDWordArray::SetAt(int nIndex, DWORD newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }

inline DWORD& CDWordArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline const DWORD* CDWordArray::GetData() const
	{ return (const DWORD*)m_pData; }
inline DWORD* CDWordArray::GetData()
	{ return (DWORD*)m_pData; }
inline int CDWordArray::Add(DWORD newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline DWORD CDWordArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline DWORD& CDWordArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }


////////////////////////////////////////////////////////////////////////////

inline int CUIntArray::GetSize() const
	{ return m_nSize; }
inline int CUIntArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CUIntArray::RemoveAll()
	{ SetSize(0); }
inline UINT CUIntArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline void CUIntArray::SetAt(int nIndex, UINT newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }

inline UINT& CUIntArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline const UINT* CUIntArray::GetData() const
	{ return (const UINT*)m_pData; }
inline UINT* CUIntArray::GetData()
	{ return (UINT*)m_pData; }
inline int CUIntArray::Add(UINT newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline UINT CUIntArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline UINT& CUIntArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }

#endif // !defined(AFX_BYTEARRAY_H__7A0D8168_8418_49F4_8589_A0142C1725CF__INCLUDED_)
