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
#include "Emulator/NativeCalls/TVirtualizedCallsPatches.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
const KUInt32 kInvocation[4] = {
	UByteSex_FromBigEndian((KUInt32) 0xE92D4000),
	UByteSex_FromBigEndian((KUInt32) 0xE59FE004),
	UByteSex_FromBigEndian((KUInt32) 0xEE00EA10),
	UByteSex_FromBigEndian((KUInt32) 0xE8BD8000)
};

const KUInt32 k717006VirtualizationPatches[] = {
	// __rt_sdiv + 8, just after the divide by zero test.
	(0x0038CA10 + 8) / sizeof(KUInt32),
	TVirtualizedCallsPatches::k__rt_sdiv,
	// __rt_udiv
	(0x0038C8FC + 8) / sizeof(KUInt32),
	TVirtualizedCallsPatches::k__rt_udiv,
	// memmove
  //	0x00382440 / sizeof(KUInt32),
  //	TVirtualizedCallsPatches::kmemmove,
	// symcmp__FPcT1
	0x00358C9C / sizeof(KUInt32),
	TVirtualizedCallsPatches::ksymcmp__FPcT1
};	

// avoid calibration screen early in the game
TROMPatch p001412f8(0x001412f8, 0xea000009, "Avoid screen calibration");
// disable "FlushDCache" since we do not emulate a DCache 
TROMPatch p00018994(0x00018994, 0xe1a0f00e, "Obsolete FlushDCache"); // #  mov pc, lr
// disable "CleanPageInDCache" since we do not emulate a DCache 
TROMPatch p0001894c(0x0001894c, 0xe1a0f00e, "Obsolete CleanPageInDCache"); // #  mov pc, lr
// disable "TGeoPortDebugLink::BeaconDetect(long)"
TROMPatch p000db0d8(0x000db0d8, 0xe3a00000, "BeaconDetect (1/2)"); // #  mov r0, 0x00000000
TROMPatch p000db0dc(0x000db0dc, 0xe1a0f00e, "BeaconDetect (2/2)"); // #  mov pc, lr


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
#endif

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
		KUInt32 buffer;
		
		if (((KUInt32) ::read( fd, &buffer, sizeof(buffer) )) != sizeof(buffer))
		{
			// Can't read.
			(void) ::close( fd );
			break;
		}
		
		if (buffer != kMagic)
		{
			// Mismatch (endian-ness problem?)
			(void) ::close( fd );
			break;
		}

		if (((KUInt32) ::read( fd, &buffer, sizeof(buffer) )) != sizeof(buffer))
		{
			// Can't read.
			(void) ::close( fd );
			break;
		}
		
		if (buffer != kVersion)
		{
			// Mismatch
			(void) ::close( fd );
			break;
		}
		
		// Read the machine string.
		char machineStrBuffer[6];
		if (((KUInt32) ::read( fd, machineStrBuffer, 6 )) != 6)
		{
			// Can't read.
			(void) ::close( fd );
			break;
		}

		if (memcmp(machineStrBuffer, inMachineString, 6))
		{
			(void) ::close( fd );
			break;
		}
		
		// Good.
		(void) ::close( fd );

		result = false;
	} while ( false );
	
	return result;
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
	(void) ::fprintf( stderr, "Creating image...\n" );

	// Create the mmap file.
	TMappedFile theImageFile(
							inPath,
							sizeof(SImage),
							O_RDWR | O_CREAT );
	
	SImage* theImagePtr = (SImage*) theImageFile.GetBuffer();
	if (theImagePtr == NULL)
	{
		::fprintf( stderr, "Can't create the image at %s\n", inPath );
		::exit(1);
	}
	
	// Fill with zeroes.
	bzero(theImagePtr, sizeof(theImagePtr));
	
	// Swap/copy the image.
