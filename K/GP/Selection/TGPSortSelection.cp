// ==============================
// Fichier:			TGPSortSelection.cp
// Projet:			K
// 
// CrŽŽ le:			6/11/2003
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
// The Original Code is TGPSortSelection.cp.
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
// $Id: TGPSortSelection.cp,v 1.2 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Selection/TGPSortSelection.h>

// ISO C++
#include <algorithm>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * NewGeneration( const vector<TGPTree*>& )
// -------------------------------------------------------------------------- //
void
TGPSortSelection::NewGeneration( const vector<TGPTree*>& inPopulation )
{
	// Copie de la population.
	mSortedPopulation = inPopulation;
	
	// Tri.
	sort( mSortedPopulation.begin(), mSortedPopulation.end() );
}

// =============================================================== //
// SCCS, the source motel!  Programs check in and never check out! //
//                 -- Ken Thompson                                 //
// =============================================================== //
