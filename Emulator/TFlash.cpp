// ==============================
// File:			TFlash.cp
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

#include "TFlash.h"

// POSIX & ANSI C
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Streams/TStream.h>

// Einstein
#include "Log/TLog.h"
#include "ROM/TROMImage.h"

// TFlash implements the persistent internal storage on the
// Newton.
//
// Einstein emulates 2 banks of 4 MB flash storage:
//
// Bank 1 is mapped to physical addresses 0x02000000 - 0x02400000
// Bank 2 is mapped to physical addresses 0x10000000 - 0x10400000
//
// When TMemory tries to access locations in these ranges, the read
// and write methods in this class will be called.

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define REX_Csum1	0x2E79C311
#define REX_Csum2	0x47566032

// -------------------------------------------------------------------------- //
//  * TFlash( TLog*, const char* )
// -------------------------------------------------------------------------- //
TFlash::TFlash(
			TLog* inLog,
			const char* inFlashPath,
			TROMImage* inROMImage )
	:
		mLog( inLog ),
		mFlashFile(
			inFlashPath,
			kFlashBank1Size + kFlashBank2Size,
			O_RDWR | O_CREAT ),
		mFlash( (KUInt8*) mFlashFile.GetBuffer() )
{
	KUInt32 checksums[10];
	Boolean corruptedRex = false;
	if (inROMImage)
	{
		// Update the checksums if required.
		inROMImage->ComputeChecksums( checksums );
	
		// Check Einstein's rex checksum.
		if ((checksums[4] != REX_Csum1)
			|| (checksums[5] != REX_Csum2))
		{
/*
 * This is generating a false "REx seems corrupted" message on Ubuntu?? -- stevenf
 *
#ifdef RELEASE
			corruptedRex = ((checksums[4] != REX_Csum1)
							|| (checksums[5] != REX_Csum2));

#else
*/
//			for (int indexChecksum = 0; indexChecksum < 10; indexChecksum++)
//			{
//				(void) ::fprintf( stderr, "Checksum-%i: %.8X\n",
//					indexChecksum,
//					(unsigned int) checksums[indexChecksum] );
//			}
/*
#endif
*/
		}
	}

	Boolean nullChecksums = false;
	if (inROMImage)
	{
		// Update the checksums if required.
		KUInt32 indexChecksum;
		for (indexChecksum = 0; indexChecksum < 10; indexChecksum++)
		{
			KUInt32 theWord = Read( 0x64 + (4*indexChecksum), 0 );
			if (theWord != checksums[indexChecksum])
			{
				if (theWord == 0)
				{
					nullChecksums = true;
				}
				
				if (mLog)
				{
					mLog->FLogLine(
							"Checksum mismatch %i:%.8X-%.8X",
							(int) indexChecksum,
							(unsigned int) theWord,
							(unsigned int) checksums[indexChecksum]);
				}
				
				Write(
					checksums[indexChecksum],
					0xFFFFFFFF,
					0x64 + (4*indexChecksum),
					0 );
			}
		}
	}

	if (nullChecksums || mFlashFile.GetCreated())
	{
		if(mLog)
		{
			mLog->LogLine("Starting from an empty flash");
		}

		// Fill the empty flash with proper data on block 0.
		Write( 0x444C4453, 0xFFFFFFFF, 0x00000000, 0 );	// sig?
		Write( 0x4F534344, 0xFFFFFFFF, 0x00000004, 0 );	// sig?
		Write( 0x0000010C, 0xFFFFFFFF, 0x00000008, 0 );	// block size? offset to block 1?
		Write( 0x00003916, 0xFFFFFFFF, 0x00000024, 0 );	// calibration data (?)
		Write( 0x0000465A, 0xFFFFFFFF, 0x00000034, 0 );	// calibration data (?)
		Write( 0x00008000, 0xFFFFFFFF, 0x0000003C, 0 );	// calibration data (?)
		Write( 0x00000000, 0xFFFFFFFF, 0x00000040, 0 );	// manufacture date
		Write( 0x444C4453, 0xFFFFFFFF, 0x00000050, 0 );	// copy of sig
		Write( 0xD7ECCC66, 0xFFFFFFFF, 0x00000054, 0 ); // some checksum
		Write( 0xFFFFFFFC, 0xFFFFFFFF, 0x00000058, 0 ); // some number
		Write( 0xFFFFFFFF, 0xFFFFFFFF, 0x0000008C, 0 ); // if calibration is valid

		// Same on block 1.
		Write( 0x444C4453, 0xFFFFFFFF, 0x00010000, 0 );	// sig?
		Write( 0x4F534344, 0xFFFFFFFF, 0x00010004, 0 );	// sig?
		Write( 0x0000010C, 0xFFFFFFFF, 0x00010008, 0 );	// block size? offset to block 1?
		Write( 0x00003916, 0xFFFFFFFF, 0x00010024, 0 );	// calibration data (?)
		Write( 0x0000465A, 0xFFFFFFFF, 0x00010034, 0 );	// calibration data (?)
		Write( 0x00008000, 0xFFFFFFFF, 0x0001003C, 0 );	// calibration data (?)
		Write( 0x00000000, 0xFFFFFFFF, 0x00010040, 0 );	// manufacture date
		Write( 0x444C4453, 0xFFFFFFFF, 0x00010050, 0 );	// copy of sig
		Write( 0xD7ECCC66, 0xFFFFFFFF, 0x00010054, 0 ); // some checksum
		Write( 0xFFFFFFF0, 0xFFFFFFFF, 0x00010058, 0 ); // some number
		Write( 0xFFFFFFFF, 0xFFFFFFFF, 0x0001008C, 0 ); // if calibration is valid
		
		// We save.
		Save();
	}
	

	if (corruptedRex)
	{
		(void) ::fprintf( stderr, "Einstein REX seems corrupted\n" );
		::abort();
	}
}

