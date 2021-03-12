// ==============================
// File:			TTcpClientSerialPortManager.cp
// Project:			Einstein
//
// Copyright 2020 by Matthias Melcher (mm@matthiasm.com).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#include "TTcpClientSerialPortManager.h"
#include "app/TPathHelper.h"

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#include <chrono>

#if TARGET_OS_WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#endif

#include "Emulator/Log/TLog.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"


/*
 The TCP Client class emulates a serial port by connecting to the server
 whenever needed through a TCP network connection.

 The actual client runs in a thread and has to watch two file descriptor.
 The first descriptor is the end of a pipe to exchange commend with the main
 thread. The second connection is the actual socket.

 The pipe is created by the main thread and stays alive with the helper
 thread from a call to run() until the class is destroyed.

 The socket is not created until NewtonOS decides to touch the serial port. The
 socket will not disconnect from the client side, but must handle a
 disconnection from the server gracefully. We can consider a timeout for a
 client disconnect if there is no more traffic on the serial port as NewtonOS
 does send a keep-alive message regularly if the connection should be kept.

 The class can be destroyed if the user changes settings in the preferences
 panel.
 */


// -------------------------------------------------------------------------- //
//  * TTcpClientSerialPortManager()
// Emulate a serial connection using a TCP client socket.
// -------------------------------------------------------------------------- //
TTcpClientSerialPortManager::TTcpClientSerialPortManager(
													 TLog* inLog,
													 TSerialPorts::EPortIndex inPortIx)
:	TBasicSerialPortManager(inLog, inPortIx)
{
#if TARGET_OS_WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif

	mServer = strdup("127.0.0.1");
//	mServer = strdup("192.168.178.63"); // testing connections over Wifi
	mPort = 3679;
}


// -------------------------------------------------------------------------- //
//  * ~TTcpClientSerialPortManager( void )
// -------------------------------------------------------------------------- //
TTcpClientSerialPortManager::~TTcpClientSerialPortManager()
{
    if (mWorkerThread) {
		Disconnect();
		TriggerEvent('q');
        mWorkerThread->join();
        delete mWorkerThread;
        mWorkerThread = nullptr;
	}
#if TARGET_OS_WIN32
	if (mTcpEvent != INVALID_HANDLE_VALUE)
		WSACloseEvent(mTcpEvent);
	if (mQuitEvent != INVALID_HANDLE_VALUE)
		WSACloseEvent(mQuitEvent);
	if (mOtherEvent != INVALID_HANDLE_VALUE)
		WSACloseEvent(mOtherEvent);
	WSACleanup();
#else
	if (mCommandPipe[0]!=-1)
		::close(mCommandPipe[0]);
	if (mCommandPipe[1]!=-1)
		::close(mCommandPipe[1]);
#endif
	if (mServer)
		free(mServer);
}

void TTcpClientSerialPortManager::LogError(const char* text, bool systemError)
{
	if (systemError) {
		int errorId = 0;
		const char* errorText = "undocumented";
#if TARGET_OS_WIN32
		auto res = WSAGetLastError();
		char buf[1024];
		buf[0] = 0;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, res, 0, buf, 1024, nullptr);
		if (buf[0]) {
			errorText = buf;
			int n = strlen(buf);
			if ((n > 0) && (buf[n - 1] == '\n')) buf[n - 1] = 0;
			if ((n > 1) && (buf[n - 2] == '\r')) buf[n - 2] = 0;
		}
		errorId = res;
#else
		const char* e = strerror(errno);
		if (e) errorText = e;
		errorId = errno;
#endif
		if (mLog)
			mLog->FLogLine("TTcpClientSerialPortManager::%s: %s (%d)\n", text, errorText, errorId);
		else
			KPrintf("TTcpClientSerialPortManager::%s: %s (%d)\n", text, errorText, errorId);
	}
	else {
		if (mLog)
			mLog->FLogLine("TTcpClientSerialPortManager::%s\n", text);
		else
			KPrintf("TTcpClientSerialPortManager::%s\n", text);
	}
}

// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void TTcpClientSerialPortManager::run(TInterruptManager* inInterruptManager,
								  TDMAManager* inDMAManager,
								  TMemory* inMemory)
{
	mInterruptManager = inInterruptManager;
	mDMAManager = inDMAManager;
	mMemory = inMemory;

	if (mWorkerThread) {
		LogError("run: worker thread is already running.");
		return;
	}

#if TARGET_OS_WIN32
	if (mQuitEvent!=INVALID_HANDLE_VALUE) {
		LogError("run: trying to create the Quit Event again.");
		return;
	}

	// create the thread communication pipe
	mQuitEvent = WSACreateEvent();
	WSAResetEvent(mQuitEvent);
	mOtherEvent = WSACreateEvent();
	WSAResetEvent(mOtherEvent);
#else
	if (mCommandPipe[0]!=-1 || mCommandPipe[1]!=-1) {
		LogError("run: trying to open command pipe again.");
		return;
	}

	// create the thread communication pipe
	int err = pipe(mCommandPipe);
	if (err==-1) {
		LogError("run: Error opening pipe", true);
		return;
	}
#endif

	// create the thread and let it run until we send it the quit signal
    mWorkerThread = new std::thread(&TTcpClientSerialPortManager::HandleDMA, this);
	if (!mWorkerThread) {
		LogError("run: error creating std::thread", true);
		return;
	}
}

// -------------------------------------------------------------------------- //
// DMA or interrupts trigger a command that must be handled by a derived class.
// -------------------------------------------------------------------------- //
void TTcpClientSerialPortManager::TriggerEvent(KUInt8 cmd)
{
#if TARGET_OS_WIN32
	if (mQuitEvent==INVALID_HANDLE_VALUE) {
		LogError("TriggerEvent: called without initialization");
		return;
	}
	switch (cmd) {
		case 'q': WSASetEvent(mQuitEvent); break;
		default:
			WSASetEvent(mOtherEvent);
			break;
	}
#else
	if (mCommandPipe[0]==-1 || mCommandPipe[1]==-1) {
		LogError("TriggerEvent: called without pipes.");
		return;
	}
	::write(mCommandPipe[1], &cmd, 1);
#endif
}


// -------------------------------------------------------------------------- //
// * Disconnect
//		Disconnect from the server.
// -------------------------------------------------------------------------- //
bool
TTcpClientSerialPortManager::Disconnect()
{
	bool wasConnected = mIsConnected;
	if (mTcpSocket!=-1) {
		time_t now;
		time(&now);
		mReconnectTimeout = now+3;
#if TARGET_OS_WIN32
		closesocket(mTcpSocket);
		WSACloseEvent(mTcpEvent);
		mTcpEvent = INVALID_HANDLE_VALUE;
#else
		::close(mTcpSocket);
#endif
		mTcpSocket = -1;
	}
	mIsConnected = false;
	return wasConnected;
}


// -------------------------------------------------------------------------- //
// * Connect
//		Create a TCP socket and try to connect to the server.
// -------------------------------------------------------------------------- //
bool
TTcpClientSerialPortManager::Connect()
{
	Disconnect();

	// reject reconnect request if we have not hit the timeout yet
	time_t now;
	time(&now);
	if (now < mReconnectTimeout)
		return false;

	// Create a new socket
#if TARGET_OS_WIN32
	mTcpSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (mTcpSocket == INVALID_SOCKET)
	{
		LogError("Connect: can't create TCP/IP socket", true);
		return false;
	}
#else
	mTcpSocket = ::socket(PF_INET, SOCK_STREAM, 0);
	if (mTcpSocket==-1) {
		LogError("Connect: error creating socket", true);
		return false;
	}
#endif

	// Create the address information to our server
	struct sockaddr_in server{};
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(static_cast<uint16_t>(mPort));

	if (inet_pton(AF_INET, mServer, &server.sin_addr)<=0)
	{
		LogError("Connect: Error in inet_pton", true);
		Disconnect();
		return false;
	}

#if TARGET_OS_WIN32
	int res = WSAConnect(mTcpSocket, (struct sockaddr*) & server, sizeof(server), NULL, NULL, NULL, NULL);
	if (res == SOCKET_ERROR)
	{
		LogError("Connect: can't connect to TCP/IP server", true);
		Disconnect();
		return false;
	}
	mTcpEvent = WSACreateEvent();
	WSAEventSelect(mTcpSocket, mTcpEvent, FD_READ|FD_CLOSE);
#else
	// Try to connect to the server
	int err = ::connect(mTcpSocket, (struct sockaddr *)&server, sizeof(server));
	if (err==-1) {
		LogError("Connect: can't connect to server", true);
		Disconnect();
		return false;
	}
#endif

	mIsConnected = true;
	TriggerEvent('c');

	return true;
}



