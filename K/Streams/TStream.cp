// ==============================
// Fichier:			TStream.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			10/6/2005
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
// The Original Code is TStream.cp.
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
// $Id: TStream.cp,v 1.2 2006/01/18 08:59:53 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TStream.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// ANSI C
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Unicode/UUTF16CStr.h>
#if HAS_EXCEPTION_HANDLING
	#include <K/Exceptions/Errors/TMemError.h>
	#include <K/Exceptions/IO/TEOFException.h>
#endif

// ------------------------------------------------------------------------- //
//  * GetCString( KUInt32 )
// ------------------------------------------------------------------------- //
KUInt8*
TStream::GetCString( KUInt32 inNbChars )
{
	KUInt8* theResult = (KUInt8*) ::malloc( inNbChars + 1 );
	
	if (theResult == nil)
	{
#if HAS_EXCEPTION_HANDLING
		throw MemError;
#else
		return 0L;
#endif
	}
	
#if HAS_EXCEPTION_HANDLING
	// Un petit try/catch pour nettoyer en sortant.
	try {
		KUInt32 theCount = inNbChars;
		Read( theResult, &theCount );
		
		if (theCount != inNbChars)
		{
			throw EOFException;
		}
		
	} catch ( ... ) {
		if (theResult)
		{
			::free( theResult );
		}
	
		throw;	// Rethrow
	}
#else
	KUInt32 theCount = inNbChars;
	Read( theResult, &theCount );
		
	if (theCount != inNbChars)
	{
		::free(theResult);
		return 0L;
	}
#endif
	
	// Terminateur.
	theResult[inNbChars] = '\0';

	return theResult;
}

