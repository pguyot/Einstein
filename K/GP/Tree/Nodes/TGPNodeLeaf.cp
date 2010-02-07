// ==============================
// Fichier:			TGPNodeLeaf.cp
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
// The Original Code is TGPNodeLeaf.cp.
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
// $Id: TGPNodeLeaf.cp,v 1.3 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNodeLeaf.h>

// ISO C++
#include <stdexcept>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPNodeLeaf( void )
// -------------------------------------------------------------------------- //
TGPNodeLeaf::TGPNodeLeaf( void )
	:
		TGPNode( 0 )
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * GetParamType( KUInt32 inParamIndex ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeLeaf::GetParamType( KUInt32 /* inParamIndex */ ) const
{
	throw range_error( "TGPNodeLeaf n'a pas de sous-noeud" );
}

// ========================================================================= //
// Go away! Stop bothering me with all your "compute this ... compute that"! //
// I'm taking a VAX-NAP.                                                     //
//                                                                           //
// logout                                                                   //
// ========================================================================= //
