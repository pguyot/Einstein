// ==============================
// Fichier:			TFileStream.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// CrŽŽ le:			11/6/2005
// Tabulation:		4 espaces
// 
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1
// 
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
// 
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
// 
// The Original Code is TFileStream.cp.
// 
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2005 the
// Initial Developer. All Rights Reserved.
// 
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
// 
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TFileStream.cp,v 1.3 2007/07/11 01:36:30 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TFileStream.h"

// ANSI C
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

// K
#if HAS_EXCEPTION_HANDLING
	#include <K/Exceptions/IO/TIOException.h>
	#include <K/Exceptions/IO/TEOFException.h>
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// Local Function Replacements
// -------------------------------------------------------------------------- //

#if _MSC_VER
	#if 0 // if we need files larger than 4GB
static inline off_t ftello(FILE *a) { return (off_t)::_ftelli64(a); }
static inline off_t fseeko(FILE *a, off_t b, int c) { return (off_t)::_fseeki64(a, b, c); }
	#else
static inline off_t ftello(FILE *a) { return (off_t)::ftell(a); }
static inline off_t fseeko(FILE *a, off_t b, int c) { return (off_t)::fseek(a, b, c); }
	#endif
#endif

// -------------------------------------------------------------------------- //
//  * TFileStream( const char*, const char* )
// -------------------------------------------------------------------------- //
TFileStream::TFileStream( const char* inPath, const char* inMode )
	:
		mFile( NULL ),
		mWeOpenedTheFile( true )
{
	mFile = ::fopen( inPath, inMode );
#if HAS_EXCEPTION_HANDLING
	if (mFile == NULL)
	{
		throw TIOException();
	}
#endif
	switch (inMode[0]) {
		case 'r':
			mIsReading = 1;
			if (inMode[1]=='+') mIsWriting = 1;
			break;
		case 'w':
		case 'a':
			mIsWriting = 1;
			if (inMode[1]=='+') mIsReading = 1;
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * TFileStream( FILE* )
// -------------------------------------------------------------------------- //
TFileStream::TFileStream( FILE* inFile )
	:
		mFile( inFile ),
		mWeOpenedTheFile( false )
{
#if TARGET_OS_WIN32
	mIsWriting = mIsReading = 1; 
	// Windows does not have a standard API for getting this value
	// see: NtQueryInformationFile
#else
	int fileflags = fcntl (fileno(inFile), F_GETFL, 0);
	if (fileflags!=-1) {
		switch (fileflags & (O_RDWR|O_WRONLY|O_RDONLY) ) {
			case O_RDONLY: mIsReading = 1; break;
			case O_WRONLY: mIsWriting = 1; break;
			case O_RDWR: mIsWriting = mIsReading = 1; break;
		}
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * ~TFileStream( void )
// -------------------------------------------------------------------------- //
TFileStream::~TFileStream( void )
{
	if (mWeOpenedTheFile && mFile)
	{
		::fclose( mFile );
		mFile = NULL;
	}
}

// ------------------------------------------------------------------------- //
//  * Read( void*, KUInt32* )
// ------------------------------------------------------------------------- //
void
TFileStream::Read( void* outBuffer, KUInt32* ioCount )
{
	size_t theCount = ::fread( outBuffer, 1, *ioCount, mFile );
	
	if (*ioCount != theCount)
	{
		*ioCount = (KUInt32)theCount;

#if HAS_EXCEPTION_HANDLING
		if (feof( mFile ) == 0)
		{
			throw TIOException();
		}
#endif
	}
}

// ------------------------------------------------------------------------- //
//  * Write( const void*, KUInt32* )
// ------------------------------------------------------------------------- //
void
TFileStream::Write( const void* inBuffer, KUInt32* ioCount )
{
	size_t theCount = ::fwrite( inBuffer, 1, *ioCount, mFile );
	
	if (*ioCount != theCount)
	{
		*ioCount = (KUInt32) theCount;
#if HAS_EXCEPTION_HANDLING
		throw TIOException();
#endif
	}
}

// ------------------------------------------------------------------------- //
//  * FlushOutput( void )
// ------------------------------------------------------------------------- //
void
TFileStream::FlushOutput( void )
{
	(void) ::fflush( mFile );
}

// ------------------------------------------------------------------------- //
//  * PeekByte( void )
// ------------------------------------------------------------------------- //
KUInt8
TFileStream::PeekByte( void )
{
	int theNextChar = getc( mFile );
	if (theNextChar == EOF)
	{
#if HAS_EXCEPTION_HANDLING		
		throw EOFException;
#else
		return EOF;
#endif
	}
	
	if (::ungetc( theNextChar, mFile ) != theNextChar)
	{
#if HAS_EXCEPTION_HANDLING
		throw TIOException();
#else
		return EOF;
#endif
	}
	
	return (KUInt8) theNextChar;
}

// ------------------------------------------------------------------------- //
//  * GetCursor( void ) const
// ------------------------------------------------------------------------- //
KSInt64
TFileStream::GetCursor( void ) const
{
	off_t thePos = ::ftello( mFile );
	return (KSInt64) thePos;
}

// ------------------------------------------------------------------------- //
//  * SetCursor( KSInt64, ECursorMode )
// ------------------------------------------------------------------------- //
void
TFileStream::SetCursor( KSInt64 inPos, ECursorMode inMode )
{
	int whence = 0;
	switch (inMode)
	{
		case kFromStart:
			whence = SEEK_SET;
			break;

		case kFromCursor:
			whence = SEEK_CUR;
			break;

		case kFromLEOF:
			whence = SEEK_END;
	}
	
	int theErr = ::fseeko( mFile, (off_t) inPos, whence );
	if (theErr != 0)
	{
#if HAS_EXCEPTION_HANDLING
		throw TIOException();
#endif
	}
}

// ------------------------------------------------------------------------- //
//  * Exists( const char *inPath )
// ------------------------------------------------------------------------- //
bool
TFileStream::Exists( const char *inPath )
{
	struct stat s;
	int theErr = ::stat( inPath, &s);
	return theErr == 0;
}

// ================================================ //
// This is an unauthorized cybernetic announcement. //
// ================================================ //
