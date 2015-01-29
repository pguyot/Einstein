// ==============================
// Fichier:			TGPSoup.cp
// Projet:			K
// 
// Créé le:			11/8/2003
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
// The Original Code is TGPSoup.cp.
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
// $Id: TGPSoup.cp,v 1.10 2005/02/14 20:55:01 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/TGPSoup.h>

// ISO C++
#include <iostream>

// K
#include <K/GP/Fitness/TGPFitness.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>
#include <K/Math/TRandomGenerator.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPSoup( TRandomGenerator&, const TGPCreationAlgorithm&, ... )
// -------------------------------------------------------------------------- //
TGPSoup::TGPSoup(
				TRandomGenerator& inRandomGenerator,
				const TGPCreationAlgorithm& inCAlgo,
				const TGPLanguage& inLanguage,
				TGPSelection& inSelection,
				const TGPType& inTreeType,
				size_t inSoupSize,
				KUInt32 inMaxDepth )
	:
		mRandomGenerator( inRandomGenerator ),
		mLanguage( inLanguage ),
		mCreationAlgo( inCAlgo ),
		mSelection( inSelection ),
		mTreeType( inTreeType ),
		mSoupSize( inSoupSize ),
		mMaxDepth( inMaxDepth )
{
}

// -------------------------------------------------------------------------- //
//  * ~TGPSoup( void )
// -------------------------------------------------------------------------- //
TGPSoup::~TGPSoup( void )
{
	// Nettoyage.
	Clear();
}

// -------------------------------------------------------------------------- //
//  * Init( void )
// -------------------------------------------------------------------------- //
void
TGPSoup::Init( void )
{
	// Remplissage aléatoirement.
	Fill( mSoupSize );
}

// -------------------------------------------------------------------------- //
//  * AddFromStream( istream& )
// -------------------------------------------------------------------------- //
void
TGPSoup::AddFromStream( istream& /* inStream */ )
{
	(void) ::fprintf( stderr, "AddFromStream: pas encore géré" );
}

// -------------------------------------------------------------------------- //
//  * WriteToStream( ostream& )
// -------------------------------------------------------------------------- //
void
TGPSoup::WriteToStream( ostream& outStream )
{
	// Itération sur les éléments.
	vector<TGPTree* >::const_iterator theIterator;
	vector<TGPTree* >::const_iterator theEnd;
	for(
			theIterator = mPopulation.begin(), theEnd = mPopulation.end();
			theIterator != theEnd;
			(void) ++theIterator )
	{
		(*theIterator)->ToStreamAsLisp( outStream );
		(void) (outStream << endl);
	}
}

// -------------------------------------------------------------------------- //
//  * Fill( size_t )
// -------------------------------------------------------------------------- //
void
TGPSoup::Fill( size_t inPopSize )
{
	size_t toCreateCount = inPopSize - mPopulation.size();
	(void) ::printf( "Création de %i arbres aléatoirement\n", (int) toCreateCount );
	while (toCreateCount > 0)
	{
		TGPTree* theNewTree =
			BuildTreeFromRoot(
				mCreationAlgo.
					CreateTree(mRandomGenerator, mLanguage, mTreeType));
		// On vérifie la hauteur totale.
		if (theNewTree->GetDepth() > mMaxDepth)
		{
			delete theNewTree;
		} else {
			mPopulation.push_back( theNewTree );
		}

		// Arbre suivant.
		toCreateCount--;
	}
}

