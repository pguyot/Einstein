// ==============================
// Fichier:			TGPCAKoza.h
// Projet:			K
//
// Cr�� le:			25/2/2003
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

// Pr�-d�clarations.
class TGPTree;
class TGPNodeRef;

///
/// Classe pour l'algorithme de g�n�ration d'arbre Ramped-Half-And-Half
/// d�crit par le sieur Koza.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPCAKoza
	:
		public TGPCreationAlgorithm
{
public:
	///
	/// Constructeur � partir des param�tres de l'algorithme.
	///
	/// \param inDepth				param�tre: profondeur des arbres.
	///
	TGPCAKoza( int inDepth );

	///
	/// Construit un sous-arbre pour le langage donn� d'un type donn�.
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire � utiliser.
	/// \param inLanguage			langage � utiliser pour cr�er cet arbre.
	/// \param inType				type de l'arbre � cr�er.
	/// \param inMaxDepth			profondeur maximale de l'arbre � cr�er.
	/// \return un nouvel arbre du type donn�.
	///
	virtual TGPNodeRef	CreateSubTree(
					TRandomGenerator& inRandomGenerator,
					const TGPLanguage& inLanguage,
					const TGPType& inType,
					size_t inMaxDepth ) const;
private:
	///
	/// Construit un arbre pour le langage donn� d'un type donn�.
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire � utiliser.
	/// \param inLanguage	langage � utiliser pour cr�er cet arbre.
	/// \param inType		type de l'arbre � cr�er.
	/// \return la racine du nouvel arbre du type donn�.
	///
	virtual TGPNodeRef	CreateRoot(
					TRandomGenerator& inRandomGenerator,
					const TGPLanguage& inLanguage,
					const TGPType& inType ) const;

	///
	/// Primitive de l'algorithme Koza.
	/// Effectue, r�cursivement, Grow ou Full suivant le param�tre
	/// \c inDoFull.
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire � utiliser.
	/// \param inLanguage	langage � utiliser pour cr�er le sous-arbre.
	/// \param inType		type du sous arbre � cr�er.
	/// \param inDepth		profondeur de l'arbre � retourner moins un.
	/// \param inDoFull		si on fait l'algorithme full plut�t que Grow.
	/// \return un nouvel arbre du type donn� ou \c nil si la profondeur
	///			n'a pas �t� respect�e.
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
