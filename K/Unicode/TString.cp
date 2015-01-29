// ==============================
// Fichier:			TString.cp
// Projet:			K
// 
// Créé le:			3/8/2003
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
// The Original Code is TString.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2003-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TString.cp,v 1.7 2004/11/23 15:11:32 paul Exp $
// ===========

#include <K/Unicode/UnicodeDefinitions.h>
#include <K/Unicode/TString.h>

// ANSI C
#include <stdlib.h>
#include <string.h>

// K Libs
#include <K/Unicode/UUTF16Conv.h>
#include <K/Unicode/UUTF16CStr.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TString( void )
// -------------------------------------------------------------------------- //
TString::TString( void )
	:
		mEncoding( kUTF16 ),
		mString( nil ),
		mHashCode( 0 ),
		mStringSize( 0 ),
		mBufferSize( 0 ),
		mHashCodeIsValid( true ),
		mStringSizeIsValid( true )
{
	mString = ::malloc( 0 );
}

// -------------------------------------------------------------------------- //
//  * TString( const char* )
// -------------------------------------------------------------------------- //
TString::TString( const char* inCString )
	:
		mEncoding( kISO88591 ),
		mString( nil ),
		mHashCode( 0 ),
		mStringSize( 0 ),
		mBufferSize( 0 ),
		mHashCodeIsValid( false ),
		mStringSizeIsValid( false )
{
	// Détermination de la taille et calcul, dans la foulée, de la somme de contrôle.
	KUInt32 theSize = 0;
	KUInt32 theHashCode = 0;
	register char theCharacter;
	register Boolean fitsOn7Bits = true;
	register const char* theInputString = inCString;
	do {
		theCharacter = *theInputString++;	// Caractère courant.
		if ((fitsOn7Bits) && (theCharacter & 0x80))
		{
			fitsOn7Bits = false;
		}
		
		theHashCode += theCharacter;
		theSize++;
	} while (theCharacter != 0);
	
	if (fitsOn7Bits)
	{
		mEncoding = kUSASCII;
	}
	
	mHashCode = theHashCode * kHashMagic;
	mHashCodeIsValid = true;

	// Création de la mémoire tampon.
	mString = ::malloc( (theSize) );
	mBufferSize = theSize;
	mStringSize = theSize - 1;
	mStringSizeIsValid = true;
	
	// Copie des caractères
	(void) ::memcpy( mString, inCString, theSize );
}

// -------------------------------------------------------------------------- //
//  * TString( const char* )
// -------------------------------------------------------------------------- //
TString::TString( const char* inCString, EEncoding inEncoding )
	:
		mEncoding( inEncoding ),
		mString( nil ),
		mHashCode( 0 ),
		mStringSize( 0 ),
		mBufferSize( 0 ),
		mHashCodeIsValid( false ),
		mStringSizeIsValid( false )
{
	// Détermination de la taille.
	KUInt32 theSize = 0;
	KUInt32 theHashCode = 0;
	register char theCharacter;
	register Boolean fitsOn7Bits = true;
	register const char* theInputString = inCString;
	do {
		theCharacter = *theInputString++;	// Caractère courant.
		if ((fitsOn7Bits) && (theCharacter & 0x80))
		{
			fitsOn7Bits = false;
		}
		
		theHashCode += theCharacter;
		theSize++;
	} while (theCharacter != 0);
	
	if (fitsOn7Bits)
	{
		mEncoding = kUSASCII;
		mHashCode = theHashCode * kHashMagic;
		mHashCodeIsValid = true;
	}
	
	// Création de la mémoire tampon.
	mString = ::malloc( (theSize) );
	mBufferSize = theSize;
	mStringSize = theSize - 1;
	mStringSizeIsValid = true;
	
	// Copie des caractères
	(void) ::memcpy( mString, inCString, theSize );
}

