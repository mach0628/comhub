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
	 * ����ʱ��
	 */
	DECLARE_TIME_INTERVAL(mTP);

	/**
	 * �˿ھ��
	 */
	SOCKET mHandle;
public:
	void Attach(CChennel & chennel);
	void Attach(int handle,sockaddr_in in);
	void Attach(int handle,const char * sIP,int port);
	void Attach(const char * sIP,int port);
	/**
	 * �ر�ͨ��
	 * @return >0 �ɹ�
	 */
	int CloseChennel();
	/**
	 * ��ͨ��
	 * @return >0 �ɹ�
	 */
	int OpenChennel();
	/**
	 * ��ȡ���ݣ�@lengthָ����󳤶�
	 * @return ʵ�ʻ�ȡ�����ݳ��ȣ�С�����ʾ����
	 */
	int ReadData(char * buff,int length);
	/**
	 * ��ȡ���ݣ�@length��ʾ��Ҫ��ȡ�ĳ��ȣ�@nMillSec��ʾ���ʱ��
	 * @return С�����ʾ����0��ʾ��ʱ��������ʾ�ɹ�
	 */
	int ReadData(char * buff,int* length,int nMillSec);
	/**
	 * д����
	 * @return >0 �ɹ�
	 */
	int WriteData(const char * buff,int length);
	/**
	 * ��ǰͨ���Ƿ������ݿɶ�
	 * @return �ɶ����ֽ���
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
	 * �Է���ַ
	 */
	char mAddr[24];
	/**
	 * �˿�
	 */
	int mPort;
};

#endif

