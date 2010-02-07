// ==============================
// Fichier:			TGPNodeOr.cp
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
// The Original Code is TGPNodeOr.cp.
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
// $Id: TGPNodeOr.cp,v 1.7 2005/02/14 20:55:04 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/BooleanNodes/TGPNodeOr.h>

// ISO C++
#include <iostream>

// K
#include <K/GP/Data/TGPDataRef.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  Constantes
// -------------------------------------------------------------------------- //
const char* TGPNodeOr::kLispFormat = "||<%i>";

// -------------------------------------------------------------------------- //
//  * TGPNodeOr( KUInt32 )
// -------------------------------------------------------------------------- //
TGPNodeOr::TGPNodeOr( KUInt32 inArity )
	:
		TGPNodeBoolean( inArity )
{
	// On fait un petit sprintf.
	char theLispName[256];
	(void) ::sprintf( theLispName, kLispFormat, inArity );
	mLispName = theLispName;
}

// -------------------------------------------------------------------------- //
//  * GetLispName( void ) const
// -------------------------------------------------------------------------- //
TStringRef
TGPNodeOr::GetLispName( void ) const
{
	return mLispName;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPDataRef*, vector<TGPDataRef>&, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPNodeOr::Evaluate(
				TGPNodeRef* inNodeRef,
				TGPDataRef* ioResult,
				vector<TGPDataRef>& /* ioStorage */,
				TGPEvaluationContext* inContext,
				KUIntPtr* ioCookie )
{
	// Evaluation des branches de gauche à droite jusqu'à ce qu'on trouve
	// une branche qui retourne \c true.
	KUInt32 theCookie = *ioCookie;
	if (theCookie == 0)
	{
		*ioCookie = 1;
		return &inNodeRef->GetChildAlways( 0 );
	}
	KUInt32 nbSubnodes = GetArity();
	if (theCookie < nbSubnodes)
	{
		// Test de la branche précédente.
		if (!ioResult->AsBoolean( inContext ))
		{	
			// Le résultat est faux.
			// Passage à la suivante.
			(*ioCookie)++;
			return &inNodeRef->GetChildAlways( theCookie );
		}
	}
	// Si toutes les branches sont fausses, je retourne la dernière.
	// Si une branche n'est pas fausse, je retourne sa valeur.
//	*ioResult = *ioResult;
	return nil;
}

// ============= //
// PLUG IT IN!!! //
// ============= //
