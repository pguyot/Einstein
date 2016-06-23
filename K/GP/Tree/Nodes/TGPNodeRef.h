// ==============================
// Fichier:			TGPNodeRef.h
// Projet:			K
// 
// Cr�� le:			17/11/2003
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
/// Classe pour une r�f�rence sur un noeud au sein d'arbres.
/// � l'inverse des autres classes pour des r�f�rences, celle-ci
/// g�re aussi la structure de l'arbre (c'est-�-dire les enfants).
/// Cette classe est utilis�e pour limiter l'utilisation de la m�moire
/// et am�liorer les performances qui sont li�es.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.6 $
///
/// \test	aucun test d�fini.
///
class TGPNodeRef
{
public:
	///
	/// R�sultat d'une recherche en profondeur (avec GetSubNode).
	///
	enum ESearchResult {
		kNotFound,	///< Le noeud n'a pas �t� trouv�.
		kThisNode,	///< Le noeud cherch� est this
		kASubNode	///< Le noeud cherch� est un sous-noeud.
	};

	///
	/// Constructeur par d�faut.
	///
	TGPNodeRef( void );

	///
	/// Constructeur � partir d'un noeud.
	///
	/// \param inNode	noeud pour cette r�f�rence.
	///
	TGPNodeRef( TGPNode* inNode );

	///
	/// Constructeur par copie.
	///
	/// \param inCopy		objet � copier
	///
	TGPNodeRef( const TGPNodeRef& inCopy );

	///
	/// Op�rateur d'assignation.
	///
	/// \param inCopy		objet � copier
	///
	TGPNodeRef& operator = ( const TGPNodeRef& inCopy );

	///
	/// Destructeur.
	///
	~TGPNodeRef( void );

	///
	/// D�termine si cette r�f�rence est \c nil.
	///
	/// \return \c true si la r�f�rence est \c nil, false
	///			sinon.
	///
	inline Boolean			IsNil( void ) const
		{
			return (mNodeRef == nil);
		}

	///
	/// Accesseur sur le noeud r�f�renc�.
	///
	/// \return le noeud r�f�renc�.
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
	/// R�cup�ration du type de ce noeud.
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
	/// \param	inNodeIndex		indice du noeud � r�cup�rer.
	/// \return	le i�me sous-noeud.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	TGPNodeRef&				GetChild( KUInt32 inNodeIndex ) const;

	///
	/// Accesseur sur un sous-noeud, lance une exception si l'arbre n'est pas complet.
	///
	/// \param	inNodeIndex		indice du noeud � r�cup�rer.
	/// \return	le i�me sous-noeud.
	/// \throws range_error s'il n'y pas un tel noeud.
	/// \throws runtime_error si le noeud est � \c nil.
	///
	TGPNodeRef&				GetChildAlways( KUInt32 inNodeIndex ) const;

	///
	/// S�lecteur sur un sous-noeud. L'ancien sous-noeud est supprim�.
	///
	/// \param	inNodeIndex		indice du noeud � changer.
	/// \param	inNewChild		nouveau noeud (copi�).
	/// \throws range_error s'il n'y pas autant de noeuds.
	///
	void					SetChild( KUInt32 inNodeIndex, const TGPNodeRef& inNewChild );

	///
	/// D�termine la profondeur de l'arbre.
	/// M�thode r�cursive pour compter les noeuds.
	///
	/// \return la profondeur de l'arbre.
	///
	KUInt32					GetDepth( void ) const;

	///
	/// Retourne le nombre de noeuds du sous-arbre.
	/// M�thode r�cursive pour compter les noeuds.
	///
	/// \return le nombre de noeuds du sous-arbre (y compris celui-ci).
	///
	KUInt32					CountNodes( void ) const;

	///
	/// Retourne le nombre de noeuds du sous-arbre d'un type donn�.
	/// M�thode r�cursive pour compter les noeuds.
	///
	/// \param	inType	type des noeuds � compter.
	/// \return le nombre de noeuds du type inType du sous-arbre
	///			(y compris celui-ci).
	///
	KUInt32					CountNodes( const TGPType& inType ) const;

	///
	/// R�cup�re le n-i�me sous-noeud de ce sous-arbre.
	/// M�thode r�cursive.
	///
	/// \param	ioIndex	indice du noeud � r�cup�rer en entr�e
	///			0 ou cet indice diminu� du nombre de sous-noeuds en sortie.
	/// \param	outNode			en sortie, n-i�me noeud (non renseign� si \c nil)
	/// \param	outParentNode	en sortie, parent du n-i�me noeud. (idem)
	/// \param	outSiblingIndex	en sortie, indice parmi les enfants du
	///							n-i�me noeud. (idem)
	/// \return le r�sultat de la recherche.
	///
	ESearchResult			GetSubNode(
									KUInt32& ioIndex,
									TGPNodeRef* outNode,
									TGPNodeRef* outParentNode,
									KUInt32* outSiblingIndex );

