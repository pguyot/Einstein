// ==============================
// Fichier:			TGPNodeRef.cp
// Projet:			K
// 
// Créé le:			17/11/2003
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
// The Original Code is TGPNodeRef.cp.
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
// $Id: TGPNodeRef.cp,v 1.5 2004/11/23 15:11:30 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

// ISO C++
#include <stdexcept>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TGPNodeRef( void )
// -------------------------------------------------------------------------- //
TGPNodeRef::TGPNodeRef( void )
	:
		mNodeRef( nil )
{
}

// -------------------------------------------------------------------------- //
//  * TGPNodeRef( TGPNode* )
// -------------------------------------------------------------------------- //
TGPNodeRef::TGPNodeRef( TGPNode* inNode )
	:
		mNodeRef( new TNodeRef( inNode ) )
{
	mNodeRef->IncrementRef();
}

// -------------------------------------------------------------------------- //
//  * TGPNodeRef( const TGPNodeRef& )
// -------------------------------------------------------------------------- //
TGPNodeRef::TGPNodeRef( const TGPNodeRef& inCopy )
	:
		mNodeRef( inCopy.mNodeRef )
{
	mNodeRef->IncrementRef();
}

// -------------------------------------------------------------------------- //
//  * ~TGPNodeRef( void )
// -------------------------------------------------------------------------- //
TGPNodeRef::~TGPNodeRef( void )
{
	// Suppression de la référence.
	if (mNodeRef && mNodeRef->DecrementRef())
	{
		delete mNodeRef;
	}
}

// -------------------------------------------------------------------------- //
//  * operator = ( const TGPNodeRef& )
// -------------------------------------------------------------------------- //
TGPNodeRef&
TGPNodeRef::operator = ( const TGPNodeRef& inCopy )
{
	TNodeRef* theCopyRef = inCopy.mNodeRef;
	if (theCopyRef)
	{
		theCopyRef->IncrementRef();
	}
	
	if (mNodeRef && mNodeRef->DecrementRef())
	{
		delete mNodeRef;
	}
	
	mNodeRef = theCopyRef;
	
	return *this;
}

// -------------------------------------------------------------------------- //
//  * DeepClone( void ) const
// -------------------------------------------------------------------------- //
TGPNodeRef
TGPNodeRef::DeepClone( void ) const
{
	if (mNodeRef == nil)
	{
		return *this;
	} else {
		// Nouvelle référence sur le noeud.
		TGPNode* theNode = GetNode();
		TGPNodeRef theResult( theNode );
		
		// On remplit les sous-noeuds.
		TGPNodeRef* theSubnodes = theResult.mNodeRef->GetSubnodes();
		TGPNodeRef* mySubnodes = mNodeRef->GetSubnodes();
		KUInt32 theArity = theNode->GetArity();
		KUInt32 indexNodes;
		for (indexNodes = 0; indexNodes < theArity; indexNodes++)
		{
			theSubnodes[indexNodes] = mySubnodes[indexNodes].DeepClone();
		}
		
		return theResult;
	}
}

// -------------------------------------------------------------------------- //
//  * GetChild( KUInt32 ) const
// -------------------------------------------------------------------------- //
TGPNodeRef&
TGPNodeRef::GetChild( KUInt32 inNodeIndex ) const
{
	KASSERT( mNodeRef != nil );

	if (inNodeIndex >= GetNode()->GetArity())
	{
		throw range_error("Indice de noeud trop grand");
	}

	TGPNodeRef* theSubnodes = mNodeRef->GetSubnodes();
	KASSERT( theSubnodes != nil );

	return theSubnodes[inNodeIndex];
}

// -------------------------------------------------------------------------- //
//  * GetChildAlways( KUInt32 ) const
// -------------------------------------------------------------------------- //
TGPNodeRef&
TGPNodeRef::GetChildAlways( KUInt32 inNodeIndex ) const
{
	KASSERT( mNodeRef != nil );

	TGPNodeRef& theChild = GetChild( inNodeIndex );
	
	if (theChild.mNodeRef == nil)
	{
		throw runtime_error( "Arbre non complet (noeud à nil)" );
	}
	
	return theChild;
}