// -------------------------------------------------------------------------- //
//  * TString( const KUInt16* )
// -------------------------------------------------------------------------- //
TString::TString( const KUInt16* inCString )
	:
		mEncoding( kUTF16 ),
		mString( nil ),
		mHashCode( 0 ),
		mStringSize( 0 ),
		mBufferSize( 0 ),
		mHashCodeIsValid( false ),
		mStringSizeIsValid( false )
{
	// Détermination de la taille et calcul de la somme de contrôle dans la
	// foulée.
	KUInt32 theBufferSize = 0;
	KUInt32 theStringSize = 0;
	KUInt32 theHashCode = 0;
	KUInt16 theCharacter;
	register Boolean justHadDoubleChar = false;
	register const KUInt16* theInputString = inCString;
	do {
		theCharacter = *theInputString++;	// Caractère courant.
		theHashCode += theCharacter;
		if (!justHadDoubleChar) {
			theStringSize++;
			if ((theCharacter >= 0xD800) || (theCharacter < 0xE000))
			{
				justHadDoubleChar = true;
			}
		} else {
			justHadDoubleChar = false;
		}
		theBufferSize++;
	} while (theCharacter != 0);
	
	mHashCode = theHashCode * kHashMagic;

	// Copie des données.
	theBufferSize *= sizeof(KUInt16);
	mString = ::malloc( theBufferSize );
	mBufferSize = theBufferSize;
	mStringSize = theStringSize - 1;
	mStringSizeIsValid = true;
	
	(void) ::memcpy( mString, inCString, theBufferSize );
}

// -------------------------------------------------------------------------- //
//  * TString( const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TString::TString( const char* inChars, KUInt32 inSize )
	:
		mEncoding( kISO88591 ),
		mString( nil ),
		mHashCode( 0 ),
		mStringSize( inSize ),
		mBufferSize( inSize + 1 ),
		mHashCodeIsValid( false ),
		mStringSizeIsValid( true )
{
	// Création de la mémoire tampon.
	mString = ::malloc( (inSize + 1) );
	
	// Copie des caractères et calcul, dans la foulée, de la somme de contrôle.
	KUInt32 theHashCode = 0;
	register Boolean fitsOn7Bits = true;
	register const char* theInputString = inChars;
	register char* theBufferString = (char*) mString;
	register KUInt32 index = inSize;
	while (index-- > 0)
	{
		register char theCharacter = *theInputString++;	// Caractère courant.

		if ((fitsOn7Bits) && (theCharacter & 0x80))
		{
			fitsOn7Bits = false;
		}

		theHashCode += theCharacter;
		*theBufferString++ = theCharacter;
	};
	
	if (fitsOn7Bits)
	{
		mEncoding = kUSASCII;
	}

	mHashCode = theHashCode * kHashMagic;
	mHashCodeIsValid = true;
}

// -------------------------------------------------------------------------- //
//  * TString( const KUInt16*, KUInt32 )
// -------------------------------------------------------------------------- //
TString::TString( const KUInt16* inChars, KUInt32 inSize )
	:
		mEncoding( kUTF16 ),
		mString( nil ),
		mHashCode( 0 ),
		mStringSize( inSize ),
		mBufferSize( (inSize + 1) * sizeof(KUInt16) ),
		mHashCodeIsValid( false ),
		mStringSizeIsValid( true )
{
	// Création de la mémoire tampon.
	mString = (KUInt16*) ::malloc( mBufferSize );
	
	// Copie des caractères.
	(void) ::memcpy( mString, inChars, inSize * sizeof(KUInt16) );
	
	// Terminateur.
	((KUInt16*) mString)[inSize] = 0;
}

