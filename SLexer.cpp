//////////////////////////////////////////////////////////////////////
// SLexer.cpp: implementation of the SLexer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLexer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define PUSHONE( a)  { if( mLength < (int)sizeof(mData)-1) mData[mLength++] = a; else { sprintf(msg,"too long token at %d",mRPtr); return -1;}  }
SLexer::SLexer(const char * p)
{
	mBuffer=p; 
	mSize = (int)strlen(p); 
	mRPtr=0;
}

SLexer::~SLexer()
{

}
short SLexer::ReadOne() 
{ 
	short val;
	if( mRPtr<mSize) val=mBuffer[mRPtr++];
	 else val=0; 
	return val;
}
static BOOL isSeparator(short ch){
        if( isspace(ch)) return TRUE;
        return ch=='='|| ch=='>'||ch=='<'||ch=='!'||ch=='+'||ch=='-'||ch=='/'||ch==';'||ch==','||ch=='%'||ch=='^'||ch=='*';
    }

int SLexer::Scan()
{
    short val;
	mLength = 0;
    while ( TRUE )  {
		val = ReadOne();
		if( val == 0 ) break;
		mFirst = mRPtr;
        char ch = char(val);
        switch (ch){
		case '\t':
        case ' ' : continue;
        case '_' :
		case ':' :
		case '#' :
        case '$' : return ReadIdentify(ch);
        case '0' : case '1' : case '2' : case '3' : case '4' :
        case '5' : case '6' : case '7' : case '8' : case '9' :
                   return ReadNumber(ch);
        case ',' : 
        case ';' : 
        case ']' : 
        case '{' : 
        case '}' : 
        case '(' : 
        case ')' : 
        case '~' : 
		case '=' :
		case '+' :
		case '-' :
		case '/' :
        case '[' :// if (block_brack) return lbrack(CL_NONE);
                   //else
		case '^':
		case '*':
        case '?' :
			mData[0]=ch;mData[1]=0;mLength=1;
			return ch;
		case '\n':
		case '\r':
			continue;
        case '"' : return ReadQuote(ch, LX_STRING);
        case '\'': return ReadQuote(ch, LX_CHAR);
        default:
            if (isalpha(ch))
                return ReadIdentify(ch);
            if (iscntrl(ch)) continue;
            sprintf(msg,"identify can not begin witch %c",ch);
            return -1;            
        }
	}
    return LX_NULL;
}
int SLexer::ReadQuote (char c, int s)
{
    mLength = 0;
	int count;
	int code =0;
	short val;
    while ( (val = ReadOne()) != c) {
		if ( val == 0 ) {
			sprintf(msg,"unexpect end  while look for %c",c);
			return -1; 
		}
		if (val == '\n'){
			sprintf(msg,"unexpect new line while look for %c",c);
			return -1;
		}
        if (val == '\\') {
			if( (val = ReadOne())==0 ){
				sprintf(msg,"unexpect end  while look for %c",c);
				return -1;
			}
            if (val == '\n')  continue;			            
			switch( val ){
		    case '\\': case '\'': case '"': 	break;
		    case 'n': val ='\n';break;
		    case 't': val ='\t';break;
			case 'r': val ='\r';break;
		    case 'f': val ='\f';break;
		    case 'b': val ='\b';break;
			case 'a': val ='\a';break;
			case 'v': val ='\v';break;
			case 'c': 
				{
					if( (val = ReadOne())==0 ){
						sprintf(msg,"unexpect end  while look for %c",c);
						return -1;
					}
					switch(val){
					case '@': val=0;break;
					case '[': val=0x1b;break;
					case '/': val=0x1c;break;
					case ']': val=0x1d;break;
					case '^': val=0x1e;break;
					case '_': val=0x1f;break;
				    default:
					 val=val&0x1F;
					}
					if( val<0 ) val ='c';
				}
				break;
			case 'x':// hex number
				{
					code =0;
					count = 0;
					ReadOne();
					if (!(val >= 'a' && val <= 'f') && !(val >= 'A' && val <= 'F')
					     && !(val >= '0' && val <= '9'))
					    break;
				    if (val >= 'a' && val <= 'f')	code += val - 'a' + 10;
				    if (val >= 'A' && val <= 'F')	code += val - 'A' + 10;
				    if (val >= '0' && val <= '9')	code += val - '0';

					ReadOne();
					if (!(val >= 'a' && val <= 'f')&& !(val >= 'A' && val <= 'F')
					    && !(val >= '0' && val <= '9'))
					   break;
				   code *=16;
				   if (val >= 'a' && val <= 'f')	code += val - 'a' + 10;
				   if (val >= 'A' && val <= 'F')	code += val - 'A' + 10;
				   if (val >= '0' && val <= '9')	code += val - '0';
				   PUSHONE(code);
				   continue;
				}
		    case '0':  case '1':  case '2':  case '3':  case '4':
			case '5':  case '6':  case '7':
				{
					code = 0;
					count = 0;
					while ((val <= '7') && (val >= '0') && (count++ < 3))
					{
						code = (code * 8) + (val - '0');
						ReadOne();
					}
					if( count>0) PUSHONE(code);
					continue;
				}
			default:
				PUSHONE('\\');   // this handles \' and \" too.
			}
		}
        PUSHONE((char)val);
    }
	PutBack();
	mData[mLength]=0;
    return s;
}

