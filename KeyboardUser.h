#ifndef __KEYBOARD_USER_H__
#define __KEYBOARD_USER_H__

#include "HubUser.h"

class KeyboardUser:public HubUser
{
	int mBlock;
	void SetBlock();
	void SetNoBlock();
public:
	KeyboardUser(HubCmd * cm);
	virtual ~KeyboardUser();
public:
	virtual void Close();
	virtual void onConnected();
	virtual void Cls();
protected:
	virtual void Show(CSerial * src,const char * msg);
	virtual char * ReadData();
};

#endif


