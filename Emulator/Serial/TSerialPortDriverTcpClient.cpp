// ==============================
// File:			TSerialPortDriverTcpClient.cp
// Project:			Einstein
//
// Copyright 2020-2025 by Matthias Melcher (mm@matthiasm.com).
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

// Throttle the emulated serial line: the time it takes to transfer one byte,
// in microseconds. 0 transfers as fast as the host can.
// 260 us is 38400 bps (ten bits per byte with start and stop bit), the
// fastest speed that NewtonOS supports.
static constexpr int kTxMicrosecondsPerByte = 90; // Newton to server
static constexpr int kRxMicrosecondsPerByte = 90; // server to Newton

#include "TSerialPortDriverTcpClient.h"
#include "app/TPathHelper.h"

// POSIX
#include <signal.h>
#include <string.h>
#include <sys/types.h>

#include <chrono>
#include <string>

#if TARGET_OS_WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#endif

#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"

// Largest number of bytes read from or written to the socket at once.
static constexpr KUInt32 kMaxChunkSize = 1024;

// Give up on a server that does not answer a connection attempt.
static constexpr std::chrono::seconds kConnectTimeout { 5 };

// Wait between connection attempts. Data that NewtonOS sends meanwhile is dropped.
static constexpr std::chrono::seconds kReconnectDelay { 3 };

/**
 \class TSerialPortDriverTcpClient
 \brief Emulate a serial port connection over a network as a TCP client.
 \author Matthias Melcher
 \note Implemented for macOS, Linux, and Windows, should work on iOS and Android.

 The TCP Client class emulates a serial port by connecting to the server
 whenever needed through a TCP network connection.

 The actual client runs in a thread and has to watch two file descriptor.
 The first descriptor is the end of a pipe to exchange commend with the main
 thread. The second connection is the actual socket.

 The pipe is created by the main thread and stays alive with the helper
 thread from a call to run() until the class is destroyed. It only wakes up
 the worker thread and never blocks the thread that writes to it. Requests
 that must not get lost (quit, reconnect) are atomic flags.

 The socket is not created until NewtonOS decides to touch the serial port. The
 socket will not disconnect from the client side, but must handle a
 disconnection from the server gracefully. We can consider a timeout for a
 client disconnect if there is no more traffic on the serial port as NewtonOS
 does send a keep-alive message regularly if the connection should be kept.

 The socket never blocks, not even while connecting, so the worker thread
 always answers a quit request quickly, and never waits for the network while
 it holds mDMAMutex. Only the worker thread uses the socket.

 Data is throttled to kTxMicrosecondsPerByte and kRxMicrosecondsPerByte, and
 sent and received in chunks of many bytes.

 The class can be destroyed if the user changes settings in the preferences
 panel.
 */

// -------------------------------------------------------------------------- //
//  * TSerialPortDriverTcpClient()
// Emulate a serial connection using a TCP client socket.
// -------------------------------------------------------------------------- //
TSerialPortDriverTcpClient::TSerialPortDriverTcpClient(
	TLog* inLog,
	TSerialPorts::EPortIndex inPortIx) :
		TBasicSerialPortManager(inLog, inPortIx),
		mTxThrottle(kTxMicrosecondsPerByte),
		mRxThrottle(kRxMicrosecondsPerByte)
{
#if TARGET_OS_WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
	mServer = strdup("127.0.0.1");
}

// -------------------------------------------------------------------------- //
//  * ~TSerialPortDriverTcpClient( void )
// -------------------------------------------------------------------------- //
TSerialPortDriverTcpClient::~TSerialPortDriverTcpClient()
{
	if (mWorkerThread)
	{
		// Tell the worker thread to disconnect and quit. Do not call
		// Disconnect() here: the worker may be using the socket right now.
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
	if (mCommandPipe[0] != -1)
		::close(mCommandPipe[0]);
	if (mCommandPipe[1] != -1)
		::close(mCommandPipe[1]);
#endif
	if (mServer)
		free(mServer);
}

void
TSerialPortDriverTcpClient::LogError(const char* text, bool systemError)
{
	if (systemError)
	{
		int errorId = 0;
		const char* errorText = "undocumented";
#if TARGET_OS_WIN32
		auto res = WSAGetLastError();
		char buf[1024];
		buf[0] = 0;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, res, 0, buf, 1024, nullptr);
		if (buf[0])
		{
			errorText = buf;
			int n = strlen(buf);
			if ((n > 0) && (buf[n - 1] == '\n'))
				buf[n - 1] = 0;
			if ((n > 1) && (buf[n - 2] == '\r'))
				buf[n - 2] = 0;
		}
		errorId = res;
#else
		const char* e = strerror(errno);
		if (e)
			errorText = e;
		errorId = errno;
#endif
		if (mLog)
			mLog->FLogLine("TSerialPortDriverTcpClient::%s: %s (%d)\n", text, errorText, errorId);
		else
			KPrintf("TSerialPortDriverTcpClient::%s: %s (%d)\n", text, errorText, errorId);
	} else
	{
		if (mLog)
			mLog->FLogLine("TSerialPortDriverTcpClient::%s\n", text);
		else
			KPrintf("TSerialPortDriverTcpClient::%s\n", text);
	}
}

// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void
TSerialPortDriverTcpClient::run(TInterruptManager* inInterruptManager,
	TDMAManager* inDMAManager,
	TMemory* inMemory)
{
	mInterruptManager = inInterruptManager;
	mDMAManager = inDMAManager;
	mMemory = inMemory;

	if (mWorkerThread)
	{
		LogError("run: worker thread is already running.");
		return;
	}

#if TARGET_OS_WIN32
	if (mQuitEvent != INVALID_HANDLE_VALUE)
	{
		LogError("run: trying to create the Quit Event again.");
		return;
	}

	// create the thread communication pipe
	mQuitEvent = WSACreateEvent();
	WSAResetEvent(mQuitEvent);
	mOtherEvent = WSACreateEvent();
	WSAResetEvent(mOtherEvent);
#else
	if (mCommandPipe[0] != -1 || mCommandPipe[1] != -1)
	{
		LogError("run: trying to open command pipe again.");
		return;
	}

	// create the thread communication pipe
	int err = pipe(mCommandPipe);
	if (err == -1)
	{
		LogError("run: Error opening pipe", true);
		return;
	}
	// Neither end blocks: the emulator thread must never wait for the worker
	// thread, and the worker thread empties the pipe without waiting.
	(void) fcntl(mCommandPipe[0], F_SETFL, fcntl(mCommandPipe[0], F_GETFL) | O_NONBLOCK);
	(void) fcntl(mCommandPipe[1], F_SETFL, fcntl(mCommandPipe[1], F_GETFL) | O_NONBLOCK);
#endif

	// create the thread and let it run until we send it the quit signal
	mWorkerThread = new std::thread(&TSerialPortDriverTcpClient::HandleDMA, this);
	if (!mWorkerThread)
	{
		LogError("run: error creating std::thread", true);
		return;
	}
}

// -------------------------------------------------------------------------- //
// DMA or interrupts trigger a command that must be handled by a derived class.
// 'q' quits the worker thread, any other command just wakes it up.
// -------------------------------------------------------------------------- //
void
TSerialPortDriverTcpClient::TriggerEvent(KUInt8 cmd)
{
	if (cmd == 'q')
		mQuitRequested = true;
#if TARGET_OS_WIN32
	if (mQuitEvent == INVALID_HANDLE_VALUE)
	{
		LogError("TriggerEvent: called without initialization");
		return;
	}
	switch (cmd)
	{
		case 'q':
			WSASetEvent(mQuitEvent);
			break;
		default:
			WSASetEvent(mOtherEvent);
			break;
	}
#else
	if (mCommandPipe[0] == -1 || mCommandPipe[1] == -1)
	{
		LogError("TriggerEvent: called without pipes.");
		return;
	}
	// If the pipe is full, the worker thread will wake up anyway.
	(void) ::write(mCommandPipe[1], &cmd, 1);
#endif
}

// -------------------------------------------------------------------------- //
// * Disconnect
//		Disconnect from the server, or stop a connection attempt.
// -------------------------------------------------------------------------- //
bool
TSerialPortDriverTcpClient::Disconnect()
{
	bool wasConnected = mIsConnected;
#if TARGET_OS_WIN32
	if (mTcpSocket != INVALID_SOCKET)
	{
		closesocket(mTcpSocket);
		mTcpSocket = INVALID_SOCKET;
		mReconnectTime = Clock::now() + kReconnectDelay;
	}
	if (mTcpEvent != INVALID_HANDLE_VALUE)
	{
		WSACloseEvent(mTcpEvent);
		mTcpEvent = INVALID_HANDLE_VALUE;
	}
	mCanReceive = false;
#else
	if (mTcpSocket != -1)
	{
		::close(mTcpSocket);
		mTcpSocket = -1;
		mReconnectTime = Clock::now() + kReconnectDelay;
	}
#endif
	mIsConnected = false;
	mIsConnecting = false;
	mSendBlocked = false;
	return wasConnected;
}