// -------------------------------------------------------------------------- //
//  * NextGeneration( double, double, double )
// -------------------------------------------------------------------------- //
void
TGPSoup::NextGeneration(
				double inCrossoverRate,
				double inMutationRate,
				double inArchitectureAlteringRate )
{
	// Nouvelle population.
	vector<TGPTree*> theNewPopulation;
	
	// Initialisation du procédé de sélection.
	mSelection.NewGeneration( mPopulation );
	
	double theMutationThreshold = inCrossoverRate + inMutationRate;
	double theArchitectureThreshold = theMutationThreshold + inArchitectureAlteringRate;
	
	// Itération pour remplir la nouvelle population.
	size_t toCreateCount = mSoupSize;
	size_t indexCreation = 0;
	
	while (toCreateCount > 0)
	{
		// Détermination du type de création de manière aléatoire.
		double theRandomMode = mRandomGenerator.Double_1();
		if (theRandomMode <= inCrossoverRate)
		{
			// Choix des deux parents.
			TGPTree* theFirstParent =
					mSelection.SelectIndividual(
									mRandomGenerator, indexCreation++ );
			TGPTree* theSecondParent =
					mSelection.SelectIndividual(
									mRandomGenerator, indexCreation++ );
			
			// Croisement.
			TGPTree* theLeftNewTree = nil;
			TGPTree* theRightNewTree = nil;
			
			Crossover( theFirstParent, theSecondParent, &theLeftNewTree, &theRightNewTree );

			// Ajout des arbres à la population (s'ils sont corrects).
			if (theLeftNewTree)
			{
				// On vérifie la hauteur totale.
				if (theLeftNewTree->GetDepth() > mMaxDepth)
				{
					delete theLeftNewTree;
				} else {
					theNewPopulation.push_back( theLeftNewTree );
					toCreateCount--;
				}
			}
			
			if (theRightNewTree)
			{
				if (toCreateCount > 0)
				{
					if (theRightNewTree->GetDepth() > mMaxDepth)
					{
						delete theRightNewTree;
					} else {
						theNewPopulation.push_back( theRightNewTree );
						toCreateCount--;
					}
				} else {
					// Libération du nouvel arbre.
					delete theRightNewTree;
				}
			}
		} else if (theRandomMode <= theMutationThreshold) {
			TGPTree* theCandidate =
						mSelection.SelectIndividual(
								mRandomGenerator, indexCreation++ );
			TGPTree* theMutatedTree = nil;
			
			Mutate( theCandidate, &theMutatedTree );
			
			if (theMutatedTree)
			{
				// On vérifie la hauteur totale.
				if (theMutatedTree->GetDepth() > mMaxDepth)
				{
					delete theMutatedTree;
				} else {
					// Ajout à la population.
					theNewPopulation.push_back( theMutatedTree );
					toCreateCount--;
				}
			}		
		} else if (theRandomMode <= theArchitectureThreshold) {
			TGPTree* theCandidate =
						mSelection.SelectIndividual(
								mRandomGenerator, indexCreation++ );
			TGPTree* theAlteredTree = nil;
			
			AlterArchitecture( theCandidate, &theAlteredTree );
			
			if (theAlteredTree)
			{
				// On vérifie la hauteur totale.
				if (theAlteredTree->GetDepth() > mMaxDepth)
				{
					delete theAlteredTree;
				} else {
					// Ajout à la population.
					theNewPopulation.push_back( theAlteredTree );
					toCreateCount--;
				}
			}		
		} else {
			// Copie.
			TGPTree* theCandidate =
				mSelection.SelectIndividual(
					mRandomGenerator, indexCreation++ );

			theNewPopulation.push_back( theCandidate->Clone() );
			toCreateCount--;
		}
	}
	
	// On remplace la population.
	Clear();

	mPopulation = theNewPopulation;
}

