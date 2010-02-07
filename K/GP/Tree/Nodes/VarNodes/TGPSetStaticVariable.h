// ==============================
// Fichier:			TGPSetStaticVariable.h
// Projet:			K
// 
// Créé le:			14/8/2003
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
// The Original Code is TGPSetStaticVariable.h.
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
// $Id: TGPSetStaticVariable.h,v 1.7 2004/11/23 15:11:31 paul Exp $
// ===========

#ifndef _TGPSETSTATICVARIABLE_H
#define _TGPSETSTATICVARIABLE_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

// ISO C++
#include <string>

///
/// Classe pour un noeud qui change la valeur d'une variable statique.
/// Par variable statique on entend ici une variable simple dont la valeur
/// n'est pas déterminée dynamiquement mais par deux noeuds, Set et Get.
/// La valeur est initialisée à chaque fois que Reset est appelé sur le
/// contexte.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.7 $
///
/// \test	aucun test défini.
///
class TGPSetStaticVariable
	:
		public TGPNode
{
public:
	///
	/// Constructeur à partir d'un nom de variable, d'un type de variable et d'un
	/// type pour le noeud.
	/// Ce noeud retourne la valeur de la variable, mais il peut être utile de définir
	/// son type comme void. Par défaut, c'est le même type que la variable.
	///
	/// \param inVarName	nom de la variable (détermine le stockage de la variable).
	/// \param inVarType	type de la variable.
	/// \param inNodeType	type du noeud.
	///
	TGPSetStaticVariable(
					const std::string& inVarName,
					const TGPType& inVarType,
					const TGPType& inNodeType );

	///
	/// Constructeur à partir d'un nom de variable et d'un type de variable.
	/// C'est le même constructeur que le précédent sauf que le type du noeud
	/// est défini comme étant celui de la variable.
	///
	/// \param inVarName	nom de la variable (détermine le stockage de la variable).
	/// \param inVarType	type de la variable.
	///
	TGPSetStaticVariable(
					const std::string& inVarName,
					const TGPType& inVarType );

	///
	/// Récupération du type de ce noeud.
	///
	/// \return le type de ce noeud (kBoolean).
	///
	virtual const TGPType&	GetType( void ) const;

	///
	/// Récupération du type d'un paramètre.
	/// Retourne le type intrinsèque, i.e. ne demande pas au
	/// sous-noeud.
	///
	/// \param	inParamIndex		indice du paramètre dont on veut le type.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	virtual const TGPType&	GetParamType( KUInt32 inParamIndex ) const;

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
	/// Constructeur par copie.
	///
	/// \param inCopy		objet à copier
	///
	TGPSetStaticVariable( const TGPSetStaticVariable& inCopy );

	///
	/// Accesseur sur le nom.
	///
	/// \return une référence sur le nom.
	///
	inline const std::string&	GetVarName( void ) const
		{
			return mVarName;
		}

private:
	///
	/// Détermine si ce noeud est constant.
	/// Méthode appelée par Invalidate si les sous-noeuds
	/// sont constants.
	///
	/// \return \c true si ce noeud est constant, \c false sinon.
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

	/// \name Constantes
	static const char*	kLispFormat;	///< Format du nom du noeud.

	/// \name Variables
	const std::string		mVarName;	///< Nom de la variable.
	const TGPType&			mVarType;	///< Type de la variable.
	const TGPType&			mNodeType;	///< Type du noeud.
	TStringRef				mLispName;	///< Nom du noeud.
};

#endif
		// _TGPSETSTATICVARIABLE_H

// ================================================ //
// An elephant is a mouse with an operating system. //
// ================================================ //
