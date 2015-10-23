//	Serial.cpp - Implementation of the CSerial class
//
//	Copyright (C) 1999-2003 Ramon de Klein (Ramon.de.Klein@ict.nl)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


//////////////////////////////////////////////////////////////////////
// Include the standard header files

#define STRICT
#include "stdafx.h"
#include "Serial.h"

#ifndef WIN32
#include <fcntl.h> //文件控制定义  
#include <termios.h>//终端控制定义  
#include <errno.h> 
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////
// Disable warning C4127: conditional expression is constant, which
// is generated when using the _RPTF and _ASSERTE macros.
#ifdef WIN32
#pragma warning(disable: 4127)
#endif

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager

#ifdef _DEBUG

#ifdef THIS_FILE
#undef THIS_FILE
#endif

static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW

#endif

#define TRACE0(a ) Trace::traceEvent(TRACE_NORMAL,a)
//////////////////////////////////////////////////////////////////////
// Code

CSerial::CSerial (): m_hFile(0)
{
	memset(mName,0,sizeof(mName));
	memset(mDevName,0,sizeof(mDevName));
	RESET_TIME_INTERVAL(mLastTime);
}

CSerial::~CSerial ()
{
	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile) {
		// Display a warning
		TRACE0("CSerial::~CSerial - Serial port not closed");

		// Close implicitly
		Close();
	}
}
CSerial::EPort CSerial::CheckPort (const char * lpszDevice)
#ifdef WIN32
{
	// Try to open the device
	HANDLE hFile = ::CreateFile(lpszDevice, 
						   GENERIC_READ|GENERIC_WRITE, 
						   0, 
						   0, 
						   OPEN_EXISTING, 
						   0,
						   0);

	// Check if we could open the device
	if (hFile == INVALID_HANDLE_VALUE)	{
		// Display error
		switch (::GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
			// The specified COM-port does not exist
			return EPortNotAvailable;

		case ERROR_ACCESS_DENIED:
			// The specified COM-port is in use
			return EPortInUse;

		default:
			// Something else is wrong
			return EPortUnknownError;
		}
	}

	// Close handle
	::CloseHandle(hFile);

	// Port is available
	return EPortAvailable;
}
#else
{
		HANDLE hFile = open(lpszDevice, O_RDWR | O_NOCTTY | O_NDELAY); 
		if( hFile == -1 ){
			switch(errno){
			case ENOENT:
				// The specified COM-port does not exist
				return EPortNotAvailable;

			case EACCES:
				// The specified COM-port is in use
				return EPortInUse;

			default:
				// Something else is wrong
				return EPortUnknownError;
			}
			
		}
		if( isatty(hFile)==0 ) {
			close(hFile);
			return EPortNotAvailable;
		}
	// Close handle
	close(hFile);

	// Port is available
	return EPortAvailable;
}
#endif
BOOL CSerial::Open (const char * lpszDevice)
{
	// Check if the port isn't already opened
	if (m_hFile)	{
		TRACE0("CSerial::Open - Port already opened");
		return FALSE;
	}
	Trace::traceEvent(TRACE_NORMAL,"open serial %s",lpszDevice);
#ifdef WIN32
	// Open the device
	m_hFile = ::CreateFile(lpszDevice, GENERIC_READ|GENERIC_WRITE,
						   0, 0, OPEN_EXISTING, 0, 0);
	if (m_hFile == INVALID_HANDLE_VALUE)	{
		// Reset file handle
		m_hFile = 0;
        Trace::traceEvent(TRACE_WARNING,"Unable to open %s, Error No=%d",lpszDevice, GetLastError());
		return FALSE;
	}

	// Setup the COM-port
	// Non-blocking reads is default
	SetupReadTimeouts(EReadTimeoutNonblocking);
	::PurgeComm(m_hFile, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  //清空缓冲区
/*
	// Setup the device for default settings
 	COMMCONFIG commConfig = {0};
	DWORD dwSize = sizeof(commConfig);
	commConfig.dwSize = dwSize;
	if (::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize))
	{
		// Set the default communication configuration
		if (!::SetCommConfig(m_hFile,&commConfig,dwSize))
		{
			// Display a warning
			TRACE0("CSerial::Open - Unable to set default communication configuration.");
		}
	}
	else
	{
		// Display a warning
		TRACE0("CSerial::Open - Unable to obtain default communication configuration.");
	}
*/
#else
		m_hFile = open(lpszDevice, O_RDWR | O_NOCTTY | O_NDELAY); 
		if( m_hFile == -1 ){
			Trace::traceEvent(TRACE_ERROR,"open %s  error, code=%d",lpszDevice,errno);
			m_hFile = 0;
			return FALSE;
		}
		if( isatty(m_hFile)==0 ) {
			Trace::traceEvent(TRACE_ERROR,"%s is not comm",lpszDevice);
			close(m_hFile);
			m_hFile = 0;
			return FALSE;
		}
		tcflush(m_hFile, TCIOFLUSH);

#endif
	memset(mDevName,0,sizeof(mDevName));
	strncpy(mDevName,lpszDevice,sizeof(mDevName)-1);
	// Return successful
	return TRUE;
}

void CSerial::Close (void)
{
	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile == 0)	return ;
	// Close COM port
	Trace::traceEvent(TRACE_NORMAL,"close serial %s",mDevName);

#ifdef WIN32
	::CloseHandle(m_hFile);
#else
	close(m_hFile);
#endif
	m_hFile = 0;

}

