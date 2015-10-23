#ifndef __NEW_DEBUG_H__
#define __NEW_DEBUG_H__
#ifdef WIN32
#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h> 
#include<crtdbg.h> 
#endif

#ifdef _DEBUG

inline void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
{
	return ::operator new(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
}

inline void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine)
{
	return ::operator new[](nSize, _NORMAL_BLOCK, lpszFileName, nLine);
}

inline void __cdecl operator delete(void* pData, LPCSTR /* lpszFileName */, int /* nLine */)
{
	::operator delete(pData);
}

inline void __cdecl operator delete[](void* pData, LPCSTR /* lpszFileName */, int /* nLine */)
{
	::operator delete(pData);
}

#define DEBUG_NEW new(THIS_FILE, __LINE__)

#endif

#ifndef ASSERT
#define ASSERT _ASSERT
#endif

#endif