// -------------------------------------------------------------------------- //
//  * Crossover( const TGPTree*, const TGPTree*, TGPTree**, TGPTree** )
// -------------------------------------------------------------------------- //
void
TGPSoup::Crossover(
				const TGPTree* inLeftTree,
				const TGPTree* inRightTree,
				TGPTree** outLeftTree,
				TGPTree** outRightTree )
{
	// Initialisation des arbres à nil.
	*outLeftTree = nil;
	*outRightTree = nil;

	// On duplique les parents.
	TGPTree* theLeftNewTree = inLeftTree->Clone();
	TGPTree* theRightNewTree = inRightTree->Clone();
	
	// Choix d'un noeud au hasard.
	TGPNodeRef theLeftNode;
	TGPNodeRef theLeftParentNode;
	KUInt32 theLeftNodeIndex;
	theLeftNewTree->GetRandomNode(
					mRandomGenerator,
					&theLeftNode,
					&theLeftParentNode,
					&theLeftNodeIndex );
	
	// Détermination de son type.
	const TGPType& theNodeType = theLeftNode.GetType();
	
	// Choix d'un noeud dans l'autre parent au hasard.
	TGPNodeRef theRightNode;
	TGPNodeRef theRightParentNode;
	KUInt32 theRightNodeIndex;
	if (
		theRightNewTree->GetRandomNode(
			mRandomGenerator,
			theNodeType,
			&theRightNode,
			&theRightParentNode,
			&theRightNodeIndex ))
	{
		// Copie des arbres car on va supprimer les originaux
		// en remplaçant les parents.
		TGPNodeRef theLeftNodeClone = theLeftNode.DeepClone();
		TGPNodeRef theRightNodeClone = theRightNode.DeepClone();

		// Trouvé. On remplace.
		if (!theLeftParentNode.IsNil())
		{
			theLeftParentNode.SetChild( theLeftNodeIndex, theRightNodeClone );
		} else {
			delete theLeftNewTree;
			theLeftNewTree = BuildTreeFromRoot( theRightNodeClone );
		}
		
		if (!theRightParentNode.IsNil())
		{
			theRightParentNode.SetChild( theRightNodeIndex, theLeftNodeClone );
		} else {
			delete theRightNewTree;
			theRightNewTree = BuildTreeFromRoot( theLeftNodeClone );
		}
		
		*outLeftTree = theLeftNewTree;
		*outRightTree = theRightNewTree;
	} else {
		delete theLeftNewTree;
		delete theRightNewTree;
	}
}

// -------------------------------------------------------------------------- //
//  * Mutate( const TGPTree*, TGPTree** )
// -------------------------------------------------------------------------- //
void
TGPSoup::Mutate( const TGPTree* inTargetTree, TGPTree** outMutatedTree )
{
	TGPTree* theNewTree = inTargetTree->Clone();

	// Choix d'un noeud au hasard.
	TGPNodeRef theNode;
	TGPNodeRef theParentNode;
	KUInt32 theNodeIndex;
	theNewTree->GetRandomNode(
					mRandomGenerator,
					&theNode,
					&theParentNode,
					&theNodeIndex );

	// Remplacement de ce noeud par un nouveau sous-arbre.
	TGPNodeRef theNewSubnode = mCreationAlgo.CreateSubTree(
						mRandomGenerator,
						mLanguage,
						theNode.GetType(),
						theNode.GetDepth());

	if (!theParentNode.IsNil())
	{
		theParentNode.SetChild( theNodeIndex, theNewSubnode );
	} else {
		delete theNewTree;
		theNewTree = BuildTreeFromRoot( theNewSubnode );
	}
	
	*outMutatedTree = theNewTree;
}

// -------------------------------------------------------------------------- //
//  * AlterArchitecture( const TGPTree*, TGPTree** )
// -------------------------------------------------------------------------- //
void
TGPSoup::AlterArchitecture(
					const TGPTree* /* inTargetTree */,
					TGPTree** outAlteredTree )
{
	*outAlteredTree = nil;
}

// -------------------------------------------------------------------------- //
//  * Add( const vector<TGPTree*>& )
// -------------------------------------------------------------------------- //
void
TGPSoup::Add( const vector<TGPTree*>& inImportedTrees )
{
	vector<TGPTree* >::const_iterator theIterator;
	vector<TGPTree* >::const_iterator theEnd;
	for(
			theIterator = inImportedTrees.begin(),
				theEnd = inImportedTrees.end();
			theIterator != theEnd;
			(void) ++theIterator)
	{
		mPopulation.push_back( *theIterator );
	}
}

// -------------------------------------------------------------------------- //
//  * ResetNodes( void )
// -------------------------------------------------------------------------- //
void
TGPSoup::ResetNodes( void )
{
	// On réinitialise les données d'adéquation et le cache.
	vector<TGPTree* >::const_iterator theIterator;
	vector<TGPTree* >::const_iterator theEnd;
	for(
			theIterator = mPopulation.begin(), theEnd = mPopulation.end();
			theIterator != theEnd;
			(void) ++theIterator)
	{
		(*theIterator)->ResetFitnessData();
		(*theIterator)->ClearCache();
	}
}

