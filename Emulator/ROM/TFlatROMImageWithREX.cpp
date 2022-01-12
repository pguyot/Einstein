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

#include "TFlatROMImageWithREX.h"

// ANSI C & POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#if TARGET_OS_WIN32
	#include <Windows.h>
	#define PATH_MAX MAX_PATH
    #include <io.h>
#else
    #include <sys/uio.h>
    #include <unistd.h>
    #include <sys/param.h>
#endif

#if TARGET_UI_FLTK
# include "app/FLTK/TFLRexImage.h"
#endif

#include "app/Version.h"


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFlatROMImageWithREX( const char*, const char*, const char[6], ... )
// -------------------------------------------------------------------------- //
TFlatROMImageWithREX::TFlatROMImageWithREX(
			    const char* inROMPath,
			    const char* inREXPath)
{
    int romfd, rexfd = 0;
    // Open the ROM file.
#if TARGET_OS_WIN32
	romfd = ::open( inROMPath, O_RDONLY|O_BINARY, 0 );
#else
	romfd = ::open( inROMPath, O_RDONLY, 0 );
#endif
	if (romfd < 0)
	{
        ::fprintf( stderr, "Can't open ROM file '%s'\n", inROMPath );
        mErrorCode = kErrorLoadingROMFile;
        return;
	}

    struct stat theInfos;
    int err = ::fstat( romfd, &theInfos );
    if (err < 0)
    {
	(void) ::fprintf( stderr, "Can't stat ROM file '%s'\n", inROMPath );
        mErrorCode = kErrorLoadingROMFile;
        ::close(romfd);
        return;
    }
    
    // Validate size of ROM file

    if (theInfos.st_size != 0x00800000)
    {
	(void) ::fprintf( stderr, "ROM file should be 8MB long\n" );
        mErrorCode = kErrorWrongSize;
        ::close(romfd);
        return;
    }
    
    // Which has the more recent modification time: the ROM or the REX?
    // Store the most recent of the two times in theModTime

    time_t theModTime = theInfos.st_mtime;

    if (inREXPath) {
#if TARGET_OS_WIN32
        rexfd = ::open( inREXPath, O_RDONLY|O_BINARY, 0 );
#else
        rexfd = ::open( inREXPath, O_RDONLY, 0 );
#endif
        if (rexfd < 0) {
            KPrintf("Can't open Einstein REX file '%s'\n", inREXPath);
            mErrorCode = kErrorLoadingEinsteinREXFile;
            ::close(romfd);
            return;
        }

        // get the date when the REX file was last modified
        if (GetLatestModDate(&theModTime, rexfd) < 0) {
            KPrintf("Can't stat Einstein REX file (%s)\n", inREXPath);
            mErrorCode = kErrorLoadingEinsteinREXFile;
            ::close(rexfd);
            ::close(romfd);
            return;
        }
    } else {
        // if we don't have a REX file, get the date when the app was compiled
        struct tm appCompileTime = { };
        appCompileTime.tm_year = COMPILE_TIME_YYYY;
        appCompileTime.tm_mon = COMPILE_TIME_MM; 
        appCompileTime.tm_mday = COMPILE_TIME_DD;
        theModTime = ::mktime(&appCompileTime);
    }

	// Create a 16 MB buffer
    KUInt8* theData = (KUInt8*) ::calloc(1, TMemoryConsts::kROMEnd);

	// Let's read the ROM file.
	// Read the 8 MB ROM into the first half of the buffer
	if (::read(romfd, (void*) theData, 0x00800000 ) != 0x00800000)
	{
        ::close(romfd);
        if (inREXPath) {
            ::close(rexfd);
        }
        ::free(theData);
        ::fprintf( stderr, "Error while reading ROM file '%s'\n", inROMPath );
        mErrorCode = kErrorLoadingROMFile;
        return;
	}
	
	(void) ::close(romfd);
	
	// Let's read the REX (ROM Extension) file.
    if (inREXPath) {
        // load the Einstein.rex from a file
        // Read the REX into the second half of the buffer
        ::read(rexfd, (void *) &theData[0x00800000], 0x00800000);
        ::close(rexfd);
    } else {
#if TARGET_UI_FLTK
        // use the builtin Einstein.rex
        memcpy(theData + 0x00800000, gEinsteinRexImage, sizeof(gEinsteinRexImage));
#else
	// TODO: we can find the Rex in the MacOS .app resources
	::free(theData);
	mErrorCode = kErrorLoadingEinsteinREXFile;
	return;
#endif
    }
	
    CreateImage( theData );

	::free(theData);
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
