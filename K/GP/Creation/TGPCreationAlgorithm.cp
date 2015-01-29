// ==============================
// Fichier:			TGPCreationAlgorithm.cp
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
// The Original Code is TGPCreationAlgorithm.cp.
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
// $Id: TGPCreationAlgorithm.cp,v 1.5 2004/11/23 15:11:29 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Creation/TGPCreationAlgorithm.h>

// K GP
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

// -------------------------------------------------------------------------- //
//  * TGPCreationAlgorithm( void )
// -------------------------------------------------------------------------- //
TGPCreationAlgorithm::TGPCreationAlgorithm( void )
{
	// Gaz ˆ tous les Žtages.
}

// -------------------------------------------------------------------------- //
//  * CreateTree( TRandomGenerator&, const TGPLanguage&, const TGPType& ) const
// -------------------------------------------------------------------------- //
TGPNodeRef
TGPCreationAlgorithm::CreateTree(
				TRandomGenerator& inRandomGenerator,
				const TGPLanguage& inLanguage,
				const TGPType& inType ) const
{
	return CreateRoot( inRandomGenerator, inLanguage, inType );
}

// ============================================================================ //
// Think of your family tonight.  Try to crawl home after the computer crashes. //
// ============================================================================ //
