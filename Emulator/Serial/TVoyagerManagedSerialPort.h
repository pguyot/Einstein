// ==============================
// File:			TVoyagerManagedSerialPort.h
// Project:			Einstein
//
// Copyright 2017 by Matthias Melcher (mm@matthiasm.com).
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

#ifndef _TVOYAGERMANAGEDSERIALPORT_H
#define _TVOYAGERMANAGEDSERIALPORT_H

#include "TVoyagerSerialPort.h"

#include <pthread.h>

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;

///
/// Class for the 4 voyager serial ports.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test	aucun test défini.
///
class TVoyagerManagedSerialPort : public TVoyagerSerialPort
{
public:

	const KUInt8 kSerCmd_TxCtrlChanged = 'c';

	///
	/// Constructor.
	///
	TVoyagerManagedSerialPort(
							  TLog* inLog,
							  ELocationID inLocationID,
							  TInterruptManager* inInterruptManager,
							  TDMAManager* inDMAManager,
							  TMemory* inMemory);

	///
	/// Destructor.
	///
	virtual ~TVoyagerManagedSerialPort( void );

	/// \name Low-level routines.

//	void ThreadRun(pipe);
//	void ThreadHandleConnect();
//	void ThreadHandleDisconnect();
//	void ThreadHandleRead();
//	void ThreadHandleWrite();

	///
	/// Write register.
	///
	virtual void WriteRegister( KUInt32 inOffset, KUInt8 inValue );

	///
	/// Read DMA register.
	///
	virtual KUInt8 ReadRegister( KUInt32 inOffset );

	///
	/// Read DMA register.
	///
	virtual KUInt32 ReadDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister );

	///
	/// Write DMA register.
	///
	virtual void WriteDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister, KUInt32 inValue );

private:

	///
	/// Read receiving DMA register.
	///
	KUInt32 ReadRxDMARegister( KUInt32 inBank, KUInt32 inRegister );

	///
	/// Write receiving DMA register.
	///
	void WriteRxDMARegister( KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue );

	///
	/// Read transmitting DMA register.
	///
	KUInt32 ReadTxDMARegister( KUInt32 inBank, KUInt32 inRegister );

	///
	/// Write transmitting DMA register.
	///
	void WriteTxDMARegister( KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue );

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
	static void *SHandleDMA(void *This) { ((TVoyagerManagedSerialPort*)This)->HandleDMA(); return 0L; }

	///
	/// Find good names for the named pipes
	///
	void FindPipeNames();

	///
	/// Create the named pipes as nodes in the file system
	///
	bool CreateNamedPipes();

	KUInt32 mTxDMAPhysicalBufferStart;		///< physical address of transmit DMA buffer start
	KUInt32 mTxDMAPhysicalData;				///< address of byte currently written by DMA
	KUInt32 mTxDMADataCountdown;			///< number of bytes that still need to be sent
	KUInt32 mTxDMABufferSize;				///< size of physical buffer
	KUInt32 mTxDMAControl;					///< bit 1 enables the DMA port
	KUInt32 mTxDMAEvent;					///< the event that triggered the interrupt?

	KUInt32 mRxDMAPhysicalBufferStart;		///< physical address of receive DMA buffer start
	KUInt32 mRxDMAPhysicalData;				///< address to store next byte read from periphery
	KUInt32 mRxDMADataCountdown;			///< number of bytes available until end of buffer
	KUInt32 mRxDMABufferSize;				///< size of physical buffer
	KUInt32 mRxDMAControl;					///< bit 1 enables the DMA port
	KUInt32 mRxDMAEvent;					///< the event that triggered the interrupt?

	int mPipe[2];							///< communication between emulator and DMA thread
	int mTxPort;							///< named pipe or serial port
	int mRxPort;							///< named pipe or serial port
	int mNamedSendPipe;						///< serial port emulation ("/Users/matt/Library/Application Support/Einstein Emulator/ExtrSerPortSend")
	int mNamedRecvPipe;						///< serial port emulation ("/Users/matt/Library/Application Support/Einstein Emulator/ExtrSerPortRecv")
	bool mDMAIsRunning;						///< set if DMA thread is active
	pthread_t mDMAThread;

	char *mTxPortName;						///< named pipe for transmitting data
	char *mRxPortName;						///< named pipe for receiving data
};

#endif
// _TVOYAGERMANAGEDSERIALPORT_H

// ================= //
// Byte your tongue. //
// ================= //
