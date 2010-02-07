// ==============================
// Fichier:			TGPTree.cp
// Projet:			K
//
// Créé le:			26/2/2003
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
// The Original Code is TGPTree.cp.
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
// $Id: TGPTree.cp,v 1.7 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/TGPTree.h>

// K
#include <K/GP/Tree/Nodes/TGPNodeRef.h>
#include <K/GP/Fitness/TGPFitness.h>
#include <K/Math/TRandomGenerator.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPTree( const TGPNodeRef* )
// -------------------------------------------------------------------------- //
TGPTree::TGPTree( const TGPNodeRef& inRoot )
	:
		mRoot( inRoot ),
		mFitness( nil )
{
}

// -------------------------------------------------------------------------- //
//  * TGPTree( const TGPTree& )
// -------------------------------------------------------------------------- //
TGPTree::TGPTree( const TGPTree& inCopy )
	:
		mRoot( inCopy.mRoot.DeepClone() ),
		mFitness( nil )
{
}

// -------------------------------------------------------------------------- //
//  * ~TGPTree( void )
// -------------------------------------------------------------------------- //
TGPTree::~TGPTree( void )
{
	if (mFitness)
	{
		delete mFitness;
	}
}

// -------------------------------------------------------------------------- //
//  * ToStreamAsLisp( ostream& outStream )
// -------------------------------------------------------------------------- //
void
TGPTree::ToStreamAsLisp( ostream& outStream )
{
	mRoot.ToStreamAsLisp( outStream, 0 );
	(void) (outStream << endl);
}

// -------------------------------------------------------------------------- //
//  * ToStreamAsDot( ostream& outStream )
// -------------------------------------------------------------------------- //
void
TGPTree::ToStreamAsDot( ostream& outStream )
{
	char theTreeName[10];
	(void) ::sprintf( theTreeName, "t%.8X", (unsigned int) this );
	(void) (outStream << "digraph " << theTreeName << " {" << endl);
	mRoot.ToStreamAsDot( outStream );
	(void) (outStream << "}" << endl);
}

// -------------------------------------------------------------------------- //
//  * GetRandomNode( TRandomGenerator&, TGPNodeRef*, TGPNodeRef*, KUInt32* )
// -------------------------------------------------------------------------- //
void
TGPTree::GetRandomNode(
				TRandomGenerator& inRandomGenerator,
				TGPNodeRef* outNode,
				TGPNodeRef* outParentNode,
				KUInt32* outSiblingIndex )
{
	// On compte les noeuds.
	KUInt32 nbNodes = mRoot.CountNodes();
	
	// Choix d'un noeud aléatoirement.
	KUInt32 indexRandomNodeIdx =
					inRandomGenerator.DistributionFast( nbNodes );

	// Récupération du noeud.
	TGPNodeRef::ESearchResult theSearchResult
		= mRoot.GetSubNode(
					indexRandomNodeIdx,
					outNode,
					outParentNode,
					outSiblingIndex );
		
	if (theSearchResult == TGPNodeRef::kThisNode)
	{
		if (outParentNode)
		{
			*outParentNode = TGPNodeRef();
		}
	}
}

// -------------------------------------------------------------------------- //
//  * GetRandomNode( TRandomGenerator&, const TGPType&, TGPNodeRef*, ... )
// -------------------------------------------------------------------------- //
Boolean
TGPTree::GetRandomNode(
				TRandomGenerator& inRandomGenerator,
				const TGPType& inType,
				TGPNodeRef* outNode,
				TGPNodeRef* outParentNode,
				KUInt32* outSiblingIndex )
{
	Boolean theResult = false;
	
	// On compte les noeuds de ce type.
	KUInt32 nbNodes = mRoot.CountNodes( inType );
	
	// Si c'est zéro, fini.
	if (nbNodes > 0)
	{
		theResult = true;	

		// Choix d'un noeud aléatoirement.
		KUInt32 indexRandomNodeIdx =
					inRandomGenerator.DistributionFast( nbNodes );

		// Récupération du noeud.
		TGPNodeRef::ESearchResult theSearchResult
						= mRoot.GetSubNode(
									inType,
									indexRandomNodeIdx,
									outNode,
									outParentNode,
									outSiblingIndex );
		
		if (theSearchResult == TGPNodeRef::kThisNode)
		{
			if (outParentNode)
			{
				*outParentNode = TGPNodeRef();
			}
		}
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GetDepth( void ) const
// -------------------------------------------------------------------------- //
KUInt32
TGPTree::GetDepth( void ) const
{
	return mRoot.GetDepth();
}

// -------------------------------------------------------------------------- //
//  * GetFitness( void ) const
// -------------------------------------------------------------------------- //
const TGPFitness*
TGPTree::GetFitness( void ) const
{
	if (!mFitness)
	{
		mFitness = ComputeFitness();
	}
	
	return mFitness;
}

// -------------------------------------------------------------------------- //
//  * ResetFitnessData( void ) const
// -------------------------------------------------------------------------- //
void
TGPTree::ResetFitnessData( void ) const
{
	if (mFitness)
	{
		delete mFitness;
		mFitness = nil;
	}
}

// -------------------------------------------------------------------------- //
//  * ClearCache( void ) const
// -------------------------------------------------------------------------- //
void
TGPTree::ClearCache( void ) const
{
	return mRoot.Invalidate();
}

// -------------------------------------------------------------------------- //
//  * operator()(const TGPTree*, const TGPTree*)
// -------------------------------------------------------------------------- //
bool
less<TGPTree*>::operator()(const TGPTree* x, const TGPTree* y)
{
	return (x->GetFitness() < y->GetFitness());
}

// ============================================================================== //
//         In the east there is a shark which is larger than all other fish.  It  //
// changes into a bird whose winds are like clouds filling the sky.  When this    //
// bird moves across the land, it brings a message from Corporate Headquarters.   //
// This message it drops into the midst of the program mers, like a seagull       //
// making its mark upon the beach.  Then the bird mounts on the wind and, with    //
// the blue sky at its back, returns home.                                        //
//         The novice programmer stares in wonder at the bird, for he understands //
// it not.  The average programmer dreads the coming of the bird, for he fears    //
// its message.  The master programmer continues to work at his terminal, for he  //
// does not know that the bird has come and gone.                                 //
//                 -- Geoffrey James, "The Tao of Programming"                    //
// ============================================================================== //
