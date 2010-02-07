// ==============================
// Fichier:			TGPNodeRef.h
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
// The Original Code is TGPNodeRef.h.
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
// $Id: TGPNodeRef.h,v 1.6 2005/05/26 06:52:21 pguyot Exp $
// ===========

#ifndef _TGPNODEREF_H
#define _TGPNODEREF_H

#include <K/Defines/KDefinitions.h>
#include <K/Misc/TRefCounter.h>

// K
#include <K/GP/Data/TGPDataRef.h>
#include <K/GP/Data/TGPType.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

///
/// Classe pour une référence sur un noeud au sein d'arbres.
/// À l'inverse des autres classes pour des références, celle-ci
/// gère aussi la structure de l'arbre (c'est-à-dire les enfants).
/// Cette classe est utilisée pour limiter l'utilisation de la mémoire
/// et améliorer les performances qui sont liées.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.6 $
///
/// \test	aucun test défini.
///
class TGPNodeRef
{
public:
	///
	/// Résultat d'une recherche en profondeur (avec GetSubNode).
	///
	enum ESearchResult {
		kNotFound,	///< Le noeud n'a pas été trouvé.
		kThisNode,	///< Le noeud cherché est this
		kASubNode	///< Le noeud cherché est un sous-noeud.
	};

	///
	/// Constructeur par défaut.
	///
	TGPNodeRef( void );

	///
	/// Constructeur à partir d'un noeud.
	///
	/// \param inNode	noeud pour cette référence.
	///
	TGPNodeRef( TGPNode* inNode );

	///
	/// Constructeur par copie.
	///
	/// \param inCopy		objet à copier
	///
	TGPNodeRef( const TGPNodeRef& inCopy );

	///
	/// Opérateur d'assignation.
	///
	/// \param inCopy		objet à copier
	///
	TGPNodeRef& operator = ( const TGPNodeRef& inCopy );

	///
	/// Destructeur.
	///
	~TGPNodeRef( void );

	///
	/// Détermine si cette référence est \c nil.
	///
	/// \return \c true si la référence est \c nil, false
	///			sinon.
	///
	inline Boolean			IsNil( void ) const
		{
			return (mNodeRef == nil);
		}

	///
	/// Accesseur sur le noeud référencé.
	///
	/// \return le noeud référencé.
	///
	inline	TGPNode*		GetNode( void ) const
		{
			if (mNodeRef)
			{
				return mNodeRef->GetNode();
			} else {
				return nil;
			}
		}
	
	///
	/// Récupération du type de ce noeud.
	///
	/// \return le type de ce noeud.
	///
	inline const TGPType&	GetType( void ) const
		{
			return mNodeRef->GetNode()->GetType();
		}

	///
	/// Effectue une copie profonde de l'arbre.
	///
	/// \return une copie profonde de cet arbre.
	///
	TGPNodeRef				DeepClone( void ) const;

	///
	/// Accesseur sur un sous-noeud.
	///
	/// \param	inNodeIndex		indice du noeud à récupérer.
	/// \return	le ième sous-noeud.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	TGPNodeRef&				GetChild( KUInt32 inNodeIndex ) const;

	///
	/// Accesseur sur un sous-noeud, lance une exception si l'arbre n'est pas complet.
	///
	/// \param	inNodeIndex		indice du noeud à récupérer.
	/// \return	le ième sous-noeud.
	/// \throws range_error s'il n'y pas un tel noeud.
	/// \throws runtime_error si le noeud est à \c nil.
	///
	TGPNodeRef&				GetChildAlways( KUInt32 inNodeIndex ) const;

	///
	/// Sélecteur sur un sous-noeud. L'ancien sous-noeud est supprimé.
	///
	/// \param	inNodeIndex		indice du noeud à changer.
	/// \param	inNewChild		nouveau noeud (copié).
	/// \throws range_error s'il n'y pas autant de noeuds.
	///
	void					SetChild( KUInt32 inNodeIndex, const TGPNodeRef& inNewChild );

	///
	/// Détermine la profondeur de l'arbre.
	/// Méthode récursive pour compter les noeuds.
	///
	/// \return la profondeur de l'arbre.
	///
	KUInt32					GetDepth( void ) const;

	///
	/// Retourne le nombre de noeuds du sous-arbre.
	/// Méthode récursive pour compter les noeuds.
	///
	/// \return le nombre de noeuds du sous-arbre (y compris celui-ci).
	///
	KUInt32					CountNodes( void ) const;

	///
	/// Retourne le nombre de noeuds du sous-arbre d'un type donné.
	/// Méthode récursive pour compter les noeuds.
	///
	/// \param	inType	type des noeuds à compter.
	/// \return le nombre de noeuds du type inType du sous-arbre
	///			(y compris celui-ci).
	///
	KUInt32					CountNodes( const TGPType& inType ) const;

	///
	/// Récupère le n-ième sous-noeud de ce sous-arbre.
	/// Méthode récursive.
	///
	/// \param	ioIndex	indice du noeud à récupérer en entrée
	///			0 ou cet indice diminué du nombre de sous-noeuds en sortie.
	/// \param	outNode			en sortie, n-ième noeud (non renseigné si \c nil)
	/// \param	outParentNode	en sortie, parent du n-ième noeud. (idem)
	/// \param	outSiblingIndex	en sortie, indice parmi les enfants du
	///							n-ième noeud. (idem)
	/// \return le résultat de la recherche.
	///
	ESearchResult			GetSubNode(
									KUInt32& ioIndex,
									TGPNodeRef* outNode,
									TGPNodeRef* outParentNode,
									KUInt32* outSiblingIndex );

