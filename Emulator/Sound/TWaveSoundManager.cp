// ==============================
// File:			TWaveSoundManager.cp
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
#include "TWaveSoundManager.h"

#include <stdlib.h>
#include <strings.h>

// Einstein.
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TWaveSoundManager( TLog* )
// -------------------------------------------------------------------------- //
TWaveSoundManager::TWaveSoundManager( TLog* inLog /* = nil */ )
	:
		TSoundManager( inLog ),
		mOutputIsRunning( false )
{
}

// -------------------------------------------------------------------------- //
//  * ~TWaveSoundManager( void )
// -------------------------------------------------------------------------- //
TWaveSoundManager::~TWaveSoundManager( void )
{
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutputBuffer( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TWaveSoundManager::ScheduleOutputBuffer( KUInt32 /* inBufferAddr */, KUInt32 inSize )
{
	if (inSize == 0)
	{
		mOutputIsRunning = false;
	} else if (mOutputIsRunning) {
		RaiseOutputInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TWaveSoundManager::StartOutput( void )
{
	mOutputIsRunning = true;
	RaiseOutputInterrupt();
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TWaveSoundManager::StopOutput( void )
{
	mOutputIsRunning = false;
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TWaveSoundManager::OutputIsRunning( void )
{
	return false;
}

// =========================================================================== //
// "If brute force doesn't solve your problems, then you aren't using enough."
// =========================================================================== //
