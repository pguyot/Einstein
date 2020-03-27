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

#include <pthread.h>

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
	virtual ~TTcpClientSerialPortManager( void );

	///
	/// Return the Identification of this driver
	///
	virtual KUInt32 GetID() { return TSerialPorts::kTcpClientDriver; }

	///
	/// Start emulation.
	///
	virtual void run(TInterruptManager* inInterruptManager,
					 TDMAManager* inDMAManager,
					 TMemory* inMemory);

	///
	/// DMA or interrupts trigger a command that must be handled by a derived class.
	///
	virtual void TriggerEvent(KUInt8 cmd);

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
	/// PThread hook.
	///
	static void *SHandleDMA(void *This) { ((TTcpClientSerialPortManager*)This)->HandleDMA(); return 0L; }

	///
	/// Create a socket and try to connect it to the server.
	///
	bool Connect();

	///
	/// Disconnect from teh server.
	///
	bool Disconnect();

	///
	/// Return true if we are connected to teh server.
	///
	bool IsConnected() { return mIsConnected; }

	int mCommandPipe[2];			///< communication between emulator and DMA thread
	int mTcpSocket;					///< TCP socket for client side
	bool mWorkerThreadIsRunning;	///< set if DMA thread is active
	pthread_t mWorkerThread;		///< the thread that does all the work
	bool mIsConnected;
	time_t mReconnectTimeout;		///< next time we allow another connection attempt
};

#endif
// _T_TCP_CLIENT_SERIAL_PORT_MANAGER_H

// ================= //
// Byte your tongue. //
// ================= //