// -------------------------------------------------------------------------- //
//  * ~TFlash( void )
// -------------------------------------------------------------------------- //
TFlash::~TFlash( void )
{
}

// -------------------------------------------------------------------------- //
//  * Write( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TFlash::Write(
				KUInt32 inWord,
				KUInt32 inMask,
				KUInt32 inOffset,
				KUInt32 inBank )
{
	KUInt32* thePointer;
	if (inBank == 0)
	{
		thePointer = (KUInt32*)
			((KUIntPtr) mFlash + inOffset);
	} else {
		thePointer = (KUInt32*)
			((KUIntPtr) mFlash + kFlashBank1Size + inOffset);
	}
	*thePointer = UByteSex_ToBigEndian(
		(UByteSex_FromBigEndian(*thePointer) & ~inMask) | inWord);
}

// -------------------------------------------------------------------------- //
//  * Erase( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TFlash::Erase(
				KUInt32 inSize,
				KUInt32 inOffset,
				KUInt32 inBank )
{
	KUInt32* thePointer;
	if (inBank == 0)
	{
		thePointer = (KUInt32*)
			((KUIntPtr) mFlash + inOffset);
	} else {
		thePointer = (KUInt32*)
			((KUIntPtr) mFlash + kFlashBank1Size + inOffset);
	}

	KUInt32 wordCount = inSize / 4;
	while (wordCount > 0)
	{
		*thePointer = 0xFFFFFFFF;
		thePointer++;
		wordCount--;
	}
}

// -------------------------------------------------------------------------- //
//  * Read( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TFlash::Read(
				KUInt32 inOffset,
				KUInt32 inBank )
{
	// Read 32-bit value from inOffset in bank inBank

	KUInt32* thePointer;
	if (inBank == 0)
	{
		thePointer = (KUInt32*)
			((KUIntPtr) mFlash + inOffset);
	} else {
		thePointer = (KUInt32*)
			((KUIntPtr) mFlash + kFlashBank1Size + inOffset);
	}

	return UByteSex_FromBigEndian( *thePointer );
}

// -------------------------------------------------------------------------- //
//  * ReadB( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TFlash::ReadB(
				KUInt32 inOffset,
				KUInt32 inBank )
{
	// Read 8-bit value from inOffset in bank inBank

	KUInt32 theWord = Read(inOffset & ~ 0x03, inBank);
	KUInt8 theResult;
	switch (inOffset & 0x03)
	{
		case 0x0:
			theResult = (theWord >> 24) & 0xFF;
			break;

		case 0x1:
			theResult = (theWord >> 16) & 0xFF;
			break;

		case 0x2:
			theResult = (theWord >> 8) & 0xFF;
			break;

		case 0x3:
		default:
			theResult = theWord & 0xFF; 
	}
	
	return theResult;
}


// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TFlash::TransferState( TStream* inStream )
{
	if (inStream->IsWriting())
		mFlashFile.Sync();

	// The flash.
	KUInt32 FlashSize = kFlashBank1Size + kFlashBank2Size;
	inStream->TransferInt32ArrayBE(
								   (KUInt32*) mFlash,
								   FlashSize / sizeof( KUInt32 ) );

	if (inStream->IsReading())
		mFlashFile.Sync();
}


// -------------------------------------------------------------------------- //
//  * Save( void ) const
// -------------------------------------------------------------------------- //
void
TFlash::Save( void ) const
{
	mFlashFile.Sync();
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void ) const
// -------------------------------------------------------------------------- //
void
TFlash::PowerOn( void ) const
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOff( void ) const
// -------------------------------------------------------------------------- //
void
TFlash::PowerOff( void ) const
{
	Save();
}

// =================================== //
// The world is not octal despite DEC. //
// =================================== //
