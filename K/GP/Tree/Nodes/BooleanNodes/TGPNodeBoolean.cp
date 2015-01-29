// ==============================
// Fichier:			TGPNodeBoolean.cp
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
// The Original Code is TGPNodeBoolean.cp.
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
// $Id: TGPNodeBoolean.cp,v 1.3 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/BooleanNodes/TGPNodeBoolean.h>

// ISO C++
#include <stdexcept>

// K
#include <K/GP/Data/TGPBooleanData.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPNodeBoolean( KUInt32 )
// -------------------------------------------------------------------------- //
TGPNodeBoolean::TGPNodeBoolean( KUInt32 inArity )
	:
		TGPNode( inArity )
{
}

// -------------------------------------------------------------------------- //
//  * GetType( void ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeBoolean::GetType( void ) const
{
	return TGPBooleanData::kClass;
}

// -------------------------------------------------------------------------- //
//  * GetParamType( KUInt32 ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeBoolean::GetParamType( KUInt32 inParamIndex ) const
{
	KUInt32 theArity = GetArity();
	
	if (inParamIndex > theArity)
	{
		throw range_error( "Ce noeud TGPNodeBoolean n'a pas autant de sous-noeuds" );
	}
	
	return TGPBooleanData::kClass;
}


// ======================================== //
// Ask not for whom the <CONTROL-G> tolls. //
// ======================================== //
