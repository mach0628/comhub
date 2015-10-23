//	Serial.h - Definition of the CSerial class
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


#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "TimeInterval.h"
//////////////////////////////////////////////////////////////////////
// The SERIAL_DEFAULT_OVERLAPPED defines if the default open mode uses
// overlapped I/O. When overlapped I/O is available (normal Win32
// platforms) it uses overlapped I/O. Windows CE doesn't allow the use
// of overlapped I/O, so it is disabled there by default.



//////////////////////////////////////////////////////////////////////
//
// CSerial - Win32 wrapper for serial communications
//
// Serial communication often causes a lot of problems. This class
// tries to supply an easy to use interface to deal with serial
// devices.
//
// The class is actually pretty ease to use. You only need to open
// the COM-port, where you need to specify the basic serial
// communication parameters. You can also choose to setup handshaking
// and read timeout behaviour.
//
// The following serial classes are available:
//
// CSerial      - Serial communication support.
// CSerialEx    - Serial communication with listener thread for events
// CSerialSync  - Serial communication with synchronized event handler
// CSerialWnd   - Asynchronous serial support, which uses the Win32
//                message queue for event notification.
// CSerialMFC   - Preferred class to use in MFC-based GUI windows.
// 
//
// Pros:
// -----
//	- Easy to use (hides a lot of nasty Win32 stuff)
//	- Fully ANSI and Unicode aware
//
// Cons:
// -----
//  - Little less flexibility then native Win32 API, however you can
//    use this API at the same time for features which are missing
//    from this class.
//  - Incompatible with Windows 95 or Windows NT v3.51 (or earlier),
//    because CancelIo isn't support on these platforms. Define the
//	  SERIAL_NO_CANCELIO macro for support of these platforms as
//	  well. When this macro is defined, then only time-out values of
//	  0 or INFINITE are valid.
//
//
// Copyright (C) 1999-2003 Ramon de Klein
//                         (Ramon.de.Klein@ict.nl)
#ifndef WIN32
#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400
#define CBR_56000           56000
#define CBR_57600           57600
#define CBR_115200          115200
#define CBR_128000          128000
#define CBR_256000          256000

#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4

#define ONESTOPBIT          0
#define ONE5STOPBITS        1
#define TWOSTOPBITS         2

#endif
class CSerial
{
// Class enumerations
public:

	// Baudrate
	typedef enum
	{
		EBaudUnknown = -1,			// Unknown
		EBaud110     = CBR_110,		// 110 bits/sec
		EBaud300     = CBR_300,		// 300 bits/sec
		EBaud600     = CBR_600,		// 600 bits/sec
		EBaud1200    = CBR_1200,	// 1200 bits/sec
		EBaud2400    = CBR_2400,	// 2400 bits/sec
		EBaud4800    = CBR_4800,	// 4800 bits/sec
		EBaud9600    = CBR_9600,	// 9600 bits/sec
		EBaud14400   = CBR_14400,	// 14400 bits/sec
		EBaud19200   = CBR_19200,	// 19200 bits/sec (default)
		EBaud38400   = CBR_38400,	// 38400 bits/sec
		EBaud56000   = CBR_56000,	// 56000 bits/sec
		EBaud57600   = CBR_57600,	// 57600 bits/sec
		EBaud115200  = CBR_115200,	// 115200 bits/sec
		EBaud128000  = CBR_128000,	// 128000 bits/sec
		EBaud256000  = CBR_256000,	// 256000 bits/sec
	}
	EBaudrate;

	// Handshaking
	typedef enum
	{
		EHandshakeUnknown		= -1,	// Unknown
		EHandshakeOff			=  0,	// No handshaking
		EHandshakeHardware		=  1,	// Hardware handshaking (RTS/CTS)
		EHandshakeSoftware		=  2	// Software handshaking (XON/XOFF)
	} 
	EHandshake;

	// Timeout settings
	typedef enum
	{
		EReadTimeoutUnknown		= -1,	// Unknown
		EReadTimeoutNonblocking	=  0,	// Always return immediately
		EReadTimeoutBlocking	=  1	// Block until everything is retrieved
	}
	EReadTimeout;


	// Port availability
	typedef enum
	{
		EPortUnknownError = -1,		// Unknown error occurred
		EPortAvailable    =  0,		// Port is available
		EPortNotAvailable =  1,		// Port is not present
		EPortInUse        =  2		// Port is in use

	} 
	EPort;

// Construction
public:
	CSerial();
	virtual ~CSerial();


// Operations
public:
	// Check if particular COM-port is available (static method).
	static EPort CheckPort (const char * lpszDevice);
	

	// Open the serial communications for a particular COM port. You
	// need to use the full devicename (i.e. "COM1") to open the port.
	// It's possible to specify the size of the input/output queues.
	BOOL Open (const char * lpszDevice);