BOOL CSerial::Setup (DWORD eBaudrate, BYTE eDataBits, BYTE eParity, BYTE eStopBits)
{
	// Check if the device is open
	if (m_hFile == 0){
		// Issue an error and quit
		TRACE0("CSerial::Setup - Device is not opened");
		return FALSE;
	}
	Trace::traceEvent(TRACE_NORMAL,"set serial %s to eBaudrate=%d  eDataBits=%d eParity=%d eStopBits=%d",mDevName,
		eBaudrate,eDataBits,eParity,eStopBits);
#ifdef WIN32
	// Obtain the DCB structure for the device
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);

	if (!::GetCommState(m_hFile,&dcb))	{
		// Display a warning
		TRACE0("CSerial::Setup - Unable to obtain DCB information");
		return FALSE;
	}

	// Set the new data
	dcb.BaudRate = eBaudrate;
	dcb.ByteSize = eDataBits;
	dcb.Parity   = eParity;
	dcb.StopBits = eStopBits;

	// Determine if parity is used
	dcb.fParity  = (eParity != NOPARITY);

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))	{
		// Display a warning
		TRACE0("CSerial::Setup - Unable to set DCB information");
		return FALSE;
	}
#else
	termios newtio;
	if  ( tcgetattr( m_hFile,&newtio)  !=  0){
		TRACE0("CSerial::Setup - Unable to obtain DCB information");
		return  FALSE;
	}
	newtio.c_cflag |= (CLOCAL | CREAD);
#ifdef CNEW_RTSCTS
        newtio.c_cflag &= ~CNEW_RTSCTS;
#endif
	newtio.c_lflag  &= ~( ICANON | ECHO |ECHONL| ECHOE | ISIG);  /*Input,不经处理直接发送*/
	newtio.c_oflag  &= ~(OPOST|ONLCR | OCRNL);   /*Output*/
	newtio.c_cflag &= ~(CSIZE|CRTSCTS|PARENB);
	newtio.c_iflag &= ~(INLCR|ICRNL|IGNCR|ISTRIP|IXON| IXOFF | IXANY);


	switch( eBaudrate )    {    
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);        
		break;    
	case 4800:        
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;    
	case 9600:        
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;    
	case 115200:        
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);        
		break;
	case 38400:
		cfsetispeed(&newtio, B38400);
		cfsetospeed(&newtio, B38400);        
		break;
	default:        
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);        
		break;    
	}

	switch(eDataBits){
		case 8:
			newtio.c_cflag |= CS8;
			break;
		case 7:
			newtio.c_cflag |= CS7;
			break;
		default:
			TRACE0("CSerial::Setup - unsupport databits");
			return FALSE;
	}
	switch(eStopBits){
	case 1:
		newtio.c_cflag &= ~CSTOPB;
		break;
	case 2:
		newtio.c_cflag |= CSTOPB;
		break;
	default:
			TRACE0("CSerial::Setup - unsupport StopBits");
			return FALSE;
	}
    switch( eParity )    {
	case ODDPARITY:                     //奇校验
		newtio.c_cflag |= PARENB;        
		newtio.c_cflag |= PARODD;        
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;    
	case EVENPARITY:                     //偶校验
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;        
		newtio.c_cflag &= ~PARODD;        
		break;    
	case NOPARITY:                    //无校验
		newtio.c_cflag &= ~PARENB;       
		newtio.c_iflag &= ~INPCK;        //enable parity checking
		break;
	case SPACEPARITY:
		newtio.c_cflag &= ~PARENB; //清除校验位
		newtio.c_cflag &= ~CSTOPB; //
		newtio.c_iflag |= INPCK; //disable pairty checking
		break;
	default:
			TRACE0("CSerial::Setup - unsupport Parity");
			return FALSE;

	}
	if(tcsetattr(m_hFile, TCSANOW, &newtio) != 0){
		TRACE0("CSerial::Setup - Unable to set DCB information");
		return FALSE;
	}
