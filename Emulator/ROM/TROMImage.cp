// ==============================
// File:			TROMImage.cp
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
#include "TROMImage.h"

// ANSI C & POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#if TARGET_OS_WIN32
	#include "CompatibilityWin32.h"
	#include <io.h>
#else
	#include <sys/uio.h>
	#include <unistd.h>
#endif

// K
#include <K/Misc/TMappedFile.h>
#include <K/Defines/UByteSex.h>

// Einstein
#include "TMemoryConsts.h"

#include "TMemory.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"

#include "TEmulator.h"
#include "TScreenManager.h"

// -------------------------------------------------------------------------- //
//  * TROMImage( void )
// -------------------------------------------------------------------------- //
TROMImage::TROMImage( void )
	:
		mMappedFile( NULL ),
		mImage( NULL )
{
	// I'll create the mmap file later, when asked to.
}

// -------------------------------------------------------------------------- //
//  * ~TROMImage( void )
// -------------------------------------------------------------------------- //
TROMImage::~TROMImage( void )
{
	if (mMappedFile)
	{
		delete mMappedFile;
		mMappedFile = NULL;
		mImage = NULL;
	} else if (mImage) {
		::free( mImage );
		mImage = NULL;
	}
}

// -------------------------------------------------------------------------- //
//  * IsImageOutdated( const char*, time_t )
// -------------------------------------------------------------------------- //
Boolean
TROMImage::IsImageOutdated(
				const char* inPath,
				time_t inModDate,
				const char inMachineString[6] )
{
	Boolean result = true;

#ifdef _DEBUG
	// allow patching the ROM at every run
	return result;
#else  // not _DEBUG
	do {
		// Check the file exists.
		struct stat theInfos;
		int err = ::stat( inPath, &theInfos );
		if (err < 0)
		{
			// The file (probably) doesn't exist.
			break;
		}
		
		// Check the modification date.
		if (theInfos.st_mtime < inModDate)
		{
			// The file is older.
			break;
		}

		// Check the size.
		if (theInfos.st_size != sizeof(SImage))
		{
			// The size doesn't match.
			break;
		}
		
		// Read magic & the version.
#if TARGET_OS_WIN32
		int fd = ::open( inPath, O_RDONLY|O_BINARY, 0 );
#else
		int fd = ::open( inPath, O_RDONLY, 0 );
#endif
		if (fd < 0)
		{
			// Can't open the file.
			break;
		}
		
		(void) ::lseek( fd, TMemoryConsts::kHighROMEnd, SEEK_SET );
		SImageInfo info;
		
		if (((KUInt32) ::read( fd, &info, sizeof(info) )) != sizeof(info)) {
			// Can't read.
			(void) ::close( fd );
			break;
		}
		if ((info.fMagic != kMagic)
		    || (info.fVersion != kVersion)
		    || (info.fJITID != JITClass::GetID())
		    || (info.fJITVersion != JITClass::GetVersion())
		    || memcmp(info.fMachineString, inMachineString, 6)) {
			// Mismatch
			(void) ::close( fd );
			break;
		}

		// Good.
		(void) ::close( fd );

		result = false;
	} while ( false );
	
	return result;
#endif // _DEBUG
}

