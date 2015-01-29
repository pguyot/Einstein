// ==============================
// Fichier:			TGPSimpleInterpreter.h
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
// The Original Code is TGPSimpleInterpreter.h.
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
// $Id: TGPSimpleInterpreter.h,v 1.6 2005/06/09 06:41:09 pguyot Exp $
// ===========

#ifndef _TGPSIMPLEINTERPRETER_H
#define _TGPSIMPLEINTERPRETER_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/TGPInterpreter.h>

// Pré-déclarations
class TGPEvaluationContext;
class TGPNodeRef;
class TGPTree;

///
/// Classe pour un interpréteur simple, procédant récursivement.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.6 $
///
/// \test	aucun test défini.
///
class TGPSimpleInterpreter
{
public:
	///
	/// Constructeur à partir d'un nombre d'étape pour l'évaluation.
	///
	/// \param inMaxStepCount	nombre maximum d'étapes de l'évaluation (par
	///							défaut, 2^32-1)
	///
	TGPSimpleInterpreter( KUInt32 inMaxStepCount = (KUInt32) -1 );

	///
	/// Destructor.
	///
	virtual ~TGPSimpleInterpreter( void ) {}

	///
	/// Évalue un arbre. Retourne le résultat de l'évaluation.
	///
	/// \param inTree		arbre à évaluer.
	/// \param inContext	contexte pour l'évaluation.
	///
	virtual TGPDataRef Evaluate(
							TGPTree* inTree,
							TGPEvaluationContext* inContext );

private:
	///
	/// Évalue un noeud. Retourne le résultat de l'évaluation.
	///
	/// \param inNode		noeud à évaluer.
	/// \param inContext	contexte pour l'évaluation.
	///
	TGPDataRef Evaluate(
							TGPNodeRef* inNode,
							TGPEvaluationContext* inContext );
	
	/// \name Variables
	KUInt32			mStepsLeft;		///< Nombre d'étapes qui restent.
	KUInt32			mMaxSteps;		///< Nombre maximum d'étapes
};

#endif
		// _TGPSIMPLEINTERPRETER_H

// ====================================================================== //
// Prof:    So the American government went to IBM to come up with a data //
//          encryption standard and they came up with ...                 //
// Student: EBCDIC!"                                                      //
// ====================================================================== //
