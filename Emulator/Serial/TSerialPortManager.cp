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

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#if !TARGET_OS_WIN32
	#include <unistd.h>
	#include <sys/time.h>
#endif

#include "../Log/TLog.h"
#include "../TInterruptManager.h"
#include "../TDMAManager.h"

#include "TEmulator.h"

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

// Names for all driver types
char const* TSerialPortManager::DriverName[] =
{
	"None", "Named Pipes", "Pseudoterminal", "BasiliskII", "Network Client"
};

// List of drivers available on this platform
KUInt32 TSerialPortManager::DriverList[] =
{
#if TARGET_OS_MAC
	kNullDriver, kPipesDriver, kPtyDriver, kBasiliskIIDriver, kTcpClientDriver
#elif TARGET_OS_ANDROID
	kNullDriver, kTcpClientDriver
#else
	kNullDriver
#endif
};

// Number of elements in the driver list
KUInt32 TSerialPortManager::DriverListSize = sizeof(DriverList) / sizeof(KUInt32);

// List of default driver for every location
KUInt32 TSerialPortManager::DefaultDriver[] =
{
#if TARGET_OS_MAC
	kTcpClientDriver, kNullDriver, kNullDriver, kNullDriver
#elif TARGET_OS_ANDROID
	kTcpClientDriver, kNullDriver, kNullDriver, kNullDriver
#else
	kNullDriver, kNullDriver, kNullDriver, kNullDriver
#endif
};

// -------------------------------------------------------------------------- //
// Static Members
// -------------------------------------------------------------------------- //

TSerialPortManager::DriverChangedCallbackType
	*TSerialPortManager::mDriverChangedCallback = NULL;

TEmulator *TSerialPortManager::mEmulator = NULL;

// Create a new driver given the type-ID
TSerialPortManager *TSerialPortManager::CreateByID(KUInt32 id, TLog* inLog, ELocationID location)
{
	switch (id) {
		default:
			return new TSerialPortManager(inLog, location);
#if TARGET_OS_MAC
		case kPipesDriver:
			return new TPipesSerialPortManager(inLog, location);
		case kPtyDriver:
			return new TPtySerialPortManager(inLog, location);
		case kBasiliskIIDriver:
			return new TBasiliskIISerialPortManager(inLog, location);
#endif
#if TARGET_OS_MAC || TAGRET_OS_ANDROID
		case kTcpClientDriver:
			return new TTcpClientSerialPortManager(inLog, location);
	}
#endif
}

// Replace a driver in the Emulator with a new one, calling the callback function
void TSerialPortManager::ReplaceDriver(ELocationID location, TSerialPortManager *drv)
{
	return mEmulator->SetSerialPortDriver(location, drv);
}

// Replace a driver in the Emulator with a new one, calling the callback function
TSerialPortManager *TSerialPortManager::CurrentDriver(ELocationID location)
{
	return mEmulator->GetSerialPortDriver(location);
}

// Request of the UI to be called back when the driver configuration changes
void TSerialPortManager::SetDriverChangedCallback(DriverChangedCallbackType, void *)
{
}

// -------------------------------------------------------------------------- //
//  * TSerialPortManager( TLog*, ELocationID )
// -------------------------------------------------------------------------- //
TSerialPortManager::TSerialPortManager(
		TLog* inLog,
		ELocationID inLocationID)
:
		mLog( inLog ),
		mLocationID( inLocationID ),
		mInterruptManager( NULL ),
		mDMAManager( NULL ),
		mMemory( NULL )
{
	if ( mLog )
	{
		mLog->FLogLine("TSerialPortManager initializing for port %c%c%c%c", (inLocationID & 0xff000000) >> 24, (inLocationID & 0x00ff0000) >> 16, (inLocationID & 0x0000ff00) >> 8, inLocationID & 0x000000ff);
	}
}

// -------------------------------------------------------------------------- //
//  * ~TSerialPortManager( void )
// -------------------------------------------------------------------------- //
TSerialPortManager::~TSerialPortManager( void )
{
}

// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void TSerialPortManager::run(TInterruptManager* inInterruptManager,
							 TDMAManager* inDMAManager,
							 TMemory* inMemory)
{
	mInterruptManager = inInterruptManager;
	mDMAManager = inDMAManager;
	mMemory = inMemory;
	// nothing to do here
}

// -------------------------------------------------------------------------- //
//  * WriteRegister( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TSerialPortManager::WriteRegister( KUInt32 inOffset, KUInt8 inValue )
{
	if (mLog)
	{
		mLog->FLogLine(
			"[%c%c%c%c] - Write %.2X to serial register %.4X",
			(char) ((mLocationID >> 24) & 0xFF),
			(char) ((mLocationID >> 16) & 0xFF),
			(char) ((mLocationID >> 8) & 0xFF),
			(char) ((mLocationID) & 0xFF),
			(unsigned int) inValue,
			(unsigned int) inOffset );
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
				"[%c%c%c%c] - Read unknown serial register %.4X : %.2X",
				(char) ((mLocationID >> 24) & 0xFF),
				(char) ((mLocationID >> 16) & 0xFF),
				(char) ((mLocationID >> 8) & 0xFF),
				(char) ((mLocationID) & 0xFF),
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
					   "[%c%c%c%c] - Read DMA register %i.%i for channel %i : %.8X",
					   (mLocationID & 0xff000000) >> 24, (mLocationID & 0x00ff0000) >> 16,
					   (mLocationID & 0x0000ff00) >> 8, mLocationID & 0x000000ff,
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
					   "[%c%c%c%c] - Write DMA bank %i, channel %i, register %i : %.8X",
					   (mLocationID & 0xff000000) >> 24, (mLocationID & 0x00ff0000) >> 16,
					   (mLocationID & 0x0000ff00) >> 8, mLocationID & 0x000000ff,
					   (int) inBank,
					   (int) inChannel, (int) inRegister,
					   (unsigned int) inValue );
	}
}




// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
