// ==============================
// File:			TSerialPortManager.cp
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

#include "TSerialPortManager.h"

#include "Emulator/Log/TStdOutLog.h"
#include "Emulator/Log/TFileLog.h"

// POSIX
#include <sys/types.h>
#include <csignal>
#include <cstring>

#if !TARGET_OS_WIN32
	#include <unistd.h>
	#include <sys/time.h>
#endif

#include "Emulator/Log/TLog.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TDMAManager.h"

#include "Emulator/TEmulator.h"

#include "Emulator/Serial/TBasicSerialPortManager.h"
#if TARGET_OS_MAC
#include "Emulator/Serial/TPipesSerialPortManager.h"
#include "Emulator/Serial/TPtySerialPortManager.h"
#include "Emulator/Serial/TBasiliskIISerialPortManager.h"
#endif
#if TARGET_OS_MAC || TAGRET_OS_ANDROID
#include "Emulator/Serial/TTcpClientSerialPortManager.h"
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
//  * TSerialPortManager( TLog*, ELocationID )
// -------------------------------------------------------------------------- //
TSerialPortManager::TSerialPortManager(
		TLog* inLog,
		TSerialPorts::EPortIndex inPortIx)
:
		mLog( inLog ),
		mNewtPortIndex( inPortIx ),
		mInterruptManager( nullptr ),
		mDMAManager( nullptr ),
		mMemory( nullptr )
{
	if ( mLog )
	{
		mLog->FLogLine("TSerialPortManager initializing for port %d", mNewtPortIndex);
	}
}

// -------------------------------------------------------------------------- //
//  * ~TSerialPortManager( void )
// -------------------------------------------------------------------------- //
TSerialPortManager::~TSerialPortManager() = default;


// -------------------------------------------------------------------------- //
//  * WriteRegister( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TSerialPortManager::WriteRegister( KUInt32 inOffset, KUInt8 inValue )
{
    (void)inOffset;
    (void)inValue;
	if (mLog)
	{
		mLog->FLogLine("[%d] - Write %.2X to serial register %.4X", mNewtPortIndex);
	}
}

// -------------------------------------------------------------------------- //
//  * ReadRegister( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TSerialPortManager::ReadRegister( KUInt32 inOffset )
{
	KUInt8 theResult = 0;
	if (inOffset == 0x4400)
	{
		// Both buffers are empty for now.
		// We also don't want a beacon.
		theResult = 0x80;
	} else if (inOffset == 0x4800) {
		// RxEOF
//		theResult = 0x80;
//		TDebugger::BreakInDebugger();
	} else {
//		TDebugger::BreakInDebugger();
		if (mLog)
		{
			mLog->FLogLine(
						   "[%d] - Read unknown serial register %.4X : %.2X",
						   mNewtPortIndex,
						   (unsigned int) inOffset,
						   (unsigned int) theResult );
		}
	}
	
	return theResult;
}


// -------------------------------------------------------------------------- //
//  * ReadDMARegister( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TSerialPortManager::ReadDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister )
{
	KUInt32 theResult = 0L;
	if (mLog)
	{
		mLog->FLogLine(
					   "[%d] - Read DMA register %i.%i for channel %i : %.8X",
					   mNewtPortIndex,
					   (int) inBank,
					   (int) inRegister, (int) inChannel,
					   (unsigned int) theResult );
	}
	return theResult;
}


// -------------------------------------------------------------------------- //
//  * WriteDMARegister( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TSerialPortManager::WriteDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister, KUInt32 inValue )
{
	if (mLog)
	{
		mLog->FLogLine(
					   "[%d] - Write DMA bank %i, channel %i, register %i : %.8X",
					   mNewtPortIndex,
					   (int) inBank,
					   (int) inChannel, (int) inRegister,
					   (unsigned int) inValue );
	}
}




// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
