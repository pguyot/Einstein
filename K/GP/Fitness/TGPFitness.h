// ==============================
// Fichier:			TGPFitness.h
// Projet:			K
// 
// Créé le:			4/9/2003
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
// The Original Code is TGPFitness.h.
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
// $Id: TGPFitness.h,v 1.5 2005/06/08 02:32:49 pguyot Exp $
// ===========

#ifndef _TGPFITNESS_H
#define _TGPFITNESS_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <iostream>

class TRandomGenerator;

///
/// Classe de base pour le coefficient d'adéquation.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.5 $
///
/// \test	aucun test défini.
///
class TGPFitness
{
public:
	///
	/// Destructor.
	///
	virtual ~TGPFitness( void ) {};

	///
	/// Comparateur avec un autre coefficent d'adéquation.
	///
	/// \param inAlterFitness	autre coefficient avec lequel comparer \c this
	/// \return \c true si \c this dénote une moins bonne adéquation que \c
	/// 		inAlterFitness, \c false sinon.
	///
	virtual bool operator < ( const TGPFitness& inAlterFitness ) const = 0;

	///
	/// Affiche le coefficient d'adéquation sur un flux de sortie.
	///
	/// \param inStream	flux de sortie pour l'affichage.
	///
	virtual void PrintToStream( std::ostream& inStream ) const = 0;

	///
	/// Clone le coefficient d'adéquation.
	/// Les sous-classes doivent dériver cette méthode et appeler leur
	/// constructeur par copie.
	///
	/// \return une copie (allouée avec \c new) de \c this
	///
	virtual TGPFitness* Clone( void  ) const = 0;

	///
	/// Additionne les valeurs d'adéquation, afin de faire une moyenne.
	///
	/// \param inAlterFitness	adéquation à ajouter à \c this.
	///
	virtual TGPFitness& operator += ( const TGPFitness& inAlterFitness ) = 0;

	///
	/// Divise les valeurs d'adéquation par un entier, afin de faire une moyenne.
	///
	/// \param inDivider	coefficient pour la division
	///
	virtual TGPFitness& operator /= ( KUInt32 inDivider ) = 0;

	///
	/// Récupère une valeur aléatoire entre deux valeurs.
	/// Cette opération est utilisée pour la sélection par probabilité
	/// et peut ne pas être gérée (auquel cas il faut lancer une exception).
	///
	/// \param	inRandomGenerator	générateur aléatoire pour calculer la valeur.
	/// \param	inAlterBound		autre valeur.
	/// \return une valeur (allouée avec \c new) aléatoire entre this
	///			et \c inAlterBound.
	///
	virtual TGPFitness* Random(
							TRandomGenerator& inRandomGenerator,
							const TGPFitness& inAlterBound ) const = 0;

protected:
	///
	/// Constructeur par défaut.
	///
	TGPFitness( void );
};

///
/// Opérateur pour afficher le coefficient d'adéquation.
///
/// \param inStream		flux de sortie pour l'affichage.
/// \param inFitness	coefficient d'adéquation.
///
inline std::ostream& operator << ( std::ostream& inStream, const TGPFitness& inFitness )
{
	inFitness.PrintToStream( inStream );
	return inStream;
}

#endif
		// _TGPFITNESS_H

// ================================================= //
// [It is] best to confuse only one issue at a time. //
//                 -- K&R                            //
// ================================================= //
