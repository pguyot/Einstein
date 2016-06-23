// ==============================
// Fichier:			TGPTruncationSelection.h
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
// The Original Code is TGPTruncationSelection.h.
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
// $Id: TGPTruncationSelection.h,v 1.4 2004/11/24 18:23:17 pguyot Exp $
// ===========

#ifndef _TGPTRUNCATIONSELECTION_H
#define _TGPTRUNCATIONSELECTION_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Selection/TGPSortSelection.h>

///
/// Classe pour une m�thode de s�lection par troncature.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
/// \test	aucun test d�fini.
///
class TGPTruncationSelection
	:
		public TGPSortSelection
{
public:
	///
	/// Constructeur � partir du pourcentage d'individus � garder.
	///
	/// \param inTruncationRatio	ratio d'individus � garder
	///
	TGPTruncationSelection( double inTruncationRatio );

	///
	/// M�thode appel�e pour choisir un individu. L'indice varie de 0 � n-1
	/// avec n-1 le dernier individu choisi dans la population.
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire.
	/// \param inIndexIndividual	indice dans le processus de s�lection.
	/// \return un individu de la population.
	///
	virtual TGPTree* SelectIndividual(
							TRandomGenerator& inRandomGenerator,
							size_t inIndexIndividual );

private:
	/// \name Variables
	double					mTruncationRatio;
};

#endif
		// _TGPTRUNCATIONSELECTION_H

// ======================================================================= //
// Unix gives you just enough rope to hang yourself -- and then a couple   //
// of more feet, just to be sure.                                          //
//                 -- Eric Allman                                          //
//                                                                         //
// ... We make rope.                                                       //
//                 -- Rob Gingell on Sun Microsystem's new virtual memory. //
// ======================================================================= //