	///
	/// R�cup�re le n-i�me sous-noeud de ce sous-arbre.
	/// M�thode r�cursive.
	///
	/// \param	inType	type du noeud � r�cup�rer.
	/// \param	ioIndex	indice du noeud � r�cup�rer en entr�e
	///			0 ou cet indice diminu� du nombre de sous-noeuds
	///			de ce type en sortie.
	/// \param	outNode			en sortie, n-i�me noeud (non renseign� si \c nil)
	/// \param	outParentNode	en sortie, parent du n-i�me noeud. (idem)
	/// \param	outSiblingIndex	en sortie, indice parmi les enfants du
	///							n-i�me noeud. (idem)
	/// \return le r�sultat de la recherche.
	///
	ESearchResult			GetSubNode(
									const TGPType& inType,
									KUInt32& ioIndex,
									TGPNodeRef* outNode,
									TGPNodeRef* outParentNode,
									KUInt32* outSiblingIndex );
	
	///
	/// Indique au noeud de r�initialiser son cache.
	/// Appel r�cursif.
	/// D�termine aussi si le sous-arbre est constant.
	/// Pour ce faire, appelle mNode->IsConst et IsConst sur les noeuds fils.
	///
	void					Invalidate( void ) const;

	///
	/// D�termine si le noeud est constant.
	/// Cette information est stock�e dans mIsConst.
	///
	/// \return \c true si le sous-arbre est constant, \c false sinon.
	///
	inline Boolean			IsConst( void ) const
		{
			return mNodeRef->GetIsConst();
		}

	///
	/// �valuation.
	///
	/// Cette m�thode est appel�e plusieurs fois par l'interpr�teur.
	/// Cette m�thode retourne un sous-noeud � �valuer pour l'interpr�teur, afin
	/// de calculer son propre r�sultat, ou \c nil lorsque l'�valuation est termin�e.
	/// Dans ce cas, le r�sultat doit �tre renseign� dans \c ioResult.
	///
	/// \param ioResult		en entr�e, r�sultat de l'�valuation du sous-noeud
	///						demand�.
	///						en sortie, r�sultat de l'�valuation de ce noeud.
	/// \param ioStorage	entrep�t pour des donn�es (laiss� � la discr�tion
	///						du noeud)
	/// \param inContext	contexte pour l'�valuation.
	/// \param ioCookie		biscuit (mis � z�ro par l'interpr�teur au d�but de
	///						l'�valuation de ce noeud).	
	/// \return un sous-noeud dont la m�thode veut le r�sultat, \c nil
	///			lorsque l'�valuation est termin�e.
	///
	TGPNodeRef*	Evaluate(
						TGPDataRef* ioResult,
						std::vector<TGPDataRef>& ioStorage,
						TGPEvaluationContext* inContext,
						KUIntPtr* ioCookie );
	
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
		/// La classe r�f�rence est une amie.
		///
		friend class TGPNodeRef;

	public:
		///
		/// Constructeur � partir d'un noeud.
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
		/// S�lecteur sur l'indicateur que le sous-arbre est constant.
		///
		inline void			SetIsConst( Boolean inIsConst )
			{
				mIsConst = inIsConst;
			}
		
		///
		/// Accesseur sur la validit� du r�sultat en cache.
		///
		inline Boolean		GetResultIsValid( void ) const
			{
				return mResultIsValid;
			}
		
		///
		/// S�lecteur sur la validit� du r�sultat en cache.
		///
		inline void			SetResultIsValid( Boolean inResultIsValid )
			{
				mResultIsValid = inResultIsValid;
			}
		
		///
		/// Accesseur sur le r�sultat en cache.
		///
		inline TGPDataRef&	GetResult( void )
			{
				return mResult;
			}
		
		///
		/// S�lecteur sur le r�sultat en cache.
		///
		inline void	SetResult( TGPDataRef& inResult )
			{
				mResult = inResult;
			}
		
		///
		/// Efface le r�sultat en cache.
		///
		inline void	ClearResult( void )
			{
				mResultIsValid = false;
				mResult = TGPDataRef();
			}

	private:
		TGPNode*			mNode;			///< R�f�rence sur le noeud.
		TGPNodeRef*			mSubnodes;		///< Sous-noeuds.
		Boolean				mIsConst;		///< Si le sous-arbre est constant.
		Boolean				mResultIsValid;	///< Si le cache est valide.
		TGPDataRef			mResult;		///< R�sultat du sous-arbre (cache).
	};

	/// \name Variables
	TNodeRef*				mNodeRef;		///< R�f�rence.
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
/// \test	aucun test d�fini.
///
template<>
struct less<TGPNodeRef*>
{
	///
	/// Comparaison des noeuds.
	///
	/// \param x	premier noeud � comparer
	/// \param y	deuxi�me noeud � comparer
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
