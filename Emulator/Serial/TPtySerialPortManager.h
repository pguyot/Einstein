// ==============================
// File:			TPtySerialPortManager.h
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

#ifndef _TPtySerialPortManager_H
#define _TPtySerialPortManager_H

#include "TBasicSerialPortManager.h"

#include <pthread.h>

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;

///
/// Emulate a serial port via a pseudo terminal (pty) in MacOS
///
/// \author Matthias Melcher
///
class TPtySerialPortManager : public TBasicSerialPortManager
{
public:

	///
	/// Constructor.
	///
	TPtySerialPortManager(TLog* inLog,
						  ELocationID inLocationID);

	///
	/// Destructor.
	///
	virtual ~TPtySerialPortManager( void );

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
	/// Launch the thread that emulates the DMA hardware
	///
	void RunDMA();

	///
	/// Emulate the DMA hardware
	///
	void HandleDMA();

	///
	/// PThread hook.
	///
	static void *SHandleDMA(void *This) { ((TPtySerialPortManager*)This)->HandleDMA(); return 0L; }

	///
	/// Find good names for the named pipes
	///
	void FindPtyName();

	///
	/// Create the named pipes as nodes in the file system
	///
	bool CreatePty();

	int mPipe[2];							///< communication between emulator and DMA thread
	int mPtyPort;							///< pseudo terminal file id
	bool mDMAIsRunning;						///< set if DMA thread is active
	pthread_t mDMAThread;
	char *mPtyName;							///< named of pseudo terminal
};

#endif
// _TPtySerialPortManager_H

// ================= //
// Byte your tongue. //
// ================= //