	///
	/// Récupère le n-ième sous-noeud de ce sous-arbre.
	/// Méthode récursive.
	///
	/// \param	inType	type du noeud à récupérer.
	/// \param	ioIndex	indice du noeud à récupérer en entrée
	///			0 ou cet indice diminué du nombre de sous-noeuds
	///			de ce type en sortie.
	/// \param	outNode			en sortie, n-ième noeud (non renseigné si \c nil)
	/// \param	outParentNode	en sortie, parent du n-ième noeud. (idem)
	/// \param	outSiblingIndex	en sortie, indice parmi les enfants du
	///							n-ième noeud. (idem)
	/// \return le résultat de la recherche.
	///
	ESearchResult			GetSubNode(
									const TGPType& inType,
									KUInt32& ioIndex,
									TGPNodeRef* outNode,
									TGPNodeRef* outParentNode,
									KUInt32* outSiblingIndex );
	
	///
	/// Indique au noeud de réinitialiser son cache.
	/// Appel récursif.
	/// Détermine aussi si le sous-arbre est constant.
	/// Pour ce faire, appelle mNode->IsConst et IsConst sur les noeuds fils.
	///
	void					Invalidate( void ) const;

	///
	/// Détermine si le noeud est constant.
	/// Cette information est stockée dans mIsConst.
	///
	/// \return \c true si le sous-arbre est constant, \c false sinon.
	///
	inline Boolean			IsConst( void ) const
		{
			return mNodeRef->GetIsConst();
		}

	///
	/// Évaluation.
	///
	/// Cette méthode est appelée plusieurs fois par l'interpréteur.
	/// Cette méthode retourne un sous-noeud à évaluer pour l'interpréteur, afin
	/// de calculer son propre résultat, ou \c nil lorsque l'évaluation est terminée.
	/// Dans ce cas, le résultat doit être renseigné dans \c ioResult.
	///
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
	TGPNodeRef*	Evaluate(
						TGPDataRef* ioResult,
						std::vector<TGPDataRef>& ioStorage,
						TGPEvaluationContext* inContext,
						KUInt32* ioCookie );
	
	///
	/// Conversion sous forme de texte (au format LISP).
	///
	/// \param	outStream	flux de sortie.
	/// \param	inLevel		niveau pour les tabulations
	/// \throws runtime_error si l'arbre n'est pas complet.
	///
	void		ToStreamAsLisp(
						std::ostream& outStream,
						KUInt32 inLevel ) const;

	///
	/// Conversion sous forme de graphe (au format dot).
	///
	/// \param	outStream	flux de sortie.
	/// \throws runtime_error si l'arbre n'est pas complet.
	///
	void		ToStreamAsDot( std::ostream& outStream ) const;

private:
	class TNodeRef
		:
			private TRefCounter
	{
		///
		/// La classe référence est une amie.
		///
		friend class TGPNodeRef;

	public:
		///
		/// Constructeur à partir d'un noeud.
		///
		TNodeRef( TGPNode* inNode );
		
		///
		/// Destructeur.
		/// Supprime le tableau des sous-noeuds.
		///
		~TNodeRef( void );

		///
		/// Accesseur sur le noeud
		///
		inline TGPNode*		GetNode( void )
			{
				return mNode;
			}
		
		///
		/// Accesseur sur le tableau des sous-noeuds.
		///
		inline TGPNodeRef*	GetSubnodes( void )
			{
				return mSubnodes;
			}
		
		///
		/// Accesseur sur l'indicateur que le sous-arbre est constant.
		///
		inline Boolean		GetIsConst( void ) const
			{
				return mIsConst;
			}
		
		///
		/// Sélecteur sur l'indicateur que le sous-arbre est constant.
		///
		inline void			SetIsConst( Boolean inIsConst )
			{
				mIsConst = inIsConst;
			}
		
		///
		/// Accesseur sur la validité du résultat en cache.
		///
		inline Boolean		GetResultIsValid( void ) const
			{
				return mResultIsValid;
			}
		
		///
		/// Sélecteur sur la validité du résultat en cache.
		///
		inline void			SetResultIsValid( Boolean inResultIsValid )
			{
				mResultIsValid = inResultIsValid;
			}
		
		///
		/// Accesseur sur le résultat en cache.
		///
		inline TGPDataRef&	GetResult( void )
			{
				return mResult;
			}
		
		///
		/// Sélecteur sur le résultat en cache.
		///
		inline void	SetResult( TGPDataRef& inResult )
			{
				mResult = inResult;
			}
		
		///
		/// Efface le résultat en cache.
		///
		inline void	ClearResult( void )
			{
				mResultIsValid = false;
				mResult = TGPDataRef();
			}

	private:
		TGPNode*			mNode;			///< Référence sur le noeud.
		TGPNodeRef*			mSubnodes;		///< Sous-noeuds.
		Boolean				mIsConst;		///< Si le sous-arbre est constant.
		Boolean				mResultIsValid;	///< Si le cache est valide.
		TGPDataRef			mResult;		///< Résultat du sous-arbre (cache).
	};

	/// \name Variables
	TNodeRef*				mNodeRef;		///< Référence.
};

namespace std
{

///
/// Comparateur pour la classe TGPNodeRef.
/// Demande aux objets TGPNode.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
template<>
struct less<TGPNodeRef*>
{
	///
	/// Comparaison des noeuds.
	///
	/// \param x	premier noeud à comparer
	/// \param y	deuxième noeud à comparer
	/// \return \c true si x < y
	///
	bool operator()(const TGPNodeRef* x, const TGPNodeRef* y);
};

}

#endif
		// _TGPNODEREF_H

// ============================================================= //
// Old programmers never die, they just branch to a new address. //
// ============================================================= //