#endif
	// Return successful
	return TRUE;
}

BOOL CSerial::SetupHandshaking (EHandshake eHandshake)
{
	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::SetupHandshaking - Device is not opened");
		return FALSE;
	}
#ifdef WIN32
	// Obtain the DCB structure for the device
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	if (!::GetCommState(m_hFile,&dcb))	{
		// Display a warning
		TRACE0("CSerial::SetupHandshaking - Unable to obtain DCB information");
		return FALSE;
	}

	// Set the handshaking flags
	switch (eHandshake)	{
	case EHandshakeOff:
		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	case EHandshakeHardware:
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
		break;

	case EHandshakeSoftware:
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	default:
		// This shouldn't be possible
		_ASSERTE(false);
		return FALSE;
	}

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))	{
		// Display a warning
		TRACE0("CSerial::SetupHandshaking - Unable to set DCB information");
		return FALSE;
	}
#else
	termios oldtio;
	if  ( tcgetattr( m_hFile,&oldtio)  !=  0){
		TRACE0("CSerial::SetupHandshaking - Unable to obtain DCB information");
		return  EHandshakeUnknown;
	}
	// Set the handshaking flags
#ifdef CNEW_RTSCTS
	oldtio.c_cflag &= ~CNEW_RTSCTS;
#endif
	oldtio.c_iflag &= ~(IXON | IXOFF | IXANY);
	switch (eHandshake)	{
		case EHandshakeHardware:
#ifdef CNEW_RTSCTS
			oldtio.c_cflag |= CNEW_RTSCTS;
#endif
			break;
		case EHandshakeSoftware:
			oldtio.c_iflag |=  (IXON | IXOFF | IXANY);
			break;
		case EHandshakeOff:
			break;
		default:
			return FALSE;
        }
	if(tcsetattr(m_hFile, TCSANOW, &oldtio) != 0){
		TRACE0("CSerial::SetupHandshaking - Unable to set DCB information");
		return FALSE;
	}
#endif
	// Return successful
	return TRUE;
}

CSerial::EHandshake CSerial::GetHandshaking (void)
{
	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::GetHandshaking - Device is not opened");
		return EHandshakeUnknown;
	}
#ifdef WIN32
	// Obtain the DCB structure for the device
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	if (!::GetCommState(m_hFile,&dcb))	{
		// Display a warning
		TRACE0("CSerial::GetHandshaking - Unable to obtain DCB information");
		return EHandshakeUnknown;
	}

	// Check if hardware handshaking is being used
	if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
		return EHandshakeHardware;

	// Check if software handshaking is being used
	if (dcb.fOutX && dcb.fInX)
		return EHandshakeSoftware;
#else
	termios oldtio;
	if  ( tcgetattr( m_hFile,&oldtio)  !=  0){
		TRACE0("CSerial::GetHandshaking - Unable to obtain DCB information");
		return  EHandshakeUnknown;
	}
#ifdef CNEW_RTSCTS
	if( (oldtio.c_cflag &CNEW_RTSCTS)!=0 ) return EHandshakeHardware;
#endif
	if( (oldtio.c_iflag & (IXON | IXOFF | IXANY))!=0 )   return EHandshakeSoftware;
#endif
	// No handshaking is being used
	return EHandshakeOff;
}

BOOL CSerial::SetupReadTimeouts (EReadTimeout eReadTimeout)
{
	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::SetupReadTimeouts - Device is not opened");
		return FALSE;
	}
	Trace::traceEvent(TRACE_NORMAL,"set serial %s  read timeout to %d",eReadTimeout);
#ifdef WIN32
	// Determine the time-outs
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto)){
		// Display a warning
		TRACE0("CSerial::SetupReadTimeouts - Unable to obtain timeout information");
		return FALSE;
	}

	// Set the new timeouts
	switch (eReadTimeout)	{
	case EReadTimeoutBlocking:
/*
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
*/
       cto.ReadIntervalTimeout = MAXDWORD;  
       cto.ReadTotalTimeoutMultiplier = 10;  
       cto.ReadTotalTimeoutConstant = 10;    
       cto.WriteTotalTimeoutMultiplier = 50;  
       cto.WriteTotalTimeoutConstant = 100; 

		break;
	case EReadTimeoutNonblocking:
		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	default:
		// This shouldn't be possible
		_ASSERTE(false);
		return FALSE;
	}

	// Set the new DCB structure
	if (!::SetCommTimeouts(m_hFile,&cto))	{
		// Display a warning
		TRACE0("CSerial::SetupReadTimeouts - Unable to set timeout information");
		return FALSE;
	}
