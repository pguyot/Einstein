// ==============================
// Fichier:			TGPLanguage.cp
// Projet:			K
//
// Créé le:			11/2/2003
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
// The Original Code is TGPLanguage.cp.
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
// $Id: TGPLanguage.cp,v 1.9 2005/02/14 20:55:03 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Language/TGPLanguage.h>

// ISO C++
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <stdexcept>
#include <sstream>

// K
#include <K/GP/Tree/Nodes/TGPNode.h>
#include <K/GP/Tree/Nodes/TGPNodeConst.h>
#include <K/Math/TRandomGenerator.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPLanguage( void )
// -------------------------------------------------------------------------- //
TGPLanguage::TGPLanguage( void )
{
}

// -------------------------------------------------------------------------- //
//  * ~TGPLanguage( void )
// -------------------------------------------------------------------------- //
TGPLanguage::~TGPLanguage( void )
{
	// Suppression de tous les noeuds
	vector<TGPNode*>::const_iterator theIterator;
	vector<TGPNode*>::const_iterator theEnd;

	for (
		theIterator = mNodes.begin(), theEnd = mNodes.end();
		theIterator != theEnd;
		(void) ++theIterator)
	{
		TGPNode* theNode = *theIterator;
		delete theNode;
	}

	// Suppression de toutes les feuilles.
	for (
		theIterator = mLeaves.begin(), theEnd = mLeaves.end();
		theIterator != theEnd;
		(void) ++theIterator)
	{
		TGPNode* theLeaf = *theIterator;
		delete theLeaf;
	}
}

// -------------------------------------------------------------------------- //
//  * GetRandomNode( TRandomGenerator&, const TGPType&, ENodeSet ) const
// -------------------------------------------------------------------------- //
TGPNode*
TGPLanguage::GetRandomNode(
				TRandomGenerator& inRandomGenerator,
				const TGPType& inType,
				ENodeSet inNodeSet /* = kAll */ ) const
{
	size_t theLeafCount = 0;
	size_t theCount = 0;
	
	// Pas seulement les non terminaux.
	if (inNodeSet != kNodes)
	{
		theLeafCount = CountNodesOfType( mLeaves, inType );
		theCount = theLeafCount;
	}

	// Pas seulement les terminaux.
	if (inNodeSet != kLeaves)
	{
		theCount += CountNodesOfType( mNodes, inType );
	}

	if (theCount == 0)
	{
		// On utilise sstream.
		ostringstream someStream;		
		
		switch( inNodeSet )
		{
			case kNodes:
				(void) (someStream << "Pas de noeud de type " << inType.GetName() );
				break;

			case kLeaves:
				(void) (someStream << "Pas de feuille de type " << inType.GetName() );
				break;

			case kAll:
			default:
				(void) (someStream << "Pas de noeud de type " << inType.GetName() );
		}

		throw runtime_error( someStream.str() );
	}

	// Choix du noeud.
	size_t chosenNode =
		(size_t) inRandomGenerator.DistributionFast( theCount );

	TGPNode* theNode;

	// Détermination du type (feuille ou noeud)
	if (chosenNode >= theLeafCount)
	{
		chosenNode -= theLeafCount;
		theNode = GetIthNodeOfType( mNodes, inType, chosenNode );
	} else {
		theNode = GetIthNodeOfType( mLeaves, inType, chosenNode );
	}
	
	return theNode;
}

// -------------------------------------------------------------------------- //
//  * CountNodes( const TGPType&, ENodeSet ) const
// -------------------------------------------------------------------------- //
size_t
TGPLanguage::CountNodes(
				const TGPType& inType,
				ENodeSet inNodeSet /* = kAll */ ) const
{
	size_t theCount = 0;
	
	// Pas seulement les non terminaux.
	if (inNodeSet != kNodes)
	{
		theCount = CountNodesOfType( mLeaves, inType );
	}

	// Pas seulement les terminaux.
	if (inNodeSet != kLeaves)
	{
		theCount += CountNodesOfType( mNodes, inType );
	}
	
	return theCount;
}

