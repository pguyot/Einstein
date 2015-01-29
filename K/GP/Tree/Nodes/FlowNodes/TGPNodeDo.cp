// ==============================
// Fichier:			TGPNodeDo.cp
// Projet:			K
// 
// Créé le:			3/9/2003
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
// The Original Code is TGPNodeDo.cp.
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
// $Id: TGPNodeDo.cp,v 1.6 2004/11/23 15:11:31 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/FlowNodes/TGPNodeDo.h>

// ANSI C
#include <stdio.h>

// K
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

// ISO C++
#include <stdexcept>
#include <sstream>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPNodeDo( const TGPType&, KUInt32  )
// -------------------------------------------------------------------------- //
TGPNodeDo::TGPNodeDo( const TGPType& inType, KUInt32 inArity )
	:
		TGPNode( inArity ),
		mType( inType )
{
	// On utilise sstream.
	ostringstream someStream;
	(void) (someStream << "do<" << inType.GetName() << ">");
	
	mLispName = someStream.str();
}

// -------------------------------------------------------------------------- //
//  * GetType( void ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeDo::GetType( void ) const
{
	return mType;
}

// -------------------------------------------------------------------------- //
//  * GetParamType( KUInt32 ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeDo::GetParamType( KUInt32 inParamIndex ) const
{
	if (inParamIndex < GetArity())
	{
		return mType;
	}
	
	throw range_error( "Le noeud TGPNodeDo n'a pas autant de sous-noeuds" );
}

// -------------------------------------------------------------------------- //
//  * GetLispName( void ) const
// -------------------------------------------------------------------------- //
TStringRef
TGPNodeDo::GetLispName( void ) const
{
	return mLispName;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPDataRef*, vector<TGPDataRef>&, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPNodeDo::Evaluate(
				TGPNodeRef* inNodeRef,
				TGPDataRef* /* ioResult */,
				vector<TGPDataRef>& /* ioStorage */,
				TGPEvaluationContext* /* inContext */,
				KUIntPtr* ioCookie )
{
	// Evaluation de toutes les branches, dans l'ordre.	
	KUInt32 indexSubnodes;
	KUInt32 nbSubnodes = GetArity();
	for (indexSubnodes = 0; indexSubnodes < nbSubnodes; indexSubnodes++)
	{
		if (*ioCookie == indexSubnodes)
		{
			(*ioCookie)++;
			return &inNodeRef->GetChildAlways( indexSubnodes );
		}
	}
	
	// On retourne le résultat de l'évaluation de la dernière branche.
//	*ioResult = *ioResult;
	return nil;
}

// ==================================================== //
// The only thing worse than X Windows: (X Windows) - X //
// ==================================================== //
