// ==============================
// Fichier:			UUTF16CStr.cp
// Projet:			K
//
// Créé le:			14/01/2002
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
// The Original Code is UUTF16CStr.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2002-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========

#include <K/Unicode/UUTF16CStr.h>

// K Unicode
#include <K/Unicode/UUTF16Conv.h>

// ----------------------------------------------------------------	//
//	* ToISO88591( const KUInt16*, KUInt8*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from UTF-16 to ISO 8859-1

void
UUTF16CStr::ToISO88591(
	const KUInt16*	inInputCString,
	KUInt8*			outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t inputSize = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = inputSize;
	}

	// Two cases: inLimit is null or it isn't.
	size_t outputSize = inLimit;
	if (outputSize == 0)
	{
		outputSize = inputSize;	// Simply because X characters in UTF-16
								// cannot be expanded to more than X
								// characters in these charsets
	} else {
		outputSize--;			// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::ToISO88591(
		inInputCString, &inputSize,
		outOutputCString, &outputSize,
		UUTF16Conv::kRepCharOnUnrepChar );
	
	// Add the null terminator.
	outOutputCString[ outputSize ] = 0;
}

// ----------------------------------------------------------------	//
//	* FromISO88591( const KUInt8*, KUInt16*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from ISO8859-1 to UTF-16

void
UUTF16CStr::FromISO88591(
	const KUInt8*	inInputCString,
	KUInt16*		outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t count = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = count;
	}

	// If inLimit isn't null, take the smallest.
	if ((inLimit != 0) && (inLimit <= count))
	{
		count = inLimit - 1;	// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::FromISO88591(
		inInputCString,
		outOutputCString,
		count );
	
	// Add the null terminator.
	outOutputCString[ count ] = 0;
}

// ----------------------------------------------------------------	//
//	* ToISO88592( const KUInt16*, KUInt8*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from UTF-16 to ISO 8859-2

void
UUTF16CStr::ToISO88592(
	const KUInt16*	inInputCString,
	KUInt8*			outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t inputSize = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = inputSize;
	}

	// Two cases: inLimit is null or it isn't.
	size_t outputSize = inLimit;
	if (outputSize == 0)
	{
		outputSize = inputSize;	// Simply because X characters in UTF-16
								// cannot be expanded to more than X
								// characters in these charsets
	} else {
		outputSize--;			// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::ToISO88592(
		inInputCString, &inputSize,
		outOutputCString, &outputSize,
		UUTF16Conv::kRepCharOnUnrepChar );
	
	// Add the null terminator.
	outOutputCString[ outputSize ] = 0;
}

// ----------------------------------------------------------------	//
//	* FromISO88592( const KUInt8*, KUInt16*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from ISO8859-2 to UTF-16

void
UUTF16CStr::FromISO88592(
	const KUInt8*	inInputCString,
	KUInt16*		outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t count = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = count;
	}

	// If inLimit isn't null, take the smallest.
	if ((inLimit != 0) && (inLimit <= count))
	{
		count = inLimit - 1;	// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::FromISO88592(
		inInputCString,
		outOutputCString,
		count );
	
	// Add the null terminator.
	outOutputCString[ count ] = 0;
}

// ----------------------------------------------------------------	//
//	* ToASCII( const KUInt16*, KUInt8*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from UTF-16 to US ASCII

void
UUTF16CStr::ToASCII(
	const KUInt16*	inInputCString,
	KUInt8*			outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t inputSize = StrLen( inInputCString );
	
	// Two cases: inLimit is null or it isn't.
	size_t outputSize = inLimit;
	if (outputSize == 0)
	{
		outputSize = inputSize;	// Simply because X characters in UTF-16
								// cannot be expanded to more than X
								// characters in these charsets
	} else {
		outputSize--;			// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::ToASCII(
		inInputCString, &inputSize,
		outOutputCString, &outputSize,
		UUTF16Conv::kRepCharOnUnrepChar );
	
	// Add the null terminator.
	outOutputCString[ outputSize ] = 0;
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = inputSize;
	}
}

// ----------------------------------------------------------------	//
//	* FromASCII( const KUInt8*, KUInt16*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from US ASCII to UTF-16

void
UUTF16CStr::FromASCII(
	const KUInt8*	inInputCString,
	KUInt16*		outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t count = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = count;
	}

	// If inLimit isn't null, take the smallest.
	if ((inLimit != 0) && (inLimit <= count))
	{
		count = inLimit - 1;	// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::FromASCII(
		inInputCString,
		outOutputCString,
		count );
	
	// Add the null terminator.
	outOutputCString[ count ] = 0;
}

// ----------------------------------------------------------------	//
//	* ToMacRoman( const KUInt16*, KUInt8*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from UTF-16 to MacRoman

void
UUTF16CStr::ToMacRoman(
	const KUInt16*	inInputCString,
	KUInt8*			outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t inputSize = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = inputSize;
	}

	// Two cases: inLimit is null or it isn't.
	size_t outputSize = inLimit;
	if (outputSize == 0)
	{
		outputSize = inputSize;	// Simply because X characters in UTF-16
								// cannot be expanded to more than X
								// characters in these charsets
	} else {
		outputSize--;			// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::ToMacRoman(
		inInputCString, &inputSize,
		outOutputCString, &outputSize,
		UUTF16Conv::kRepCharOnUnrepChar );
	
	// Add the null terminator.
	outOutputCString[ outputSize ] = 0;
}

