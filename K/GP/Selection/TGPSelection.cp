// ==============================
// Fichier:			TGPSelection.cp
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
// The Original Code is TGPSelection.cp.
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
// $Id: TGPSelection.cp,v 1.2 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Selection/TGPSelection.h>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TGPSelection( void )
// -------------------------------------------------------------------------- //
TGPSelection::TGPSelection( void )
	:
		mPopulation( nil )
{
}

// -------------------------------------------------------------------------- //
//  * ~TGPSelection( void )
// -------------------------------------------------------------------------- //
TGPSelection::~TGPSelection( void )
{
}

// -------------------------------------------------------------------------- //
//  * NewGeneration( const vector<TGPTree*>& )
// -------------------------------------------------------------------------- //
void
TGPSelection::NewGeneration( const vector<TGPTree*>& inPopulation )
{
	mPopulation = &inPopulation;
}

// ============================================================================= //
// At about 2500 A.D., humankind discovers a computer problem that *must* be     //
// solved.  The only difficulty is that the problem is NP complete and will      //
// take thousands of years even with the latest optical biologic technology      //
// available.  The best computer scientists sit down to think up some solution.  //
// In great dismay, one of the C.S. people tells her husband about it.  There    //
// is only one solution, he says.  Remember physics 103, Modern Physics, general //
// relativity and all.  She replies, "What does that have to do with solving     //
// a computer problem?"                                                          //
//         "Remember the twin paradox?"                                          //
//         After a few minutes, she says, "I could put the computer on a very    //
// fast machine and the computer would have just a few minutes to calculate but  //
// that is the exact opposite of what we want... Of course!  Leave the           //
// computer here, and accelerate the earth!"                                     //
//         The problem was so important that they did exactly that.  When        //
// the earth came back, they were presented with the answer:                     //
//                                                                               //
//         IEH032 Error in JOB Control Card.                                     //
// ============================================================================= //
