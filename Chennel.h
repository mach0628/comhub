// Chennel.h: interface for the CChennel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CHENNEL_H__)
#define __CHENNEL_H__
#include "stdafx.h"

#include "TimeInterval.h"

//##ModelId=43586653003E
class  CChennel  
{
protected:
	/**
	 * 空闲时间
	 */
	DECLARE_TIME_INTERVAL(mTP);

	/**
	 * 端口句柄
	 */
	SOCKET mHandle;
public:
	void Attach(CChennel & chennel);
	void Attach(int handle,sockaddr_in in);
	void Attach(int handle,const char * sIP,int port);
	void Attach(const char * sIP,int port);
	/**
	 * 关闭通道
	 * @return >0 成功
	 */
	int CloseChennel();
	/**
	 * 打开通道
	 * @return >0 成功
	 */
	int OpenChennel();
	/**
	 * 读取数据，@length指明最大长度
	 * @return 实际获取的数据长度，小于零表示错误
	 */
	int ReadData(char * buff,int length);
	/**
	 * 读取数据，@length表示需要读取的长度，@nMillSec表示最大时间
	 * @return 小于零表示错误，0表示超时，其他表示成功
	 */
	int ReadData(char * buff,int* length,int nMillSec);
	/**
	 * 写数据
	 * @return >0 成功
	 */
	int WriteData(const char * buff,int length);
	/**
	 * 当前通道是否有数据可读
	 * @return 可读的字节数
	 */
	int CanRead();

	int IsClose();
	/**
	 *
	 */
	unsigned long GetFreeTime(){
		STAMP_TIME_INTERVAL(mTP);
		return GET_TIME_INTERVAL(mTP);
	};
	CChennel();
	CChennel(char * sAddr,int port);
	virtual ~CChennel();
public:
	/**
	 * 对方地址
	 */
	char mAddr[24];
	/**
	 * 端口
	 */
	int mPort;
};

#endif

