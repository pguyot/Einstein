// ==============================
// Fichier:			TGPDoubleFitness.h
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
// The Original Code is TGPDoubleFitness.h.
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
// $Id: TGPDoubleFitness.h,v 1.4 2005/02/14 20:55:03 pguyot Exp $
// ===========

#ifndef _TGPDOUBLEFITNESS_H
#define _TGPDOUBLEFITNESS_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Fitness/TGPFitness.h>

///
/// Classe pour l'adéquation sous forme d'un réel (avec double précision).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
/// \test	aucun test défini.
///
class TGPDoubleFitness
	:
		public TGPFitness
{
public:
	///
	/// Constructeur à partir d'un réel.
	///
	/// \param inValue	valeur du coefficient d'adéquation
	///
	TGPDoubleFitness( double inValue );

	///
	/// Comparateur avec un autre coefficent d'adéquation.
	///
	/// \param inAlterFitness	autre coefficient avec lequel comparer \c this
	/// \return \c true si \c this dénote une moins bonne adéquation que \c
	/// 		inAlterFitness, \c false sinon.
	///
	virtual bool operator < ( const TGPFitness& inAlterFitness ) const;

	///
	/// Affiche le coefficient d'adéquation sur un flux de sortie.
	///
	/// \param inStream	flux de sortie pour l'affichage.
	///
	virtual void PrintToStream( std::ostream& inStream ) const;

	///
	/// Clone le coefficient d'adéquation.
	/// Les sous-classes doivent dériver cette méthode et appeler leur
	/// constructeur par copie.
	///
	/// \return une copie (allouée avec \c new) de \c this
	///
	virtual TGPFitness* Clone( void  ) const;

	///
	/// Additionne les valeurs d'adéquation, afin de faire une moyenne.
	///
	/// \param inAlterFitness	adéquation à ajouter à \c this.
	///
	virtual TGPFitness& operator += ( const TGPFitness& inAlterFitness );

	///
	/// Divise les valeurs d'adéquation par un entier, afin de faire une moyenne.
	///
	/// \param inDivider	coefficient pour la division
	///
	virtual TGPFitness& operator /= ( KUInt32 inDivider );

	///
	/// Récupère une valeur aléatoire entre deux valeurs.
	///
	/// \param	inRandomGenerator	générateur aléatoire pour calculer la valeur.
	/// \param	inAlterBound		autre valeur.
	/// \return une valeur (allouée avec \c new) aléatoire entre this
	///			et \c inAlterBound.
	///
	virtual TGPFitness* Random(
							TRandomGenerator& inRandomGenerator,
							const TGPFitness& inAlterBound ) const;

private:
	double			mValue;	///< Valeur du coefficient.
};

#endif
		// _TGPDOUBLEFITNESS_H

// ========================================================================= //
// ... Jesus cried with a loud voice: Lazarus, come forth; the bug hath been //
// found and thy program runneth.  And he that was dead came forth...        //
//                 -- John 11:43-44 [version 2.0?]                           //
// ========================================================================= //
