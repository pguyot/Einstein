// ==============================
// Fichier:			TInt64.cp
// Projet:			K
// 
// CrŽŽ le:			21/2/2004
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
// The Original Code is TInt64.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TInt64.cp,v 1.4 2005/02/24 06:49:58 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Math/TInt64.h>

// --------------------------------------------------------------------	//
//	* operator &= ( const KUInt32 )
// --------------------------------------------------------------------	//
TInt64&
TInt64::operator &= ( const KUInt32 inArgument )
{
	mLo &= inArgument;
	mHi = 0;
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator &= ( const TInt64& )
// --------------------------------------------------------------------	//
TInt64&
TInt64::operator &= ( const TInt64& inArgument )
{
	// Idem.
	mLo &= inArgument.GetLo();
	mHi &= inArgument.GetHi();
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator |= ( const KUInt32 )
// --------------------------------------------------------------------	//
TInt64&
TInt64::operator |= ( const KUInt32 inArgument )
{
	mLo |= inArgument;
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator |= ( const TInt64& )
// --------------------------------------------------------------------	//
TInt64&
TInt64::operator |= ( const TInt64& inArgument )
{
	// Idem.
	mLo |= inArgument.GetLo();
	mHi |= inArgument.GetHi();
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator ^= ( const KUInt32 )
// --------------------------------------------------------------------	//
TInt64&
TInt64::operator ^= ( const KUInt32 inArgument )
{
	mLo ^= inArgument;
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator ^= ( const TInt64& )
// --------------------------------------------------------------------	//
TInt64&
TInt64::operator ^= ( const TInt64& inArgument )
{
	// Idem.
	mLo ^= inArgument.GetLo();
	mHi ^= inArgument.GetHi();
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator ~ ( void ) const
// --------------------------------------------------------------------	//
TInt64
TInt64::operator ~ ( void ) const
{
	return TInt64( ~mHi, ~mLo );
}

// --------------------------------------------------------------------	//
//	* ShiftRight( void )
// --------------------------------------------------------------------	//
bool
TInt64::ShiftRight( void )
{
	bool theResult = (mLo & 0x01) != 0;
	mLo >>= 1;
	if (mHi & 0x01)
		mLo |= 0x80000000;
	mHi >>= 1;
	
	return theResult;
}

// --------------------------------------------------------------------	//
//	* ShiftLeft( void )
// --------------------------------------------------------------------	//
bool
TInt64::ShiftLeft( void )
{
	bool theResult = (mHi & 0x80000000) != 0;
	mHi <<= 1;
	if (mLo & 0x80000000)
		mHi |= 0x01;
	mLo <<= 1;
	
	return theResult;
}

// OpŽrateurs binaires (non inline)

// --------------------------------------------------------------------	//
//	* operator == ( const TInt64&, const TInt64& )
// --------------------------------------------------------------------	//
bool
operator == ( const TInt64& inArgOne, const TInt64& inArgTwo )
{
	return (inArgOne.GetHi() == inArgTwo.GetHi())
		&& (inArgOne.GetLo() == inArgTwo.GetLo());
}

// --------------------------------------------------------------------	//
//	* operator != ( const TInt64&, const TInt64& )
// --------------------------------------------------------------------	//
bool
operator != ( const TInt64& inArgOne, const TInt64& inArgTwo )
{
	return ((inArgOne.GetLo() != inArgTwo.GetLo())
		|| (inArgOne.GetHi() != inArgTwo.GetHi()));
}

// ================================================================ //
// FORTRAN is a good example of a language which is easier to parse //
// using ad hoc techniques.                                         //
//                 -- D. Gries                                      //
//                 [What's good about it?  Ed.]                     //
// ================================================================ //
