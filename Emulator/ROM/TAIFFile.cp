// ==============================
// File:			TAIFFile.h
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
#include "TAIFFile.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <stdio.h>

// K
#include <K/Defines/UByteSex.h>

// -------------------------------------------------------------------------- //
//  * TAIFFile( FILE* inFile )
// -------------------------------------------------------------------------- //
TAIFFile::TAIFFile( FILE* inFile )
	:
		mFile( inFile )
{
	long startOffset = 0;
	(void) ::fseek( inFile, startOffset, SEEK_SET );

	if ( ::fread( &mHeader, sizeof( mHeader ), 1, inFile ) != 1)
	{
		(void) ::fprintf(
					stderr,
					"Couldn't read AIF header (file too short?)\n" );
		::abort();
	}
	
	// Swap the header.
#if TARGET_RT_LITTLE_ENDIAN
	KUInt32* cursor = (KUInt32*) &mHeader;
	KUInt32* end = (KUInt32*) (((KUIntPtr) cursor) + sizeof(SHeader));
	do {
		*cursor = UByteSex::Swap( *cursor );
		cursor++;
	} while (cursor < end);
#endif
}

// -------------------------------------------------------------------------- //
//  * ReadROImage( KUInt8* )
// -------------------------------------------------------------------------- //
void
TAIFFile::ReadROImage( KUInt8* outImage )
{
	long startOffset;
	// Where should we start?
	if ((mHeader.fEntryPoint & 0xFF000000) == 0xEB000000)
	{
		// It's an executable image. Let's start from 0.
		startOffset = 0;
	} else {
		startOffset = sizeof( SHeader );
	}

	(void) ::fseek( mFile, startOffset, SEEK_SET );
	
	// Then read the image.
	if ( ::fread(
			outImage, sizeof( char ), mHeader.fReadOnlySize, mFile )
				!= mHeader.fReadOnlySize)
	{
		(void) ::fprintf(
					stderr,
					"Couldn't read AIF RO Image (is it really an AIF image?)\n" );
		::abort();
	}

#if TARGET_RT_LITTLE_ENDIAN
	// Swap the image.
	KUInt32* cursor = (KUInt32*) &outImage;
	KUInt32* end = (KUInt32*) &outImage[mHeader.fReadOnlySize];
	do {
		*cursor = UByteSex::Swap( *cursor );
	} while (++cursor < end);
#endif
}

// -------------------------------------------------------------------------- //
//  * ReadRWImage( KUInt8* )
// -------------------------------------------------------------------------- //
void
TAIFFile::ReadRWImage( KUInt8* outImage )
{
	long startOffset;
	// Where should we start?
	if ((mHeader.fEntryPoint & 0xFF000000) == 0xEB000000)
	{
		// It's an executable image. Let's start from 0.
		startOffset = 0;
	} else {
		startOffset = sizeof( SHeader );
	}
	
	startOffset += mHeader.fReadOnlySize;

	(void) ::fseek( mFile, startOffset, SEEK_SET );
	
	// Then read the image.
	if ( ::fread(
			outImage, sizeof( char ), mHeader.fReadWriteSize, mFile )
				!= mHeader.fReadWriteSize)
	{
		(void) ::fprintf(
					stderr,
					"Couldn't read AIF RW Image (is it really an AIF image?)\n" );
		::abort();
	}

#if TARGET_RT_LITTLE_ENDIAN
	// Swap the image.
	KUInt32* cursor = (KUInt32*) &outImage;
	KUInt32* end = (KUInt32*) &outImage[mHeader.fReadWriteSize];
	do {
		*cursor = UByteSex::Swap( *cursor );
	} while (++cursor < end);
#endif
}

// ========== //
// << WAIT >> //
// ========== //