static void sigpipe_handler(int unused)
{
	// don't do anything. The server disconnected faster than the client would realize that.
}


// -------------------------------------------------------------------------- //
//  * HandleDMA()
//		This endless loop watches DMA registers as they are changed by the
//		OS, and read and writes data via the TCP socket.
//		It can also trigger interrupts when buffers empty, fill, or overflow.
// -------------------------------------------------------------------------- //
void
TTcpClientSerialPortManager::HandleDMA()
{
#if TARGET_OS_WIN32
	// thread loops and handles pipe, port, and DMA
	DWORD timeout = 0;
	for (;;)
	{
		bool needTimer = false;

		if (mTxDMAControl & 0x00000002) { // DMA is enabled
			if (mTxDMADataCountdown) {
				needTimer = true;
				timeout = 1;
			}
		}

		// if there is still (or already) data in the receive buffer, just trigger the message
		//  - this should happen automatically, but we had situation where it didn't, so just make sure
		if (mTcpEvent != INVALID_HANDLE_VALUE) {
			u_long nAvailable = 0;
			ioctlsocket(mTcpSocket, FIONREAD, &nAvailable);
			if (nAvailable>0) WSASetEvent(mTcpEvent);
		}

		int nEvent = (mTcpEvent == INVALID_HANDLE_VALUE) ? 2 : 3;
		HANDLE watchFDs[] = { mQuitEvent, mOtherEvent, mTcpEvent  };
		DWORD ret = WSAWaitForMultipleEvents(nEvent, watchFDs, FALSE, needTimer ? timeout : WSA_INFINITE, FALSE);
		if (ret == WSA_WAIT_FAILED) {
			LogError("HandleDMA: TCP/IP scoket wait failed", true);
			continue;
		}

		// handle receiving DMA
		if (ret == WSA_WAIT_EVENT_0+2) {
			if (IsConnected()) {
				HandleDMAReceive();
			}
			WSAResetEvent(mTcpEvent);
		}

		// handle transmitting DMA
		HandleDMASend(); // timer resolution is too coarse, so send up to 4 bytes now
		HandleDMASend();
		HandleDMASend();
		HandleDMASend();

		// handle commands from mQuitEvent 
		if (ret == WSA_WAIT_EVENT_0) {
			Disconnect();
			WSAResetEvent(mQuitEvent);
			break;
		}
		if (ret == WSA_WAIT_EVENT_0+1) {
			WSAResetEvent(mOtherEvent);
			// just break the wait
		}
	}
#else
    static struct sigaction action { {sigpipe_handler} };
	sigaction(SIGPIPE, &action, nullptr);

	// thread loops and handles pipe, port, and DMA
	struct timeval timeout{};
	for (;;)
	{
		bool needTimer = false;
		fd_set watchFDs;

		FD_ZERO(&watchFDs);
		FD_SET(mCommandPipe[0], &watchFDs);
		if (IsConnected())
			FD_SET(mTcpSocket, &watchFDs);

		if (mTxDMAControl&0x00000002) { // DMA is enabled
			if (mTxDMADataCountdown) {
				needTimer = true;
				timeout.tv_sec = 0;
				timeout.tv_usec = 260; // one byte at 38400bps serial port speed
			}
		}

		int ret = select(FD_SETSIZE, &watchFDs, 0L, 0L, needTimer ? &timeout : 0L);
		if (ret==-1) {
			LogError("HandleDMA: error waiting for sockets", true);
			continue;
		}
		
		// handle receiving DMA
		if (IsConnected() && FD_ISSET(mTcpSocket, &watchFDs))
			HandleDMAReceive();

		// handle transmitting DMA
		HandleDMASend();

		// handle commands from the command pipe
		if (FD_ISSET(mCommandPipe[0], &watchFDs)) {
			KUInt8 cmd = 'e';
			::read(mCommandPipe[0], &cmd, 1);
			switch (cmd) {
				case 'q':	// quit this thread
					Disconnect();
					return;
				case 'c':
					// connect, continue, cycle, just any event but quit
					break;
				default:
					LogError("HandleDMA: Error reading pipe", true);
					break;
			}
		}

	}
#endif
}


