// ==============================
// Fichier:			TGPSInt32Data.cp
// Projet:			K
//
// CrŽŽ le:			23/2/2003
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
// The Original Code is TGPSInt32Data.cp.
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
// $Id: TGPSInt32Data.cp,v 1.3 2005/02/14 20:55:02 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/Numbers/TGPSInt32Data.h>

// K
#include <K/GP/Data/TGPDataRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  Constantes
// -------------------------------------------------------------------------- //
const TGPSInt32Data::TClass TGPSInt32Data::kClass;
const TStringRef TGPSInt32Data::TClass::kName( "SInt32" );

// -------------------------------------------------------------------------- //
//  * ValueFromString( const char* )
// -------------------------------------------------------------------------- //
Boolean
TGPSInt32Data::TClass::ValueFromString(
							TGPDataRef& outValue,
							const char* inString ) const
{
	Boolean theResult = true;
	int theValue;

	if (::sscanf( inString, "%i", &theValue ) == 1)
	{
		outValue = (KSInt32) theValue;
	} else {
		theResult = false;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * TGPSInt32Data( KSInt32, const TGPData& )
// -------------------------------------------------------------------------- //
TGPSInt32Data::TGPSInt32Data(
					KSInt32 inInteger,
					const TGPType& inClass /* = kClass */ )
	:
		TGPNumberData( inClass ),
		mInteger( inInteger )
{
	// On fait un petit sprintf pour l'entier.
	char theIntegerAsStr[10];
	(void) ::sprintf( theIntegerAsStr, "%i", (int) inInteger );
	mIntStr = theIntegerAsStr;
}

// ------------------------------------------------------------------------- //
//  * AsString( TGPEvaluationContext* ) const
// ------------------------------------------------------------------------- //
const string&
TGPSInt32Data::AsString( TGPEvaluationContext* /* inContext */ ) const
{
	return mIntStr;
}

// -------------------------------------------------------------------------- //
//  * AsInteger( TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
KSInt32
TGPSInt32Data::AsInteger( TGPEvaluationContext* /* inContext */ ) const
{
	return mInteger;
}

// -------------------------------------------------------------------------- //
//  * Equal( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPSInt32Data::Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const
{
	bool theResult = false;
	if (inAlter.IsInstanceOf(kClass))
	{
		theResult = (mInteger == inAlter.AsInteger(inContext));
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GreaterThan( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPSInt32Data::GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const
{
	bool theResult = false;
	if (inAlter.IsInstanceOf(TGPNumberData::kClass))
	{
		theResult = (mInteger > inAlter.AsInteger(inContext));
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * PrintToStream( ostream& ) const
// -------------------------------------------------------------------------- //
void
TGPSInt32Data::PrintToStream( ostream& inStream ) const
{
	(void) (inStream << mInteger);
}

// ======================================================== //
// Your computer account is overdrawn.  Please reauthorize. //
// ======================================================== //
