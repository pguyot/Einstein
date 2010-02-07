// ==============================
// Fichier:			TGPStringData.cp
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
// The Original Code is TGPStringData.cp.
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
// $Id: TGPStringData.cp,v 1.4 2005/02/14 20:55:02 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPStringData.h>

// K
#include <K/GP/Data/TGPDataRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  Constantes
// -------------------------------------------------------------------------- //
const TGPStringData::TClass TGPStringData::kClass;
const TStringRef TGPStringData::TClass::kName( "String" );

// -------------------------------------------------------------------------- //
//  * ValueFromString( const char* )
// -------------------------------------------------------------------------- //
Boolean
TGPStringData::TClass::ValueFromString(
							TGPDataRef& outValue,
							const char* inString ) const
{
	string theValue = inString;
	outValue = theValue;
	
	return true;
}

// -------------------------------------------------------------------------- //
//  * TGPStringData( const string, const TGPType& )
// -------------------------------------------------------------------------- //
TGPStringData::TGPStringData(
					const string inString,
					const TGPType& inClass /* = kClass */ )
	:
		TGPData( inClass ),
		mString( inString )
{
}

// -------------------------------------------------------------------------- //
//  * AsString( TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
const string&
TGPStringData::AsString( TGPEvaluationContext* /* inContext */ ) const
{
	return mString;
}

// -------------------------------------------------------------------------- //
//  * Equal( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPStringData::Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const
{
	bool theResult = false;
	if (inAlter.IsInstanceOf(kClass))
	{
		theResult = (mString == inAlter.AsString(inContext));
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GreaterThan( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPStringData::GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const
{
	bool theResult = false;
	if (inAlter.IsInstanceOf(kClass))
	{
		theResult = (mString > inAlter.AsString(inContext));
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * PrintToStream( ostream& ) const
// -------------------------------------------------------------------------- //
void
TGPStringData::PrintToStream( ostream& inStream ) const
{
	(void) (inStream << mString);
}

// =========================================================================== //
// The use of COBOL cripples the mind; its teaching should, therefore, be      //
// regarded as a criminal offence.                                             //
//                 -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5 //
// =========================================================================== //
