// ==============================
// Fichier:			TGPNodeNot.cp
// Projet:			K
//
// CrŽŽ le:			25/2/2003
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
// The Original Code is TGPNodeNot.cp.
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
// $Id: TGPNodeNot.cp,v 1.7 2005/02/14 20:55:04 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/BooleanNodes/TGPNodeNot.h>

// ISO C++
#include <iostream>

// K
#include <K/GP/Data/TGPDataRef.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TGPNodeNot( void )
// -------------------------------------------------------------------------- //
TGPNodeNot::TGPNodeNot( void )
	:
		TGPNodeBoolean( 1 ),
		mLispName( new TString( "!" ) )
{
}

// -------------------------------------------------------------------------- //
//  * GetLispName( void ) const
// -------------------------------------------------------------------------- //
TStringRef
TGPNodeNot::GetLispName( void ) const
{
	return mLispName;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPDataRef*, vector<TGPDataRef>&, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPNodeNot::Evaluate(
				TGPNodeRef* inNodeRef,
				TGPDataRef* ioResult,
				vector<TGPDataRef>& /* ioStorage */,
				TGPEvaluationContext* inContext,
				KUIntPtr* ioCookie )
{
	// Evaluation de la sous-branche.
	if (*ioCookie == 0)
	{
		*ioCookie = 1;
		return &inNodeRef->GetChildAlways( 0 );
	}

	*ioResult = TGPDataRef( (Boolean) !ioResult->AsBoolean( inContext ) );
	return nil;
}

// =========================================== //
// Bus error -- please leave by the rear door. //
// =========================================== //