#else
	long flags = fcntl( m_hFile, F_GETFL, 0);
	
	switch(eReadTimeout){
		case EReadTimeoutBlocking:
			flags =0;
			break;
		case EReadTimeoutNonblocking:
			flags |= O_NONBLOCK;//fcntl(fd, F_SETFL, FNDELAY);
			break;
		default: return FALSE;
	}
	if( fcntl(m_hFile,F_SETFL,flags)<0 ) return FALSE;
#endif
	// Return successful
	return TRUE;
}

BOOL CSerial::GetCommStat(int& baudrate,int & databits,int & parity,int& stopsbits )
{
	// Check if the device is open
	if (m_hFile == 0){
		// Issue an error and quit
		TRACE0("CSerial::GetCommStat - Device is not opened");
		return FALSE;
	}
#ifdef WIN32	
	// Obtain the DCB structure for the device
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	if (!::GetCommState(m_hFile,&dcb)){
		// Display a warning
		TRACE0("CSerial::GetCommStat - Unable to obtain DCB information");
		return FALSE;
	}
	baudrate = dcb.BaudRate;
	databits = dcb.ByteSize;
	parity = dcb.fParity;
	stopsbits = dcb.StopBits;
#else
	termios oldtio;
	if  ( tcgetattr( m_hFile,&oldtio)  !=  0){
		TRACE0("CSerial::GetCommStat - Unable to obtain DCB information");
		return  FALSE;
	}
	baudrate = cfgetispeed(&oldtio);
	switch( oldtio.c_cflag  & CSIZE ){
		case CS5: databits = 5;break;
		case CS6: databits =6; break;
		case CS7: databits =7; break;
		case CS8: databits =8; break;
		default:
			break;
	}
	if( (oldtio.c_cflag & CSTOPB)==0 ) stopsbits=1; /*停止位为1*/
	else stopsbits = 2;
	if( oldtio.c_cflag &PARENB ){//校验位
		if( oldtio.c_cflag & PARODD ) parity = ODDPARITY;
		else parity = EVENPARITY;
	} else parity = NOPARITY;        


#endif
	return TRUE;
}

BOOL CSerial::Write (const void* pData, int iLen)
{


	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::Write - Device is not opened");
		return FALSE;
	}
	//Trace::traceEventN(TRACE_NORMAL,"%s-->",mName);
	//for(int i=0;i<iLen;i++) Trace::traceEventRaw(" %02X",((char *)pData)[i]);
	//Trace::traceEventRaw("\n");
	RESET_TIME_INTERVAL(mLastTime);
#ifdef WIN32
	// Use our own variable for read count
	DWORD dwWritten =0;
	// Write the data
	if (!::WriteFile(m_hFile,pData,iLen,&dwWritten,0))	{
		// Issue an error and quit
		TRACE0("CSerial::Write - Unable to write the data");
		return FALSE;
	}
#else
	int dwWritten=0;
	dwWritten = write(m_hFile,pData,(int)iLen);
	if( dwWritten<0 ) {
		if( errno!=EAGAIN ){ 
			Trace::traceEvent(TRACE_NORMAL,"Unable to write the data,error =%d",errno);
			return FALSE;
		}
	}
#endif
	// Return successfully
	return TRUE;
}

BOOL CSerial::Write (LPCSTR pString)
{
	// Determine the length of the string
	int k =(int)strlen(pString);
	if( k>2 ){
		Trace::traceEvent(TRACE_NORMAL,"%s ===> %s",mName,pString);
		if( pString[k-1]==CR ) Trace::traceEventRaw("\n");
	}
	return Write(pString,k);
}

BOOL CSerial::Read (void* pData, int iLen, int * pdwRead)
{
	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::Read - Device is not opened");
		return FALSE;
	}

#ifdef _DEBUG
	// The debug version fills the entire data structure with
	// 0xDC bytes, to catch buffer errors as soon as possible.
	memset(pData,0xDC,iLen);
#endif
	int dwRead;