// -------------------------------------------------------------------------- //
//  * TString( const TString& )
// -------------------------------------------------------------------------- //
TString::TString( const TString& inCopy )
	:
		TRefCounter( inCopy ),
		mEncoding( inCopy.mEncoding ),
		mString( nil ),
		mHashCode( inCopy.mHashCode ),
		mStringSize( inCopy.mStringSize ),
		mBufferSize( inCopy.mBufferSize ),
		mHashCodeIsValid( inCopy.mHashCodeIsValid ),
		mStringSizeIsValid( inCopy.mStringSizeIsValid )
{
	// Création de la mémoire tampon.
	mString = ::malloc( mBufferSize );
	
	// Tant qu'à faire à copier les données, autant calculer la taille
	// et la valeur de hachage.
	Boolean hasHashableEncoding = false;
	if (!mHashCodeIsValid)
	{
		hasHashableEncoding = ((mEncoding == kUTF16)
							|| (mEncoding == kUSASCII)
							|| (mEncoding == kISO88591));
	}
	
	if (!mStringSizeIsValid || hasHashableEncoding)
	{
		// Copie des données à la mimine avec calcul de la taille
		// et/ou de la valeur de hachage.
		if (hasHashableEncoding)
		{
		} else {
		}
		
		(void) ::memcpy( mString, inCopy.mString, mBufferSize );
	} else {
		// Copie des données avec memcpy.
		(void) ::memcpy( mString, inCopy.mString, mBufferSize );
	}
}

// -------------------------------------------------------------------------- //
//  * ~TString( void )
// -------------------------------------------------------------------------- //
TString::~TString( void )
{
	if (mString)
	{
		::free( mString );
	}
}

// -------------------------------------------------------------------------- //
//  * operator = ( const TString& )
// -------------------------------------------------------------------------- //
TString&
TString::operator = ( const TString& inCopy )
{
	if (mString)
	{
		::free( mString );
	}
	mEncoding = inCopy.mEncoding;
	mString = nil;
	mHashCode = inCopy.mHashCode;
	mStringSize = inCopy.mStringSize;
	mBufferSize = inCopy.mBufferSize;
	mHashCodeIsValid = inCopy.mHashCodeIsValid;
	mStringSizeIsValid = inCopy.mStringSizeIsValid;
	
	// Création de la mémoire tampon.
	mString = (KUInt16*) ::malloc( mBufferSize );
	
	// Copie des données.
	(void) ::memcpy( mString, inCopy.mString, mBufferSize );
	
	return *this;
}

