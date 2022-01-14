// ==============================
// File:			TDMAManager.cp
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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

#include <K/Streams/TStream.h>
#include "TDMAManager.h"

// Einstein
#include "TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "TEmulator.h"
#include "Serial/TSerialPortManager.h"
#include "Log/TLog.h"

// TDMAManager is invoked by TMemory when physical addresses corresponding
// to DMA registers are accessed.
//
// Currently the only register that is tracked is the Channel Assignment
// register which resides at physical address 0x0F08FC00 and is read/write.

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TDMAManager( TLog*, TMemory*, TInterruptManager* )
// -------------------------------------------------------------------------- //
TDMAManager::TDMAManager(
			TLog* inLog,
			TEmulator* inEmulator,
			TMemory* inMemory,
			TInterruptManager* inInterruptManager )
	:
		mLog( inLog ),
		mMemory( inMemory ),
		mInterruptManager( inInterruptManager ),
        mEmulator( inEmulator )
{
}

// -------------------------------------------------------------------------- //
//  * ~TDMAManager( void )
// -------------------------------------------------------------------------- //
TDMAManager::~TDMAManager( void )
{
}

// -------------------------------------------------------------------------- //
//  * ReadChannelAssignmentRegister( void )
// -------------------------------------------------------------------------- //
KUInt32
TDMAManager::ReadChannelAssignmentRegister( void )
{
	if (mLog)
	{
		mLog->FLogLine(
			"Read DMA Assignment register : %.8X",
			(unsigned int) mAssignmentReg );
	}
	return mAssignmentReg;
}

// -------------------------------------------------------------------------- //
//  * WriteChannelAssignmentRegister( KUInt32 )
// -------------------------------------------------------------------------- //
void
TDMAManager::WriteChannelAssignmentRegister( KUInt32 inValue )
{
	if (mLog)
	{
		mLog->FLogLine(
			"Write DMA Assignment register : %.8X",
			(unsigned int) inValue );
	}
	mAssignmentReg = inValue;
}

// -------------------------------------------------------------------------- //
//  * WriteEnableRegister( KUInt32 )
// -------------------------------------------------------------------------- //
void
TDMAManager::WriteEnableRegister( KUInt32 inValue )
{
	if (mLog)
	{
		mLog->FLogLine(
			"Write DMA Enable register : %.8X",
			(unsigned int) inValue );
	}

	// We should start or resume the DMA transfer.
//	mInterruptManager->RaiseInterrupt( inValue << 7 );
//	TDebugger::BreakInDebugger();
}

// -------------------------------------------------------------------------- //
//  * ReadStatusRegister( void )
// -------------------------------------------------------------------------- //
KUInt32
TDMAManager::ReadStatusRegister( void )
{
	KUInt32 theResult = 0;

	if (mLog)
	{
		mLog->FLogLine(
			"Read DMA Status register : %.8X",
			(unsigned int) theResult );
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * WriteDisableRegister( KUInt32 )
// -------------------------------------------------------------------------- //
void
TDMAManager::WriteDisableRegister( KUInt32 inValue )
{
	if (mLog)
	{
		mLog->FLogLine(
			"Write DMA Disable register : %.8X",
			(unsigned int) inValue );
	}

	// We should abort the DMA transfer.
//	mInterruptManager->RaiseInterrupt( inValue << 7 );
//	TDebugger::BreakInDebugger();
}

// -------------------------------------------------------------------------- //
//  * ReadWordStatusRegister( void )
// -------------------------------------------------------------------------- //
KUInt32
TDMAManager::ReadWordStatusRegister( void )
{
	KUInt32 theResult = 0;

	if (mLog)
	{
		mLog->FLogLine(
			"Read DMA Word Status register : %.8X",
			(unsigned int) theResult );
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * ReadChannel1Register( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TDMAManager::ReadChannel1Register( KUInt32 inChannel, KUInt32 inRegister )
{
	KUInt32 theResult = 0;

	if (inChannel==kSerialPort0Receive || inChannel==kSerialPort0Transmit) {
		TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
		if (extr)
			return extr->ReadDMARegister(1, inChannel, inRegister);
	}

	if (mLog)
	{
		mLog->FLogLine(
					   "Read DMA register 1.%i for channel %i : %.8X",
					   (int) inRegister, (int) inChannel,
					   (unsigned int) theResult );
	}

	//	TDebugger::BreakInDebugger();

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * WriteChannel1Register( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TDMAManager::WriteChannel1Register(
						KUInt32 inChannel,
						KUInt32 inRegister,
						KUInt32 inValue )
{
	if (inChannel==kSerialPort0Receive || inChannel==kSerialPort0Transmit) {
		TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
		if (extr)
			return extr->WriteDMARegister(1, inChannel, inRegister, inValue);
	}

	// 2.3 -> first parameter of ConfigureInterrupt
	if (mLog)
	{
		mLog->FLogLine(
					   "Write DMA register 1.%i for channel %i : %.8X",
					   (int) inRegister, (int) inChannel,
					   (unsigned int) inValue );
	}

	//	TDebugger::BreakInDebugger();
}

// -------------------------------------------------------------------------- //
//  * ReadChannel2Register( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TDMAManager::ReadChannel2Register( KUInt32 inChannel, KUInt32 inRegister )
{
	KUInt32 theResult = 0;

	if (inChannel==kSerialPort0Receive || inChannel==kSerialPort0Transmit) {
		TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
		if (extr)
			return extr->ReadDMARegister(2, inChannel, inRegister);
	}

	if (mLog)
	{
		mLog->FLogLine(
					   "Read DMA register 2.%i for channel %i : %.8X",
					   (int) inRegister, (int) inChannel,
					   (unsigned int) theResult );
	}

	//	TDebugger::BreakInDebugger();

	return theResult;
}


// -------------------------------------------------------------------------- //
//  * WriteChannel2Register( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TDMAManager::WriteChannel2Register(
						KUInt32 inChannel,
						KUInt32 inRegister,
						KUInt32 inValue )
{
	if (inChannel==kSerialPort0Receive || inChannel==kSerialPort0Transmit) {
		TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
		if (extr)
			return extr->WriteDMARegister(2, inChannel, inRegister, inValue);
	}

	if (mLog)
	{
		mLog->FLogLine(
					   "Write DMA register 2.%i for channel %i : %.8X",
					   (int) inRegister, (int) inChannel,
					   (unsigned int) inValue );
	}

	//	TDebugger::BreakInDebugger();
}


// -------------------------------------------------------------------------- //
//  * void TransferState( TStream* )
// -------------------------------------------------------------------------- //
void TDMAManager::TransferState( TStream* inStream )
{
	inStream->TransferInt32BE(mAssignmentReg);
}


// ======================================================== //
// The moving cursor writes, and having written, blinks on. //
// ======================================================== //
