// ==============================
// File:			TNullSoundManager.cp
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

#include "TNullSoundManager.h"

#include <stdlib.h>

#ifdef TARGET_OS_WIN32
	#include <string.h>
#else
	#include <strings.h>
#endif

// Einstein.
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TNullSoundManager( TLog* )
// -------------------------------------------------------------------------- //
TNullSoundManager::TNullSoundManager( TLog* inLog /* = nil */ )
	:
		TSoundManager( inLog ),
		mOutputIsRunning( false )
{
}

// -------------------------------------------------------------------------- //
//  * ~TNullSoundManager( void )
// -------------------------------------------------------------------------- //
TNullSoundManager::~TNullSoundManager( void )
{
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutputBuffer( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNullSoundManager::ScheduleOutputBuffer( KUInt32 /* inBufferAddr */, KUInt32 inSize )
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
TNullSoundManager::StartOutput( void )
{
	mOutputIsRunning = true;
	RaiseOutputInterrupt();
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TNullSoundManager::StopOutput( void )
{
	mOutputIsRunning = false;
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
bool
TNullSoundManager::OutputIsRunning( void )
{
	return false;
}

// ============================================================================= //
// As in Protestant Europe, by contrast, where sects divided endlessly into      //
// smaller competing sects and no church dominated any other, all is different   //
// in the fragmented world of IBM.  That realm is now a chaos of conflicting     //
// norms and standards that not even IBM can hope to control.  You can buy a     //
// computer that works like an IBM machine but contains nothing made or sold by  //
// IBM itself.  Renegades from IBM constantly set up rival firms and establish   //
// standards of their own.  When IBM recently abandoned some of its original     //
// standards and decreed new ones, many of its rivals declared a puritan         //
// allegiance to IBM's original faith, and denounced the company as a divisive   //
// innovator.  Still, the IBM world is united by its distrust of icons and       //
// imagery.  IBM's screens are designed for language, not pictures.  Graven      //
// images may be tolerated by the luxurious cults, but the true IBM faith relies //
// on the austerity of the word.                                                 //
//                 -- Edward Mendelson, "The New Republic", February 22, 1988    //
// ============================================================================= //