// -------------------------------------------------------------------------- //
//  * operator == ( const TString& ) const
// -------------------------------------------------------------------------- //
Boolean
TString::operator == ( const TString& inAlter ) const
{
	Boolean theResult = false;
	do {
		if ((mStringSizeIsValid && inAlter.mStringSizeIsValid)
			&& (mStringSize != inAlter.mStringSize))
		{
			// Les chaînes sont différentes.
			break;
		}

		if ((mHashCodeIsValid && inAlter.mHashCodeIsValid)
			&& (mHashCode != inAlter.mHashCode))
		{
			// Les chaînes sont différentes.
			break;
		}
		
		// Sinon, on compare les mémoires tampon.
		if ((mEncoding == inAlter.mEncoding)
			&& (mEncoding == kUTF16))
		{
			// Comparaison UTF-16.
			theResult = (CompareUTF16Strings( inAlter ) == 0);
		} else if (((mEncoding == kISO88591) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kISO88591))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kISO88591) && (inAlter.mEncoding == kISO88591)))
		{
			// Comparaison directe.
			theResult = (CompareLatin1Strings( inAlter ) == 0);
		} else if ((mEncoding == inAlter.mEncoding)
			|| ((mEncoding == kISO88592) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kISO88592))
			|| ((mEncoding == kUTF8) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kUTF8)))
		{
			// Comparaison directe.
			theResult = (Compare8BitsStrings( inAlter ) == 0);
		} else {
			// Traduction (au moins d'un) en UTF-16.
			// Il reste:	UTF-8 & UTF-16
			//				UTF-8 & ISO 8859-1
			//				UTF-8 & ISO 8859-2
			//				UTF-8 & MacRoman
			//				UTF-16 & ISO 8859-1
			//				UTF-16 & ISO 8859-2
			//				UTF-16 & MacRoman
			//				ISO 8859-1 & ISO 8859-2
			//				ISO 8859-1 & MacRoman
			//				ISO 8859-2 & MacRoman
//			UUTF16Conv
		}
	} while (false);
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * operator == ( TString& ) const
// -------------------------------------------------------------------------- //
Boolean
TString::operator == ( TString& inAlter ) const
{
	Boolean theResult = false;
	do {
		if ((mStringSizeIsValid && inAlter.mStringSizeIsValid)
			&& (mStringSize != inAlter.mStringSize))
		{
			// Les chaînes sont différentes.
			break;
		}

		if ((mHashCodeIsValid && inAlter.mHashCodeIsValid)
			&& (mHashCode != inAlter.mHashCode))
		{
			// Les chaînes sont différentes.
			break;
		}
		
		// Sinon, on compare les mémoires tampon.
		if ((mEncoding == inAlter.mEncoding)
			&& (mEncoding == kUTF16))
		{
			// Comparaison UTF-16.
			theResult = (CompareUTF16Strings( inAlter ) == 0);
		} else if (((mEncoding == kISO88591) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kISO88591))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kISO88591) && (inAlter.mEncoding == kISO88591)))
		{
			// Comparaison directe.
			theResult = (CompareLatin1Strings( inAlter ) == 0);
		} else if ((mEncoding == inAlter.mEncoding)
			|| ((mEncoding == kISO88592) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kISO88592))
			|| ((mEncoding == kUTF8) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kUTF8)))
		{
			// Comparaison directe.
			theResult = (Compare8BitsStrings( inAlter ) == 0);
		} else {
			// Traduction (au moins d'un) en UTF-16.
			// Il reste:	UTF-8 & UTF-16
			//				UTF-8 & ISO 8859-1
			//				UTF-8 & ISO 8859-2
			//				UTF-8 & MacRoman
			//				UTF-16 & ISO 8859-1
			//				UTF-16 & ISO 8859-2
			//				UTF-16 & MacRoman
			//				ISO 8859-1 & ISO 8859-2
			//				ISO 8859-1 & MacRoman
			//				ISO 8859-2 & MacRoman
//			UUTF16Conv
		}
	} while (false);
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * operator < ( TString& ) const
// -------------------------------------------------------------------------- //
Boolean
TString::operator < ( TString& inAlter ) const
{
	int theResult = 0;
	do {
		// On compare les mémoires tampon.
		if ((mEncoding == inAlter.mEncoding)
			&& (mEncoding == kUTF16))
		{
			// Comparaison UTF-16.
			theResult = CompareUTF16Strings( inAlter );
		} else if (((mEncoding == kISO88591) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kISO88591))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kISO88591) && (inAlter.mEncoding == kISO88591)))
		{
			// Comparaison directe.
			theResult = CompareLatin1Strings( inAlter );
		} else if ((mEncoding == inAlter.mEncoding)
			|| ((mEncoding == kISO88592) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kISO88592))
			|| ((mEncoding == kUTF8) && (inAlter.mEncoding == kUSASCII))
			|| ((mEncoding == kUSASCII) && (inAlter.mEncoding == kUTF8)))
		{
			// Comparaison directe.
			theResult = Compare8BitsStrings( inAlter );
		} else {
			// Traduction (au moins d'un) en UTF-16.
			// Il reste:	UTF-8 & UTF-16
			//				UTF-8 & ISO 8859-1
			//				UTF-8 & ISO 8859-2
			//				UTF-8 & MacRoman
			//				UTF-16 & ISO 8859-1
			//				UTF-16 & ISO 8859-2
			//				UTF-16 & MacRoman
			//				ISO 8859-1 & ISO 8859-2
			//				ISO 8859-1 & MacRoman
			//				ISO 8859-2 & MacRoman
//			UUTF16Conv
		}
	} while (false);
	
	return (theResult < 0);
}

