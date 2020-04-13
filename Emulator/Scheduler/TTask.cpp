// ==============================
// File:			TTask.cp
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
#include "TTask.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TTask( void )
// -------------------------------------------------------------------------- //
TTask::TTask( void )
	:
		mThread( this )
{
}

// -------------------------------------------------------------------------- //
//  * ~TTask( void )
// -------------------------------------------------------------------------- //
TTask::~TTask( void )
{
}

// -------------------------------------------------------------------------- //
//  * Run( void )
// -------------------------------------------------------------------------- //
void
TTask::Run( void )
{
}

// -------------------------------------------------------------------------- //
//  * IsCurrentTask( void ) const
// -------------------------------------------------------------------------- //
Boolean
TTask::IsCurrentTask( void ) const
{
	return mThread->IsCurrentThread();
}

// -------------------------------------------------------------------------- //
//  * SuspendAtomic( void )
// -------------------------------------------------------------------------- //
void
TTask::SuspendAtomic( void )
{
	if (!mAtomicSuspended)
	{
		mAtomicSuspended = true;
		mThread->Suspend();
	}
}

// -------------------------------------------------------------------------- //
//  * ResumeAtomic( void )
// -------------------------------------------------------------------------- //
void
TTask::ResumeAtomic( void )
{
	if (mAtomicSuspended)
	{
		mAtomicSuspended = false;
		mThread->Resume();
	}
}

// ============================================== //
// Message from Our Sponsor on ttyTV at 13:58 ... //
// ============================================== //
