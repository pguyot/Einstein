// ==============================
// Fichier:			TGPTournamentSelection.cp
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
// The Original Code is TGPTournamentSelection.cp.
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
// $Id: TGPTournamentSelection.cp,v 1.2 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Selection/TGPTournamentSelection.h>

// K
#include <K/GP/Fitness/TGPFitness.h>
#include <K/GP/Tree/TGPTree.h>
#include <K/Math/TRandomGenerator.h>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TGPTournamentSelection( KUInt32 )
// -------------------------------------------------------------------------- //
TGPTournamentSelection::TGPTournamentSelection( KUInt32 inTournamentSize )
	:
		mTournamentSize( inTournamentSize )
{
}

// -------------------------------------------------------------------------- //
//  * NewGeneration( const std::vector<TGPTree*>& )
// -------------------------------------------------------------------------- //
void
TGPTournamentSelection::NewGeneration(
								const std::vector<TGPTree*>& inPopulation )
{
	TGPSelection::NewGeneration( inPopulation );
	
	mPopulationSize = inPopulation.size();
}

// -------------------------------------------------------------------------- //
//  * NewGeneration( const std::vector<TGPTree*>& )
// -------------------------------------------------------------------------- //
TGPTree*
TGPTournamentSelection::SelectIndividual(
							TRandomGenerator& inRandomGenerator,
							size_t /* inIndexIndividual */ )
{
	const vector<TGPTree*>& thePopulation = GetPopulation();
	
	KUInt32 indexTournament;
	TGPTree* theBestTree = nil;
	const TGPFitness* theBestFitness = nil;
	for (
			indexTournament = 0;
			indexTournament < mTournamentSize;
			++indexTournament)
	{
		TGPTree* theTree = thePopulation[inRandomGenerator( mPopulationSize )];
		const TGPFitness* theTreeFitness = theTree->GetFitness();
		if ((theBestTree == nil) || (*theBestFitness < *theTreeFitness))
		{
			theBestTree = theTree;
			theBestFitness = theTreeFitness;
		}
	}
	
	return theBestTree;
}

// =============================================== //
// The tao that can be tar(1)ed                    //
// is not the entire Tao.                          //
// The path that can be specified                  //
// is not the Full Path.                           //
//                                                 //
// We declare the names                            //
// of all variables and functions.                 //
// Yet the Tao has no type specifier.              //
//                                                 //
// Dynamically binding, you realize the magic.     //
// Statically binding, you see only the hierarchy. //
//                                                 //
// Yet magic and hierarchy                         //
// arise from the same source,                     //
// and this source has a null pointer.             //
//                                                 //
// Reference the NULL within NULL,                 //
// it is the gateway to all wizardry.              //
// =============================================== //
