// ==============================
// Fichier:			TGPVoidData.cp
// Projet:			K
// 
// CrŽŽ le:			16/10/2003
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
// The Original Code is TGPVoidData.cp.
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
// $Id: TGPVoidData.cp,v 1.3 2005/02/14 20:55:02 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPVoidData.h>

// K
#include <K/GP/Data/TGPDataRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
const TGPVoidData::TClass TGPVoidData::kClass;
const TStringRef TGPVoidData::TClass::kName( "Void" );

// -------------------------------------------------------------------------- //
//  * ValueFromString( const char* )
// -------------------------------------------------------------------------- //
Boolean
TGPVoidData::TClass::ValueFromString(
							TGPDataRef& outValue,
							const char* /* inString */ ) const
{
	outValue = new TGPVoidData( *this );
	return true;
}

// -------------------------------------------------------------------------- //
//  * TGPVoidData( const TGPType& )
// -------------------------------------------------------------------------- //
TGPVoidData::TGPVoidData( const TGPType& inClass /* = kClass */ )
	:
		TGPData( inClass )
{
}

// -------------------------------------------------------------------------- //
//  * Equal( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPVoidData::Equal(
				const TGPData& /* inAlter */,
				TGPEvaluationContext* /* inContext */ ) const
{
	return false;
}

// -------------------------------------------------------------------------- //
//  * GreaterThan( const TGPData&, TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPVoidData::GreaterThan(
				const TGPData& /* inAlter */,
				TGPEvaluationContext* /* inContext */ ) const
{
	return false;
}

// -------------------------------------------------------------------------- //
//  * PrintToStream( ostream& ) const
// -------------------------------------------------------------------------- //
void
TGPVoidData::PrintToStream( ostream& inStream ) const
{
	(void) (inStream << "void");
}

// ============================================================================= //
//         The master programmer moves from program to program without fear.  No //
// change in management can harm him.  He will not be fired, even if the project //
// is canceled. Why is this?  He is filled with the Tao.                         //
//                 -- Geoffrey James, "The Tao of Programming"                   //
// ============================================================================= //
