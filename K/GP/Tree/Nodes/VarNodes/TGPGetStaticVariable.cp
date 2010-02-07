// ==============================
// Fichier:			TGPGetStaticVariable.cp
// Projet:			K
// 
// CrŽŽ le:			14/8/2003
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
// The Original Code is TGPGetStaticVariable.cp.
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
// $Id: TGPGetStaticVariable.cp,v 1.7 2005/02/14 20:55:04 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/VarNodes/TGPGetStaticVariable.h>

// ISO C++
#include <stdexcept>

// K
#include <K/GP/TGPEvaluationContext.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
const char* TGPGetStaticVariable::kLispFormat = "set_staticvar<%s>";

// -------------------------------------------------------------------------- //
//  * TGPGetStaticVariable( const string&, const TGPType&, TGPDataRef& )
// -------------------------------------------------------------------------- //
TGPGetStaticVariable::TGPGetStaticVariable(
					const string& inVarName,
					const TGPType& inVarType,
					TGPDataRef& inDefaultValue )
	:
		TGPNode( 0 ),
		mVarName( inVarName ),
		mType( inVarType ),
		mDefaultValue( inDefaultValue )
{
	// On fait un petit sprintf.
	char theLispName[256];
	(void) ::sprintf( theLispName, kLispFormat, inVarName.c_str() );
	mLispName = new TString( theLispName );
}

// -------------------------------------------------------------------------- //
//  * TGPGetStaticVariable( const string&, TGPDataRef& )
// -------------------------------------------------------------------------- //
TGPGetStaticVariable::TGPGetStaticVariable(
					const string& inVarName,
					TGPDataRef& inDefaultValue )
	:
		TGPNode( 0 ),
		mVarName( inVarName ),
		mType( inDefaultValue.GetClass() ),
		mDefaultValue( inDefaultValue )
{
	// On fait un petit sprintf.
	char theLispName[256];
	(void) ::sprintf( theLispName, kLispFormat, inVarName.c_str() );
	mLispName = theLispName;
}

// -------------------------------------------------------------------------- //
//  * TGPGetStaticVariable( const TGPGetStaticVariable& )
// -------------------------------------------------------------------------- //
TGPGetStaticVariable::TGPGetStaticVariable( const TGPGetStaticVariable& inCopy )
	:
		TGPNode( inCopy ),
		mVarName( inCopy.mVarName ),
		mType( inCopy.mType ),
		mDefaultValue( inCopy.mDefaultValue ),
		mLispName( inCopy.mLispName )
{
}

// -------------------------------------------------------------------------- //
//  * GetType( void ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPGetStaticVariable::GetType( void ) const
{
	return mType;
}

// -------------------------------------------------------------------------- //
//  * GetParamType( KUInt32 ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPGetStaticVariable::GetParamType( KUInt32 /* inParamIndex */ ) const
{
	throw range_error(
		"Les noeuds TGPGetStaticVariable n'ont pas de sous-noeud" );
}

// -------------------------------------------------------------------------- //
//  * GetLispName( void ) const
// -------------------------------------------------------------------------- //
TStringRef
TGPGetStaticVariable::GetLispName( void ) const
{
	return mLispName;
}

// -------------------------------------------------------------------------- //
//  * IsConst( void ) const
// -------------------------------------------------------------------------- //
Boolean
TGPGetStaticVariable::IsConst( void ) const
{
	return false;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPDataRef*, vector<TGPDataRef>&, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPGetStaticVariable::Evaluate(
				TGPNodeRef* /* inNodeRef */,
				TGPDataRef* ioResult,
				vector<TGPDataRef>& /* ioStorage */,
				TGPEvaluationContext* inContext,
				KUIntPtr* /* ioCookie */ )
{
	// RŽcupŽration de la valeur.
	if (inContext->HasCookie( GetVarName() ))
	{
		*ioResult = inContext->GetCookie( GetVarName() );
	} else {
		*ioResult = GetDefaultValue();
	}
	
	return nil;
}

// ============================================================================= //
//         "Uncle Cosmo ... why do they call this a word processor?"             //
//         "It's simple, Skyler ... you've seen what food processors do to food, //
// right?"                                                                       //
//                 -- MacNelley, "Shoe"                                          //
// ============================================================================= //