// -------------------------------------------------------------------------- //
// * Connect
//		Create a TCP socket and start connecting to the server. The socket
//		does not block. FinishConnect() handles the result.
// -------------------------------------------------------------------------- //
bool
TSerialPortDriverTcpClient::Connect()
{
	Disconnect();

	// reject reconnect request if we have not hit the timeout yet
	Clock::time_point now = Clock::now();
	if (now < mReconnectTime)
		return false;
	mReconnectTime = now + kReconnectDelay;

	// The UI and the emulator thread may change the server at any time.
	std::string serverAddress;
	int serverPort;
	{
		std::lock_guard<std::mutex> lock(mServerMutex);
		serverAddress = mServer ? mServer : "";
		serverPort = mPort;
	}

	// Create the address information to our server
	struct sockaddr_in server { };
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(static_cast<uint16_t>(serverPort));

	if (inet_pton(AF_INET, serverAddress.c_str(), &server.sin_addr) <= 0)
	{
		LogError("Connect: Error in inet_pton", true);
		return false;
	}

	// Create a new socket
#if TARGET_OS_WIN32
	mTcpSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (mTcpSocket == INVALID_SOCKET)
	{
		LogError("Connect: can't create TCP/IP socket", true);
		return false;
	}
	mTcpEvent = WSACreateEvent();
	if (mTcpEvent == WSA_INVALID_EVENT)
	{
		mTcpEvent = INVALID_HANDLE_VALUE;
		LogError("Connect: can't create TCP/IP event", true);
		Disconnect();
		return false;
	}
	// Report the connection, data, free buffer space, and a disconnect.
	// This also makes the socket non-blocking.
	WSAEventSelect(mTcpSocket, mTcpEvent, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);

	int res = ::connect(mTcpSocket, (struct sockaddr*) &server, sizeof(server));
	if (res == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		LogError("Connect: can't connect to TCP/IP server", true);
		Disconnect();
		return false;
	}
#else
	mTcpSocket = ::socket(PF_INET, SOCK_STREAM, 0);
	if (mTcpSocket == -1)
	{
		LogError("Connect: error creating socket", true);
		return false;
	}
	// The worker sends while it holds mDMAMutex, so a send must never wait for
	// the server to read, and it must not wait for a slow connect either.
	(void) fcntl(mTcpSocket, F_SETFL, fcntl(mTcpSocket, F_GETFL) | O_NONBLOCK);

	int res = ::connect(mTcpSocket, (struct sockaddr*) &server, sizeof(server));
	if (res == -1 && errno != EINPROGRESS)
	{
		LogError("Connect: can't connect to server", true);
		Disconnect();
		return false;
	}
#endif

	if (res == 0)
	{
		mIsConnected = true;
	} else
	{
		mIsConnecting = true;
		mConnectDeadline = now + kConnectTimeout;
	}
	return true;
}

// -------------------------------------------------------------------------- //
// * FinishConnect
//		inDone is true when the socket reported the end of the connection
//		attempt, with inError set to a system error code if it failed.
//		Otherwise, give up after kConnectTimeout.
// -------------------------------------------------------------------------- //
void
TSerialPortDriverTcpClient::FinishConnect(bool inDone, int inError)
{
	if (!inDone)
	{
		if (Clock::now() >= mConnectDeadline)
		{
			LogError("Connect: the server does not answer");
			Disconnect();
		}
		return;
	}
	if (inError != 0)
	{
#if TARGET_OS_WIN32
		WSASetLastError(inError);
#else
		errno = inError;
#endif
		LogError("Connect: can't connect to server", true);
		Disconnect();
		return;
	}
	mIsConnecting = false;
	mIsConnected = true;
}

static void
sigpipe_handler(int unused)
{
	(void) unused;
	// don't do anything. The server disconnected faster than the client would realize that.
}