// -------------------------------------------------------------------------- //
//  * AddNode( TGPNode* )
// -------------------------------------------------------------------------- //
Boolean
TGPLanguage::AddNode( TGPNode* inNode )
{
	TStringRef theLispName = inNode->GetLispName();

	vector<TGPNode*>* theVector;
	// Est-ce une feuille?
	if (inNode->GetArity() == 0)
	{
		theVector = &mLeaves;
	} else {
		theVector = &mNodes;
	}
	
	Boolean addIt = true;
	
	do {
		// Vérifions qu'on n'a pas déjà ce noeud.
		vector<TGPNode*>::const_iterator theIterator;
		vector<TGPNode*>::const_iterator theEnd;

		// Itération.
		for (
			theIterator = theVector->begin(),
				theEnd = theVector->end();
			theIterator != theEnd;
			(void) ++theIterator)
		{
			if ((*theIterator)->GetLispName() == theLispName)
			{
				// On l'a déjà.
				addIt = false;
				break;
			}
		}
	} while ( false );
	
	if (addIt)
	{
		// Ajout du noeud.
		theVector->push_back( inNode );
	}
	
	return addIt;
}

// -------------------------------------------------------------------------- //
//  * AddConstantValue( TGPDataRef& )
// -------------------------------------------------------------------------- //
Boolean
TGPLanguage::AddConstantValue( TGPDataRef& inValue )
{
	// Nouveau noeud.
	TGPNode* theNode = new TGPNodeConst( inValue );
	
	Boolean theResult = AddNode( theNode );
	if (theResult)
	{
		mConstants[ &inValue.GetClass() ].push_back( inValue );
	} else {
		delete theNode;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * AddConstantValue( const TGPType&, TGPDataRef& )
// -------------------------------------------------------------------------- //
Boolean
TGPLanguage::AddConstantValue( const TGPType& inType, TGPDataRef& inValue )
{
	// Nouveau noeud.
	TGPNode* theNode = new TGPNodeConst( inType, inValue );
	
	Boolean theResult = AddNode( theNode );
	if (theResult)
	{
		mConstants[ &inType ].push_back( inValue );
	} else {
		delete theNode;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * CountNodesOfType( count vector<TGPNode*>, const TGPType& )
// -------------------------------------------------------------------------- //
size_t
TGPLanguage::CountNodesOfType(
					const vector<TGPNode*> inVector,
					const TGPType& inType )
{
	size_t theResult = 0;
	
	// Itération sur les noeuds.
	vector<TGPNode*>::const_iterator theIterator;
	vector<TGPNode*>::const_iterator theEnd;

	for (
		theIterator = inVector.begin(), theEnd = inVector.end();
		theIterator != theEnd;
		(void) ++theIterator)
	{
		TGPNode* theNode = *theIterator;
		if (theNode->GetType().IsTypeOf( inType ))
		{
			theResult++;
		}
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GetIthNodeOfType( const vector<TGPNode*>, const TGPType&, size_t )
// -------------------------------------------------------------------------- //
TGPNode*
TGPLanguage::GetIthNodeOfType(
					const vector<TGPNode*> inVector,
					const TGPType& inType,
					size_t inNodeIndex )
{
	size_t theIndex = 0;
	
	// Itération sur les noeuds.
	vector<TGPNode*>::const_iterator theIterator;
	vector<TGPNode*>::const_iterator theEnd;

	for (
		theIterator = inVector.begin(), theEnd = inVector.end();
		theIterator != theEnd;
		(void) ++theIterator)
	{
		TGPNode* theNode = *theIterator;
		if (theNode->GetType().IsTypeOf( inType ))
		{
			if (theIndex == inNodeIndex)
			{
				return theNode;
			}
			
			theIndex++;
		}
	}
	
	return nil;
}

// -------------------------------------------------------------------------- //
//  * GetNodeFromName( const vector<TGPNode*>, const string& )
// -------------------------------------------------------------------------- //
TGPNode*
TGPLanguage::GetNodeFromName(
					const vector<TGPNode*> inVector,
					const string& inName )
{
	return GetNodeFromName( inVector, TStringRef( inName ) );
}

// -------------------------------------------------------------------------- //
//  * GetNodeFromName( const vector<TGPNode*>, TStringRef )
// -------------------------------------------------------------------------- //
TGPNode*
TGPLanguage::GetNodeFromName(
					const vector<TGPNode*> inVector,
					TStringRef inName )
{
	// Itération sur les noeuds.
	vector<TGPNode*>::const_iterator theIterator;
	vector<TGPNode*>::const_iterator theEnd;

	for (
		theIterator = inVector.begin(), theEnd = inVector.end();
		theIterator != theEnd;
		(void) ++theIterator)
	{
		TGPNode* theNode = *theIterator;
		if (theNode->GetLispName() == inName)
		{
			return theNode;
		}
	}
	
	return nil;
}

// -------------------------------------------------------------------------- //
//  * BuildNodeFromStreamAsLisp( istream& ) const
// -------------------------------------------------------------------------- //
TGPNodeRef
TGPLanguage::BuildNodeFromStreamAsLisp( istream& inStream ) const
{
	TGPNodeRef theResult;

	int theNextChar = SkipBlanks( inStream );
	if (theNextChar == '(')
	{
		// C'est une parenthèse, donc un noeud (et non pas une feuille)

		// On bouffe la parenthèse.
		(void) inStream.ignore();
	
		// Lecture du nom du noeud.
		string theSymbol = NextToken( inStream );
	
		// Vérification que le symbole a pu être lu.
		if (theSymbol.size() == 0)
		{
			throw runtime_error( "Symbole attendu mais non trouvé" );
		}
	
		// Recherche du noeud dans l'ensemble des noeuds du langage.
		TGPNode* theNode = GetNodeFromName( mNodes, theSymbol );
		
		if (theNode == nil)
		{
			throw runtime_error( "Noeud inconnu" );
		}
		
		theResult = theNode;
		
		// Lecture des sous-noeuds.
		theNextChar = SkipBlanks( inStream );
		KUInt32 indexChildren = 0;
		KUInt32 theArity = theNode->GetArity();
		
		while (theNextChar != ')')
		{
			theResult.SetChild( indexChildren++, BuildNodeFromStreamAsLisp( inStream ) );
			
			theNextChar = SkipBlanks( inStream );
			
			if (indexChildren > theArity)
			{
				throw runtime_error( "L'arbre dans le flux a trop de sous-noeuds" );
			}
		}

		if (indexChildren != theArity)
		{
			throw runtime_error( "L'arbre dans le flux n'a pas assez de sous-noeuds" );
		}
		
		// On avale la parenthèse.
		(void) inStream.ignore();
	} else {
		// Pas de parenthèse, c'est donc une feuille.

		// Lecture du nom de la feuille.
		string theSymbol = NextToken( inStream );
	
		// Vérification que le symbole a pu être lu.
		if (theSymbol.size() == 0)
		{
			throw runtime_error( "Symbole attendu mais non trouvé" );
		}
		
		// Recherche de la feuille dans l'ensemble des feuilles du langage.
		TGPNode* theNode = GetNodeFromName( mLeaves, theSymbol );
		
		if (theNode == nil)
		{
			throw runtime_error( "Feuille inconnue" );
		}
		
		theResult = theNode;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * SkipBlanks( istream& )
// -------------------------------------------------------------------------- //
char
TGPLanguage::SkipBlanks( istream& inStream )
{
	int theNextChar;
	do {
		theNextChar = inStream.peek();
		if (theNextChar == EOF)
		{
			throw runtime_error( "Fin de flux inattendue" );
		}
		if (
			(theNextChar != ' ')
			&& (theNextChar != '\t')
			&& (theNextChar != '\r')
			&& (theNextChar != '\n'))
		{
			// Un non blanc. On sort.
			break;
		}
		
		(void) inStream.ignore();
	} while (true);
	
	return (char) theNextChar;
}

// -------------------------------------------------------------------------- //
//  * NextToken( istream& )
// -------------------------------------------------------------------------- //
string
TGPLanguage::NextToken( istream& inStream )
{
	int theNextChar;
	do {
		theNextChar = inStream.peek();
		if (theNextChar == EOF)
		{
			throw runtime_error( "Fin de flux inattendue" );
		}
		if (
			(theNextChar != ' ')
			&& (theNextChar != '\t')
			&& (theNextChar != '\r')
			&& (theNextChar != '\n'))
		{
			// Un non blanc. On sort.
			break;
		}
		
		(void) inStream.ignore();
	} while (true);
	
	ostringstream resultStringStream;
	while (
		(theNextChar != EOF)
		&& (theNextChar != '(')
		&& (theNextChar != ')')
		&& (theNextChar != ' ')
		&& (theNextChar != '\t')
		&& (theNextChar != '\r')
		&& (theNextChar != '\n'))
	{
		(void) resultStringStream.put( theNextChar );
		(void) inStream.ignore();
		theNextChar = inStream.peek();
	}
	
	return resultStringStream.str();
}

// ============================================================================= //
// Shopping at this grody little computer store at the Galleria for a            //
// totally awwwesome Apple.  Fer suuure.  I mean Apples are nice you know?       //
// But, you know, there is this cute guy who works there and HE says that        //
// VAX's are cooler!  I mean I don't really know, you know? He says that he      //
// has this totally tubular VAX at home and it's stuffed with memory-to-the-max! //
// Right, yeah.  And he wants to take me home to show it to me.  Oh My God!      //
// I'm suuure.  Gag me with a Prime!                                             //
// ============================================================================= //
