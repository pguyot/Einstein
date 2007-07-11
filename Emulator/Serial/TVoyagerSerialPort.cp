// ==============================
// File:			TVoyagerSerialPort.cp
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

#include <K/Defines/KDefinitions.h>
#include "TVoyagerSerialPort.h"

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#if TARGET_OS_WIN32
#else
#	include <unistd.h>
#	include <sys/time.h>
#endif

#include "../Log/TLog.h"
#include "../TInterruptManager.h"
#include "../TDMAManager.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TVoyagerSerialPort( TLog*, ELocationID )
// -------------------------------------------------------------------------- //
TVoyagerSerialPort::TVoyagerSerialPort(
		TLog* inLog,
		ELocationID inLocationID,
		TInterruptManager* inInterruptManager,
		TDMAManager* inDMAManager )
	:
		mLog( inLog ),
		mLocationID( inLocationID ),
		mInterruptManager( inInterruptManager ),
		mDMAManager( inDMAManager )
{
}

// -------------------------------------------------------------------------- //
//  * ~TVoyagerSerialPort( void )
// -------------------------------------------------------------------------- //
TVoyagerSerialPort::~TVoyagerSerialPort( void )
{
}

// -------------------------------------------------------------------------- //
//  * WriteRegister( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TVoyagerSerialPort::WriteRegister( KUInt32 inOffset, KUInt8 inValue )
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
TVoyagerSerialPort::ReadRegister( KUInt32 inOffset )
{
	KUInt8 theResult = 0;
/*	if (mLog)
	{
		mLog->FLogLine(
			"[%c%c%c%c] - Read serial register %.4X : %.2X",
			(char) ((mLocationID >> 24) & 0xFF),
			(char) ((mLocationID >> 16) & 0xFF),
			(char) ((mLocationID >> 8) & 0xFF),
			(char) ((mLocationID) & 0xFF),
			(unsigned int) inOffset,
			(unsigned int) theResult );
	}
*/
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


// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
