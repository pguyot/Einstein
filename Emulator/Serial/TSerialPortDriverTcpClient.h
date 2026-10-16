// ==============================
// File:			TSerialPortDriverTcpClient.h
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

#ifndef _T_SERIAL_PORT_DRIVER_TCP_CLIENT_H
#define _T_SERIAL_PORT_DRIVER_TCP_CLIENT_H

#include "TBasicSerialPortManager.h"

#if TARGET_OS_WIN32
#include <Windows.h>
#include <Winsock2.h>
#endif
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;

//
// Emulate a serial port connection over a network as a TCP client.
//
class TSerialPortDriverTcpClient : public TBasicSerialPortManager
{
public:
	///
	/// Constructor.
	///
	TSerialPortDriverTcpClient(TLog* inLog,
		TSerialPorts::EPortIndex inPortIx);

	///
	/// Destructor.
	///
	~TSerialPortDriverTcpClient() override;

	///
	/// Return the Identification of this driver
	///
	TSerialPorts::EDriverID
	GetID() override
	{
		return TSerialPorts::kTcpClientDriver;
	}

	///
	/// Start emulation.
	///
	void run(TInterruptManager* inInterruptManager,
		TDMAManager* inDMAManager,
		TMemory* inMemory) override;

	///
	/// DMA or interrupts trigger a command that must be handled by a derived class.
	///
	void TriggerEvent(KUInt8 cmd) override;

	//
	// Give NewtonScript access to our list of options.
	//
	void NSGetOptions(TNewt::RefArg frame) override;

	//
	// Set options from NewtonScript
	//
	void NSSetOptions(TNewt::RefArg frame) override;

	//
	// Change the server address.
	//
	void SetServerAddress(const char* inAddress);

	//
	// Change the server port.
	//
	void SetServerPort(int inPort);

	//
	// Get a copy of the server address.
	//
	char* GetServerAddressDup();

	//
	// Get the server port.
	//
	int GetServerPort();

protected:
	using Clock = std::chrono::steady_clock;

	///
	/// Pace a stream of bytes to a fixed time per byte.
	///
	class TThrottle
	{
	public:
		explicit TThrottle(int inMicrosecondsPerByte) :
				mMicrosecondsPerByte(inMicrosecondsPerByte)
		{
		}

		/// Number of bytes that may be transferred now, at most inMax.
		KUInt32 Allowance(KUInt32 inMax);

		/// Count bytes that were transferred.
		void Consume(KUInt32 inBytes);

		/// Microseconds until inBytes (or one time slice of bytes) may be
		/// transferred, 0 if they may go now.
		long long Delay(KUInt32 inBytes) const;

	private:
		int mMicrosecondsPerByte;
		Clock::time_point mReadyTime {};
	};

	///
	/// Host user interface error message
	///
	void LogError(const char* text, bool systemError = false);

	//
	// Worker thread: Emulate the DMA hardware
	//
	void HandleDMA();

	//
	// Worker thread: Send data pending in memory to the server.
	//
	void HandleDMASend();

	//
	// Worker thread: number of bytes DMA has to send (takes mDMAMutex).
	//
	KUInt32 PendingTxBytes();

	//
	// Worker thread: Receive data from the server.
	//
	void HandleDMAReceive();

	///
	/// Create a socket and start connecting it to the server.
	///
	bool Connect();

	///
	/// Finish a connection that Connect() started.
	///
	void FinishConnect(bool inDone, int inError);

	///
	/// Disconnect from the server.
	///
	bool Disconnect();

	///
	/// Return true if we are connected to the server.
	///
	bool
	IsConnected()
	{
		return mIsConnected;
	}

	/// Protects mServer and mPort: the UI and the emulator thread change them
	/// while the worker thread connects.
	std::mutex mServerMutex;

	/// Server name or IP address as a string
	char* mServer { nullptr };

	/// Server side port number
	int mPort { 3679 };

	/// Set by TriggerEvent('q'), so a quit request can't get lost.
	std::atomic<bool> mQuitRequested { false };

	/// Set by NSSetOptions() when the server changed.
	std::atomic<bool> mReconnectRequested { false };

#if TARGET_OS_WIN32
	WSAEVENT mTcpEvent { INVALID_HANDLE_VALUE };
	WSAEVENT mQuitEvent { INVALID_HANDLE_VALUE };
	WSAEVENT mOtherEvent { INVALID_HANDLE_VALUE };
	SOCKET mTcpSocket { INVALID_SOCKET };
#else
	/// communication between emulator and DMA thread
	int mCommandPipe[2] { -1, -1 };

	/// TCP socket for client side
	int mTcpSocket { -1 };
#endif
	/// the thread that does all the work
	std::thread* mWorkerThread { nullptr };

	/// The members below are only used by the worker thread.

	/// set to true if there is a connection to a server
	bool mIsConnected { false };

	/// set to true while a connection attempt is in progress
	bool mIsConnecting { false };

	/// the socket did not take all data; wait until it can take more
	bool mSendBlocked { false };

#if TARGET_OS_WIN32
	/// the socket reported data or a disconnect that we did not read yet
	bool mCanReceive { false };
#endif

	/// give up the connection attempt at this time
	Clock::time_point mConnectDeadline {};

	/// next time we allow another connection attempt
	Clock::time_point mReconnectTime {};

	/// pace sending and receiving
	TThrottle mTxThrottle;
	TThrottle mRxThrottle;
};

#endif
// _T_SERIAL_PORT_DRIVER_TCP_CLIENT_H

// ================= //
// Byte your tongue. //
// ================= //
