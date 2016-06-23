// ==============================
// Fichier:			TGPCreationAlgorithm.h
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
// The Original Code is TGPCreationAlgorithm.h.
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
// $Id: TGPCreationAlgorithm.h,v 1.7 2005/06/08 02:32:48 pguyot Exp $
// ===========

#ifndef _TGPCREATIONALGORITHM_H
#define _TGPCREATIONALGORITHM_H

#include <K/Defines/KDefinitions.h>

// K GP
#include <K/GP/Language/TGPLanguage.h>
#include <K/GP/Tree/Nodes/TGPNode.h>
#include <K/GP/Data/TGPType.h>

///
/// Classe pour un algorithme de g�n�ration d'arbres.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPCreationAlgorithm
{
public:
	///
	/// Constructeur par d�faut.
	///
	TGPCreationAlgorithm( void );

	///
	/// Destructor.
	///
	virtual ~TGPCreationAlgorithm( void ) {};

	///
	/// Construit un arbre pour le langage donn� d'un type donn�.
	/// Appelle CreateRoot puis ensuite termine l'arbre (appelle Invalidate
	/// sur la racine).
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire � utiliser.
	/// \param inLanguage			langage � utiliser pour cr�er cet arbre.
	/// \param inType				type de l'arbre � cr�er.
	/// \return la racine d'un nouvel arbre du type donn�.
	///
	TGPNodeRef	CreateTree(
					TRandomGenerator& inRandomGenerator,
					const TGPLanguage& inLanguage,
					const TGPType& inType ) const;

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
					size_t inMaxDepth ) const = 0;

private:
	///
	/// Construit un arbre pour le langage donn� d'un type donn�.
	/// Retourne la racine de l'arbre.
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire � utiliser.
	/// \param inLanguage	langage � utiliser pour cr�er cet arbre.
	/// \param inType		type de l'arbre � cr�er.
	/// \return un nouvel arbre du type donn�.
	///
	virtual TGPNodeRef	CreateRoot(
					TRandomGenerator& inRandomGenerator,
					const TGPLanguage& inLanguage,
					const TGPType& inType ) const = 0;

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPCreationAlgorithm( const TGPCreationAlgorithm& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPCreationAlgorithm& operator = ( const TGPCreationAlgorithm& inCopy );
};

#endif
		// _TGPCREATIONALGORITHM_H

// ========================================================================== //
// OS/2 Beer: Comes in a 32-oz can. Does allow you to drink several DOS       //
// Beers simultaneously. Allows you to drink Windows 3.1 Beer simultaneously  //
// too, but somewhat slower. Advertises that its cans won't explode when you  //
// open them, even if you shake them up. You never really see anyone          //
// drinking OS/2 Beer, but the manufacturer (International Beer               //
// Manufacturing) claims that 9 million six-packs have been sold.             //
// ========================================================================== //