/**
 \brief Worker thread: Emulate the DMA hardware.

 This endless loop watches DMA registers as they are changed by the OS, and
 read and writes data via the TCP socket. It can also trigger interrupts when
 buffers empty, fill, or overflow.

 The loop waits for a command, the socket, or the throttle, whatever comes
 first. It never waits while it holds mDMAMutex.

 Disconnect, stop the loop, and exit the thread by calling TriggerEvent('q').
 */
void
TSerialPortDriverTcpClient::HandleDMA()
{
#if TARGET_OS_WIN32
	for (;;)
	{
		// Decide how long to wait. mTcpEvent reports the connection, new
		// data, and free buffer space after a blocked send.
		long long timeout = -1; // microseconds, -1 waits for an event
		auto waitAtMost = [&timeout](long long us) {
			if (timeout < 0 || us < timeout)
				timeout = us;
		};
		if (mIsConnecting)
		{
			waitAtMost(MicrosecondsUntil(mConnectDeadline));
		} else
		{
			KUInt32 pending = PendingTxBytes();
			if (pending && !(mIsConnected && mSendBlocked))
				waitAtMost(mTxThrottle.Delay(pending));
			// (without room in receive DMA, NewtonOS wakes us when there is)
			if (mIsConnected && mCanReceive && RxDMARoom() > 0)
				waitAtMost(mRxThrottle.Delay(1));
		}

		HANDLE watchEvents[] = { mQuitEvent, mOtherEvent, mTcpEvent };
		DWORD nEvents = (mTcpEvent == INVALID_HANDLE_VALUE) ? 2 : 3;
		DWORD ms = (timeout < 0) ? WSA_INFINITE : (DWORD) ((timeout + 999) / 1000);
		DWORD ret = WSAWaitForMultipleEvents(nEvents, watchEvents, FALSE, ms, FALSE);
		if (ret == WSA_WAIT_FAILED)
		{
			LogError("HandleDMA: TCP/IP socket wait failed", true);
			continue;
		}

		// Commands from other threads. The event only wakes us up, the flags
		// tell us what to do. Reset it first, so no new command gets lost.
		WSAResetEvent(mOtherEvent);
		if (mQuitRequested)
		{
			Disconnect();
			break;
		}
		if (mReconnectRequested.exchange(false))
		{
			Disconnect();
			mReconnectTime = {}; // connect to the new server right away
		}

		// Collect the network events, which also resets mTcpEvent. FD_READ is
		// reported again after the next recv() if there is more data.
		bool connectDone = false;
		int connectError = 0;
		if (mTcpEvent != INVALID_HANDLE_VALUE)
		{
			WSANETWORKEVENTS networkEvents {};
			if (WSAEnumNetworkEvents(mTcpSocket, mTcpEvent, &networkEvents) == 0)
			{
				if (networkEvents.lNetworkEvents & FD_CONNECT)
				{
					connectDone = true;
					connectError = networkEvents.iErrorCode[FD_CONNECT_BIT];
				}
				if (networkEvents.lNetworkEvents & (FD_READ | FD_CLOSE))
					mCanReceive = true;
				if (networkEvents.lNetworkEvents & FD_WRITE)
					mSendBlocked = false;
			}
		}

		// Connect when NewtonOS has data to send.
		if (mIsConnecting)
			FinishConnect(connectDone, connectError);
		else if (!IsConnected() && PendingTxBytes())
			Connect();

		if (IsConnected() && mCanReceive)
			HandleDMAReceive();
		HandleDMASend();
	}
	WSAResetEvent(mQuitEvent);
#else
	static struct sigaction action { };
	action.sa_handler = sigpipe_handler;
	sigaction(SIGPIPE, &action, nullptr);

	for (;;)
	{
		// Decide what to wait for, and how long.
		long long timeout = -1; // microseconds, -1 waits for an event
		auto waitAtMost = [&timeout](long long us) {
			if (timeout < 0 || us < timeout)
				timeout = us;
		};
		int tcpSocket = mTcpSocket;
		bool watchRead = false;
		bool watchWrite = false;
		if (mIsConnecting)
		{
			// the socket becomes writable when connect() has finished
			watchWrite = true;
			waitAtMost(MicrosecondsUntil(mConnectDeadline));
		} else
		{
			KUInt32 pending = PendingTxBytes();
			if (IsConnected() && mSendBlocked)
				watchWrite = true;
			else if (pending)
				waitAtMost(mTxThrottle.Delay(pending));
			// Leave new data in the socket until the throttle allows it, and
			// while receive DMA has no room (NewtonOS wakes us when it has).
			if (IsConnected() && RxDMARoom() > 0)
			{
				long long rxDelay = mRxThrottle.Delay(1);
				if (rxDelay == 0)
					watchRead = true;
				else
					waitAtMost(rxDelay);
			}
		}

		fd_set readFDs;
		fd_set writeFDs;
		FD_ZERO(&readFDs);
		FD_ZERO(&writeFDs);
		FD_SET(mCommandPipe[0], &readFDs);
		int maxFD = mCommandPipe[0];
		if (watchRead)
			FD_SET(tcpSocket, &readFDs);
		if (watchWrite)
			FD_SET(tcpSocket, &writeFDs);
		if ((watchRead || watchWrite) && tcpSocket > maxFD)
			maxFD = tcpSocket;

		struct timeval tv { };
		if (timeout >= 0)
		{
			tv.tv_sec = (time_t) (timeout / 1000000);
			tv.tv_usec = (suseconds_t) (timeout % 1000000);
		}
		int ret = select(maxFD + 1, &readFDs, &writeFDs, nullptr, (timeout >= 0) ? &tv : nullptr);
		if (ret == -1)
		{
			if (errno != EINTR)
				LogError("HandleDMA: error waiting for sockets", true);
			continue;
		}
		bool canRead = watchRead && FD_ISSET(tcpSocket, &readFDs);
		bool canWrite = watchWrite && FD_ISSET(tcpSocket, &writeFDs);

		// Commands from other threads. The pipe only wakes us up, the flags
		// tell us what to do. Empty the pipe first, so no new command gets lost.
		if (FD_ISSET(mCommandPipe[0], &readFDs))
		{
			KUInt8 commands[64];
			while (::read(mCommandPipe[0], commands, sizeof(commands)) > 0)
			{
			}
		}
		if (mQuitRequested)
		{
			Disconnect();
			return;
		}
		if (mReconnectRequested.exchange(false))
		{
			Disconnect();
			mReconnectTime = {}; // connect to the new server right away
			canRead = false;
			canWrite = false;
		}

		// Connect when NewtonOS has data to send.
		if (mIsConnecting)
		{
			int error = 0;
			if (canWrite)
			{
				socklen_t len = sizeof(error);
				if (getsockopt(mTcpSocket, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
					error = errno;
			}
			FinishConnect(canWrite, error);
		} else if (!IsConnected() && PendingTxBytes())
		{
			Connect();
		} else if (canWrite)
		{
			mSendBlocked = false;
		}

		if (IsConnected() && canRead)
			HandleDMAReceive();
		HandleDMASend();
	}
#endif
}

/**
 \brief Worker thread: Send data pending in memory to the server.

 Send as many bytes as the throttle allows in one packet. Without a connection
 to the server, the bytes are dropped at the same rate.
 */
void
TSerialPortDriverTcpClient::HandleDMASend()
{
	// Wait until the connection is made, or the socket can take more data.
	if (mIsConnecting || (IsConnected() && mSendBlocked))
		return;

	// Suspend() holds this mutex while the emulator state is saved or loaded
	std::lock_guard<std::mutex> lock(mDMAMutex);

	if (!(mTxDMAIntEnable & 0x00000002) || !mTxDMADataCountdown)
		return; // DMA is disabled or has nothing to send
	KUInt32 n = mTxThrottle.Allowance((mTxDMADataCountdown < kMaxChunkSize) ? mTxDMADataCountdown : kMaxChunkSize);
	if (n == 0)
		return;

	// Collect the bytes from the circular DMA buffer.
	KUInt8 buf[kMaxChunkSize];
	KUInt32 address = mTxDMAPhysicalData;
	KUInt32 bytesToBufferEnd = mTxDMABytesToBufferEnd;
	for (KUInt32 i = 0; i < n; i++)
	{
		mMemory->ReadBP(address, buf[i]);
		address++;
		if (--bytesToBufferEnd == 0)
			address = mTxDMAPhysicalBufferStart;
	}

	KUInt32 nSent = n; // without a connection, the bytes are dropped
	if (IsConnected())
	{
		// The socket does not block. If it takes only some of the bytes, the
		// rest is sent in the next round. If it takes none, wait until it can.
#if TARGET_OS_WIN32
		WSABUF abuf = { n, (CHAR*) buf };
		DWORD nWritten = 0;
		if (WSASend(mTcpSocket, &abuf, 1, &nWritten, 0, nullptr, nullptr) == 0)
		{
			nSent = (KUInt32) nWritten;
		} else if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			mSendBlocked = true; // FD_WRITE tells us when it can take more
			return;
		} else
		{
			LogError("HandleDMASend: Can't write to TCP/IP socket", true);
			Disconnect();
		}
#else
		ssize_t nWritten = ::write(mTcpSocket, buf, n);
		if (nWritten >= 0)
		{
			nSent = (KUInt32) nWritten;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			mSendBlocked = true; // select() tells us when it can take more
			return;
		} else if (errno == EINTR)
		{
			return;
		} else
		{
			LogError("HandleDMASend: Can't write to TCP/IP socket", true);
			Disconnect();
		}
#endif
	}
	mTxThrottle.Consume(nSent);

	for (KUInt32 i = 0; i < nSent; i++)
	{
		mTxDMAPhysicalData++;
		if (--mTxDMABytesToBufferEnd == 0)
			mTxDMAPhysicalData = mTxDMAPhysicalBufferStart;
	}
	mTxDMADataCountdown -= nSent;
	if (mTxDMADataCountdown == 0)
	{
		// trigger a "send buffer empty" interrupt
		mTxDMAEvent = 0x00000080;
		mInterruptManager->RaiseInterrupt(0x00000100);
	}
}

/**
 \brief Worker thread: Receive data from the server.

 The worker thread was notified of an event from the receiving socket.
 If the server closed the connection, close if from the client side as well.
 If the server sent data, copy as many bytes as the throttle allows to the
 Newton DMA buffer and notify NewtonOS by raising an interrupt. The rest
 stays in the socket until the next round.
 */
void
TSerialPortDriverTcpClient::HandleDMAReceive()
{
	// no more than receive DMA can take; the rest waits at the host's end
	KUInt32 room = RxDMARoom();
	if (room == 0)
		return;
	KUInt32 nAllowed = mRxThrottle.Allowance((room < kMaxChunkSize) ? room : kMaxChunkSize);
	if (nAllowed == 0)
		return;

	KUInt8 buf[kMaxChunkSize];
#if TARGET_OS_WIN32
	DWORD nReceived = 0;
	DWORD flags = 0;
	WSABUF abuf = { nAllowed, (CHAR*) buf };
	int n = -1;
	if (WSARecv(mTcpSocket, &abuf, 1, &nReceived, &flags, nullptr, nullptr) == 0)
	{
		n = (int) nReceived;
	} else if (WSAGetLastError() == WSAEWOULDBLOCK)
	{
		mCanReceive = false; // wait for the next FD_READ
		return;
	}
#else
	int n = (int) ::read(mTcpSocket, buf, nAllowed);
	if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		return; // nothing to read after all (the socket does not block)
#endif
	if (n == -1)
	{
		LogError("HandleDMAReceive: TCP/IP socket receive failed", true);
		Disconnect();
		return;
	}
	if (n == 0)
	{
		LogError("HandleDMAReceive: Server side disconnect.");
		Disconnect();
		return;
	}
	mRxThrottle.Consume((KUInt32) n);

	// Suspend() holds this mutex while the emulator state is saved or loaded
	std::lock_guard<std::mutex> lock(mDMAMutex);
	for (int i = 0; i < n; i++)
	{
		KUInt8 data = buf[i];
		// KPrintf("Received 0x%02x\n", data);
		mMemory->WriteBP(mRxDMAPhysicalData, data);
		mRxDMAPhysicalData++;
		mRxDMABytesToBufferEnd--;
		if (mRxDMABytesToBufferEnd == 0)
		{
			mRxDMAPhysicalData = mRxDMAPhysicalBufferStart;
		}
		mRxDMADataCountdown--;
		if (mRxDMADataCountdown == 0)
		{
			// buffer overflow?
			LogError("HandleDMAReceive: DMA Overflow on Receive?");
		}
	}
	mRxDMAEvent = 0x00000040;
	mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
}

/**
 \brief Give NewtonScript access to our list of options.

 Add slots to the frame that contain options that are specific to this driver.

 \param[inout] frame Call with an empty frame. On return, the frame will
 have two new entries, tcpServer: "address" and tcpPort: "port". Port is
 a string containing the decimal port number.
 */
void
TSerialPortDriverTcpClient::NSGetOptions(TNewt::RefArg frame)
{
	using namespace TNewt;
	std::string server;
	char buf[32];
	{
		std::lock_guard<std::mutex> lock(mServerMutex);
		server = mServer ? mServer : "";
		snprintf(buf, 30, "%d", mPort);
	}
	SetFrameSlot(frame, RefVar(MakeSymbol("tcpServer")), RefVar(MakeString(server.c_str())));
	SetFrameSlot(frame, RefVar(MakeSymbol("tcpPort")), RefVar(MakeString(buf)));
}

/**
 \brief Set options from NewtonScript.

 Change the setting for this driver by sending a frame with two slot:
 - tcpServer: string containing the address or name of the server
 - tcpPort: string containing the port number

 Changing the settings will disconnect from the current server and ask the
 runner to reconnect.

 \param[in] inFrame An NS frame with slots for server name and port number.
 */
void
TSerialPortDriverTcpClient::NSSetOptions(TNewt::RefArg inFrame)
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
	if (RefIsString(tcpServerRef))
	{
		RefToString(tcpServerRef, server, sizeof(server));
		setServer = true;
	}
	NewtRef tcpPortRef = GetFrameSlot(frame, MakeSymbol("tcpPort"));
	if (RefIsString(tcpPortRef))
	{
		RefToString(tcpPortRef, portStr, (int) sizeof(portStr));
		port = atoi(portStr);
		if (port == 0)
			port = 3679;
		setPort = true;
	}

	{
		// The worker thread reads the server and port when it connects.
		std::lock_guard<std::mutex> lock(mServerMutex);
		// KPrintf("INFO: TSerialPortDriverTcpClient::NSSetOptions: (\"%s\", %d)\n", mServer, mPort);
		if (setServer)
		{
			if (strcmp(mServer, server) != 0)
			{
				if (mServer)
					::free(mServer);
				mServer = strdup(server);
				mustReconnect = true;
			}
			// KPrintf("INFO:             Setting server to \"%s\".\n", server);
		}
		if (setPort)
		{
			if (mPort != port)
			{
				mPort = port;
				mustReconnect = true;
			}
			// KPrintf("INFO:             Setting port to %d.\n", port);
		}
	}

	if (mustReconnect)
	{
		// We are on the emulator thread, but only the worker thread may touch
		// the socket. Ask it to disconnect, it reconnects when there is data.
		mReconnectRequested = true;
		TriggerEvent('r');
		// KPrintf("INFO: TSerialPortDriverTcpClient::NSSetOptions: must reconnect\n");
	}
}