// -------------------------------------------------------------------------- //
//  * SetChild( KUInt32, const TGPNodeRef& )
// -------------------------------------------------------------------------- //
void
TGPNodeRef::SetChild( KUInt32 inNodeIndex, const TGPNodeRef& inNewChild )
{
	KASSERT( mNodeRef != nil );

	if (inNodeIndex >= GetNode()->GetArity())
	{
		throw range_error("Indice de noeud trop grand");
	}
	
	TGPNodeRef* theSubnodes = mNodeRef->GetSubnodes();
	KASSERT( theSubnodes != nil );

	theSubnodes[inNodeIndex] = inNewChild;
}

// -------------------------------------------------------------------------- //
//  * operator()(const TGPNodeRef*, const TGPNodeRef*)
// -------------------------------------------------------------------------- //
bool
less<TGPNodeRef*>::operator()(const TGPNodeRef* x, const TGPNodeRef* y)
{
	return less<TGPNode*>()(x->GetNode(), y->GetNode());
}

// -------------------------------------------------------------------------- //
//  * GetDepth( void ) const
// -------------------------------------------------------------------------- //
KUInt32
TGPNodeRef::GetDepth( void ) const
{
	KASSERT( mNodeRef != nil );

	KUInt32 theResult = 0;

	// On demande à chacun des sous-noeuds.
	KUInt32 indexNodes;
	KUInt32 theArity = GetNode()->GetArity();

	TGPNodeRef* theSubnodes = mNodeRef->GetSubnodes();
	KASSERT( (theArity == 0) || (theSubnodes != nil) );

	for (indexNodes = 0; indexNodes < theArity; indexNodes++)
	{
		KUInt32 theSubDepth = theSubnodes[indexNodes].GetDepth();
		
		if (theSubDepth > theResult)
		{
			theResult = theSubDepth;
		}
	}
	
	return theResult + 1;
}

