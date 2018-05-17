// ==============================
// File:			TBasiliskIISerialPortManager.h
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

#ifndef _TBasiliskIISerialPortManager_H
#define _TBasiliskIISerialPortManager_H

#include "TBasicSerialPortManager.h"

#include <pthread.h>

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;

///
/// Emulate a serial port via a pseudo terminal (pty) in MacOS
///
/// This driver works around a bug in BasiliskII, where Basilisk will hang
/// forever when closing a serial port.
///
/// This driver provides a PTY via /tmp/pty.BasiliskII . It will try to
/// determine when an app inside Basilisk closes the serial port. The Einstein
/// driver will then remove the PTY, which will in turn break the endless
/// loop in Basilisk.
///
/// This workaround functions well for NTK and NCU, running inside BasiliskII.
///
/// \author Matthias Melcher
///
class TBasiliskIISerialPortManager : public TBasicSerialPortManager
{
	typedef enum {
		kStateInit = 0,
		kStateOpen,
		kStateFlushRW
	} ComState;

public:

	///
	/// Constructor.
	///
	TBasiliskIISerialPortManager(TLog* inLog,
								 ELocationID inLocationID);

	///
	/// Destructor.
	///
	virtual ~TBasiliskIISerialPortManager( void );

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
	static void *SHandleDMA(void *This) { ((TBasiliskIISerialPortManager*)This)->HandleDMA(); return 0L; }

	///
	/// Open a PTY that is linked to /tmp/pty.BasiliskII
	///
	bool OpenPTY();

	///
	/// Close the BasiliskII special PTY, breaking Basilisks endless loop.
	///
	void ClosePTY();

	int mPipe[2];							///< communication between emulator and DMA thread
//	int mPtyPort;							///< pseudo terminal file id
	bool mDMAIsRunning;						///< set if DMA thread is active
	pthread_t mDMAThread;
//	char *mPtyName;							///< named of pseudo terminal

	static const char *kBasiliskPipe;		///< Base name of the pasilisk pty
	char *pBasiliskSlaveName;				///< PTY slave filename
	int pBasiliskMaster; 					///< PTY master file id
	int pBasiliskSlave;						///< PTY slave file id
	ComState pComState;						///< state of the connection to Basilisk
};

#endif
// _TBasiliskIISerialPortManager_H

// ================= //
// Byte your tongue. //
// ================= //
