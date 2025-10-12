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
	// Worker thread: Receive data from the server.
	//
	void HandleDMAReceive();

	///
	/// Create a socket and try to connect it to the server.
	///
	bool Connect();

	///
	/// Disconnect from teh server.
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

	/// Server name or IP address as a string
	char* mServer { nullptr };

	/// Server side port number
	int mPort { 3679 };

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

	/// set to true if there is a connection to a server
	bool mIsConnected { false };

	/// next time we allow another connection attempt
	time_t mReconnectTimeout { 0 };
};

#endif
// _T_SERIAL_PORT_DRIVER_TCP_CLIENT_H

// ================= //
// Byte your tongue. //
// ================= //