#if TARGET_RT_LITTLE_ENDIAN
	{
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
	theImagePtr->fMagic = kMagic;
	theImagePtr->fVersion = kVersion;
	
	// Copy the machine string.
	(void) ::memcpy(
				theImagePtr->fMachineString,
				inMachineString,
				6);
	
	// This is where I could patch the ROM.
	PatchROM(theImagePtr);
	
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
		::fprintf( stderr, "Can't load the image at %s\n", inPath );
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
		outChecksums[index] = mImage->fChecksums[index];
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
	ComputeSegmentChecksums( thePointer, theBaseSize, &inImage->fChecksums[0] );
	
	// Compute REX checksums.
	KUInt32 indexRexes;
	for (indexRexes = 0; indexRexes < 4; indexRexes++)
	{
		ComputeSegmentChecksums(
			thePointer + theRexBases[indexRexes],
			theRexSizes[indexRexes],
			&inImage->fChecksums[(2*indexRexes)+2] );
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

// -------------------------------------------------------------------------- //
//  * PatchROM( SImage* )
// -------------------------------------------------------------------------- //
void
TROMImage::PatchROM( SImage* inImagePtr )
{
	if (::memcmp(inImagePtr->fMachineString, "717006", 6) == 0)
	{
		// Patch for 717006.
		DoPatchROMFromDatabase(inImagePtr);
		// Patch for 717006.
		DoPatchROMVirtualizationCalls(
               inImagePtr,
               k717006VirtualizationPatches,
               (sizeof(k717006VirtualizationPatches) / (sizeof(KUInt32) * 2)));

	}
}

// -------------------------------------------------------------------------- //
//  * DoPatchROMSimple( SImage* )
// -------------------------------------------------------------------------- //
void
TROMImage::DoPatchROMFromDatabase( SImage* inImagePtr )
{
	// Get a pointer to the ROM.
	KUInt32* thePointer = (KUInt32*) inImagePtr->fROM;
	
	// Iterate on patches.
  TROMPatch *p;
  for (p=TROMPatch::first(); p; p=p->next()) 
  {
		KUInt32 address = p->address();
		KUInt32 value = p->value();
		thePointer[address] = value;
  }
}

// -------------------------------------------------------------------------- //
//  * DoPatchROMVirtualizationCalls( SImage* )
// -------------------------------------------------------------------------- //
void
TROMImage::DoPatchROMVirtualizationCalls( SImage* inImagePtr, const KUInt32* inPatches, KUInt32 inCount )
{
	// Get a pointer to the ROM.
	KUInt32* thePointer = (KUInt32*) inImagePtr->fROM;
	
	// Iterate on patches.
	KUInt32 indexPatches;
	for (indexPatches = 0; indexPatches < inCount; indexPatches++)
	{
		// Patch.
		// Write all 5 words there.
		KUInt32 address = inPatches[2*indexPatches];
		KUInt32 value = inPatches[(2*indexPatches) + 1] | 0x80000000;
    
		thePointer[address++] = UByteSex_ToBigEndian( kInvocation[0] );
		thePointer[address++] = UByteSex_ToBigEndian( kInvocation[1] );
		thePointer[address++] = UByteSex_ToBigEndian( kInvocation[2] );
		thePointer[address++] = UByteSex_ToBigEndian( kInvocation[3] );
		thePointer[address] = value;
	}
}

// -------------------------------------------------------------------------- //
//  * TROMPatch first member of database
// -------------------------------------------------------------------------- //
TROMPatch *TROMPatch::first_ = 0L;
JITFuncPtr *TROMPatch::stub_ = 0;
KUInt32 TROMPatch::nStub = 0, TROMPatch::NStub = 0;

// -------------------------------------------------------------------------- //
//  * TROMPatch constructor
// -------------------------------------------------------------------------- //
TROMPatch::TROMPatch(KUInt32 addr, KUInt32 val, const char *name)
: next_(first_),
  address_(addr>>2),
  value_(val)
{
  first_ = this;
  printf("Adding ROM patch: %s\n", name);
}

// -------------------------------------------------------------------------- //
//  * TROMPatch constructor for Albert calls
// -------------------------------------------------------------------------- //
TROMPatch::TROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name)
: next_(first_),
  address_(addr>>2),
  value_(0xef800000)
{
  first_ = this;
  printf("Adding ROM patch linking into Albert: %3d = %s\n", (int)nStub, name);
  value_ |= addStub(stub);
}

// -------------------------------------------------------------------------- //
//  * Call a native stub in Albert
// -------------------------------------------------------------------------- //
JITFuncPtr TROMPatch::albertStub(KUInt32 index) 
{
  if (index>=nStub)
    return 0L;
  return stub_[index];
}

// -------------------------------------------------------------------------- //
//  * Add another stub to the array of stubs and return the index
// -------------------------------------------------------------------------- //
KUInt32 TROMPatch::addStub(JITFuncPtr stub)
{
  if (nStub==NStub) {
    NStub += 1024;
    stub_ = (JITFuncPtr*)realloc(stub_, NStub*sizeof(JITFuncPtr));
  }
  stub_[nStub++] = stub;
  return (nStub-1);
}

// ====================================================== //
// Is a computer language with goto's totally Wirth-less? //
// ====================================================== //
