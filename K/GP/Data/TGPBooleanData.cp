// ==============================
// Fichier:			TGPBooleanData.cp
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
// The Original Code is TGPBooleanData.cp.
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
// $Id: TGPBooleanData.cp,v 1.4 2005/02/14 20:55:02 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPBooleanData.h>

// K
#include <K/GP/Data/TGPDataRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  Constantes
// -------------------------------------------------------------------------- //
const string TGPBooleanData::kTrueStr( "true" );
const string TGPBooleanData::kFalseStr( "false" );
const TGPBooleanData::TClass TGPBooleanData::kClass;
const TStringRef TGPBooleanData::TClass::kName( "Boolean" );

// -------------------------------------------------------------------------- //
//  * ValueFromString( const char* )
// -------------------------------------------------------------------------- //
Boolean
TGPBooleanData::TClass::ValueFromString(
							TGPDataRef& outValue,
							const char* inString ) const
{
	Boolean theResult = true;

	if (kTrueStr == inString)
	{
		outValue = (Boolean) true;
	} else if (kFalseStr == inString) {
		outValue = (Boolean) false;
	} else {
		theResult = false;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * TGPBooleanData( Boolean, const TGPType& )
// -------------------------------------------------------------------------- //
TGPBooleanData::TGPBooleanData(
					Boolean inBoolean,
					const TGPType& inClass /* = kClass */ )
	:
		TGPData( inClass ),
		mBoolean( inBoolean )
{
}

// ------------------------------------------------------------------------- //
//  * AsString( TGPEvaluationContext* ) const
// ------------------------------------------------------------------------- //
const string&
TGPBooleanData::AsString( TGPEvaluationContext* /* inContext */ ) const
{
	return mBoolean ? kTrueStr : kFalseStr;
}

// -------------------------------------------------------------------------- //
//  * AsBoolean( TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPBooleanData::AsBoolean( TGPEvaluationContext* /* inContext */ ) const
{
	return mBoolean;
}

// -------------------------------------------------------------------------- //
//  * Equal( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPBooleanData::Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const
{
	bool theResult = false;
	if (inAlter.IsInstanceOf(kClass))
	{
		theResult = (mBoolean == inAlter.AsBoolean( inContext ));
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GreaterThan( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPBooleanData::GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const
{
	bool theResult = false;
	if (inAlter.IsInstanceOf(kClass))
	{
		theResult = (mBoolean && !inAlter.AsBoolean( inContext ));
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * PrintToStream( ostream& ) const
// -------------------------------------------------------------------------- //
void
TGPBooleanData::PrintToStream( ostream& inStream ) const
{
	if (mBoolean)
	{
		(void) (inStream << kTrueStr);
	} else {
		(void) (inStream << kFalseStr);
	}
}

// ======================================================== //
// The absence of labels [in ECL] is probably a good thing. //
//                 -- T. Cheatham                           //
// ======================================================== //
