// SLexer.h: interface for the CSLexer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SLEXER_H__)
#define __SLEXER_H__


class SLexer  
{
	const char * mBuffer;
	int  mSize;
	char msg[512];
	int  mRPtr;
	char  mData[1024];
	int mLength;
	int mFirst;
public:

	int Scan();
	SLexer(const char * p);
	virtual ~SLexer();
public:
    enum WORDTYPE {
		LX_NULL  =0,
		LX_ID = 256,
		LX_STRING =257,
		LX_CHAR = 258,
		LX_INT = 259,
		LX_FLOAT = 260
	};
	int   pos() { return mFirst; };
	const char * txt(){ return mData; };
	const char * error(){ return msg; };
	union{
		int mValueInt;
		float mValueFloat;
	};
protected:
	short ReadOne() ;
	void PutBack(){ mRPtr--; if( mRPtr<0 ) mRPtr=0; };
	BOOL Eof(){ return mRPtr>=mSize; };
	int  ReadQuote (char c, int s);
	int ReadIdentify (char ch);
	int ReadNumber (char ch);

	int ParseInt(int& low);
	int ParseFloat(float & value);

};

#endif // !defined(AFX_SLEXER_H__BD346914_CA36_4054_B25B_AAC773809408__INCLUDED_)
