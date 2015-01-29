// ==============================
// Fichier:			TGPNumberData.cp
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
// The Original Code is TGPNumberData.cp.
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
// $Id: TGPNumberData.cp,v 1.3 2005/02/14 20:55:02 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/Numbers/TGPNumberData.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  Constantes
// -------------------------------------------------------------------------- //
const TGPNumberData::TClass TGPNumberData::kClass;
const TStringRef TGPNumberData::TClass::kName( "Number" );

// -------------------------------------------------------------------------- //
//  * ValueFromString( const char* )
// -------------------------------------------------------------------------- //
Boolean
TGPNumberData::TClass::ValueFromString(
							TGPDataRef& /* outValue */,
							const char* /* inString */ ) const
{
	return false;
}

// -------------------------------------------------------------------------- //
//  * TGPNumberData( const TGPType& )
// -------------------------------------------------------------------------- //
TGPNumberData::TGPNumberData( const TGPType& inClass /* = kClass */ )
	:
		TGPData( inClass )
{
	// This space for rent.
}

// ==================================================================== //
// A computer scientist is someone who fixes things that aren't broken. //
// ==================================================================== //
