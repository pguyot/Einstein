// ==============================
// Fichier:			TGPSimpleInterpreter.h
// Projet:			K
// 
// Cr�� le:			23/10/2003
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

// Pr�-d�clarations
class TGPEvaluationContext;
class TGPNodeRef;
class TGPTree;

///
/// Classe pour un interpr�teur simple, proc�dant r�cursivement.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.6 $
///
/// \test	aucun test d�fini.
///
class TGPSimpleInterpreter
{
public:
	///
	/// Constructeur � partir d'un nombre d'�tape pour l'�valuation.
	///
	/// \param inMaxStepCount	nombre maximum d'�tapes de l'�valuation (par
	///							d�faut, 2^32-1)
	///
	TGPSimpleInterpreter( KUInt32 inMaxStepCount = (KUInt32) -1 );

	///
	/// Destructor.
	///
	virtual ~TGPSimpleInterpreter( void ) {}

	///
	/// �value un arbre. Retourne le r�sultat de l'�valuation.
	///
	/// \param inTree		arbre � �valuer.
	/// \param inContext	contexte pour l'�valuation.
	///
	virtual TGPDataRef Evaluate(
							TGPTree* inTree,
							TGPEvaluationContext* inContext );

private:
	///
	/// �value un noeud. Retourne le r�sultat de l'�valuation.
	///
	/// \param inNode		noeud � �valuer.
	/// \param inContext	contexte pour l'�valuation.
	///
	TGPDataRef Evaluate(
							TGPNodeRef* inNode,
							TGPEvaluationContext* inContext );
	
	/// \name Variables
	KUInt32			mStepsLeft;		///< Nombre d'�tapes qui restent.
	KUInt32			mMaxSteps;		///< Nombre maximum d'�tapes
};

#endif
		// _TGPSIMPLEINTERPRETER_H

// ====================================================================== //
// Prof:    So the American government went to IBM to come up with a data //
//          encryption standard and they came up with ...                 //
// Student: EBCDIC!"                                                      //
// ====================================================================== //