// -------------------------------------------------------------------------- //
//  * CreateImage( const char*, const KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TROMImage::CreateImage(
				const char* inPath,
				const KUInt8* inBuffer,
				KUInt32 inBufferSize,
				const char inMachineString[6] )
{
	fprintf(stderr, "Creating image from ROM and REX...\n" );

	// Create the mmap file.
	TMappedFile theImageFile(
							inPath,
							sizeof(SImage),
							O_RDWR | O_CREAT );
	
	SImage* theImagePtr = (SImage*) theImageFile.GetBuffer();
	if (theImagePtr == NULL)
	{
		fprintf(stderr, "Can't create the image at %s\n", inPath );
		::exit(1);
	}
	
	// Fill with zeroes.
	bzero(theImagePtr, theImageFile.GetSize());
	
	// inBuffer contains 16 MB consisting of the ROM followed by the REX.
	// Write this at the start of the image.
	
#if TARGET_RT_LITTLE_ENDIAN
	{
		// Endian swap it first
		
		KUInt32* src = (KUInt32*) inBuffer;
		KUInt32* dest = (KUInt32*) theImagePtr;
		KUInt32* end = (KUInt32*) &inBuffer[inBufferSize];
		do {
			*dest = UByteSex::Swap( *src );
			++dest;
		} while (++src < end);
	}
#else
	(void) ::memcpy(theImagePtr, inBuffer, inBufferSize);
#endif

	// Set the magic & the version.
	theImagePtr->fInfo.fMagic = kMagic;
	theImagePtr->fInfo.fVersion = kVersion;
	theImagePtr->fInfo.fJITID = JITClass::GetID();
	theImagePtr->fInfo.fJITVersion = JITClass::GetVersion();
	
	// Copy the machine string.
	(void) ::memcpy(
				theImagePtr->fInfo.fMachineString,
				inMachineString,	// like "717006"
				6);
	
	JITClass::PatchROM((KUInt32*) theImagePtr->fROM, inMachineString);
	
	// Compute the checksum.
	DoComputeChecksums(theImagePtr);
}

// -------------------------------------------------------------------------- //
//  * Init( const char*, Boolean )
// -------------------------------------------------------------------------- //
void
TROMImage::Init( const char* inPath, Boolean inMonitorMode )
{
	// Create the mmap file.
	TMappedFile* theMappedFile = new TMappedFile(
							inPath,
							sizeof(SImage) );
	SImage* theImage = (SImage*) theMappedFile->GetBuffer();
	if (theImage == NULL)
	{
		fprintf(stderr, "Can't load the image at %s\n", inPath );
		::exit(1);
	}

	if (inMonitorMode)
	{
		mImage = (SImage*) ::malloc( sizeof(SImage) );
		(void) ::memcpy( mImage, theImage, sizeof(SImage) );
		delete theMappedFile;
	} else {
		mMappedFile = theMappedFile;
		mImage = theImage;
	}
}

// -------------------------------------------------------------------------- //
//  * GetLatestModDate( struct timespec*, const char* )
// -------------------------------------------------------------------------- //
int
TROMImage::GetLatestModDate(
				time_t* ioModDate,
				const char* inPath )
{
	int err;
	do {
		// Check the file exists.
		struct stat theInfos;
		err = ::stat( inPath, &theInfos );
		if (err < 0)
		{
			// The file (probably) doesn't exist.
			break;
		}
		
		// Compare with ioModDate and update it if what we have is newer.
		if (theInfos.st_mtime > *ioModDate)
		{
			*ioModDate = theInfos.st_mtime;
		}
	} while (false);
	
	return err;
}

// -------------------------------------------------------------------------- //
//  * ComputeChecksums( KUInt32 outChecksums[10] ) const
// -------------------------------------------------------------------------- //
void
TROMImage::ComputeChecksums( KUInt32 outChecksums[10] ) const
{
	// Copy checksum from the SImagePtr.
	KUInt32 index;
	for (index = 0; index < 10; index++)
	{
		outChecksums[index] = mImage->fInfo.fChecksums[index];
	}
}

// -------------------------------------------------------------------------- //
//  * DoComputeChecksums( SImage* )
// -------------------------------------------------------------------------- //
void
TROMImage::DoComputeChecksums( SImage* inImage )
{
	const KUInt8* thePointer = inImage->fROM;

	// Find RExes.
	KUInt32 theBaseSize;
	KUInt32 theRexBases[4];
	KUInt32 theRexSizes[4];
	
	KUInt32 nbRexes = LookForREXes(
						thePointer,
						&theBaseSize,
						theRexBases,
						theRexSizes );
	for (; nbRexes < 4; nbRexes++)
	{
		theRexBases[nbRexes] = 0;
		theRexSizes[nbRexes] = 0;
	}

	// Compute base checksum.
	ComputeSegmentChecksums( thePointer, theBaseSize, &inImage->fInfo.fChecksums[0] );
	
	// Compute REX checksums.
	KUInt32 indexRexes;
	for (indexRexes = 0; indexRexes < 4; indexRexes++)
	{
		ComputeSegmentChecksums(
			thePointer + theRexBases[indexRexes],
			theRexSizes[indexRexes],
			&inImage->fInfo.fChecksums[(2*indexRexes)+2] );
	}
}

