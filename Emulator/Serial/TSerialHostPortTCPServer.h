// ==============================
// File:			TSerialHostPortTCPServer.h
// Project:			Einstein
//
// Copyright 2026 Eckhart Koeppen
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

#ifndef _TSERIAL_HOST_PORT_TCP_SERVER
#define _TSERIAL_HOST_PORT_TCP_SERVER

#include "TSerialHostPort.h"
#include <K/Misc/TCircleBuffer.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

///
/// Class to handle host emulated serial port as a TCP Server
///
/// This class runs a TCP server socket (defaulting to 127.0.0.1:3636)
/// using C++17 blocking reader and writer threads, allowing clients
/// (such as BasiliskII or network tools) to connect, send and receive
/// serial bytes cleanly and portably across platforms.
class TSerialHostPortTCPServer
		: public TSerialHostPort
{
public:
	TSerialHostPortTCPServer(
		TLog* inLog,
		KUInt32 inLocation,
		const std::string& inConfig,
		TEmulator* inEmulator);

	virtual ~TSerialHostPortTCPServer() override;

	virtual void PutByte(KUInt8 nextChar) override;
	virtual KUInt8 GetByte() override;
	virtual void SetSerialOutputs(SerialOutputControl) override;
	virtual void ClearSerialOutputs(SerialOutputControl) override;

	virtual void SetInterruptEnable(bool enable) override;
	virtual void Reset() override;
	virtual void SetBreak(bool assert) override;

	virtual InterfaceSpeed SetSpeed(BitRate bitsPerSec) override;
	virtual void SetIOParms(TCMOSerialIOParms* opt) override;
	virtual void Reconfigure() override;

	virtual KSInt32 InitByOption(TOption* initOpt) override;
	virtual KSInt32 ProcessOption(TOption* opt) override;
	virtual KSInt32 SetSerialMode(SerialMode mode) override;

	virtual RxErrorStatus GetByteAndStatus(KUInt8* nextCharPtr) override;
	virtual bool AllSent() override;

	virtual KSInt32 WaitForAllSent() override;

	bool CreateServer();
	bool IsOpen() const { return mListenSocket != -1; }

protected:
	void CloseClient();
	void AcceptThreadEntry();
	void ReadThreadEntry(int inClientSocket);
	void WriteThreadEntry(int inClientSocket);
	static void CloseSocketDescriptor(int inSocket);
	static void ShutdownSocketDescriptor(int inSocket);

	std::string mHost { "127.0.0.1" };
	int mPort { 3636 };

	int mListenSocket { -1 };
	int mClientSocket { -1 };

	std::atomic<bool> mQuitRequested { false };
	std::atomic<bool> mClientConnected { false };

	/// Listener/Accept thread
	std::thread mAcceptThread;

	/// Worker threads for active client connection
	std::thread mReadThread;
	std::thread mWriteThread;

	/// Mutex protecting client socket lifecycle & client state transitions
	std::mutex mClientMutex;

	/// Mutex & condition variable for incoming read buffer & backpressure
	std::mutex mReadMutex;
	std::condition_variable mReadCV;
	TCircleBuffer* mReadBuffer { nullptr };
	static constexpr KUInt32 kMaxReadBufferSize = 64 * 1024;

	/// Mutex & condition variable for outgoing write buffer
	std::mutex mWriteMutex;
	std::condition_variable mWriteCV;
	TCircleBuffer* mWriteBuffer { nullptr };
};

#endif
