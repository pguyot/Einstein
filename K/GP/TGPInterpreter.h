// ==============================
// Fichier:			TGPInterpreter.h
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
// The Original Code is TGPInterpreter.h.
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
// $Id: TGPInterpreter.h,v 1.2 2004/11/23 15:11:29 paul Exp $
// ===========

#ifndef _TGPINTERPRETER_H
#define _TGPINTERPRETER_H

#include <K/Defines/KDefinitions.h>

// K
#include <K/GP/Data/TGPDataRef.h>

// Pré-déclarations
class TGPTree;
class TGPEvaluationContext;

///
/// Classe pour l'interpréteur des arbres.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TGPInterpreter
{
public:
	///
	/// Constructeur par défaut.
	///
	TGPInterpreter( void );

	///
	/// Destructeur.
	///
	virtual ~TGPInterpreter( void );

	///
	/// Évalue un arbre. Retourne le résultat de l'évaluation.
	///
	virtual TGPDataRef Evaluate(
							TGPTree* inTree,
							TGPEvaluationContext* inContext ) = 0;

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPInterpreter( const TGPInterpreter& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPInterpreter& operator = ( const TGPInterpreter& inCopy );
};

#endif
		// _TGPINTERPRETER_H

// ========================================================================= //
// The net is like a vast sea of lutefisk with tiny dinosaur brains embedded //
// in it here and there. Any given spoonful will likely have an IQ of 1, but //
// occasional spoonfuls may have an IQ more than six times that!             //
//         -- James 'Kibo' Parry                                             //
// ========================================================================= //
