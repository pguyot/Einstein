// ==============================
// File:			TTcpClientSerialPortManager.h
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

#ifndef _T_TCP_CLIENT_SERIAL_PORT_MANAGER_H
#define _T_TCP_CLIENT_SERIAL_PORT_MANAGER_H

#include "TBasicSerialPortManager.h"

#include <thread>

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;

///
/// Emulate a serial port via named pipes in MacOS
///
/// \author Matthias Melcher
///
class TTcpClientSerialPortManager : public TBasicSerialPortManager
{
public:

	///
	/// Constructor.
	///
	TTcpClientSerialPortManager(TLog* inLog,
								TSerialPorts::EPortIndex inPortIx);

	///
	/// Destructor.
	///
	~TTcpClientSerialPortManager( ) override;

	///
	/// Return the Identification of this driver
	///
	KUInt32 GetID() override { return TSerialPorts::kTcpClientDriver; }

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

	///
	/// GIve NewtonScrip access to our list of options
	///
	void NSGetOptions(TNewt::RefArg frame) override;

	///
	/// Set options from NewtonScript
	///
	void NSSetOptions(TNewt::RefArg frame) override;

	void SetServerAddress(const char *inAddress);

	void SetServerPort(int inPort);

	char *GetServerAddressDup();

	int GetServerPort();

protected:

	///
	/// Emulate the DMA hardware
	///
	void HandleDMA();

	///
	/// Send data pending in memory to the server.
	///
	void HandleDMASend();

	///
	/// Receive data from the server.
	///
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
	bool IsConnected() { return mIsConnected; }

	char *mServer = nullptr;
	int mPort = 0;

    int mCommandPipe[2] = { -1, -1 };       ///< communication between emulator and DMA thread
    int mTcpSocket = -1;                    ///< TCP socket for client side
	std::thread *mWorkerThread = nullptr;   ///< the thread that does all the work
	bool mIsConnected = false;              ///< set to true if there is a connection to a server
	time_t mReconnectTimeout = 0;   		///< next time we allow another connection attempt
};

#endif
// _T_TCP_CLIENT_SERIAL_PORT_MANAGER_H

// ================= //
// Byte your tongue. //
// ================= //
