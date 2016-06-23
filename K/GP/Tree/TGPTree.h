// ==============================
// Fichier:			TGPTree.h
// Projet:			K
//
// Cr�� le:			26/2/2003
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
// The Original Code is TGPTree.h.
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
// $Id: TGPTree.h,v 1.9 2005/05/26 06:52:21 pguyot Exp $
// ===========

#ifndef _TGPTREE_H
#define _TGPTREE_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <stdexcept>
#include <vector>

// K
#include <K/GP/Data/TGPDataRef.h>
#include <K/GP/Data/TGPType.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

// Pr�-d�clarations
class TGPFitness;
class TRandomGenerator;

///
/// Classe pour un arbre qui sera �valu�.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPTree
{
public:
	///
	/// Constructeur � partir d'une racine.
	///
	/// \param inRoot	racine de l'arbre.
	///
	TGPTree( const TGPNodeRef& inRoot );

	///
	/// Constructeur par copie.
	/// Copie totalement l'arbre (fait une copie profonde de la racine).
	///
	/// \param inCopy		objet � copier
	///
	TGPTree( const TGPTree& inCopy );

	///
	/// Destructeur.
	///
	/// D�truit la racine de l'arbre (et donc l'arbre tout entier).
	///
	virtual ~TGPTree( void );

	///
	/// Accesseur sur la racine de l'arbre.
	///
	/// \return un pointeur sur la racine de l'arbre.
	///
	inline TGPNodeRef&	GetRoot( void )
		{
			return mRoot;
		}

	///
	/// D�termine la profondeur de l'arbre.
	///
	/// \return la profondeur de l'arbre.
	///
	KUInt32	GetDepth( void ) const;

	///
	/// Choisit un noeud al�atoirement.
	///
	/// \param	inRandomGenerator	g�n�rateur al�atoire pour choisir le noeud.
	/// \param	outNode				en sortie, n-i�me noeud (non renseign� si \c nil)
	/// \param	outParentNode		en sortie, parent du n-i�me noeud. (idem)
	/// \param	outSiblingIndex		en sortie, indice parmi les enfants du
	///								n-i�me noeud. (idem)
	///
	void GetRandomNode(
				TRandomGenerator& inRandomGenerator,
				TGPNodeRef* outNode,
				TGPNodeRef* outParentNode,
				KUInt32* outSiblingIndex );

	///
	/// Choisit un noeud al�atoirement d'un type donn�.
	///
	/// \param	inRandomGenerator	g�n�rateur al�atoire pour choisir le noeud.
	/// \param	inType				type du noeud � choisir.
	/// \param	outNode				en sortie, n-i�me noeud (non renseign� si \c nil)
	/// \param	outParentNode		en sortie, parent du n-i�me noeud. (idem)
	/// \param	outSiblingIndex		en sortie, indice parmi les enfants du
	///								n-i�me noeud. (idem)
	/// \return \c true si le noeud a �t� trouv�.
	///
	Boolean GetRandomNode(
				TRandomGenerator& inRandomGenerator,
				const TGPType& inType,
				TGPNodeRef* outNode,
				TGPNodeRef* outParentNode,
				KUInt32* outSiblingIndex );
	
	///
	/// Conversion sous forme de texte (au format LISP).
	///
	/// \param	outStream	flux de sortie.
	/// \throws runtime_error si l'arbre n'est pas complet.
	///
	void				ToStreamAsLisp( std::ostream& outStream );

	///
	/// Conversion sous forme de graphe (au format dot).
	///
	/// \param	outStream	flux de sortie.
	/// \throws runtime_error si l'arbre n'est pas complet.
	///
	void				ToStreamAsDot( std::ostream& outStream );

	///
	/// R�cup�re le coefficient d'ad�quation pour cet arbre, le calcule si n�cessaire.
	///
	/// \return le coefficent d'ad�quation de l'arbre.
	///
	const TGPFitness*	GetFitness( void ) const;

	///
	/// Calcule le coefficient d'ad�quation pour cet arbre.
	/// Ce coefficient sera ensuite supprim� avec delete.
	///
	/// \return le coefficent d'ad�quation de l'arbre.
	///
	virtual TGPFitness*	ComputeFitness( void ) const = 0;

	///
	/// R�initialise les statistiques d'ad�quation de cet arbre.
	///
	/// Par d�faut supprime l'objet mFitness.
	///
	virtual void		ResetFitnessData( void ) const;

	///
	/// Clone l'arbre.
	/// Les sous-classes doivent d�river cette m�thode et appeler leur
	/// constructeur par copie.
	/// Le constructeur par copie de TGPTree clone la racine.
	///
	/// \return une copie de cet arbre.
	///
	virtual TGPTree*	Clone( void ) const = 0;

	///
	/// Vide le cache d'�valuation.
	///
	void				ClearCache( void ) const;

private:
	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPTree& operator = ( const TGPTree& inCopy );

	/// \name Variables
	TGPNodeRef				mRoot;				///< Racine de l'arbre
	mutable TGPFitness*		mFitness;			///< Coefficient d'ad�quation.
};

namespace std
{

///
/// Comparateur pour la classe TGPTree.
/// Utilise le coefficient d'ad�quation.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
template<>
struct less<TGPTree*>
{
	///
	/// Comparaison des noeuds.
	///
	/// \param x	premier noeud � comparer
	/// \param y	deuxi�me noeud � comparer
	/// \return \c true si x < y
	///
	bool operator()(const TGPTree* x, const TGPTree* y);
};

}

#endif
		// _TGPTree_H

// ============================================================ //
// If Machiavelli were a hacker, he'd have worked for the CSSG. //
//                 -- Phil Lapsley                              //
// ============================================================ //
