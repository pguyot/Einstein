// ==============================
// File:			TBufferedSoundManager.cp
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
#include "TBufferedSoundManager.h"

// ANSI C & POSIX
#include <assert.h>

// Einstein
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TBufferedSoundManager( TLog* )
// -------------------------------------------------------------------------- //
TBufferedSoundManager::TBufferedSoundManager( TLog* inLog /* = nil */ )
	:
		TSoundManager( inLog ),
		mBuffer( nil )
{
#if TARGET_RT_LITTLE_ENDIAN
	mBuffer = (KUInt8*) ::malloc(kNewtonBufferSize);
#endif
}

// -------------------------------------------------------------------------- //
//  * ~TBufferedSoundManager( void )
// -------------------------------------------------------------------------- //
TBufferedSoundManager::~TBufferedSoundManager( void )
{
#if TARGET_RT_LITTLE_ENDIAN
	if (mBuffer)
	{
		::free( mBuffer );
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutputBuffer( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TBufferedSoundManager::ScheduleOutputBuffer( KUInt32 inBufferAddr, KUInt32 inSize )
{
#if TARGET_RT_LITTLE_ENDIAN
	// Copy data (and swap it, too).
	assert(inSize <= kNewtonBufferSize);
	if (GetMemory()->FastReadBuffer(inBufferAddr, inSize, mBuffer))
	{
		if (GetLog())
		{
			GetLog()->FLogLine(
				"Couldn't read data from %.8X\n",
				inBufferAddr);
		}
	} else {
		ScheduleOutput(mBuffer, inSize);
	}
#else
	// Get the pointer directly.
	KUInt8* thePointer;
	if (GetMemory()->GetDirectPointerToRAM(inBufferAddr, &thePointer))
	{
		if (GetLog())
		{
			GetLog()->FLogLine(
				"Couldn't get direct pointer for %.8X\n",
				inBufferAddr);
		}
	} else {
		ScheduleOutput(thePointer, inSize);
	}
#endif
}

// =========================================================================== //
// Disraeli was pretty close: actually, there are Lies, Damn lies, Statistics, //
// Benchmarks, and Delivery dates.                                             //
// =========================================================================== //