// -------------------------------------------------------------------------- //
//  * HandleDMASend()
//		Send a single byte. We are throtteling data transfor to match a
//		maximum of 38400bps which is the maximum that NewtonSO can handle.
// -------------------------------------------------------------------------- //
void
TTcpClientSerialPortManager::HandleDMASend()
{
	if (mTxDMAControl&0x00000002) { // DMA is enabled
		if (mTxDMADataCountdown) {
			// write a byte
			KUInt8 data = 0;
			mMemory->ReadBP(mTxDMAPhysicalData, data);
			if (!IsConnected())
				Connect();
			// TODO: if we can't get a connection at this point, should we flush the entire buffer?
			if (IsConnected()) {
#if TARGET_OS_WIN32
				WSABUF buf = { 1, (CHAR*)&data };
				DWORD nSent = 0;
				if (WSASend(mTcpSocket, &buf, 1, &nSent, 0, nullptr, nullptr) != 0) {
					// FIXME: WSAGetLastError()==10053: connection aborted by host
					LogError("HandleDMASend: Can't write to TCP/IP socket!", true);
				}
#else
				::write(mTcpSocket, &data, 1);
#endif
				//KPrintf("Sending 0x%02x\n", data);
			} else {
				//KPrintf("Sending to null 0x%02x\n", data);
			}
			mTxDMAPhysicalData++;
			mTxDMABufferSize--;
			if (mTxDMABufferSize==0) {
				mTxDMAPhysicalData = mTxDMAPhysicalBufferStart;
			}
			mTxDMADataCountdown--;
			if (mTxDMADataCountdown==0) {
				// trigger a "send buffer empty" interrupt
				mTxDMAEvent = 0x00000080;
				mInterruptManager->RaiseInterrupt(0x00000100);
			}
		}
	}
}


// -------------------------------------------------------------------------- //
//  * HandleDMAReceive()
// -------------------------------------------------------------------------- //
void
TTcpClientSerialPortManager::HandleDMAReceive()
{
	// The original Newton hardware and OS
	// did have timing issues whan the server PC communicated faster as
	// expected in the year 1996, which leaconnecting to d to CPU cycle burning software
	// like "slowdown.exe".

	// read up to 1024 bytes that come in through the serial port
	KUInt8 buf[1026];
#if TARGET_OS_WIN32
	DWORD nRequested = 32;
	DWORD nReceived = 0;
	DWORD flags = 0;
	WSABUF abuf = { nRequested, (CHAR*)buf };
	// FIXME: WSABUF abuf = { 1024, (CHAR*)buf };
	int ret = WSARecv(mTcpSocket, &abuf, 1, &nReceived, &flags, nullptr, nullptr);
	int n = (ret == 0) ? nReceived : -1;
#else
	int n = (int)::read(mTcpSocket, buf, 1024);
#endif
	if (n==-1) {
		LogError("HandleDMAReceive: TCP/IP socket receive failed", true);
		Disconnect();
	} else if (n==0) {
		LogError("HandleDMAReceive: Server side disconnect.");
		Disconnect();
	} else {
		// delay up to 1/10th of a second, so that we do not overwhelm the Newton
		std::this_thread::sleep_for(std::chrono::microseconds(n*100));
		for (KUInt32 i=0; i<n; i++) {
			KUInt8 data = buf[i];
			//KPrintf("Received 0x%02x\n", data);
			mMemory->WriteBP(mRxDMAPhysicalData, data);
			mRxDMAPhysicalData++;
			mRxDMABufferSize--;
			if (mRxDMABufferSize==0) {
				mRxDMAPhysicalData = mRxDMAPhysicalBufferStart;
			}
			mRxDMADataCountdown--;
			if (mRxDMADataCountdown==0) {
				// buffer overflow?
				LogError("HandleDMAReceive: DMA Overflow on Receive?");
			}
		}
		mRxDMAEvent = 0x00000040;
		mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
	}
}