int SLexer::ReadIdentify(char ch)
{
    //register BOOL maybe_kw = TRUE;
    //register short bs = 0;
	short val=ch;
    while ( !isSeparator(val) ) {
        mData[mLength++] = (char)val;       // don't worry about overflow
        val = ReadOne();
    }
    mData[mLength] = '\0';
	PutBack();
    return LX_ID;
}
int SLexer::ReadNumber(char ch)
{
    int s = LX_INT;
	int hex = 0;
	short val;
    mLength = 0;
    PUSHONE(ch);
	while ( (val=ReadOne())>0){
		if( isSeparator(val) && val!='/' ) break;
		PUSHONE((char)val);
		if( isdigit(val) ){
			continue;
		}
		if(s== LX_ID ) continue;
		switch(val){
		case 'A':case 'B':case 'C': case'D': case'E': case 'F':
		case 'a': case 'b': case 'c': case'd': case 'e': case 'f':
			if( hex!=0 ) s = LX_ID;
			break;
		case 'x':
		case 'X':
			if( mLength==1 && mData[0]=='0' ) hex=1;
			else s = LX_ID;
			break;
		case '.':
			if( s==LX_INT) s = LX_FLOAT;
			else s = LX_ID;
			break;
		default:
			s = LX_ID;
		}
	}
	if( Eof()|| isSeparator(val) ){
		PutBack();
		mData[mLength]=0;
		if( s==LX_INT ){
			if( ParseInt(mValueInt)<0 ) return -1;
		}
		if( s==LX_FLOAT){
			if( ParseFloat(mValueFloat)<0) return -1;
		}
		return s;
	}
	sprintf(msg,"error number syntax");
	return -1;
}
#define ISDIGIT(c) ( (c>='0')&&(c<='9') )
#define ISXDIGIT(c) ( ISDIGIT(c)||( (c>='A')&&(c<='F') )||( (c>='a')&&(c<='f') ) )
#define HOST_BITS_PER_WIDE_INT     32
#define HOST_BITS_PER_CHAR          8
#define HOST_WIDE_INT       unsigned int 
int SLexer::ParseFloat(float & value)
{
	if( sscanf(msg,"%f",&value)>0 ) return 1;
	return 0;
}
int SLexer::ParseInt(int& low)
{
  int base = 10;
  int count = 0;
  int largest_digit = 0;
  int numdigits = 0;
  int overflow = 0;
  int len = (int)strlen(mData);
  int c;
  int spec_unsigned = 0;
  int spec_long = 0;
  int spec_long_long = 0;
  int suffix_lu = 0;
  int warn = 0, i;
  const char *p;
  
  /* We actually store only HOST_BITS_PER_CHAR bits in each part.
     The code below which fills the parts array assumes that a host
     int is at least twice as wide as a host char, and that 
     HOST_BITS_PER_WIDE_INT is an even multiple of HOST_BITS_PER_CHAR.
     Two HOST_WIDE_INTs is the largest int literal we can store.
     In order to detect overflow below, the number of parts (TOTAL_PARTS)
     must be exactly the number of parts needed to hold the bits
     of two HOST_WIDE_INTs.  */
#define TOTAL_PARTS ((HOST_BITS_PER_WIDE_INT / HOST_BITS_PER_CHAR) )
  unsigned int parts[TOTAL_PARTS];
  
  /* Optimize for most frequent case.  */
  if (len == 1)  {
	low =mData[0] - '0';	
	return 1;
  }
  for (count = 0; count < TOTAL_PARTS; count++)    parts[count] = 0;

  /* len is known to be >1 at this point.  */
  p = mData;
  if( strchr(p,'.') ){
	sprintf(msg,"can not use '.' in  integer");
	 goto syntax_error;
  }

  if (len >2 && mData[0] == '0' && (mData[1] == 'x' || mData[1] == 'X'))
    {
      base = 16;
      p = mData + 2;
    }
  /* The ISDIGIT check is so we are not confused by a suffix on 0.  */
  else if (mData[0] == '0' && ISDIGIT (mData[1]))
    {
      base = 8;
      p = mData + 1;
    }

  do
    {
      c = *p++;
    
    if (c == '_') continue;
	/* Possible future extension: silently ignore _ in numbers,
	   permitting cosmetic grouping - e.g. 0x8000_0000 == 0x80000000
	   but somewhat easier to read.  Ada has this?  */
//	pParser->Error(line,"underscore in number");
	  int n;
	  /* It is not a decimal point.
	     It should be a digit (perhaps a hex digit).  */
	  if (ISDIGIT (c) || (base == 16 && ISXDIGIT (c))) {
		  n = c<='9'?c-'0':c<='F'?c-'A'+10:c-'a'+10;
	    } else if (base <= 10 && (c == 'e' || c == 'E')) {
	      base = 10;
	      break;
	    } else {
	      p--;
	      break;  /* start of suffix */
	    }

	  if (n >= largest_digit)  largest_digit = n;
	  numdigits++;

	  for (count = 0; count < TOTAL_PARTS; count++)   {
	      parts[count] *= base;
	      if (count) {
			  parts[count]
				+= (parts[count-1] >> HOST_BITS_PER_CHAR);
			  parts[count-1]
				&= (1 << HOST_BITS_PER_CHAR) - 1;
		  }   else parts[0] += n;
	    }

	  /* If the highest-order part overflows (gets larger than
	     a host char will hold) then the whole number has 
	     overflowed.  Record this and truncate the highest-order
	     part.  */
	  if (parts[TOTAL_PARTS - 1] >> HOST_BITS_PER_CHAR)  {
	      overflow = 1;
	      parts[TOTAL_PARTS - 1] &= (1 << HOST_BITS_PER_CHAR) - 1;
	    }
    }  while (p < mData + len);

  /* This can happen on input like `int i = 0x;' */
	if (numdigits == 0){
		sprintf(msg,"numeric constant with no digits");
	   goto syntax_error;
	}

	if (largest_digit >= base){
		sprintf(msg,"numeric constant contains digits beyond the radix");
		goto syntax_error;
	}


      while (p < mData + len)	{
	  c = *p++;
	  switch (c)	    {
	    case 'u': case 'U':
	      if (spec_unsigned) sprintf(msg,"two 'u' suffixes on integer constant");
	      spec_unsigned = 1;
	      if (spec_long) suffix_lu = 1;
	      break;
	    case 'l': case 'L':
	      if (spec_long) {
			  if (spec_long_long)
				sprintf(msg,"three 'l' suffixes on integer constant");
			  else if (suffix_lu)
				sprintf(msg,"'lul' is not a valid integer suffix");
			  else if (c != spec_long)
				sprintf(msg,"'Ll' and 'lL' are not valid integer suffixes");
			  else {
				  sprintf(msg,"ISO C89 forbids long long integer constants");
				  spec_long_long = 1;
			  }
		  }
	      spec_long = c;
	      if (spec_unsigned) suffix_lu = 1;
	      break;
	    default:
	      sprintf(msg,"invalid suffix on integer constant");
		  goto syntax_error;
	    }
	}

      /* If the literal overflowed, pedwarn about it now.  */
    if (overflow)	{
	  warn = 1;
	  sprintf(msg,"integer constant is too large for  the compiler");
	  return -1;
	}
	if( spec_long_long ){
		sprintf(msg,"not support long long integer");
		return -1;
	}
	if (p < mData + len){
		sprintf(msg,"missing white space after number '%.*s'", (int) (p - mData), mData);
		return -1;
	}
	
      /* This is simplified by the fact that our constant
	 is always positive.  */

      low = 0;

      for (i = 0; i < HOST_BITS_PER_WIDE_INT / HOST_BITS_PER_CHAR; i++)	{
	  low |= (HOST_WIDE_INT) parts[i] << (i * HOST_BITS_PER_CHAR);
	  }       
  return 1;

 syntax_error:
	return -1;
}