/**
 \brief Change the server address.
 Set a new server name. This will not disconnect or reconnect. The next
 connection goes to the new server.
 \param[in] inAddress new server address as a string.
 */
void
TSerialPortDriverTcpClient::SetServerAddress(const char* inAddress)
{
	std::lock_guard<std::mutex> lock(mServerMutex);
	if (mServer)
		::free(mServer);
	mServer = strdup(inAddress);
}

/**
 \brief Change the server port.
 Set a new server port. This will not disconnect or reconnect. The next
 connection goes to the new port.
 \param[in] inPort new server port number.
 */
void
TSerialPortDriverTcpClient::SetServerPort(int inPort)
{
	std::lock_guard<std::mutex> lock(mServerMutex);
	mPort = inPort;
}

/**
 \brief Get a copy of the server address.
 Returns the server name. This is not necessarily the name of the currently
 connected server.
 \return a "C" string, caller is responsible to free the string.
 */
char*
TSerialPortDriverTcpClient::GetServerAddressDup()
{
	std::lock_guard<std::mutex> lock(mServerMutex);
	return strdup(mServer);
}

/**
 \brief Get the server port.
 Returns the server port.
 \return the port number as an integer.
 */
int
TSerialPortDriverTcpClient::GetServerPort()
{
	std::lock_guard<std::mutex> lock(mServerMutex);
	return mPort;
}

// ================================================================== //
// You never finish a program, you just stop working on it.           //
//  - Anonymous                                                       //
//                                                                    //
// So true!                                                           //
//  - Matthias Melcher                                                //
// ================================================================== //
