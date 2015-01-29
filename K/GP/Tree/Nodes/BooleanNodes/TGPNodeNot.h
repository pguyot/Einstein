// ==============================
// Fichier:			TGPNodeNot.h
// Projet:			K
//
// Créé le:			25/2/2003
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
// The Original Code is TGPNodeNot.h.
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
// $Id: TGPNodeNot.h,v 1.7 2004/11/23 15:11:30 paul Exp $
// ===========

#ifndef _TGPNODENOT_H
#define _TGPNODENOT_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/BooleanNodes/TGPNodeBoolean.h>

#include <string>

///
/// Classe pour une négation.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPNodeNot
	:
		public TGPNodeBoolean
{
public:
	///
	/// Constructeur par défaut.
	///
	TGPNodeNot( void );

	///
	/// Récupère le nom du noeud.
	/// Ce nom doit être unique et permettre de recréer le noeud (sans
	/// les sous-noeuds).
	///
	/// \return	une référence sur la chaîne qui identifie le noeud.
	///
	virtual TStringRef		GetLispName( void ) const;

private:
	///
	/// Évaluation.
	///
	/// Cette méthode est appelée plusieurs fois par l'interpréteur.
	/// Elle retourne un sous-noeud à évaluer pour l'interpréteur, afin de
	/// calculer son propre résultat, ou \c nil lorsque l'évaluation est terminée.
	/// Dans ce cas, le résultat doit être renseigné dans \c ioResult.
	///
	/// \param inNodeRef	référence sur le noeud évalué.
	/// \param ioResult		en entrée, résultat de l'évaluation du sous-noeud
	///						demandé.
	///						en sortie, résultat de l'évaluation de ce noeud.
	/// \param ioStorage	entrepôt pour des données (laissé à la discrétion
	///						du noeud)
	/// \param inContext	contexte pour l'évaluation.
	/// \param ioCookie		biscuit (mis à zéro par l'interpréteur au début de
	///						l'évaluation de ce noeud).	
	/// \return un sous-noeud dont la méthode veut le résultat, \c nil
	///			lorsque l'évaluation est terminée.
	///
	virtual TGPNodeRef*	Evaluate(
							TGPNodeRef* inNodeRef,
							TGPDataRef* ioResult,
							std::vector<TGPDataRef>& ioStorage,
							TGPEvaluationContext* inContext,
							KUIntPtr* ioCookie );

	/// \name Constantes
	TStringRef				mLispName;	///< Nom du noeud.
};

#endif
		// _TGPNODENOT_H

// ===================================================== //
// Intel CPUs are not defective, they just act that way. //
//                 -- Henry Spencer                      //
// ===================================================== //