// -------------------------------------------------------------------------- //
//  * CompareLatin1Strings( const TString& ) const
// -------------------------------------------------------------------------- //
int
TString::CompareLatin1Strings( const TString& inAlter ) const
{
	int theResult = 0;
	KUInt32 minBufferSize = mBufferSize;
	if (inAlter.mBufferSize < minBufferSize)
	{
		minBufferSize = inAlter.mBufferSize;
	}
	
	const KUInt8* thisBuffer = (const KUInt8*) mString;
	const KUInt8* alterBuffer = (const KUInt8*) inAlter.mString;
	KUInt32 theHashCode = 0;
	KUInt32 theSize = 0;
	for (; minBufferSize > 0; minBufferSize--, theSize++)
	{
		KUInt8 thisChar = *thisBuffer++;
		KUInt8 alterChar = *alterBuffer++;
		
		if (thisChar == 0)
		{
			mHashCode = theHashCode * kHashMagic;
			mHashCodeIsValid = true;
			mStringSize = theSize;
			mStringSizeIsValid = true;
			
			if (alterChar == 0)
			{
				inAlter.mHashCode = mHashCode;
				inAlter.mHashCodeIsValid = true;
				inAlter.mStringSize = theSize;
				inAlter.mStringSizeIsValid = true;
//				theResult = 0;
			} else {
				theResult = -1;
			}
			
			break;
		}
		
		if (alterChar == 0)
		{
			inAlter.mHashCode = theHashCode * kHashMagic;
			inAlter.mHashCodeIsValid = true;
			inAlter.mStringSize = theSize;
			inAlter.mStringSizeIsValid = true;
			theResult = 1;
			break;
		}
		
		if (thisChar < alterChar)
		{
			theResult = -1;
			break;
		} else if (thisChar > alterChar) {
			theResult = 1;
			break;
		}

		theHashCode += thisChar;
	}
	return theResult;
}


