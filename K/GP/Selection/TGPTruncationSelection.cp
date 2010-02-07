// ==============================
// Fichier:			TGPTruncationSelection.cp
// Projet:			K
// 
// Créé le:			6/11/2003
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
// The Original Code is TGPTruncationSelection.cp.
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
// $Id: TGPTruncationSelection.cp,v 1.6 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Selection/TGPTruncationSelection.h>

// K
#include <K/Math/TRandomGenerator.h>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TGPTruncationSelection( double )
// -------------------------------------------------------------------------- //
TGPTruncationSelection::TGPTruncationSelection( double inTruncationRatio )
	:
		TGPSortSelection(),
		mTruncationRatio( inTruncationRatio )
{
}

// -------------------------------------------------------------------------- //
//  * SelectIndividual( TRandomGenerator&, size_t )
// -------------------------------------------------------------------------- //
TGPTree*
TGPTruncationSelection::SelectIndividual(
				TRandomGenerator& inRandomGenerator,
				size_t /* inIndexIndividual */ )
{
	const vector<TGPTree*>& thePopulation = GetSortedPopulation();
	size_t thePopSize = thePopulation.size();
	size_t theTruncationSize = (size_t)
								(((double) thePopSize) * mTruncationRatio);
	
	// Choix de l'individu, aléatoirement.
	int theIndividualIndex =
					thePopSize - 1
					- inRandomGenerator.DistributionFast( theTruncationSize );

	if (theIndividualIndex < 0)
	{
		theIndividualIndex = 0;
	}
	if (theIndividualIndex >= (int) thePopSize)
	{
		theIndividualIndex = thePopSize - 1;
	}
	
	return thePopulation[theIndividualIndex];
}

// ========================================================================= //
// Real software engineers don't like the idea of some inexplicable and      //
// greasy hardware several aisles away that may stop working at any          //
// moment.  They have a great distrust of hardware people, and wish that     //
// systems could be virtual at *___all* levels.  They would like personal //
// computers (you know no one's going to trip over something and kill your   //
// DFA in mid-transit), except that they need 8 megabytes to run their       //
// Correctness Verification Aid packages.                                    //
// ========================================================================= //
