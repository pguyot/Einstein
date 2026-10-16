// ==============================
// File:			TPipesSerialPortManager.h
// Project:			Einstein
//
// Copyright 2018 by Matthias Melcher (mm@matthiasm.com).
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

#ifndef _TVOYAGERMANAGEDSERIALPORTNAMEDPIPES_H
#define _TVOYAGERMANAGEDSERIALPORTNAMEDPIPES_H

#include "TBasicSerialPortManager.h"

#include <atomic>
#include <pthread.h>
#include <string>

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;

///
/// Emulate a serial port via named pipes in MacOS
///
/// \author Matthias Melcher
///
class TPipesSerialPortManager : public TBasicSerialPortManager
{
public:
	///
	/// Constructor.
	///
	TPipesSerialPortManager(TLog* inLog,
		TSerialPorts::EPortIndex inPortIx);

	///
	/// Destructor.
	///
	~TPipesSerialPortManager() override;

	///
	/// Return the Identification of this driver
	///
	TSerialPorts::EDriverID
	GetID() override
	{
		return TSerialPorts::kPipesDriver;
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

protected:
	///
	/// Launch the thread that emulates the DMA hardware
	///
	void RunDMA();

	///
	/// Emulate the DMA hardware
	///
	void HandleDMA();

	///
	/// Worker thread: send data pending in memory to the sending pipe.
	///
	void HandleDMASend();

	///
	/// Worker thread: receive data from the receiving pipe.
	///
	void HandleDMAReceive();

	///
	/// PThread hook.
	///
	static void*
	SHandleDMA(void* This)
	{
		((TPipesSerialPortManager*) This)->HandleDMA();
		return 0L;
	}

	///
	/// Find good names for the named pipes
	///
	void FindPipeNames();

	///
	/// Create the named pipes as nodes in the file system
	///
	bool CreateNamedPipes();

	/// communication between emulator and DMA thread
	int mPipe[2] { -1, -1 };

	/// named pipe or serial port
	int mTxPort { -1 };

	/// named pipe or serial port
	int mRxPort { -1 };

	/// set if the DMA thread was started; the destructor joins it
	bool mDMAIsRunning { false };

	/// Set by TriggerEvent('q'), so a quit request can't get lost.
	std::atomic<bool> mQuitRequested { false };

	/// The members below are only used by the worker thread.

	/// the port did not take all data; wait until it can take more
	bool mSendBlocked { false };

	/// read again at this time after a read error
	Clock::time_point mReadRetryTime {};

	/// a read error was reported; don't report it again until a read works
	bool mReadErrorReported { false };

	/// pace sending and receiving
	TThrottle mTxThrottle;
	TThrottle mRxThrottle;

	pthread_t mDMAThread { 0 };

	/// named pipe for transmitting data
	std::string mTxPortName { };

	/// named pipe for receiving data
	std::string mRxPortName { };
};

#endif
// _TVOYAGERMANAGEDSERIALPORTNAMEDPIPES_H

// ================= //
// Byte your tongue. //
// ================= //
