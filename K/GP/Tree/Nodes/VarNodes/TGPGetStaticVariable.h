// ==============================
// Fichier:			TGPGetStaticVariable.h
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
// The Original Code is TGPGetStaticVariable.h.
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
// $Id: TGPGetStaticVariable.h,v 1.8 2005/02/14 20:55:05 pguyot Exp $
// ===========

#ifndef _TGPGETSTATICVARIABLE_H
#define _TGPGETSTATICVARIABLE_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

// ISO C++
#include <string>

// K
#include <K/GP/Data/TGPType.h>

///
/// Classe pour un noeud qui récupère la valeur d'une variable statique.
/// Par variable statique on entend ici une variable simple dont la valeur
/// n'est pas déterminée dynamiquement mais par deux noeuds, Set et Get.
/// La valeur est initialisée à chaque fois que Reset est appelé sur le
/// contexte.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.8 $
///
/// \test	aucun test défini.
///
class TGPGetStaticVariable
	:
		public TGPNode
{
public:
	///
	/// Constructeur à partir d'un nom, d'un type et d'une valeur par défaut.
	///
	/// \param inVarName		nom de la variable
	/// \param inType			type de la variable (et donc du noeud)
	/// \param inDefaultValue	valeur par défaut de la variable (avant que Set ne soit
	///							appelé).
	///
	TGPGetStaticVariable(
					const std::string& inVarName,
					const TGPType& inType,
					TGPDataRef& inDefaultValue );

	///
	/// Constructeur à partir d'un nom et d'une valeur par défaut. Le type est
	/// déduit de la valeur par défaut.
	///
	/// \param inVarName		nom de la variable
	/// \param inDefaultValue	valeur par défaut de la variable (avant que Set ne soit
	///							appelé).
	///
	TGPGetStaticVariable(
					const std::string& inVarName,
					TGPDataRef& inDefaultValue );

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
	TGPGetStaticVariable( const TGPGetStaticVariable& inCopy );
	
	///
	/// Accesseur sur le nom.
	///
	/// \return une référence sur le nom.
	///
	inline const std::string&	GetVarName( void ) const
		{
			return mVarName;
		}

	///
	/// Accesseur sur la valeur par défaut.
	///
	/// \return la valeur par défaut.
	///
	inline const TGPDataRef&	GetDefaultValue( void ) const
		{
			return mDefaultValue;
		}

	///
	/// Accesseur sur la valeur par défaut.
	///
	/// \return la valeur par défaut.
	///
	inline TGPDataRef	GetDefaultValue( void )
		{
			return mDefaultValue;
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
	const std::string		mVarName;		///< Nom de la variable.
	const TGPType&			mType;			///< Type de la variable.
	TGPDataRef				mDefaultValue;	///< Valeur par défaut (avant Set)
	TStringRef				mLispName;		///< Nom du noeud.
};

#endif
		// _TGPGETSTATICVARIABLE_H

// ============================================================== //
// Real Programmers think better when playing Adventure or Rogue. //
// ============================================================== //
