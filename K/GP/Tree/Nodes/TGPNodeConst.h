// ==============================
// Fichier:			TGPNodeConst.h
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
// The Original Code is TGPNodeConst.h.
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
// $Id: TGPNodeConst.h,v 1.8 2005/02/14 20:55:04 pguyot Exp $
// ===========

#ifndef _TGPNODECONST_H
#define _TGPNODECONST_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNodeLeaf.h>

// ISO C++
#include <string>

///
/// Classe pour un noeud représentant une constante.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPNodeConst
	:
		public TGPNodeLeaf
{
public:
	///
	/// Constructeur à partir d'une donnée.
	/// Le type est celui de la donnée.
	///
	/// \param inValue	valeur de la constante.
	///
	TGPNodeConst( TGPDataRef& inValue );

	///
	/// Constructeur à partir d'un type et d'une donnée.
	///
	/// \param inType	type du noeud.
	/// \param inValue	valeur de la constante.
	///
	TGPNodeConst( const TGPType& inType, TGPDataRef& inValue );

	///
	/// Récupération du type de ce noeud.
	///
	/// \return le type de ce noeud.
	///
	virtual const TGPType&	GetType( void ) const;

	///
	/// Récupère le nom du noeud.
	/// Ce nom doit être unique et permettre de recréer le noeud (sans
	/// les sous-noeuds).
	///
	/// \return	une référence sur la chaîne qui identifie le noeud.
	///
	virtual TStringRef		GetLispName( void ) const;

protected:
	///
	/// Accesseur sur la valeur.
	///
	inline const TGPDataRef	GetValue( void ) const
		{
			return mValue;
		}

private:
	///
	/// Détermine si ce noeud est constant.
	/// Méthode appelée par Invalidate si les sous-noeuds
	/// sont constants.
	///
	/// \return \c true
	///
	virtual Boolean			IsConst( void ) const;

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

	/// \name Variables
	const TGPDataRef	mValue;		///< Valeur du noeud.
	const TGPType&		mType;		///< Type du noeud.
	TStringRef		 	mLispName;	///< Nom du noeud.
};

#endif
		// _TGPNODECONST_H

// =========================================================================== //
// Thus spake the master programmer:                                           //
//         "A well-written program is its own heaven; a poorly-written program //
// is its own hell."                                                           //
//                 -- Geoffrey James, "The Tao of Programming"                 //
// =========================================================================== //
