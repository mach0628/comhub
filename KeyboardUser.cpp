///////////////////////////////////////////////////////////////
//KeyboardUser
//
#include "stdafx.h"
#include "KeyboardUser.h"
#include "Prefs.h"
#include <fcntl.h> //文件控制定义  
#ifdef WIN32
#include <conio.h>
#define kbhit()  _kbhit()
#define getch() _getch()
#define putch(a) _putch(a)
#else
#include <termios.h>
#define getch() getchar()
#define putch(a) putchar(a)
static struct termios ori_attr;
#endif

KeyboardUser::KeyboardUser(HubCmd * cm):HubUser(cm)
{
	strcpy(mName,"keyboard");
	mBlock = 1;
}
KeyboardUser::~KeyboardUser()
{
}
void KeyboardUser::Show(CSerial * src,const char * msg)
{
	printf("%s",msg);
}
void KeyboardUser::onConnected()
{
	Trace::traceEvent(TRACE_NORMAL,"keyboard is up");
	HubUser::onConnected();
	SetNoBlock();
}
void KeyboardUser::SetBlock()
{
#ifndef WIN32
	if( mBlock ) return;
	tcdrain(STDIN_FILENO);
	//tcflush(TCIOFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &ori_attr);  
	mBlock = 1;
#endif
}
void KeyboardUser::SetNoBlock()
{
#ifndef WIN32
	if( !mBlock ) return;
	struct termios cur_attr;
//	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);   // 设置为非阻塞形式
	if ( tcgetattr(STDIN_FILENO, &ori_attr) )      return;
	memcpy(&cur_attr, &ori_attr, sizeof(cur_attr) );
	cur_attr.c_lflag  &= ~( ICANON | ECHO |ECHONL| ECHOE );  /*Input,不经处理直接发送*/
    //cur_attr.c_cflag &= ~(CSIZE|CRTSCTS|PARENB);
    cur_attr.c_iflag &= ~(ISTRIP|IXON| IXOFF | IXANY);

	cur_attr.c_cc[VMIN] = 0;
	cur_attr.c_cc[VTIME] = 0;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &cur_attr) != 0) return;
	mBlock = 0;
#endif
}
char *  KeyboardUser::ReadData()
{
#ifdef WIN32
		if( kbhit() ){
#endif
			int ch = getch();
			if( ch<0 ) return NULL;
			if( ch==0xe0 ){
				//control key
				ch = getch();
				if( ch==0x53){//del
					mWritePtr--;
					mData[mWritePtr]=0;
					putch(' ');
					putch(ch);
				}
				return NULL;
			}
#ifndef WIN32
			if( gPrefs->mNoESC ){
				// disable direct key
				if( ch==0x1b ){// ESC
					for(int i=0;i<10;i++) if( getch()<0 ) break;
					return NULL;
				}
			}
#endif
			printf("%c",ch);
			if( ch=='\b' ){
				if( mWritePtr>0 ){
					mWritePtr--;
					mData[mWritePtr]=0;
					putch(' ');
					putch(ch);
				}
			} else {			
				mData[mWritePtr]=ch;
				mWritePtr++;
				if( ch==CR||ch==LF )	{
					if( ch==CR ) putch(LF);
					return mData;
				}
				
			}
#ifdef WIN32
		}
#endif
	return NULL;
}
void KeyboardUser::Close()
{
	unLock(); 
	SetBlock();
	HubUser::Close();
}
void KeyboardUser::Cls()
{
#ifdef WIN32
	system("cls");
#else
	printf("\033[2J");
	printf("\033[H");
#endif
}


