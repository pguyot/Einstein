// ==============================
// File:			TFBScreenManager.cp
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
#include "TFBScreenManager.h"

// POSIX
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

// Linux
#include <linux/fb.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFBScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TFBScreenManager::TFBScreenManager(
			TLog* inLog /* = nil */,
			KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
			KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */ )
	:
		TScreenManager( inLog, inPortraitWidth, inPortraitHeight ),
		mFrameBufferFD( -1 )
{
	// Open the device.
	Open();
}

// -------------------------------------------------------------------------- //
//  * ~TFBScreenManager( void )
// -------------------------------------------------------------------------- //
TFBScreenManager::~TFBScreenManager( void )
{
	// Close the device.
	Close();
}

// -------------------------------------------------------------------------- //
//  * Open( void )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::Open( void )
{
	const char* fbname;
	if (access("/dev/.devfsd", F_OK) == 0)
	{
		fbname = "/dev/fb/0";
	} else {
		fbname = "/dev/fb0";
	}
	
	mFrameBufferFD = open(fbname, O_RDONLY | O_NONBLOCK);
	if (mFrameBufferFD == -1)
	{
		(void) ::fprintf( stderr, "open r/o failed for %s (%i)\n", fbname, errno );
		::abort();
	}
	
	// Open it R/W.
	mFrameBufferFD = open(fbname, O_RDWR);
	if (mFrameBufferFD == -1)
	{
		(void) ::fprintf( stderr, "open r/w failed for %s (%i)\n", fbname, errno );
		::abort();
	}
}

// -------------------------------------------------------------------------- //
//  * Close( void )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::Close( void )
{
	close( mFrameBufferFD );
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::PowerOnScreen( void )
{
	// Just ignore it.
	(void) ::fprintf( stderr, "PowerOnScreen\n" );
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::PowerOffScreen( void )
{
	// Just ignore it.
	(void) ::fprintf( stderr, "PowerOffScreen\n" );
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( bool )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::BacklightChanged( bool )
{
	// Just ignore it.
	(void) ::fprintf( stderr, "BacklightChanged\n" );
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::ContrastChanged( KUInt32 )
{
	// Just ignore it.
	(void) ::fprintf( stderr, "ContrastChanged\n" );
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::ScreenOrientationChanged( EOrientation )
{
	// Just ignore it.
	(void) ::fprintf( stderr, "ScreenOrientationChanged\n" );
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::TabletOrientationChanged( EOrientation )
{
	// Just ignore it.
	(void) ::fprintf( stderr, "TabletOrientationChanged\n" );
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TFBScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{
	(void) ::fprintf( stderr, "UpdateScreenRect\n" );
}

// ============================================================================== //
// panic: kernel segmentation violation. core dumped               (only kidding) //
// ============================================================================== //
