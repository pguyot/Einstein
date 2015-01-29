// ==============================
// Fichier:			TGPNode.h
// Projet:			K
//
// Créé le:			16/1/2003
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
// The Original Code is TGPNode.h.
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

#ifndef _TGPNODE_H
#define _TGPNODE_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <vector>
#include <functional>

// K
#include <K/GP/Data/TGPDataRef.h>
#include <K/GP/Data/TGPType.h>
#include <K/Unicode/TStringRef.h>

// Pré-déclarations
class TGPEvaluationContext;
class TGPNodeRef;

///
/// Classe pour un noeud dans un programme de programmation génétique.
/// Un noeud peut-être une primitive ou une feuille
/// (suivant l'arité)
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPNode
{
public:
	///
	/// Destructeur.
	///
	virtual ~TGPNode( void );

	///
	/// Accesseur sur l'arité.
	///
	inline KUInt32			GetArity( void ) const
		{
			return mArity;
		}
	
	///
	/// Récupération du type de ce noeud.
	///
	/// \return le type de ce noeud.
	///
	virtual const TGPType&	GetType( void ) const = 0;

	///
	/// Récupération du type d'un paramètre.
	/// Retourne le type intrinsèque, i.e. ne demande pas au
	/// sous-noeud.
	///
	/// \param	inParamIndex		indice du paramètre dont on veut le type.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	virtual const TGPType&	GetParamType( KUInt32 inParamIndex ) const = 0;

	///
	/// Récupère le nom du noeud.
	/// Ce nom doit être unique et permettre de recréer le noeud (sans
	/// les sous-noeuds).
	///
	/// \return	une référence sur la chaîne qui identifie le noeud.
	///
	virtual TStringRef		GetLispName( void ) const = 0;

protected:
	///
	/// Constructeur par copie.
	/// Vide la liste des sous-noeuds.
	/// Ce constructeur doit être appelé via le constructeur par
	/// copie des classes concrètes.
	///
	/// \param inCopy		objet à copier
	///
	TGPNode( const TGPNode& inCopy );

	///
	/// Constructeur à partir de l'arité.
	///
	/// \param	inArity		arité du noeud.
	///
	TGPNode( KUInt32 inArity );

private:
	friend class TGPNodeRef;

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPNode& operator = ( const TGPNode& inCopy );

	///
	/// Détermine si ce noeud est constant (au sens où si les sous-noeuds
	/// sont constants, son évaluation retournera le même résultat et le contexte
	/// ne sera pas modifié).
	///
	/// Par défaut répond \c true.
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
							KUIntPtr* ioCookie ) = 0;

	/// \name Variables	
	KUInt32				mArity;			///< Arité du noeud, i.e. nombre de
										///< sous-noeuds.	
};

#ifdef __STL_BEGIN_NAMESPACE
__STL_BEGIN_NAMESPACE
#else
namespace std
{
#endif

///
/// Comparateur pour la classe TGPNode.
/// Utilise GetLispName et ignore les sous-noeuds.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
template<>
struct less<TGPNode*>
{
	///
	/// Comparaison des noeuds.
	///
	/// \param x	premier noeud à comparer
	/// \param y	deuxième noeud à comparer
	/// \return \c true si x < y
	///
	bool operator()(const TGPNode* x, const TGPNode* y);
};

#ifdef __STL_END_NAMESPACE
__STL_END_NAMESPACE
#else
}
#endif

#endif
		// _TGPNODE_H

// ====================================== //
// Frankly, Scarlett, I don't have a fix. //
//                 -- Rhett Buggler       //
// ====================================== //
