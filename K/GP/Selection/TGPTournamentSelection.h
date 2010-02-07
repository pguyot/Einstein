// ==============================
// Fichier:			TGPTournamentSelection.h
// Projet:			K
// 
// Créé le:			6/11/2003
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
// The Original Code is TGPTournamentSelection.h.
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
// $Id: TGPTournamentSelection.h,v 1.2 2004/11/23 15:11:30 paul Exp $
// ===========

#ifndef _TGPTOURNAMENTSELECTION_H
#define _TGPTOURNAMENTSELECTION_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Selection/TGPSelection.h>

///
/// Classe pour la fonction de sélection par tournois.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TGPTournamentSelection
	:
		public TGPSelection
{
public:
	///
	/// Constructeur à partir de la taille des tournois.
	///
	TGPTournamentSelection( KUInt32 inTournamentSize );

	///
	/// Méthode appelée au début du processus de sélection.
	/// Note la taille de la population.
	///
	/// \param inPopulation		référence sur la population
	///
	virtual void NewGeneration( const std::vector<TGPTree*>& inPopulation );

	///
	/// Méthode appelée pour choisir un individu. L'indice varie de 0 à n-1
	/// avec n-1 le dernier individu choisi dans la population.
	///
	/// \param inRandomGenerator	générateur aléatoire pour choisir l'individu.
	/// \param inIndexIndividual	indice dans le processus de sélection.
	/// \return un individu de la population.
	///
	virtual TGPTree* SelectIndividual(
						TRandomGenerator& inRandomGenerator,
						size_t inIndexIndividual );

private:
	/// \name Variables
	KUInt32				mPopulationSize;	///< Taille de la population.
	KUInt32				mTournamentSize;	///< Taille du tournois.
};

#endif
		// _TGPTOURNAMENTSELECTION_H

// ========================================================================== //
// So you see Antonio, why worry about one little core dump, eh?  In reality  //
// all core dumps happen at the same instant, so the core dump you will have  //
// tomorrow, why, it already happened.  You see, it's just a little universal //
// recursive joke which threads our lives through the infinite potential of   //
// the instant.  So go to sleep, Antonio, your thread could break any moment  //
// and cast you out of the safe security of the instant into the dark void of //
// eternity, the anti-time.  So go to sleep...                                //
// ========================================================================== //
