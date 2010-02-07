// ==============================
// Fichier:			TGPData.cp
// Projet:			K
//
// Créé le:			13/2/2003
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
// The Original Code is TGPData.cp.
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
// $Id: TGPData.cp,v 1.4 2005/02/14 20:55:02 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPData.h>

// ISO C++
#include <stdexcept>
#include <iostream>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPData( const TGPType& )
// -------------------------------------------------------------------------- //
TGPData::TGPData( const TGPType& inClass )
	:
		mClass( inClass )
{
}

// -------------------------------------------------------------------------- //
//  * ~TGPData( void )
// -------------------------------------------------------------------------- //
TGPData::~TGPData( void )
{
}

// ------------------------------------------------------------------------- //
//  * AsString( TGPEvaluationContext* ) const
// ------------------------------------------------------------------------- //
const string&
TGPData::AsString( TGPEvaluationContext* /* inContext */ ) const
{
	PrintToStream( cerr );
	throw runtime_error( "Cet objet ne peut être converti en chaîne" );
}

// ------------------------------------------------------------------------- //
//  * AsBoolean( TGPEvaluationContext* ) const
// ------------------------------------------------------------------------- //
Boolean
TGPData::AsBoolean( TGPEvaluationContext* /* inContext */ ) const
{
	PrintToStream( cerr );
	throw runtime_error( "Cet objet ne peut être converti en booléen" );
}

// ------------------------------------------------------------------------- //
//  * AsInteger( TGPEvaluationContext* ) const
// ------------------------------------------------------------------------- //
KSInt32
TGPData::AsInteger( TGPEvaluationContext* /* inContext */ ) const
{
	PrintToStream( cerr );
	throw runtime_error( "Cet objet ne peut être converti en entier" );
}

// ============================================== //
// Marvelous!  The super-user's going to boot me! //
// What a finely tuned response to the situation! //
// ============================================== //
