// ==============================
// File:			TNullScreenManager.cp
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

#include "TNullScreenManager.h"

// Einstein
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::PowerOn( void )
{
	GetLog()->LogLine("TNullScreenManager::PowerOn()");
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::PowerOff( void )
{
	GetLog()->LogLine("TNullScreenManager::PowerOff()");
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::PowerOnScreen( void )
{
	GetLog()->LogLine("TNullScreenManager::PowerOnScreen()");
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::PowerOffScreen( void )
{
	GetLog()->LogLine("TNullScreenManager::PowerOffScreen()");
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( bool )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::BacklightChanged( bool backlight )
{
	GetLog()->FLogLine("TNullScreenManager::BacklightChanged(%d)", (int) backlight);
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::ContrastChanged( KUInt32 contrast )
{
	GetLog()->FLogLine("TNullScreenManager::ContrastChanged(%u)", (unsigned int) contrast);
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::ScreenOrientationChanged( EOrientation inNewOrientation )
{
	GetLog()->FLogLine("TNullScreenManager::ScreenOrientationChanged(%u)", (unsigned int) inNewOrientation);
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::TabletOrientationChanged( EOrientation inNewOrientation )
{
	GetLog()->FLogLine("TNullScreenManager::TabletOrientationChanged(%u)", (unsigned int) inNewOrientation);
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TNullScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{
    // Ignore this.
}

// ========================================================================= //
// ... Any resemblance between the above views and those of my employer,     //
// my terminal, or the view out my window are purely coincidental.  Any      //
// resemblance between the above and my own views is non-deterministic.  The //
// question of the existence of views in the absence of anyone to hold them  //
// is left as an exercise for the reader.  The question of the existence of  //
// the reader is left as an exercise for the second god coefficient.  (A     //
// discussion of non-orthogonal, non-integral polytheism is beyond the scope //
// of this article.)                                                         //
// ========================================================================= //
