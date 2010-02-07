// ==============================
// Fichier:			TGPCAKoza.h
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
// The Original Code is TGPCAKoza.h.
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
// $Id: TGPCAKoza.h,v 1.7 2004/11/23 15:11:29 paul Exp $
// ===========

#ifndef _TGPCAKOZA_H
#define _TGPCAKOZA_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Creation/TGPCreationAlgorithm.h>

// K
#include <K/GP/Language/TGPLanguage.h>
#include <K/GP/Data/TGPType.h>

// Pré-déclarations.
class TGPTree;
class TGPNodeRef;

///
/// Classe pour l'algorithme de génération d'arbre Ramped-Half-And-Half
/// décrit par le sieur Koza.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPCAKoza
	:
		public TGPCreationAlgorithm
{
public:
	///
	/// Constructeur à partir des paramètres de l'algorithme.
	///
	/// \param inDepth				paramètre: profondeur des arbres.
	///
	TGPCAKoza( int inDepth );

	///
	/// Construit un sous-arbre pour le langage donné d'un type donné.
	///
	/// \param inRandomGenerator	générateur aléatoire à utiliser.
	/// \param inLanguage			langage à utiliser pour créer cet arbre.
	/// \param inType				type de l'arbre à créer.
	/// \param inMaxDepth			profondeur maximale de l'arbre à créer.
	/// \return un nouvel arbre du type donné.
	///
	virtual TGPNodeRef	CreateSubTree(
					TRandomGenerator& inRandomGenerator,
					const TGPLanguage& inLanguage,
					const TGPType& inType,
					size_t inMaxDepth ) const;
private:
	///
	/// Construit un arbre pour le langage donné d'un type donné.
	///
	/// \param inRandomGenerator	générateur aléatoire à utiliser.
	/// \param inLanguage	langage à utiliser pour créer cet arbre.
	/// \param inType		type de l'arbre à créer.
	/// \return la racine du nouvel arbre du type donné.
	///
	virtual TGPNodeRef	CreateRoot(
					TRandomGenerator& inRandomGenerator,
					const TGPLanguage& inLanguage,
					const TGPType& inType ) const;

	///
	/// Primitive de l'algorithme Koza.
	/// Effectue, récursivement, Grow ou Full suivant le paramètre
	/// \c inDoFull.
	///
	/// \param inRandomGenerator	générateur aléatoire à utiliser.
	/// \param inLanguage	langage à utiliser pour créer le sous-arbre.
	/// \param inType		type du sous arbre à créer.
	/// \param inDepth		profondeur de l'arbre à retourner moins un.
	/// \param inDoFull		si on fait l'algorithme full plutôt que Grow.
	/// \return un nouvel arbre du type donné ou \c nil si la profondeur
	///			n'a pas été respectée.
	///
	static TGPNodeRef	Primitive(
					TRandomGenerator& inRandomGenerator,
					const TGPLanguage& inLanguage,
					const TGPType& inType,
					int inDepth,
					Boolean inDoFull );
	
	/// \name Variables
	int			mDepth;		///< Profondeur des arbres.
};

#endif
		// _TGPCAKOZA_H

// ========================================================================= //
// Simulations are like miniskirts, they show a lot and hide the essentials. //
//                 -- Hubert Kirrman                                         //
// ========================================================================= //
