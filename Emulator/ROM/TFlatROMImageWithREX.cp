// ==============================
// File:			TFlatROMImageWithREX.cp
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
#include "TFlatROMImageWithREX.h"

// ANSI C & POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/param.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFlatROMImageWithREX( const char*, const char*, const char[6], ... )
// -------------------------------------------------------------------------- //
TFlatROMImageWithREX::TFlatROMImageWithREX(
						const char* inROMPath,
						const char* inREXPath,
						const char inMachineString[6],
						Boolean inMonitorMode /* = false */ )
{	
	struct stat theInfos;
	int err = ::stat( inROMPath, &theInfos );
	if (err < 0)
	{
		(void) ::fprintf( stderr, "Can't stat ROM file '%s'\n", inROMPath );
		::exit( 1 );
	}
	
	if (theInfos.st_size != 0x00800000)
	{
		(void) ::fprintf( stderr, "ROM file should be 8MB long\n" );
		::exit( 1 );
	}
	
	time_t theModTime = theInfos.st_mtime;

	if (GetLatestModDate( &theModTime, inREXPath ) < 0)
	{
		(void) ::fprintf( stderr, "Can't stat REX file (%s)\n", inREXPath );
		::exit( 1 );
	}

	// Create the image path.
	char theImagePath[PATH_MAX];
	(void) ::sprintf( theImagePath, "%s.img", inROMPath );
	
	// Check if we need to read the ROM file.
	if (IsImageOutdated(theImagePath, theModTime, inMachineString))
	{	
		KUInt8* theData = (KUInt8*) ::calloc(1, 0x01000000);

		// Let's read the ROM file.
		int fd = ::open( inROMPath, O_RDONLY, 0 );
		if (fd < 0)
		{
			(void) ::fprintf( stderr, "Can't open ROM file '%s'\n", inROMPath );
			::exit( 1 );
		}
	
		if (::read(	fd, (void*) theData, 0x00800000 ) != 0x00800000)
		{
			(void) ::close( fd );
			(void) ::fprintf( stderr, "Error while reading ROM file '%s'\n", inROMPath );
			::exit( 1 );
		}
		
		(void) ::close( fd );
		
		// Let's read the REX file.
		fd = ::open( inREXPath, O_RDONLY, 0 );
		if (fd < 0)
		{
			(void) ::fprintf( stderr, "Can't open REX file '%s'\n", inREXPath );
			::exit( 1 );
		}
		
		(void) ::read( fd, (void*) &theData[0x00800000], 0x00800000 );
		(void) ::close( fd );

		CreateImage( theImagePath, theData, 0x01000000, inMachineString );

		::free(theData);
	}
	
	// Finally load the image.
	Init(theImagePath, inMonitorMode);
}

// -------------------------------------------------------------------------- //
//  * ~TFlatROMImageWithREX( void )
// -------------------------------------------------------------------------- //
TFlatROMImageWithREX::~TFlatROMImageWithREX( void )
{
}

// ===================================== //
// God is real, unless declared integer. //
// ===================================== //