// -------------------------------------------------------------------------- //
//  * ComputeSegmentChecksums( KUInt32 outChecksums[10] )
// -------------------------------------------------------------------------- //
void
TROMImage::ComputeSegmentChecksums(
						const KUInt8* inPointer,
						KUInt32 inSize,
						KUInt32 outChecksums[2] )
{
	// if the segment is empty, the checksums are -1, -1.
	if (inSize == 0)
	{
		outChecksums[0] = 0xFFFFFFFF;
		outChecksums[1] = 0xFFFFFFFF;
	} else {
		KUInt32* theCursor = (KUInt32*) inPointer;
		KUInt32* theEnd = (KUInt32*) (inPointer + inSize);
		KUInt32 highBits = 0;
		KUInt32 lowBits = 0;
		do
		{
			KUInt32 value = *theCursor++;
			lowBits += value & 0x0000FFFF;
			highBits += value >> 16;
		} while (theCursor != theEnd);
		
		outChecksums[0] = highBits;
		outChecksums[1] = lowBits;
	}
}

// -------------------------------------------------------------------------- //
//  * LookForREXes( const KUInt8*, KUInt32*, KUInt32*, KUInt32* )
// -------------------------------------------------------------------------- //
KUInt32
TROMImage::LookForREXes(
					const KUInt8* inBuffer,
					KUInt32* outBaseSize,
					KUInt32 outRexBases[4],
					KUInt32 outRexSizes[4] )
{
	KUInt32* swappedROM = (KUInt32*) inBuffer;
	// The size of the base ROM.
	KUInt32 theBaseSize = swappedROM[0x3C/4];
	if (theBaseSize >= 0x00800000)
	{
		(void) ::fprintf(
					stderr,
					"Couldn't determine base ROM size (bad ROM?)\n" );
		::abort();
	}

	*outBaseSize = theBaseSize;
	
	// Look for rexes just afterwards.
	KUInt32 nbRexes = 0;
	KUInt32 rexCursor = theBaseSize;
	do {
		if (rexCursor > 0x00800000 - 0x20)
		{
			break;
		}
		if ((swappedROM[rexCursor / 4] == 'RExB')
			&& (swappedROM[(rexCursor / 4) + 1] == 'lock'))
		{
			// Found a rex.
			KUInt32 theRexSize = swappedROM[(rexCursor + 0x18) / 4];
			if (
				(theRexSize < 0x20)
				|| (rexCursor + theRexSize > 0x00800000))
			{
				(void) ::fprintf(
							stderr,
							"Invalid size for REx %i at %.8X\n",
							(int) nbRexes,
							(unsigned int) rexCursor);
				::abort();
			}
			
			outRexBases[nbRexes] = rexCursor;
			outRexSizes[nbRexes++] = theRexSize;
			rexCursor += theRexSize;
		} else {
			break;
		}
	} while (nbRexes < 4);
	
	// Look for rexes at 0x00800000.
	if ((nbRexes < 4) && (rexCursor < 0x00800000))
	{
		rexCursor = 0x00800000;

		do {
			if ((swappedROM[rexCursor / 4] == 'RExB')
				&& (swappedROM[(rexCursor / 4) + 1] == 'lock'))
			{
				// Found a rex.
				KUInt32 theRexSize = swappedROM[(rexCursor + 0x18) / 4];
				if (
					(theRexSize < 0x20)
					|| (rexCursor + theRexSize > 0x01000000))
				{
					(void) ::fprintf(
								stderr,
								"Invalid size for REx %i at %.8X\n",
								(int) nbRexes,
								(unsigned int) rexCursor);
					::abort();
				}
				
				outRexBases[nbRexes] = rexCursor;
				outRexSizes[nbRexes++] = theRexSize;
				rexCursor += theRexSize;
			} else {
				break;
			}
		} while (true);
	}

	return nbRexes;
}

// ====================================================== //
// Is a computer language with goto's totally Wirth-less? //
// ====================================================== //
