// ==============================
// Fichier:			UUTF16CStr.h
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
// The Original Code is UUTF16CStr.h.
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

#ifndef __UUTF16CSTR__
#define __UUTF16CSTR__

#include <K/Unicode/UnicodeDefinitions.h>

// ================================================ //
//   ••••• Definition for class UUTF16CStr •••••    //
// ================================================ //

// UTF-16 to/from another charset converter of CStrings.
// Based on UUTF16Conv.h
// Only fixed-size charsets are handled here.

// Calling convention for all conversion functions:
//	Converts the characters from C String (i.e. null terminated)
//	inInputCString to C String outOutputCString.
//	inLimit is the maximum size of the output buffer.
//  outStringSize, if not null, is filled with the string size (in characters).
//	this value is undefined if the string was truncated.
//	Since the functions add a null terminator to the output
//	string, n means that at most n-1 characters will be translated.
//	If inLimit is 0 (default) no limit is assumed.
//	This can be dangerous.
// Result is unknown if input string isn't correct.
// Like UUTF16Conv class, UTF-16 strings are big endian
// whatever the sex of your bytes is.

class UUTF16CStr
{
public:
	static void ToISO88591(
		const KUInt16*	inInputCString,
		KUInt8*			outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	static void FromISO88591(
		const KUInt8*	inInputCString,
		KUInt16*		outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	
	static void ToISO88592(
		const KUInt16*	inInputCString,
		KUInt8*			outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	static void FromISO88592(
		const KUInt8*	inInputCString,
		KUInt16*		outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	
	static void ToASCII(
		const KUInt16*	inInputCString,
		KUInt8*			outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	static void FromASCII(
		const KUInt8*	inInputCString,
		KUInt16*		outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	
	static void ToMacRoman(
		const KUInt16*	inInputCString,
		KUInt8*			outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	static void FromMacRoman(
		const KUInt8*	inInputCString,
		KUInt16*		outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);

	static void ToUCS4(
		const KUInt16*	inInputCString,
		KUInt32*		outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
//	static void FromUCS4(
//		const KUInt32*	inInputCString,
//		KUInt16*		outOutputCString,
//		size_t			inLimit = 0,
//		size_t*			outStringSize = nil
//		);
	static void ToUTF8(
		const KUInt16*	inInputCString,
		KUInt8*			outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);
	static void FromUTF8(
		const KUInt8*	inInputCString,
		KUInt16*		outOutputCString,
		size_t			inLimit = 0,
		size_t*			outStringSize = nil
		);

	// Return the size of the CStrings minus the null terminator.
	// (just like ANSI C strlen).
	static size_t StrLen( const KUInt32* inCString );
	static size_t StrLen( const KUInt16* inCString );
	static size_t StrLen( const KUInt8* inCString );
};

#endif
		// __UUTF16CSTR__

// ================================================= //
//     *** System shutdown message from root *** //
//                                                   //
// System going down in 60 seconds                   //
// ================================================= //
