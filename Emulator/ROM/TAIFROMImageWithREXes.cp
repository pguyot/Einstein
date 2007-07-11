// ==============================
// File:			TAIFROMImageWithREXes.cp
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
#include "TAIFROMImageWithREXes.h"

// ANSI C & POSIX
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#if TARGET_OS_WIN32
#	include "CompatibilityWin32.h"
#	include <io.h>
#else
#	include <sys/uio.h>
#	include <unistd.h>
#	include <sys/param.h>
#endif

// K
#include <K/Defines/UByteSex.h>

// Einstein
#include "TAIFFile.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TAIFROMImageWithREXes( const char*, const char*, const char*, ... )
// -------------------------------------------------------------------------- //
TAIFROMImageWithREXes::TAIFROMImageWithREXes(
						const char* inAIFPath,
						const char* inREX0Path,
						const char* inREX1Path,
						const char inMachineString[6],
						Boolean inMonitorMode /* = false */ )
{
	// Check the modification dates.
	struct stat theInfos;
	int err = ::stat( inAIFPath, &theInfos );
	if (err < 0)
	{
		(void) ::fprintf( stderr, "Can't stat AIF file (%s)\n", inAIFPath );
		::exit( 1 );
	}

	time_t theModDate = theInfos.st_mtime;

	if (GetLatestModDate( &theModDate, inREX0Path ) < 0)
	{
		(void) ::fprintf( stderr, "Can't stat REX0 file (%s)\n", inREX0Path );
		::exit( 1 );
	}
	if (GetLatestModDate( &theModDate, inREX1Path ) < 0)
	{
		(void) ::fprintf( stderr, "Can't stat REX1 file (%s)\n", inREX1Path );
		::exit( 1 );
	}

	// Create the image path.
	char theImagePath[PATH_MAX];
	(void) ::sprintf( theImagePath, "%s.img", inAIFPath );
	
	// Check if we need to read the ROM file.
	if (IsImageOutdated(theImagePath, theModDate, inMachineString))
	{	
		// A priori, on a 16 Mo de ROM.
		KUInt8* theData = (KUInt8*) ::calloc(1, 0x01000000);
	
		// On ouvre le fichier ROM.
		FILE* theFile = ::fopen(inAIFPath, "r");
		if (theFile == NULL)
		{
			(void) ::fprintf( stderr, "Can't open AIF file '%s'\n", inAIFPath );
			::exit( 1 );
		}
		
		KUInt32 theTotalSize;
		{
			TAIFFile theAIFFile( theFile );
			
			// Read the R/O part of the file.
			KUInt32 theROSize = theAIFFile.GetROImageSize();
			if (theROSize > 0x00800000)
			{
				(void) ::fprintf( stderr,
									"Read/only image from AIF file seems too big (%.8X > 8MB)\n",
									(unsigned int) theROSize );
				::exit( 1 );
			}
			
			theAIFFile.ReadROImage( theData );
		
			// Read the R/W part of the file.
			theTotalSize = theROSize + theAIFFile.GetRWImageSize();
			if (theTotalSize > 0x00800000)
			{
				(void) ::fprintf( stderr,
									"Read/write image from AIF file seems too big (data+code=%.8X > 8MB)\n",
									(unsigned int) theTotalSize );
				::exit( 1 );
			}
			
			theAIFFile.ReadRWImage( &theData[theROSize] );
		}
		
		::fclose( theFile );
		
		// Read first rex and put it just afterwards.
#if TARGET_OS_WIN32
		int fd = ::open( inREX0Path, O_RDONLY|O_BINARY, 0 );
#else
		int fd = ::open( inREX0Path, O_RDONLY, 0 );
#endif
		if (fd < 0)
		{
			(void) ::fprintf( stderr, "Can't open REX 0 file '%s'\n", inREX0Path );
			::exit( 1 );
		}
		
		(void) ::read(
							fd,
							(void*) &theData[theTotalSize],
							(0x00800000 - theTotalSize) );
		(void) ::close( fd );

		// Read second rex.
#if TARGET_OS_WIN32
		fd = ::open( inREX1Path, O_RDONLY|O_BINARY, 0 );
#else
		fd = ::open( inREX1Path, O_RDONLY, 0 );
#endif
		if (fd < 0)
		{
			(void) ::fprintf( stderr, "Can't open REX 1 file '%s'\n", inREX1Path );
			::exit( 1 );
		}
		
		(void) ::read(
							fd,
							(void*) &theData[0x00800000],
							0x00800000 );
		(void) ::close( fd );
		
		CreateImage( theImagePath, theData, 0x01000000, inMachineString );

		::free(theData);
	}
	
	// Finally load the image.
	Init(theImagePath, inMonitorMode);
}

// -------------------------------------------------------------------------- //
//  * ~TAIFROMImageWithREXes( void )
// -------------------------------------------------------------------------- //
TAIFROMImageWithREXes::~TAIFROMImageWithREXes( void )
{
}

// ===================================================== //
// You have a massage (from the Swedish prime minister). //
// ===================================================== //
