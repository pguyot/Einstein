// ==============================
// Fichier:			TGPSimpleInterpreter.cp
// Projet:			K
// 
// Créé le:			23/10/2003
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
// The Original Code is TGPSimpleInterpreter.cp.
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
// $Id: TGPSimpleInterpreter.cp,v 1.5 2004/11/23 15:11:29 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/TGPSimpleInterpreter.h>

// ISO C++
#include <vector>

// K
#include <K/GP/TGPEvaluationContext.h>
#include <K/GP/Tree/TGPTree.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TGPSimpleInterpreter( KUInt32 )
// -------------------------------------------------------------------------- //
TGPSimpleInterpreter::TGPSimpleInterpreter( KUInt32 inMaxSteps /* = -1 */ )
	:
		mMaxSteps( inMaxSteps )
{
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPTree*, TGPEvaluationContext* )
// -------------------------------------------------------------------------- //
TGPDataRef
TGPSimpleInterpreter::Evaluate(
							TGPTree* inTree,
							TGPEvaluationContext* inContext )
{
	// Récupération de la racine.
	TGPNodeRef& theRoot = inTree->GetRoot();
	
	// Initialisation de l'évaluation.
	mStepsLeft = mMaxSteps;
	inContext->Reset();
	
	// Évaluation de la racine.
	return Evaluate( &theRoot, inContext );
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPEvaluationContext* )
// -------------------------------------------------------------------------- //
TGPDataRef
TGPSimpleInterpreter::Evaluate(
							TGPNodeRef* inNode,
							TGPEvaluationContext* inContext )
{
	// Évaluation du noeud.
	TGPDataRef theResult;
	
	if (mStepsLeft > 0)
	{
		mStepsLeft--;

		// Zone de stockage à la discrétion du noeud.
		vector<TGPDataRef> theStorage;

		KUIntPtr theCookie = 0;

		while ( (mStepsLeft > 0)
				&& (inContext->ContinueEvaluation()) )
		{
			TGPNodeRef* theSubnode =
				inNode->Evaluate( &theResult, theStorage, inContext, &theCookie );
		
			if (theSubnode == nil)
			{
				break;
			}
			
			// Évaluation du sous-noeud.
			theResult = Evaluate( theSubnode, inContext );
		}
	}

	return theResult;
}

// ======================================================================= //
//         How many seconds are there in a year?  If I tell you there  are //
// 3.155  x  10^7, you won't even try to remember it.  On the other hand,  //
// who could forget that, to within half a percent, pi seconds is a        //
// nanocentury.                                                            //
//                 -- Tom Duff, Bell Labs                                  //
// ======================================================================= //