///
/// Give NewtonScript access to our list of options
///
void TTcpClientSerialPortManager::NSGetOptions(TNewt::RefArg frame)
{
	using namespace TNewt;
	char buf[32];
	snprintf(buf, 30, "%d", mPort);
	SetFrameSlot(frame, RefVar(MakeSymbol("tcpServer")), RefVar(MakeString(mServer)));
	SetFrameSlot(frame, RefVar(MakeSymbol("tcpPort")), RefVar(MakeString(buf)));
}

///
/// Set options from NewtonScript
///
void TTcpClientSerialPortManager::NSSetOptions(TNewt::RefArg inFrame)
{
	using namespace TNewt;
	char server[256] = { 0 };
	char portStr[80] = { 0 };
	KSInt32 port = 0;
	bool setServer = false;
	bool setPort = false;
	bool mustReconnect = false;

	NewtRef frame = inFrame.Ref();
	NewtRef tcpServerRef = GetFrameSlotRef(frame, MakeSymbol("tcpServer"));
	if (RefIsString(tcpServerRef)) {
		RefToString(tcpServerRef, server, sizeof(server));
		setServer = true;
	}
	NewtRef tcpPortRef = GetFrameSlot(frame, MakeSymbol("tcpPort"));
	if (RefIsString(tcpPortRef)) {
		RefToString(tcpPortRef, portStr, (int)sizeof(portStr));
		port = atoi(portStr);
		if (port==0) port = 3679;
		setPort = true;
	}

	//KPrintf("INFO: TTcpClientSerialPortManager::NSSetOptions: (\"%s\", %d)\n", mServer, mPort);
	if (setServer) {
		if (strcmp(mServer, server)!=0) {
			if (mServer) ::free(mServer);
			mServer = strdup(server);
			mustReconnect = true;
		}
		//KPrintf("INFO:             Setting server to \"%s\".\n", server);
	}
	if (setPort) {
		if (mPort!=port) {
			mPort = port;
			mustReconnect = true;
		}
		//KPrintf("INFO:             Setting port to %d.\n", port);
	}

	if (mustReconnect) {
		Disconnect(); // force the server to reconnect
		//KPrintf("INFO: TTcpClientSerialPortManager::NSSetOptions: must reconnect\n");
	}
}

void TTcpClientSerialPortManager::SetServerAddress(const char *inAddress)
{
	if (mServer) ::free(mServer);
	mServer = strdup(inAddress);
}

void TTcpClientSerialPortManager::SetServerPort(int inPort)
{
	mPort = inPort;
}

char *TTcpClientSerialPortManager::GetServerAddressDup()
{
	return strdup(mServer);
}

int TTcpClientSerialPortManager::GetServerPort()
{
	return mPort;
}


// ================================================================== //
// You never finish a program, you just stop working on it.           //
//  - Anonymous                                                       //
//                                                                    //
// So true!                                                           //
//  - Matthias Melcher                                                //
// ================================================================== //
