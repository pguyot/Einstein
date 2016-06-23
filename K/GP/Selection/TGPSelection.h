// ==============================
// Fichier:			TGPSelection.h
// Projet:			K
// 
// Cr�� le:			6/11/2003
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
// The Original Code is TGPSelection.h.
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
// $Id: TGPSelection.h,v 1.4 2004/11/23 15:11:30 paul Exp $
// ===========

#ifndef _TGPSELECTION_H
#define _TGPSELECTION_H

#include <K/Defines/KDefinitions.h>

// ANSI C
#include <stdlib.h>

// ISO C++
#include <vector>

// Pr�-d�clarations
class TGPTree;
class TRandomGenerator;

///
/// Classe de base pour la fonction de s�lection d'individus lors du calcul de
/// la prochaine g�n�ration.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
/// \test	aucun test d�fini.
///
class TGPSelection
{
public:
	///
	/// Constructeur par d�faut.
	///
	TGPSelection( void );

	///
	/// Destructeur.
	///
	virtual ~TGPSelection( void );

	///
	/// M�thode appel�e au d�but du processus de s�lection (permet de r�initialiser
	/// des valeurs stock�es dans l'objet si n�cessaire).
	/// Par d�faut, stocke la r�f�rence sur la population.
	///
	/// \param inPopulation		r�f�rence sur la population
	///
	virtual void NewGeneration( const std::vector<TGPTree*>& inPopulation );

	///
	/// M�thode appel�e pour choisir un individu. L'indice varie de 0 � n-1
	/// avec n-1 le dernier individu choisi dans la population.
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire pour choisir l'individu.
	/// \param inIndexIndividual	indice dans le processus de s�lection.
	/// \return un individu de la population.
	///
	virtual TGPTree* SelectIndividual(
						TRandomGenerator& inRandomGenerator,
						size_t inIndexIndividual ) = 0;

protected:
	///
	/// Accesseur sur la population.
	///
	/// \return une r�f�rence sur la population (fournie par la soupe lors
	/// 		du dernier appel � NewGeneration).
	///
	inline const std::vector<TGPTree*>&	GetPopulation( void ) const
		{
			return *mPopulation;
		}

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPSelection( const TGPSelection& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPSelection& operator = ( const TGPSelection& inCopy );

	/// \name Variables
	const std::vector<TGPTree*>*	mPopulation;	///< R�f�rence sur la
													///< population.
};

#endif
		// _TGPSELECTION_H

// =========================================================================== //
// Disraeli was pretty close: actually, there are Lies, Damn lies, Statistics, //
// Benchmarks, and Delivery dates.                                             //
// =========================================================================== //
