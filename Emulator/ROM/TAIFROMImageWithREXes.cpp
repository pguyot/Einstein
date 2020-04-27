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

#include "TAIFROMImageWithREXes.h"

// ANSI C & POSIX
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#if TARGET_OS_WIN32
#include <Windows.h>
//#include "CompatibilityWin32.h"
#include <io.h>
#define PATH_MAX MAX_PATH
#else
#include <sys/uio.h>
#include <unistd.h>
#include <sys/param.h>
#endif

// K
#include <K/Defines/UByteSex.h>

// Einstein
#include "TAIFFile.h"
#include "Drivers/EinsteinRex.h"
#include "app/Version.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TAIFROMImageWithREXes( const char*, const char*, const char*, ... )
// -------------------------------------------------------------------------- //
TAIFROMImageWithREXes::TAIFROMImageWithREXes(const char* inAIFPath,
                                             const char* inREX0Path,
                                             const char* inREX1Path)
{
    // Check the modification dates.
    struct stat theInfos;
    int err = ::stat( inAIFPath, &theInfos );
    if (err < 0)
    {
        (void) ::fprintf( stderr, "Can't stat AIF file (%s)\n", inAIFPath );
        mErrorCode = kErrorLoadingROMFile;
        return;
    }

    time_t theModDate = theInfos.st_mtime;

    if (GetLatestModDate( &theModDate, inREX0Path ) < 0)
    {
        (void) ::fprintf( stderr, "Can't stat REX0 file (%s)\n", inREX0Path );
        mErrorCode = kErrorLoadingNewtonREXFile;
        return;
    }
    if (inREX1Path) {
        // get the date when the REX file was last modified
        if (GetLatestModDate(&theModDate, inREX1Path) < 0) {
            (void) ::fprintf(stderr, "Can't stat REX1 file (%s)\n", inREX1Path);
            mErrorCode = kErrorLoadingEinsteinREXFile;
            return;
        }
    } else {
        // if we don't have a REX file, get the date when the app was compiled
        struct tm appCompileTime = { };
        appCompileTime.tm_year = COMPILE_TIME_YYYY;
        appCompileTime.tm_mon = COMPILE_TIME_MM;
        appCompileTime.tm_mday = COMPILE_TIME_DD;
        theModDate = ::mktime(&appCompileTime);
    }

    // A priori, on a 16 Mo de ROM.
    KUInt8* theData = (KUInt8*) ::malloc(TMemoryConsts::kROMEnd);
    memset(theData, 0xff, TMemoryConsts::kLowROMEnd); // default fill is 0xff

    // On ouvre le fichier ROM.
    FILE* theFile = ::fopen(inAIFPath, "rb");
    if (theFile == NULL)
    {
        (void) ::fprintf( stderr, "Can't open AIF file '%s'\n", inAIFPath );
        ::fclose( theFile );
        ::free(theData);
        mErrorCode = kErrorLoadingROMFile;
        return;
    }

    KUInt32 theTotalSize;

    TAIFFile theAIFFile( theFile );

    // Read the R/O part of the file.
    KUInt32 theROSize = theAIFFile.GetROImageSize();
    if (theROSize > 0x00800000)
    {
        (void) ::fprintf( stderr,
                         "Read/only image from AIF file seems too big (%.8X > 8MB)\n",
                         (unsigned int) theROSize );
        ::fclose( theFile );
        ::free(theData);
        mErrorCode = kErrorWrongSize;
        return;
    }

    theAIFFile.ReadROImage( theData );

    // Read the R/W part of the file.
    theTotalSize = theROSize + theAIFFile.GetRWImageSize();
    if (theTotalSize > 0x00800000)
    {
        (void) ::fprintf( stderr,
                         "Read/write image from AIF file seems too big (data+code=%.8X > 8MB)\n",
                         (unsigned int) theTotalSize );
        ::fclose( theFile );
        ::free(theData);
        mErrorCode = kErrorLoadingROMFile;
        return;
    }

    theAIFFile.ReadRWImage( &theData[theROSize] );

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
        ::free(theData);
        mErrorCode = kErrorLoadingEinsteinREXFile;
        return;
    }

    (void) ::read(
                  fd,
                  (void*) &theData[theTotalSize],
                  (0x00800000 - theTotalSize) );
    (void) ::close( fd );

    // Read second rex.
    if (inREX1Path==nullptr) {
        // use the builtin Einstein.rex
        memcpy(theData+0x00800000, Einstein_rex, Einstein_rex_len);
    } else {
        // load the Einstein.rex from a file
#if TARGET_OS_WIN32
        fd = ::open( inREX1Path, O_RDONLY|O_BINARY, 0 );
#else
        fd = ::open(inREX1Path, O_RDONLY, 0);
#endif
        if (fd < 0) {
            (void) ::fprintf(stderr, "Can't open REX 1 file '%s'\n", inREX1Path);
            ::free(theData);
            mErrorCode = kErrorLoadingEinsteinREXFile;
            return;
        }

        (void) ::read(
                      fd,
                      (void *) &theData[0x00800000],
                      0x00800000);
        (void) ::close(fd);
    }

    CreateImage( theData );

    ::free(theData);
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