#ifdef WIN32
	// Use our own variable for read count
	if (pdwRead == 0)	pdwRead = &dwRead;	

	// Reset the number of bytes read
	*pdwRead = 0;
	// Read the data
	if (!::ReadFile(m_hFile,pData,iLen,(DWORD *)pdwRead,0))	{
		// Issue an error and quit
		TRACE0("CSerial::Read - Unable to read the data");
		return FALSE;
	}
#else
	dwRead = read(m_hFile,pData,iLen);
	if( dwRead <0 ){
		if( errno!=EAGAIN ){ 
			Trace::traceEvent(TRACE_NORMAL,"Unable to read the data,error =%d",errno);
			return FALSE;
		}
		dwRead=0;
	}
    pdwRead[0] = dwRead;
#endif
#if 0
int fsSel;
fd_set fsRead;
struct timeval time;

FD_ZERO(&fsRead);
FD_SET (m_hFile, &fsRead);

/* 超时时间25毫秒 */
time.tv_sec         = 0;
time.tv_usec         = 25000;

/* 使用select实现非阻塞的串口接收 */
fsSel = select( m_hFile+1, &fsRead, NULL, NULL, &time );

if ( FD_ISSET(m_hFile, &fsRead) ) {
        dwRead = read(m_hFile, rxBuf, Len);
		pdwRead[0] = dwRead;
        return TRUE;

} else {
        return FALSE;
}
#endif
	// Return successfully
	return TRUE;
}

BOOL CSerial::Purge()
{
	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::Purge - Device is not opened");
		return FALSE;
	}
#ifdef WIN32
	if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR))	{
		TRACE0("CSerial::Purge - Overlapped completed without result");
	}
#else
	tcflush(m_hFile, TCIOFLUSH);
#endif
	// Return successfully
	return TRUE;
}

BOOL CSerial::Break (void)
{
	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::Break - Device is not opened");
		return FALSE;
	}
#ifdef WIN32
    // Set the RS-232 port in break mode for a little while
    ::SetCommBreak(m_hFile);
    ::Sleep(100);
    ::ClearCommBreak(m_hFile);
#else
	tcsendbreak(m_hFile,100);
#endif
	// Return successfully
	return TRUE;
}
/*
CSerial::EError CSerial::GetError (void)
{
	// Check if the device is open
	if (m_hFile == 0)	{
		// Issue an error and quit
		TRACE0("CSerial::GetError - Device is not opened");
		return EErrorUnknown;
	}

	// Obtain COM status
	DWORD dwErrors = 0;
	if (!::ClearCommError(m_hFile,&dwErrors,0))	{
		// Issue an error and quit
		TRACE0("CSerial::GetError - Unable to obtain COM status");
		return EErrorUnknown;
	}

	// Return the error
	return EError(dwErrors);
}

BOOL CSerial::GetModemStatus(bool& CTS,bool & DSR,bool& Ring,bool & RLSD)
{
	// Check if the device is open
	if (m_hFile == 0){
		// Issue an error and quit
		TRACE0("CSerial::GetError - Device is not opened");
		return FALSE;
	}
	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))	{
		// Display a warning
		TRACE0("CSerial::GetCTS - Unable to obtain the modem status");
		return FALSE;
	}
	CTS = (dwModemStat & MS_CTS_ON)!=0;
	DSR = (dwModemStat & MS_DSR_ON) != 0;
	Ring = (dwModemStat & MS_RING_ON) != 0;
	RLSD = (dwModemStat & MS_RLSD_ON) != 0;
	return TRUE;
}
*/
void CSerial::SetName(const char * p)
{
	memset(mName,0,sizeof(mName));
	strncpy(mName,p,sizeof(mName)-1);
}
BOOL  CSerial::SetUser(const char * p)
{
	if( p!=NULL ){
		if( mUser[0]!=0 ) return FALSE;
		memset(mUser,0,sizeof(mUser));
		strncpy(mUser,p,sizeof(mUser)-1);
	} else {
		mUser[0]=0;
	}
	return TRUE;
}
long CSerial::GetTimeSnap()
{
	STAMP_TIME_INTERVAL(mLastTime);
	return GET_TIME_INTERVAL(mLastTime);
}
void  CSerial::DumpHead(char * buffer)
{
	sprintf(buffer," Index \tName            \tDevice          \tLocker                  \t Status \t Time \r\n");
}
void CSerial::Dump(int index,char * buffer)
{
	sprintf(buffer," %d \t %-16s \t %-16s \t %-24s \t%-8s\t %ld\r\n",index,mName,mDevName,mUser,IsOpen()?"open":"closed", GetTimeSnap());
}