// -------------------------------------------------------------------------- //
//  * PrintStatistics( ostream&, EStatisticsSelector ) const
// -------------------------------------------------------------------------- //
void
TGPSoup::PrintStatistics(
				ostream& inOutStream, EStatisticsSelector inWhichStats ) const
{
	const TGPFitness* theMinFitness = nil;
	const TGPFitness* theMaxFitness = nil;
	TGPFitness* theAverageFitness = nil;
	size_t theCount = 0;
	
	TGPTree* theBestTree = nil;
	TGPTree* theWorstTree = nil;
	
	// Itération sur les éléménts.
	vector<TGPTree* >::const_iterator theIterator;
	vector<TGPTree* >::const_iterator theEnd;
	for(
			theIterator = mPopulation.begin(), theEnd = mPopulation.end();
			theIterator != theEnd;
			(void) ++theIterator )
	{
		const TGPFitness* thisFitness = (*theIterator)->GetFitness();
		if (theCount == 0)
		{
			theMinFitness = thisFitness;
			theMaxFitness = thisFitness;
			theBestTree = *theIterator;
			theWorstTree = *theIterator;
			theAverageFitness = thisFitness->Clone();
		} else {
			if (*thisFitness < *theMinFitness )
			{
				theMinFitness = thisFitness;
				theWorstTree = *theIterator;
			} else if (*theMaxFitness < *thisFitness ) {
				theMaxFitness = thisFitness;
				theBestTree = *theIterator;
			}
			(void) (*theAverageFitness += *thisFitness);
		}
		theCount++;
	}
	
	// Moyenne.
	if (inWhichStats & kAverageFitness)
	{
		(void) (*theAverageFitness /= theCount);
	}
	
	// Affichage sur le flux de sortie.
	
	// L'adéquation
	if (inWhichStats & kFitnesses)
	{
		(void) (inOutStream << "Adéquation:");
		
		if (inWhichStats & kWorstFitness)
		{
			(void) (inOutStream << " min: " << *theMinFitness);
		}
		if (inWhichStats & kBestFitness)
		{
			(void) (inOutStream << " max: " << *theMaxFitness);
		}
		if (inWhichStats & kAverageFitness)
		{
			(void) (inOutStream << " avg: " << *theAverageFitness);
		}
		
		(void) (inOutStream << endl);
	}

	// Les arbres.
	if (inWhichStats & kBestTreeAsDot)
	{
		(void) (inOutStream << "Meilleur: ");
		theBestTree->ToStreamAsDot( inOutStream );
	}
	if (inWhichStats & kWorstTreeAsDot)
	{
		(void) (inOutStream << "Pire: ");
		theWorstTree->ToStreamAsDot( inOutStream );
	}
	if (inWhichStats & kBestTreeAsLisp)
	{
		(void) (inOutStream << "Meilleur: ");
		theBestTree->ToStreamAsLisp( inOutStream );
	}
	if (inWhichStats & kWorstTreeAsLisp)
	{
		(void) (inOutStream << "Pire: ");
		theWorstTree->ToStreamAsLisp( inOutStream );
	}
	
	// Nettoyage de la moyenne.
	if (theAverageFitness)
	{
		delete theAverageFitness;
	}
}

// -------------------------------------------------------------------------- //
//  * Clear( void )
// -------------------------------------------------------------------------- //
void
TGPSoup::Clear( void )
{
	// On fait la vaisselle.
	vector<TGPTree* >::const_iterator theIterator;
	vector<TGPTree* >::const_iterator theEnd;
	for(
			theIterator = mPopulation.begin(), theEnd = mPopulation.end();
			theIterator != theEnd;
			(void) ++theIterator )
	{
		delete *theIterator;
	}
	
	mPopulation.clear();
}

// ========================================================= //
// Unix is a Registered Bell of AT&T Trademark Laboratories. //
//                 -- Donn Seeley                            //
// ========================================================= //