// -------------------------------------------------------------------------- //
//  * CountNodes( void ) const
// -------------------------------------------------------------------------- //
KUInt32
TGPNodeRef::CountNodes( void ) const
{
	KASSERT( mNodeRef != nil );

	KUInt32 theResult = 1;

	// On compte les sous-noeuds.
	KUInt32 indexNodes;
	KUInt32 theArity = GetNode()->GetArity();;

	TGPNodeRef* theSubnodes = mNodeRef->GetSubnodes();
	KASSERT( (theArity == 0) || (theSubnodes != nil) );

	for (indexNodes = 0; indexNodes < theArity; indexNodes++)
	{
		theResult += theSubnodes[indexNodes].CountNodes();
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * CountNodes( const TGPType& ) const
// -------------------------------------------------------------------------- //
KUInt32
TGPNodeRef::CountNodes( const TGPType& inType ) const
{
	KASSERT( mNodeRef != nil );

	KUInt32 theResult;
	
	if (GetNode()->GetType().IsTypeOf(inType))
	{
		theResult = 1;
	} else {
		theResult = 0;
	}

	// On compte les sous-noeuds.
	KUInt32 indexNodes;
	KUInt32 theArity = GetNode()->GetArity();;

	TGPNodeRef* theSubnodes = mNodeRef->GetSubnodes();
	KASSERT( (theArity == 0) || (theSubnodes != nil) );

	for (indexNodes = 0; indexNodes < theArity; indexNodes++)
	{
		theResult += theSubnodes[indexNodes].CountNodes( inType );
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GetSubNode( KUInt32&, TGPNodeRef*, TGPNodeRef*, KUInt32*  )
// -------------------------------------------------------------------------- //
TGPNodeRef::ESearchResult
TGPNodeRef::GetSubNode(
					KUInt32& ioIndex,
					TGPNodeRef* outNode,
					TGPNodeRef* outParentNode,
					KUInt32* outSiblingIndex )
{
	KASSERT( mNodeRef != nil );

	ESearchResult theResult = kNotFound;
	if (ioIndex == 0)
	{
		theResult = kThisNode;
		if (outNode)
		{
			*outNode = *this;
		}
	} else {
		// On itère sur les sous-noeuds.
		ioIndex--;
		
		KUInt32 indexNodes;
		KUInt32 theArity = GetNode()->GetArity();;

		TGPNodeRef* theSubnodes = mNodeRef->GetSubnodes();
		KASSERT( (theArity == 0) || (theSubnodes != nil) );

		for (indexNodes = 0; indexNodes < theArity; indexNodes++)
		{
			theResult = theSubnodes[indexNodes].GetSubNode(
							ioIndex, outNode, outParentNode, outSiblingIndex );
			if (theResult != kNotFound)	// Trouvé?
			{
				if (theResult == kThisNode)	// Enfant direct, on remplit
											// les paramètres.
				{
					theResult = kASubNode;	// Ce n'est pas nous.
					if (outParentNode)
					{
						*outParentNode = *this;
					}
					if (outSiblingIndex)
					{
						*outSiblingIndex = indexNodes;
					}
				}
				
				break; // Fini.
			}
		}
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GetSubNode( const TGPType&, KUInt32&, TGPNodeRef*, TGPNodeRef*, ...  )
// -------------------------------------------------------------------------- //
TGPNodeRef::ESearchResult
TGPNodeRef::GetSubNode(
					const TGPType& inType,
					KUInt32& ioIndex,
					TGPNodeRef* outNode,
					TGPNodeRef* outParentNode,
					KUInt32* outSiblingIndex )
{
	KASSERT( mNodeRef != nil );

	ESearchResult theResult = kNotFound;
	Boolean thisType = GetNode()->GetType().IsTypeOf(inType);
	if (thisType && (ioIndex == 0))
	{
		theResult = kThisNode;
		if (outNode)
		{
			*outNode = *this;
		}
	} else {
		// On itère sur les sous-noeuds.
		if (thisType)
		{
			ioIndex--;
		}
		
		KUInt32 indexNodes;
		KUInt32 theArity = GetNode()->GetArity();;

		TGPNodeRef* theSubnodes = mNodeRef->GetSubnodes();
		KASSERT( (theArity == 0) || (theSubnodes != nil) );

		for (indexNodes = 0; indexNodes < theArity; indexNodes++)
		{
			theResult = theSubnodes[indexNodes].GetSubNode(
					inType, ioIndex, outNode, outParentNode, outSiblingIndex );
			if (theResult != kNotFound)	// Trouvé?
			{
				if (theResult == kThisNode)	// Enfant direct, on remplit
											// les paramètres.
				{
					theResult = kASubNode;	// Ce n'est pas nous.
					if (outParentNode)
					{
						*outParentNode = *this;
					}
					if (outSiblingIndex)
					{
						*outSiblingIndex = indexNodes;
					}
				}
				
				break; // Fini.
			}
		}
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPDataRef*, vector<TGPDataRef>&, TGPEvaluationContext, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPNodeRef::Evaluate(
				TGPDataRef* ioResult,
				vector<TGPDataRef>& ioStorage,
				TGPEvaluationContext* inContext,
				KUIntPtr* ioCookie )
{
	KASSERT( mNodeRef != nil );
	
	TGPNodeRef* theResult = nil;
	
	Boolean isConst = mNodeRef->GetIsConst();
	if ((!isConst) || (!mNodeRef->GetResultIsValid()))
	{
		theResult = GetNode()->Evaluate(
							this, ioResult, ioStorage, inContext, ioCookie );
		if ((theResult == nil) && (isConst))
		{
			mNodeRef->SetResultIsValid( true );
			mNodeRef->SetResult( *ioResult );
		}
	} else {
		*ioResult = mNodeRef->GetResult();
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * Invalidate( void ) const
// -------------------------------------------------------------------------- //
void
TGPNodeRef::Invalidate( void ) const
{
	// Appel récursif.
	KUInt32 indexNodes;
	KUInt32 theArity = GetNode()->GetArity();;

	KASSERT( (theArity == 0) || (mNodeRef->GetSubnodes() != nil) );

	for (indexNodes = 0; indexNodes < theArity; indexNodes++)
	{
		GetChildAlways( indexNodes ).Invalidate();
	}
	
	// Libération de la mémoire.
	mNodeRef->ClearResult();
	
	// Ce noeud est-il constant?
	// On demande d'abord aux fils.
	Boolean isConst = true;
	for (indexNodes = 0; indexNodes < theArity; indexNodes++)
	{
		isConst = GetChildAlways( indexNodes ).IsConst();
		if (!isConst)
		{
			break;
		}
	}
	
	if (isConst)
	{
		// On regarde alors pour ce noeud.
		isConst = GetNode()->IsConst();
	}
	
	// Enregistrement du résultat.
	mNodeRef->SetIsConst( isConst );
}

// -------------------------------------------------------------------------- //
//  * ToStreamAsLisp( ostream&, KUInt32 ) const
// -------------------------------------------------------------------------- //
void
TGPNodeRef::ToStreamAsLisp( ostream& outStream, KUInt32 inLevel ) const
{
	// Tabulations.
	KUInt32 indexLevel;
	for (indexLevel = 0; indexLevel < inLevel; indexLevel++)
	{
		(void) (outStream << "\t");
	}
	
	// nom ou ( nom paramètre-1 paramètre-2 ... paramètre-n )
	KUInt32 theArity = GetNode()->GetArity();;
	if (theArity == 0)
	{
		// Nom du noeud.
		(void) (outStream << GetNode()->GetLispName());
	} else {
		// Parenthèse ouvrante & nom du noeud.
		(void) (outStream << "(" << GetNode()->GetLispName());

		// Sous-noeuds
		KUInt32 indexSubnode;
		for (indexSubnode = 0; indexSubnode < theArity; indexSubnode++)
		{
			(void) (outStream << endl);
			GetChildAlways(indexSubnode).ToStreamAsLisp( outStream, inLevel+1 );
		}
		
		// Parenthèse fermante
		(void) (outStream << ")");
	}	
}

// -------------------------------------------------------------------------- //
//  * ToStreamAsDot( ostream& ) const
// -------------------------------------------------------------------------- //
void
TGPNodeRef::ToStreamAsDot( ostream& outStream ) const
{
	// On crée le nom.
	char theName[10];
	(void) ::sprintf( theName, "n%.8X", (unsigned int) this );
	
	(void) (outStream << theName << "[label=\"" << GetNode()->GetLispName() << "\"];" << endl);

	// Sous-noeuds
	KUInt32 indexSubnode;
	KUInt32 theArity = GetNode()->GetArity();;
	for (indexSubnode = 0; indexSubnode < theArity; indexSubnode++)
	{
		TGPNodeRef& theNodeRef = GetChildAlways(indexSubnode);
		char theSubnodeName[10];
		(void) ::sprintf(
					theSubnodeName,
					"n%.8X", (unsigned int) (TGPNodeRef*) &theNodeRef );
		
		if (theArity > 1)
		{
			(void) (outStream << theName << "->" << theSubnodeName
							<< "[label=\"" << indexSubnode << "\"];" << endl);
		} else {
			(void) (outStream << theName << "->" << theSubnodeName << endl);
		}
				
		theNodeRef.ToStreamAsDot( outStream );
	}
}

// -------------------------------------------------------------------------- //
//  * TNodeRef( TGPNode* )
// -------------------------------------------------------------------------- //
TGPNodeRef::TNodeRef::TNodeRef( TGPNode* inNode )
	:
		mNode( inNode ),
		mSubnodes( nil ),
		mIsConst( false ),
		mResultIsValid( false )
{
	if (inNode)
	{
		KUInt32 theArity = inNode->GetArity();
		if (theArity > 0)
		{
			mSubnodes = new TGPNodeRef[inNode->GetArity()];
		}
	}
}


// -------------------------------------------------------------------------- //
//  * ~TNodeRef( void )
// -------------------------------------------------------------------------- //
TGPNodeRef::TNodeRef::~TNodeRef( void )
{
	// Suppression du tableau.
	if (mSubnodes)
	{
		delete [] mSubnodes;
	}
}

// ======================================================================== //
// Trying to establish voice contact ... please ____yell into keyboard. //
// ======================================================================== //
