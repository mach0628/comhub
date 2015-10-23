#ifndef __LOG_USER_H__
#define __LOG_USER_H__


#include "HubUser.h"

class HubCenter;

class LogUser :public HubUser
{
	HubCenter * mCenter;
	int mStatus;
	int mIndex;
public:
	LogUser(HubCenter * center);
	virtual ~LogUser();

	virtual BOOL canShow(CSerial * src){ return TRUE; }
	virtual void Dump(int index,char * buffer);
	virtual void Cls(){};

protected:
	virtual void Show(CSerial * src,const char * msg);
	virtual char * ReadData();
};


#endif



