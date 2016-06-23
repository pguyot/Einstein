// ==============================
// Fichier:			TGPCAKoza.cp
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
// The Original Code is TGPCAKoza.cp.
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
// $Id: TGPCAKoza.cp,v 1.6 2004/11/23 15:11:29 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Creation/TGPCAKoza.h>

// K
#include <K/GP/Tree/Nodes/TGPNode.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>
#include <K/GP/Tree/TGPTree.h>
#include <K/Math/TRandomGenerator.h>

// -------------------------------------------------------------------------- //
//  * TGPCAKoza( int )
// -------------------------------------------------------------------------- //
TGPCAKoza::TGPCAKoza( int inDepth )
	:
		TGPCreationAlgorithm(),
		mDepth( inDepth )
{
}

// -------------------------------------------------------------------------- //
//  * CreateSubTree( TRandomGenerator&, const TGPLanguage&, const TGPType&, ...
// -------------------------------------------------------------------------- //
TGPNodeRef
TGPCAKoza::CreateSubTree(
				TRandomGenerator& inRandomGenerator,
				const TGPLanguage& inLanguage,
				const TGPType& inType,
				size_t inMaxDepth ) const
{
	// Appel de la primitive.
	// On a une chance sur deux de choisir Grow ou Full.
	Boolean doFull = inRandomGenerator.Coin();
	
	TGPNodeRef theResult;

	do {
		theResult = Primitive(
				inRandomGenerator,
				inLanguage,
				inType,
				(int) inMaxDepth,
				doFull );
	} while (theResult.IsNil());

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * CreateRoot( TRandomGenerator&, const TGPLanguage&, const TGPType& ) const
// -------------------------------------------------------------------------- //
TGPNodeRef
TGPCAKoza::CreateRoot(
				TRandomGenerator& inRandomGenerator,
				const TGPLanguage& inLanguage,
				const TGPType& inType ) const
{
	// Appel de la primitive.
	// On a une chance sur deux de choisir Grow ou Full.
	Boolean doFull = inRandomGenerator.Coin();
	
	TGPNodeRef theResult;
	
	do {
		theResult = Primitive(
				inRandomGenerator,
				inLanguage,
				inType,
				mDepth - 1,
				doFull );
	} while (theResult.IsNil());
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * Primitive( TRandomGenerator&, const TGPLanguage&, const TGPType&, int, ...
// -------------------------------------------------------------------------- //
TGPNodeRef
TGPCAKoza::Primitive(
				TRandomGenerator& inRandomGenerator,
				const TGPLanguage& inLanguage,
				const TGPType& inType,
				int inDepth,
				Boolean inDoFull )
{
	TGPNodeRef theResult;
	
	if (inDepth == 0)
	{
		// Symbole terminal
		// (sauf s'il n'y en a pas assez)
		if (inLanguage.CountNodes( inType, TGPLanguage::kLeaves ) > 0)
		{
			theResult = TGPNodeRef(
					inLanguage.GetRandomNode(
						inRandomGenerator, inType, TGPLanguage::kLeaves ) );
		}
	} else {
		// Choix alŽatoire d'un terminal ou d'un fonctionnel (Grow)
		// Choix alŽatoire d'un fonctionnel (Full)
		TGPLanguage::ENodeSet theSet;
		if (inDoFull
			&& (inLanguage.CountNodes( inType, TGPLanguage::kNodes ) > 0))
		{
			// On prend dans tous les noeuds s'il n'y a pas assez de fonctionnels.
			theSet = TGPLanguage::kNodes;
		} else {
			theSet = TGPLanguage::kAll;
		}
		TGPNode* theNode =
				inLanguage.GetRandomNode( inRandomGenerator, inType, theSet );
		theResult = TGPNodeRef( theNode );
		
		// Ensuite, on remplit avec Grow ou Full.
		KUInt32 nbSubnodes = theNode->GetArity();
		KUInt32 indexSubnodes;
		for (indexSubnodes = 0; indexSubnodes < nbSubnodes; indexSubnodes++)
		{
			TGPNodeRef theNewChild = Primitive(
					inRandomGenerator,
					inLanguage,
					theNode->GetParamType(indexSubnodes),
					inDepth - 1,
					inDoFull );
			if (theNewChild.IsNil())
			{
				theResult = TGPNodeRef();
				break;
			}
			theResult.SetChild( indexSubnodes, theNewChild );
		}
	}
	
	return theResult;
}

// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
