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
//    #include "CompatibilityWin32.h"
#include <io.h>
#else
#include <sys/uio.h>
#include <unistd.h>
#include <sys/param.h>
#endif

#include "Drivers/EinsteinRex.h"
#include "app/Version.h"


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFlatROMImageWithREX( const char*, const char*, const char[6], ... )
// -------------------------------------------------------------------------- //
TFlatROMImageWithREX::TFlatROMImageWithREX(
                                           const char* inROMPath,
                                           const char* inREXPath,
                                           const char *inImagePath /* 0L */ )
{
    struct stat theInfos;
    int err = ::stat( inROMPath, &theInfos );
    if (err < 0)
    {
        (void) ::fprintf( stderr, "Can't stat ROM file '%s'\n", inROMPath );
        ::exit( 1 );
    }

    // Validate size of ROM file

    if (theInfos.st_size != 0x00800000)
    {
        (void) ::fprintf( stderr, "ROM file should be 8MB long\n" );
        ::exit( 1 );
    }

    // Which has the more recent modification time: the ROM or the REX?
    // Store the most recent of the two times in theModTime

    time_t theModTime = theInfos.st_mtime;

    if (inREXPath) {
        // get the date when the REX file was last modified
        if (GetLatestModDate(&theModTime, inREXPath) < 0) {
            (void) ::fprintf(stderr, "Can't stat REX file (%s)\n", inREXPath);
            ::exit(1);
        }
    } else {
        // if we don;t have a REX file, get the date when the app was compiled
        struct tm appCompileTime = { };
        appCompileTime.tm_year = COMPILE_TIME_YYYY;
        appCompileTime.tm_mon = COMPILE_TIME_MM;
        appCompileTime.tm_mday = COMPILE_TIME_DD;
        theModTime = ::mktime(&appCompileTime);
    }

    // Create the image path. We're going to create a separate image of
    // the ROM at this path.

    char theImagePath[PATH_MAX];
    if (inImagePath)
        strcpy(theImagePath, inImagePath);
    else
        (void) ::sprintf( theImagePath, "%s.img", inROMPath );

    // Check if we need to re-create the image file

    // Create a 16 MB buffer

    KUInt8* theData = (KUInt8*) ::calloc(1, TMemoryConsts::kROMEnd);

    // Let's read the ROM file.
#if TARGET_OS_WIN32
    int fd = ::open( inROMPath, O_RDONLY|O_BINARY, 0 );
#else
    int fd = ::open( inROMPath, O_RDONLY, 0 );
#endif
    if (fd < 0)
    {
        (void) ::fprintf( stderr, "Can't open ROM file '%s'\n", inROMPath );
        ::exit( 1 );
    }

    // Read the 8 MB ROM into the first half of the buffer

    if (::read(fd, (void*) theData, 0x00800000 ) != 0x00800000)
    {
        (void) ::close( fd );
        (void) ::fprintf( stderr, "Error while reading ROM file '%s'\n", inROMPath );
        ::exit( 1 );
    }

    (void) ::close( fd );

    // Let's read the REX (ROM Extension) file.
    if (inREXPath==nullptr) {
        // use the builtin Einstein.rex
        memcpy(theData+0x00800000, Einstein_rex, Einstein_rex_len);
    } else {
        // load the Einstein.rex from a file
#if TARGET_OS_WIN32
        fd = ::open( inREXPath, O_RDONLY|O_BINARY, 0 );
#else
        fd = ::open(inREXPath, O_RDONLY, 0);
#endif
        if (fd < 0) {
            (void) ::fprintf(stderr, "Can't open REX file '%s'\n", inREXPath);
            ::exit(1);
        }

        // Read the REX into the second half of the buffer

        (void) ::read(fd, (void *) &theData[0x00800000], 0x00800000);
        (void) ::close(fd);
    }
    
    // The .img file consists of:
    // - The ROM (8MB)
    // - The REX (8MB)
    // - Some metadata (a magic number, ROM version string, padding)
    // - Checksums

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
