// ==============================
// Fichier:			TGPDoubleFitness.h
// Projet:			K
// 
// Cr�� le:			4/9/2003
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
/// Classe pour l'ad�quation sous forme d'un r�el (avec double pr�cision).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
/// \test	aucun test d�fini.
///
class TGPDoubleFitness
	:
		public TGPFitness
{
public:
	///
	/// Constructeur � partir d'un r�el.
	///
	/// \param inValue	valeur du coefficient d'ad�quation
	///
	TGPDoubleFitness( double inValue );

	///
	/// Comparateur avec un autre coefficent d'ad�quation.
	///
	/// \param inAlterFitness	autre coefficient avec lequel comparer \c this
	/// \return \c true si \c this d�note une moins bonne ad�quation que \c
	/// 		inAlterFitness, \c false sinon.
	///
	virtual bool operator < ( const TGPFitness& inAlterFitness ) const;

	///
	/// Affiche le coefficient d'ad�quation sur un flux de sortie.
	///
	/// \param inStream	flux de sortie pour l'affichage.
	///
	virtual void PrintToStream( std::ostream& inStream ) const;

	///
	/// Clone le coefficient d'ad�quation.
	/// Les sous-classes doivent d�river cette m�thode et appeler leur
	/// constructeur par copie.
	///
	/// \return une copie (allou�e avec \c new) de \c this
	///
	virtual TGPFitness* Clone( void  ) const;

	///
	/// Additionne les valeurs d'ad�quation, afin de faire une moyenne.
	///
	/// \param inAlterFitness	ad�quation � ajouter � \c this.
	///
	virtual TGPFitness& operator += ( const TGPFitness& inAlterFitness );

	///
	/// Divise les valeurs d'ad�quation par un entier, afin de faire une moyenne.
	///
	/// \param inDivider	coefficient pour la division
	///
	virtual TGPFitness& operator /= ( KUInt32 inDivider );

	///
	/// R�cup�re une valeur al�atoire entre deux valeurs.
	///
	/// \param	inRandomGenerator	g�n�rateur al�atoire pour calculer la valeur.
	/// \param	inAlterBound		autre valeur.
	/// \return une valeur (allou�e avec \c new) al�atoire entre this
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