// -------------------------------------------------------------------------- //
//  * Compare8BitsStrings( const TString& ) const
// -------------------------------------------------------------------------- //
int
TString::Compare8BitsStrings( const TString& inAlter ) const
{
	int theResult = 0;
	KUInt32 minBufferSize = mBufferSize;
	if (inAlter.mBufferSize < minBufferSize)
	{
		minBufferSize = inAlter.mBufferSize;
	}
	
	const KUInt8* thisBuffer = (const KUInt8*) mString;
	const KUInt8* alterBuffer = (const KUInt8*) inAlter.mString;
	for (; minBufferSize > 0; minBufferSize--)
	{
		KUInt8 thisChar = *thisBuffer++;
		KUInt8 alterChar = *alterBuffer++;
		
		if (thisChar != alterChar)
		{
			if (thisChar < alterChar)
			{
				theResult = -1;
			} else if (thisChar > alterChar) {
				theResult = 1;
			}
			
			break;
		}

		if (thisChar == 0)
		{
			theResult = true;
			break;
		}
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * CompareUTF16Strings( const TString& ) const
// -------------------------------------------------------------------------- //
int
TString::CompareUTF16Strings( const TString& inAlter ) const
{
	int theResult = 0;
	KUInt32 minBufferSize = mBufferSize;
	if (inAlter.mBufferSize < minBufferSize)
	{
		minBufferSize = inAlter.mBufferSize;
	}
	
	const KUInt16* thisBuffer = (const KUInt16*) mString;
	const KUInt16* alterBuffer = (const KUInt16*) inAlter.mString;
	KUInt32 theHashCode = 0;
	KUInt32 theSize = 0;
	for (; minBufferSize > 0; minBufferSize--, theSize++)
	{
		KUInt16 thisChar = *thisBuffer++;
		KUInt16 alterChar = *alterBuffer++;
		
		if (thisChar == 0)
		{
			mHashCode = theHashCode * kHashMagic;
			mHashCodeIsValid = true;
			mStringSize = theSize;
			mStringSizeIsValid = true;
			
			if (alterChar == 0)
			{
				inAlter.mHashCode = mHashCode;
				inAlter.mHashCodeIsValid = true;
				inAlter.mStringSize = theSize;
				inAlter.mStringSizeIsValid = true;
//				theResult = 0;
			} else {
				theResult = -1;
			}
			
			break;
		}
		
		if (alterChar == 0)
		{
			inAlter.mHashCode = theHashCode * kHashMagic;
			inAlter.mHashCodeIsValid = true;
			inAlter.mStringSize = theSize;
			inAlter.mStringSizeIsValid = true;
			theResult = 1;
			break;
		}
		
		if (thisChar < alterChar)
		{
			theResult = -1;
			break;
		} else if (thisChar > alterChar) {
			theResult = 1;
			break;
		}

		theHashCode += thisChar;
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * PrintToStream( ostream& ) const
// -------------------------------------------------------------------------- //
void
TString::PrintToStream( ostream& inStream ) const
{
	// On détermine l'encodage.
	
	// Puis on affiche.
	
	// (pour le moment, on se contente d'afficher tel quel)
	(void) (inStream << (const char*) mString);
}

// -------------------------------------------------------------------------- //
//  * Convert( void*, size_t*, EEncoding )
// -------------------------------------------------------------------------- //
UUTF16Conv::EResult
TString::Convert(
				void* outBuffer,
				size_t* ioLength,
				EEncoding inEncoding )
{
	UUTF16Conv::EResult theResult = UUTF16Conv::kInputExhausted;
	// Absence de conversion.
	if ((inEncoding == mEncoding)
		|| ((mEncoding == kUSASCII) && (inEncoding == kISO88591))
		|| ((mEncoding == kUSASCII) && (inEncoding == kISO88592))
		|| ((mEncoding == kUSASCII) && (inEncoding == kUTF8))
		|| ((mEncoding == kUSASCII) && (inEncoding == kMacRoman)))
	{
		if (mStringSizeIsValid)
		{
			// Copie avec memcpy.
			size_t amount = *ioLength;
			size_t theStringSizeInBytes = mStringSize;
			if (mEncoding == kUTF16)
			{
				theStringSizeInBytes *= sizeof( KUInt16 );
			}
			
			if (amount > theStringSizeInBytes)
			{
				amount = theStringSizeInBytes;
				*ioLength = amount;
			}
			(void) ::memcpy(
						(void*) outBuffer,
						(const void*) mString,
						amount );
		} else {
			if (mEncoding == kUTF16)
			{
				// Copie en calculant la taille.
				KUInt32 indexChars = 0;
				const KUInt16* theInputCursor = (const KUInt16*) mString;
				KUInt16* theOutputCursor = (KUInt16*) outBuffer;
				const size_t theOutputLength = (*ioLength) / 2;
				while (indexChars < theOutputLength)
				{
					const KUInt16 theChar = *theInputCursor++;
					if (theChar == '\0')
					{
						// Fini.
						mStringSizeIsValid = true;
						mStringSize = indexChars;
						break;
					} else {
						// Copie.
						*theOutputCursor++ = theChar;
						indexChars++;
					}
				}
				
				*ioLength = indexChars * sizeof( KUInt16 );
			} else {
				// Copie en calculant la taille.
				KUInt32 indexChars = 0;
				const KUInt8* theInputCursor = (const KUInt8*) mString;
				KUInt8* theOutputCursor = (KUInt8*) outBuffer;
				const size_t theOutputLength = *ioLength;
				while (indexChars < theOutputLength)
				{
					const KUInt8 theChar = *theInputCursor++;
					if (theChar == '\0')
					{
						// Fini.
						mStringSizeIsValid = true;
						mStringSize = indexChars;
						break;
					} else {
						// Copie.
						*theOutputCursor++ = theChar;
						indexChars++;
					}
				}
				
				*ioLength = indexChars;
			}
		}
	} else {
		// Conversion, via UTF-16.
		if (mEncoding != kUTF16)
		{
			// Passage à l'UTF-16.
			KUInt32 newBufferSize;
			if (mStringSizeIsValid)
			{
				newBufferSize = (mStringSize + 1) * sizeof( KUInt16 );
			} else {
				newBufferSize = mBufferSize * sizeof( KUInt16 );
			}
			
			KUInt16* theNewBuffer = (KUInt16*) ::malloc( newBufferSize );
			
			switch ( mEncoding )
			{
				case kUTF8:
					UUTF16CStr::FromUTF8(
									(const KUInt8*) mString,
									theNewBuffer,
									mBufferSize,
									(size_t*) &mStringSize );
					break;

				case kUSASCII:
					UUTF16CStr::FromASCII(
									(const KUInt8*) mString,
									theNewBuffer,
									mBufferSize,
									(size_t*) &mStringSize );
					break;

				case kISO88591:
					UUTF16CStr::FromISO88591(
									(const KUInt8*) mString,
									theNewBuffer,
									mBufferSize,
									(size_t*) &mStringSize );
					break;

				case kISO88592:
					UUTF16CStr::FromISO88592(
									(const KUInt8*) mString,
									theNewBuffer,
									mBufferSize,
									(size_t*) &mStringSize );
					break;

				case kMacRoman:
					UUTF16CStr::FromMacRoman(
									(const KUInt8*) mString,
									theNewBuffer,
									mBufferSize,
									(size_t*) &mStringSize );
					break;
				
				default:
					// Shouldn't happen.
					break;
			}
			
			mEncoding = kUTF16;
			::free( mString );
			mString = theNewBuffer;
			mBufferSize = newBufferSize;
			mStringSizeIsValid = true;
		}

		// Conversion depuis l'UTF-16.
		size_t inputCount = mStringSize;
	
		switch ( inEncoding )
		{
			case kUTF8:
				theResult = UUTF16Conv::ToUTF8(
								(const KUInt16*) mString,
								&inputCount,
								outBuffer,
								ioLength );
				break;

			case kUTF16:
				{
					size_t amount = *ioLength;
					inputCount *= sizeof( KUInt16 );
					
					if (amount > inputCount)
					{
						amount = inputCount;
						*ioLength = amount;
					}
					(void) ::memcpy(
								(void*) outBuffer,
								(const void*) mString,
								amount );
				}
				break;

			case kUSASCII:
				theResult = UUTF16Conv::ToASCII(
								(const KUInt16*) mString,
								&inputCount,
								(KUInt8*) outBuffer,
								ioLength,
								UUTF16Conv::kRepCharOnUnrepChar );
								
			case kISO88591:
				theResult = UUTF16Conv::ToISO88591(
								(const KUInt16*) mString,
								&inputCount,
								(KUInt8*) outBuffer,
								ioLength,
								UUTF16Conv::kRepCharOnUnrepChar );
								
			case kISO88592:
				theResult = UUTF16Conv::ToISO88592(
								(const KUInt16*) mString,
								&inputCount,
								(KUInt8*) outBuffer,
								ioLength,
								UUTF16Conv::kRepCharOnUnrepChar );
								
			case kMacRoman:
				theResult = UUTF16Conv::ToMacRoman(
								(const KUInt16*) mString,
								&inputCount,
								(KUInt8*) outBuffer,
								ioLength,
								UUTF16Conv::kRepCharOnUnrepChar );
		}
	}
	
	return theResult;
}

// =========================================== //
// MSDOS is not dead, it just smells that way. //
//                 -- Henry Spencer            //
// =========================================== //