	// Close the serial port.
	void Close (void);

	// Setup the communication settings such as baudrate, databits,
	// parity and stopbits. The default settings are applied when the
	// device has been opened. Call this function if these settings do
	// not apply for your application. If you prefer to use integers
	// instead of the enumerated types then just cast the integer to
	// the required type. So the following two initializations are
	// equivalent:
	//
	//   Setup(EBaud9600,EData8,EParNone,EStop1)
	//
	// or
	//
	//   Setup(EBaudrate(9600),EDataBits(8),EParity(NOPARITY),EStopBits(ONESTOPBIT))
	//
	// In the latter case, the types are not validated. So make sure
	// that you specify the appropriate values.
	BOOL Setup (DWORD eBaudrate=CBR_9600, BYTE eDataBits=8, BYTE eParity=NOPARITY, BYTE eStopBits=ONESTOPBIT);


	// Set the event mask, which indicates what events should be
	// monitored. The WaitEvent method can only monitor events that
	// have been enabled. The default setting only monitors the
	// error events and data events. An application may choose to
	// monitor CTS. DSR, RLSD, etc as well.
	//BOOL SetMask (DWORD dwMask = EEventBreak|EEventError|EEventRecv);

	// The WaitEvent method waits for one of the events that are
	// enabled (see SetMask).
	//BOOL WaitEvent ( DWORD & e);

	// Setup the handshaking protocol. There are three forms of
	// handshaking:
	//
	// 1) No handshaking, so data is always send even if the receiver
	//    cannot handle the data anymore. This can lead to data loss,
	//    when the sender is able to transmit data faster then the
	//    receiver can handle.
	// 2) Hardware handshaking, where the RTS/CTS lines are used to
	//    indicate if data can be sent. This mode requires that both
	//    ports and the cable support hardware handshaking. Hardware
	//    handshaking is the most reliable and efficient form of
	//    handshaking available, but is hardware dependant.
	// 3) Software handshaking, where the XON/XOFF characters are used
	//    to throttle the data. A major drawback of this method is that
	//    these characters cannot be used for data anymore.
	BOOL SetupHandshaking (EHandshake eHandshake);

	// Read operations can be blocking or non-blocking. You can use
	// this method to setup wether to use blocking or non-blocking
	// reads. Non-blocking reads is the default, which is required
	// for most applications.
	//
	// 1) Blocking reads, which will cause the 'Read' method to block
	//    until the requested number of bytes have been read. This is
	//    useful if you know how many data you will receive.
	// 2) Non-blocking reads, which will read as many bytes into your
	//    buffer and returns almost immediately. This is often the
	//    preferred setting.
	BOOL SetupReadTimeouts (EReadTimeout eReadTimeout);

	// Obtain communication settings
	BOOL GetCommStat(int& baudrate,int & databits,int & parity,int& stopsbits );

	EHandshake GetHandshaking (void);
	
	// Write data to the serial port. Note that we are only able to
	// send ANSI strings, because it probably doesn't make sense to
	// transmit Unicode strings to an application.
	BOOL Write (const void* pData, int iLen);
	BOOL Write (const char * pString);

	// Read data from the serial port. Refer to the description of
	// the 'SetupReadTimeouts' for an explanation about (non) blocking
	// reads and how to use this.
	BOOL Read (void* pData, int iLen, int * pdwRead=0 );

	// Send a break
	BOOL Break (void);

	// Determine what caused the event to trigger
	//EEvent GetEventType (void);

	// Obtain the error
	//EError GetError (void);
	
	// Obtain the COMM and event handle
	HANDLE GetCommHandle (void)		{ return m_hFile; }

	// Check if com-port is opened
	bool IsOpen (void) const		{ return (m_hFile != 0); }

	// Obtain last error status
#ifdef WIN32
	LONG GetLastError (void) const	{ return ::GetLastError(); }
#else 
	LONG GetLastError (void) const	{ return errno; }
#endif
	// Obtain CTS/DSR/RING/RLSD settings
	//BOOL GetModemStatus(bool& CTS,bool & DSR,bool& Ring,bool & RLSD);
	// Purge all buffers
	BOOL Purge ();

// Attributes
	const char * Name() const { return mName; };
	const char * Dev() const { return mDevName; };
	const char * User() const { return mUser[0]==0?NULL:mUser; };
	void SetName(const char * p);
	BOOL  SetUser(const char * p);
	static void  DumpHead(char * buffer);
	virtual void Dump(int index,char * buffer);
	long GetTimeSnap();
protected:
	HANDLE	m_hFile;			// File handle
	char mName[32];
	char mDevName[64];
	char mUser[64];
	DECLARE_TIME_INTERVAL(mLastTime);
};

#endif	// __SERIAL_H
