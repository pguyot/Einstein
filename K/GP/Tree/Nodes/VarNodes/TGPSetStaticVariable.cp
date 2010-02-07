// ==============================
// Fichier:			TGPSetStaticVariable.cp
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
// The Original Code is TGPSetStaticVariable.cp.
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
// $Id: TGPSetStaticVariable.cp,v 1.6 2004/11/23 15:11:31 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/VarNodes/TGPSetStaticVariable.h>

// ISO C++
#include <stdexcept>

// K
#include <K/GP/TGPEvaluationContext.h>
#include <K/GP/Data/TGPType.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
const char* TGPSetStaticVariable::kLispFormat = "set_staticvar<%s>";

// -------------------------------------------------------------------------- //
//  * TGPSetStaticVariable( const string&, const TGPType&, const TGPType& )
// -------------------------------------------------------------------------- //
TGPSetStaticVariable::TGPSetStaticVariable(
					const string& inVarName,
					const TGPType& inVarType,
					const TGPType& inNodeType )
	:
		TGPNode( 1 ),
		mVarName( inVarName ),
		mVarType( inVarType ),
		mNodeType( inNodeType )
{
	// On fait un petit sprintf.
	char theLispName[256];
	(void) ::sprintf( theLispName, kLispFormat, inVarName.c_str() );
	mLispName = new TString( theLispName );
}

// -------------------------------------------------------------------------- //
//  * TGPSetStaticVariable( const string&, const TGPType& )
// -------------------------------------------------------------------------- //
TGPSetStaticVariable::TGPSetStaticVariable(
					const string& inVarName,
					const TGPType& inVarType )
	:
		TGPNode( 1 ),
		mVarName( inVarName ),
		mVarType( inVarType ),
		mNodeType( inVarType )
{
	// On fait un petit sprintf.
	char theLispName[256];
	(void) ::sprintf( theLispName, kLispFormat, inVarName.c_str() );
	mLispName = theLispName;
}

// -------------------------------------------------------------------------- //
//  * TGPSetStaticVariable( const TGPSetStaticVariable& )
// -------------------------------------------------------------------------- //
TGPSetStaticVariable::TGPSetStaticVariable( const TGPSetStaticVariable& inCopy )
	:
		TGPNode( inCopy ),
		mVarName( inCopy.mVarName ),
		mVarType( inCopy.mVarType ),
		mNodeType( inCopy.mNodeType ),
		mLispName( inCopy.mLispName )
{
}

// -------------------------------------------------------------------------- //
//  * GetType( void ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPSetStaticVariable::GetType( void ) const
{
	return mNodeType;
}

// -------------------------------------------------------------------------- //
//  * GetParamType( KUInt32 ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPSetStaticVariable::GetParamType( KUInt32 inParamIndex ) const
{
	if (inParamIndex >= 1)
	{
		throw range_error(
			"Les noeuds TGPSetStaticVariable n'ont qu'un sous-noeud" );
	}
	
	return mVarType;
}

// -------------------------------------------------------------------------- //
//  * GetLispName( void ) const
// -------------------------------------------------------------------------- //
TStringRef
TGPSetStaticVariable::GetLispName( void ) const
{
	return mLispName;
}

// -------------------------------------------------------------------------- //
//  * IsConst( void ) const
// -------------------------------------------------------------------------- //
Boolean
TGPSetStaticVariable::IsConst( void ) const
{
	return false;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPDataRef*, vector<TGPDataRef>&, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPSetStaticVariable::Evaluate(
				TGPNodeRef* inNodeRef,
				TGPDataRef* ioResult,
				vector<TGPDataRef>& /* ioStorage */,
				TGPEvaluationContext* inContext,
				KUIntPtr* ioCookie )
{
	// RŽcupŽration de la nouvelle valeur de la variable (premier sous-noeud).
	if (*ioCookie == 0)
	{
		*ioCookie = 1;
		return &inNodeRef->GetChildAlways( 0 );
	}
	
	// Stockage de la valeur pour la variable.
	inContext->SetCookie( GetVarName(), *ioResult );
	
	// On retourne la valeur.
//	*ioResult = *ioResult;
	
	return nil;
}

// ========================================================================== //
// Thus spake the master programmer:                                          //
//         "Though a program be but three lines long, someday it will have to //
//         be maintained."                                                    //
//                 -- Geoffrey James, "The Tao of Programming"                //
// ========================================================================== //