// ------------------------------------------------------------------------- //
//  * GetCString( void )
// ------------------------------------------------------------------------- //
KUInt8*
TStream::GetCString( void )
{
	size_t bufferLength = 10; 	// Taille de la mémoire tampon
	size_t strLength = 0;		// Taille de la chaîne
	KUInt8* theResult = (KUInt8*) ::malloc( bufferLength );
	
#if HAS_EXCEPTION_HANDLING	
	if (theResult == nil)
	{
		throw MemError;
	}
	
	try {
		KUInt8 theChar;
		KUInt32 count = 1;
		
		do {
			Read( &theChar, &count );
			
			if (count == 0)	// EOF.
			{
				throw EOFException;
			}
			
			// Add this byte.
			if (strLength == bufferLength)
			{
				// Increase the buffer size with realloc.
				bufferLength += 10;
				theResult = (KUInt8*)  ::realloc( theResult, bufferLength );
			}
			
			theResult[ strLength ] = theChar;
			
		} while (theChar != '\0');
		
	} catch ( ... ) {
		if (theResult)
		{
			::free( theResult );
		}
	
		throw;	// Rethrow
	}
#else
	if (theResult == nil) return 0L;
	KUInt8 theChar;
	KUInt32 count = 1;
		
	do {
		Read( &theChar, &count );
			
		if (count == 0)	// EOF.
		{
			::free( theResult );
			return 0L;
		}

		if (strLength == bufferLength)
		{
			bufferLength += 10;
			theResult = (KUInt8*)  ::realloc( theResult, bufferLength );
		}
		
		theResult[ strLength ] = theChar;
		
	} while (theChar != '\0');
#endif
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * PutCString( const KUInt8* )
// ------------------------------------------------------------------------- //
void
TStream::PutCString( const KUInt8* inString )
{
	// Taille de la chaîne plus le caractère nul.
	KUInt32 strLength = (KUInt32) ::strlen( (char*) inString ) + 1;
	
	Write( inString, &strLength );
}

// ------------------------------------------------------------------------- //
//  * PutString( const char* )
// ------------------------------------------------------------------------- //
void
TStream::PutString( const char* inString )
{
	// Taille de la chaîne sans le caractère nul.
	KUInt32 strLength = (KUInt32) ::strlen( (char*) inString );
	
	Write( inString, &strLength );
}

// ------------------------------------------------------------------------- //
//  * GetUniString( void )
// ------------------------------------------------------------------------- //
KUInt16*
TStream::GetUniString( void )
{
	int bufferLength = 10;
	int strLength = 0;
	KUInt16* theResult =
					(KUInt16*) ::malloc( bufferLength * sizeof( KUInt16 ) );
	
#if HAS_EXCEPTION_HANDLING
	if (theResult == nil)
	{
		throw MemError;
	}
	
	try {
		KUInt16 theChar;
		KUInt32 count = 2;
		
		do {
			Read( &theChar, &count );
			
			if (count != 2)	// EOF.
			{
				throw EOFException;
			}
			
			// Add this character.
			if (strLength == bufferLength)
			{
				// Increase the buffer size with realloc.
				bufferLength += 10;
				theResult = (KUInt16*)
					::realloc( theResult, bufferLength * sizeof( KUInt16 ) );
			}
			
			theResult[ strLength ] = theChar;
			
		} while (theChar != '\0');
		
	} catch ( ... ) {
		if (theResult)
			::free( theResult );
	
		throw;	// Rethrow
	}
#else
	if (theResult == nil) return 0L;
	KUInt16 theChar;
	KUInt32 count = 2;
		
	do {
		Read( &theChar, &count );
		
		if (count != 2)	// EOF.
		{
			::free(theResult);
			return 0L;
		}
			
		if (strLength == bufferLength)
		{
			bufferLength += 10;
			theResult = (KUInt16*)
			::realloc( theResult, bufferLength * sizeof( KUInt16 ) );
		}
			
		theResult[ strLength ] = theChar;
			
	} while (theChar != '\0');
#endif
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * PutUniString( const KUInt16* )
// ------------------------------------------------------------------------- //
void
TStream::PutUniString( const KUInt16* inString )
{
	KUInt32 strLength = (KUInt32) UUTF16CStr::StrLen( inString ) + 1;
	
	Write( inString, &strLength );
}

// ------------------------------------------------------------------------- //
//  * GetInt32( void )
// ------------------------------------------------------------------------- //
KUInt32
TStream::GetInt32( void )
{
	KUInt32 theResult;						// Mémoire tampon
	KUInt32 length = sizeof( theResult );	// Taille de la mémoire tampon.
	
	Read( &theResult, &length );
	
	if (length < sizeof( theResult ))
	{
#if HAS_EXCEPTION_HANDLING
		throw EOFException;
#else
		return 0;
#endif
	}
	
	return UByteSex_FromBigEndian( theResult );
}

// ------------------------------------------------------------------------- //
//  * PutInt32( const KUInt32 )
// ------------------------------------------------------------------------- //
void
TStream::PutInt32( const KUInt32 inLong )
{
	KUInt32 length = sizeof( inLong );
	Write( &inLong, &length );
}

// ------------------------------------------------------------------------- //
//  * GetXLong( void )
// ------------------------------------------------------------------------- //
KUInt32
TStream::GetXLong( void )
{
	KUInt32 theResult;
	
	// Lecture du premier octet.
	KUInt8 theFirstByte;	// Premier octet.
	KUInt32 length = sizeof( theFirstByte );	// Taille de l'octet.
	Read( &theFirstByte, &length );
	
	if (length < sizeof( theFirstByte ))
	{
#if HAS_EXCEPTION_HANDLING
		throw EOFException;
#else
		return 0;
#endif
	}

	if (theFirstByte == 0xFF)
	{
		theResult = GetInt32BE();
	} else {
		theResult = theFirstByte;
	}
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * PutXLong( const KUInt32 )
// ------------------------------------------------------------------------- //
void
TStream::PutXLong( const KUInt32 inLong )
{
	// Le paramètre tient-il sur un octet?
	if (inLong <= 254)
	{
		PutByte( (KUInt8) inLong );
	} else {
		// Ecriture de 0xFF
		PutByte( 0xFF );
		
		// Ecriture du long
		PutInt32BE( inLong );
	}
}


// ------------------------------------------------------------------------- //
//  * GetInt16( void )
// ------------------------------------------------------------------------- //
KUInt16
TStream::GetInt16( void )
{
	KUInt16 theResult;						// Mémoire tampon
	KUInt32 length = sizeof( theResult );	// Taille de la mémoire tampon.
	
	Read( &theResult, &length );

	if (length < sizeof( theResult ))
	{
#if HAS_EXCEPTION_HANDLING
		throw EOFException;
#else
		return 0;
#endif
	}
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * PutInt16( const KUInt32 )
// ------------------------------------------------------------------------- //
void
TStream::PutInt16( const KUInt16 inShort )
{
	KUInt32 length = sizeof( inShort );
	Write( &inShort, &length );
}

// ------------------------------------------------------------------------- //
//  * GetInt32ArrayBE( const KUInt32*, const KUInt32 )
// ------------------------------------------------------------------------- //
void
TStream::GetInt32ArrayBE(
				KUInt32* outArray,
				const KUInt32 inCount )
{
	KUInt32* cursor = outArray;
	KUInt32 count = inCount;
	while (count-- > 0)
	{
		*cursor++ = GetInt32BE();
	}
}

// ------------------------------------------------------------------------- //
//  * GetInt32ArrayLE( const KUInt32*, const KUInt32 )
// ------------------------------------------------------------------------- //
void
TStream::GetInt32ArrayLE(
				KUInt32* outArray,
				const KUInt32 inCount )
{
	KUInt32* cursor = outArray;
	KUInt32 count = inCount;
	while (count-- > 0)
	{
		*cursor++ = GetInt32LE();
	}
}

// ------------------------------------------------------------------------- //
//  * PutInt32ArrayBE( const KUInt32*, const KUInt32 )
// ------------------------------------------------------------------------- //
void
TStream::PutInt32ArrayBE(
				const KUInt32* inArray,
				const KUInt32 inCount )
{
	const KUInt32* cursor = inArray;
	KUInt32 count = inCount;
	while (count-- > 0)
	{
		PutInt32BE( *cursor++ );
	}
}

// ------------------------------------------------------------------------- //
//  * PutInt32ArrayLE( const KUInt32*, const KUInt32 )
// ------------------------------------------------------------------------- //
void
TStream::PutInt32ArrayLE(
				const KUInt32* inArray,
				const KUInt32 inCount )
{
	const KUInt32* cursor = inArray;
	KUInt32 count = inCount;
	while (count-- > 0)
	{
		PutInt32LE( *cursor++ );
	}
}

// ------------------------------------------------------------------------- //
//  * GetByte( void )
// ------------------------------------------------------------------------- //
KUInt8
TStream::GetByte( void )
{
	KUInt8 theResult;						// Mémoire tampon
	KUInt32 length = sizeof( theResult );	// Taille de la mémoire tampon.
	
	Read( &theResult, &length );
	
	if (length < sizeof( theResult ))
	{
#if HAS_EXCEPTION_HANDLING
		throw EOFException;
#else
		return 0;
#endif
	}
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * PutByte( const KUInt8 )
// ------------------------------------------------------------------------- //
void
TStream::PutByte( const KUInt8 inByte )
{
	KUInt32 length = sizeof( inByte );	// Taille de la mémoire tampon
	
	Write( &inByte, &length );
}


// ------------------------------------------------------------------------- //
//  * TransferBoolean( Boolean& )
// ------------------------------------------------------------------------- //
void TStream::TransferBoolean(Boolean &myBool)
{
  if (IsReading()) {
    KUInt8 v = GetByte();
    myBool = v;
  } else if (IsWriting()) {
    PutByte(myBool);
  }
}


// ------------------------------------------------------------------------- //
//  * TransferByte( KUInt8& )
// ------------------------------------------------------------------------- //
void TStream::TransferByte(KUInt8 &myByte)
{
  if (IsReading()) {
    myByte = GetByte();
  } else if (IsWriting()) {
    PutByte(myByte);
  }
}


// ------------------------------------------------------------------------- //
//  * TransferInt32BE( KUInt32& )
// ------------------------------------------------------------------------- //
void TStream::TransferInt32BE(KUInt32 &myWord)
{
	if (IsReading()) {
		myWord = GetInt32BE();
	} else if (IsWriting()) {
		PutInt32BE(myWord);
	}
}


// ------------------------------------------------------------------------- //
//  * TransferInt32BE( KSInt32& )
// ------------------------------------------------------------------------- //
void TStream::TransferInt32BE(KSInt32 &myWord)
{
	union { KUInt32 U32; KSInt32 S32; } v;
	if (IsReading()) {
		v.U32 = GetInt32BE();
		myWord = v.S32;
	} else if (IsWriting()) {
		v.S32 = myWord;
		PutInt32BE(v.U32);
	}
}


// ------------------------------------------------------------------------- //
//  * TransferInt16BE( KUInt16& )
// ------------------------------------------------------------------------- //
void TStream::TransferInt16BE(KUInt16 &myWord)
{
	if (IsReading()) {
		myWord = GetInt16BE();
	} else if (IsWriting()) {
		PutInt16BE(myWord);
	}
}


// ------------------------------------------------------------------------- //
//  * void TransferInt32ArrayBE( KUInt32* , const KUInt32 )
// ------------------------------------------------------------------------- //
void TStream::TransferInt32ArrayBE(KUInt32* myArray, const KUInt32 myCount)
{
	if (IsReading()) {
		GetInt32ArrayBE(myArray, myCount);
	} else if (IsWriting()) {
		PutInt32ArrayBE(myArray, myCount);
	}
}

// ------------------------------------------------------------------------- //
//  * void Transfer( void*, KUInt32* )
// ------------------------------------------------------------------------- //
void TStream::Transfer( void* inoutBuffer, KUInt32* ioCount )
{
	if (IsReading()) {
		Read(inoutBuffer, ioCount);
	} else if (IsWriting()) {
		Write(inoutBuffer, ioCount);
	}
}


// =============================== //
// System going down in 5 minutes. //
// =============================== //