// ----------------------------------------------------------------	//
//	* FromMacRoman( const KUInt8*, KUInt16*, size_t )
// ----------------------------------------------------------------	//
// Convert a C String from MacRoman to UTF-16

void
UUTF16CStr::FromMacRoman(
	const KUInt8*	inInputCString,
	KUInt16*		outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t count = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = count;
	}

	// If inLimit isn't null, take the smallest.
	if ((inLimit != 0) && (inLimit <= count))
	{
		count = inLimit - 1;	// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::FromMacRoman(
		inInputCString,
		outOutputCString,
		count );
	
	// Add the null terminator.
	outOutputCString[ count ] = 0;
}

// ----------------------------------------------------------------	//
//	* ToUCS4( const KUInt16*, KUInt32*, size_t )
// ----------------------------------------------------------------	//
void
UUTF16CStr::ToUCS4(
	const KUInt16*	inInputCString,
	KUInt32*		outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t inputSize = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = inputSize;
	}

	// Two cases: inLimit is null or it isn't.
	size_t outputSize = inLimit;
	if (outputSize == 0)
	{
		outputSize = inputSize;	// Simply because X characters in UTF-16
								// cannot be expanded to more than X
								// characters in these charsets
	} else {
		outputSize--;			// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::ToUCS4(
		inInputCString, &inputSize,
		outOutputCString, &outputSize );
	
	// Add the null terminator.
	outOutputCString[ outputSize ] = 0;
}

// ----------------------------------------------------------------	//
//	* ToUTF8( const KUInt16*, KUInt32*, size_t )
// ----------------------------------------------------------------	//
void
UUTF16CStr::ToUTF8(
	const KUInt16*	inInputCString,
	KUInt8*			outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Determine input string size.
	size_t inputSize = StrLen( inInputCString );
	
	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = inputSize;
	}

	// Two cases: inLimit is null or it isn't.
	size_t outputSize = inLimit;
	if (outputSize == 0)
	{
		// X caractères UTF-16 peuvent faire au plus 3.X caractères UTF-8.
		outputSize = 3 * inputSize;
	} else {
		outputSize--;			// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::ToUTF8(
		inInputCString, &inputSize,
		outOutputCString, &outputSize );
	
	// Add the null terminator.
	outOutputCString[ outputSize ] = 0;
}

// ----------------------------------------------------------------	//
//	* FromUTF8( const KUInt16*, KUInt32*, size_t )
// ----------------------------------------------------------------	//
void
UUTF16CStr::FromUTF8(
	const KUInt8*	inInputCString,
	KUInt16*		outOutputCString,
	size_t			inLimit /* = 0 */,
	size_t*			outStringSize /* = nil */
	)
{
	// Détermination de la taille de la chaîne en entrée.
	size_t inputSize = StrLen( inInputCString );
	
	// Two cases: inLimit is null or it isn't.
	size_t outputSize = inLimit;
	if (outputSize == 0)
	{
		// X caractères UTF-8 ne peuvent faire plus de X caractères
		// UTF-16.
		outputSize = inputSize;
	} else {
		outputSize--;			// Leave room for null terminator.
	}
	
	(void) UUTF16Conv::FromUTF8(
		inInputCString, &inputSize,
		outOutputCString, &outputSize );
	
	// Add the null terminator.
	outOutputCString[ outputSize ] = 0;

	// Fill the string size.
	if (outStringSize)
	{
		*outStringSize = outputSize;
	}
}

// --------------------------------------------------------------------------------	//
//	* StrLen( const KUInt32* )
// --------------------------------------------------------------------------------	//

size_t
UUTF16CStr::StrLen( const KUInt32* inCString )
{
	size_t theSize = 0;	// Counter for the characters.
	while (inCString[theSize] != 0)
	{
		theSize++;
	}
	
	return theSize;
}

// --------------------------------------------------------------------------------	//
//	* StrLen( const KUInt16* )
// --------------------------------------------------------------------------------	//

size_t
UUTF16CStr::StrLen( const KUInt16* inCString )
{
	size_t theSize = 0;	// Counter for the characters.
	while (inCString[theSize] != 0)
	{
		theSize++;
	}
	
	return theSize;
}

// --------------------------------------------------------------------------------	//
//	* StrLen( const KUInt8* )
// --------------------------------------------------------------------------------	//

size_t
UUTF16CStr::StrLen( const KUInt8* inCString )
{
	size_t theSize = 0;	// Counter for the characters.
	while (inCString[theSize] != 0)
	{
		theSize++;
	}
	
	return theSize;
}


// ======================================================================= //
// "... all the good computer designs are bootlegged; the formally planned //
// products, if they are built at all, are dogs!"                          //
//                 -- David E. Lundstrom, "A Few Good Men From Univac",    //
//                    MIT Press, 1987                                      //
// ======================================================================= //
