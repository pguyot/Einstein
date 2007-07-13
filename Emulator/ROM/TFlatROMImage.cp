// ==============================
// File:			TFlatROMImage.cp
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
#include "TFlatROMImage.h"

// ANSI C & POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#if TARGET_OS_WIN32
	#include "CompatibilityWin32.h"
	#include <io.h>
#else
	#include <sys/uio.h>
	#include <unistd.h>
	#include <sys/param.h>
#endif

// ISO C++
#include <stdexcept>

// K
#include <K/Defines/UByteSex.h>
#include <K/Misc/TMappedFile.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFlatROMImage( const char*, const char [6], Boolean )
// -------------------------------------------------------------------------- //
TFlatROMImage::TFlatROMImage(
					const char* inROMPath,
					const char inMachineString[6],
					Boolean inMonitorMode /* = false */ )
{
	struct stat theInfos;
	int err = ::stat( inROMPath, &theInfos );
	if (err < 0)
	{
		(void) ::fprintf( stderr, "Can't stat ROM file\n" );
		::exit( 1 );
	}

	Boolean is8MB = false;
	KUInt32 theSize = theInfos.st_size;
	if (theSize == 0x00800000)
	{
		is8MB = true;
	} else if (theSize != 0x01000000) {
		(void) ::fprintf( stderr, "ROM file should be 8MB or 16MB long\n" );
		::exit( 1 );
	}
	
	// Create the image path.
	char theImagePath[PATH_MAX];
	(void) ::sprintf( theImagePath, "%s.img", inROMPath );
	
	// Check if we need to read the ROM file.
	if (IsImageOutdated(theImagePath, theInfos.st_mtime, inMachineString))
	{	
		TMappedFile theImageFile( inROMPath );
		CreateImage(
			theImagePath,
			(const KUInt8*) theImageFile.GetBuffer(),
			theSize,
			inMachineString );
	}

	// Finally load the image.
	Init(theImagePath, inMonitorMode);
}

// -------------------------------------------------------------------------- //
//  * ~TFlatROMImage( void )
// -------------------------------------------------------------------------- //
TFlatROMImage::~TFlatROMImage( void )
{
}

// ========================================= //
// No extensible language will be universal. //
//                 -- T. Cheatham            //
// ========================================= //
