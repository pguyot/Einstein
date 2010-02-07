// ==============================
// Fichier:			TGPNodeIfThenElse.cp
// Projet:			K
//
// Créé le:			25/2/2003
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
// The Original Code is TGPNodeIfThenElse.cp.
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
// $Id: TGPNodeIfThenElse.cp,v 1.7 2005/02/14 20:55:04 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/FlowNodes/TGPNodeIfThenElse.h>

// ANSI C
#include <stdio.h>

// ISO C++
#include <stdexcept>
#include <sstream>

// K
#include <K/GP/Data/TGPBooleanData.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPNodeIfThenElse( const TGPType&  )
// -------------------------------------------------------------------------- //
TGPNodeIfThenElse::TGPNodeIfThenElse( const TGPType& inType )
	:
		TGPNode( 3 ),
		mType( inType )
{
	// On utilise sstream.
	ostringstream someStream;
	(void) (someStream << "if<" << inType.GetName() << ">");
	
	mLispName = someStream.str();
}

// -------------------------------------------------------------------------- //
//  * GetType( void ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeIfThenElse::GetType( void ) const
{
	return mType;
}

// -------------------------------------------------------------------------- //
//  * GetParamType( KUInt32 ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeIfThenElse::GetParamType( KUInt32 inParamIndex ) const
{
	if (inParamIndex == 0)
	{
		return TGPBooleanData::kClass;
	}
	
	if ((inParamIndex == 1) || (inParamIndex == 2))
	{
		return mType;
	}
	
	throw range_error( "Le noeud TGPNodeIfThenElse n'a que trois sous-noeuds" );
}

// -------------------------------------------------------------------------- //
//  * GetLispName( void ) const
// -------------------------------------------------------------------------- //
TStringRef
TGPNodeIfThenElse::GetLispName( void ) const
{
	return mLispName;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPDataRef*, vector<TGPDataRef>&, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPNodeIfThenElse::Evaluate(
				TGPNodeRef* inNodeRef,
				TGPDataRef* ioResult,
				vector<TGPDataRef>& /* ioStorage */,
				TGPEvaluationContext* inContext,
				KUIntPtr* ioCookie )
{
	// Evaluation de la branche gauche.
	if (*ioCookie == 0)
	{
		*ioCookie = 1;
		return &inNodeRef->GetChildAlways( 0 );
	}
	
	// Evaluation de la branche du milieu ou de la branche droite suivant le
	// résultat.
	if (*ioCookie == 1)
	{
		*ioCookie = 2;
		
		KSInt32 theIndex;
		if (ioResult->AsBoolean( inContext ))
		{
			theIndex = 1;
		} else {
			theIndex = 2;
		}
	
		return &inNodeRef->GetChildAlways( theIndex );
	}
	
	// On retourne le résultat de la branche du milieu ou celle de droite.
//	*ioResult = *ioResult;
	return nil;
}

// ======================================================================== //
// UNIX was not designed to stop you from doing stupid things, because that //
// would also stop you from doing clever things.                            //
//                 -- Doug Gwyn                                             //
// ======================================================================== //
