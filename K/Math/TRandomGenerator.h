// ==============================
// Fichier:			TRandomGenerator.h
// Projet:			K
// 
// Créé le:			19/11/2003
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
// The Original Code is TRandomGenerator.h.
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
// $Id: TRandomGenerator.h,v 1.3 2005/02/14 20:55:05 pguyot Exp $
// ===========

#ifndef _TRANDOMGENERATOR_H
#define _TRANDOMGENERATOR_H

#include <K/Defines/KDefinitions.h>

///
/// Classe pour un générateur aléatoire.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test défini.
///
class TRandomGenerator
{
public:
	///
	/// Constructeur par défaut.
	///
	TRandomGenerator( void );

	///
	/// Destructeur.
	///
	virtual ~TRandomGenerator( void );

	///
	/// Opérateur ISO C++
	/// (cette classe peut être utilisée avec les patrons de l'ISO C++, pour peu que
	/// difference_type soit compatible avec KUInt32)
	///
	inline KUInt32 operator () ( KUInt32 inBound )
		{
			return DistributionFast( inBound );
		}

	///
	/// Génère un nombre aléatoire de 32 bits ([0, 0xFFFFFFFF])
	///
	/// \return un nombre entre 0 et 2^32-1
	///
	virtual KUInt32 Int_32( void ) = 0;

	///
	/// Génère un nombre aléatoire de 31 bits ([0, 0x7FFFFFFF])
	/// Par défaut, appelle la méthode précédente et supprime le bit
	/// de poids faible (en décalant l'ensemble).
	///
	/// \return un nombre entre 0 et 2^31-1
	///
	virtual KUInt32 Int_31( void );

	///
	/// Génère un nombre aléatoire réel dans [0,1] avec une faible précision
	/// (par exemple 32 bits).
	/// Par défaut, appelle la méthode Int_32 et effectue une division.
	///
	/// \return un nombre dans [0,1]
	///
	virtual double Double_1( void );

	///
	/// Génère un nombre aléatoire réel dans [0,1[ avec une faible précision
	/// (par exemple 32 bits).
	/// Par défaut, appelle la méthode Int_32 et effectue une division.
	///
	/// \return un nombre dans [0,1[
	///
	virtual double Double_2( void );

	///
	/// Génère un nombre aléatoire réel dans ]0,1[ avec une faible précision
	/// (par exemple 32 bits).
	/// Par défaut, appelle la méthode Int_32 et effectue une division.
	///
	/// \return un nombre dans ]0,1[
	///
	virtual double Double_3( void );

	///
	/// Génère un nombre aléatoire réel dans [0,1[ avec une précision de 53
	/// bits (la mantisse des doubles fait 52 bits).
	/// Par défaut, appelle la méthode Int_32 deux fois.
	///
	/// \return un nombre dans [0,1[
	///
	virtual double Double_53( void );
	
	///
	/// Génère un nombre aléatoire entier entre 0 et N-1.
	/// Ceci est la méthode rapide avec erreur d'arrondi.
	///
	/// \param inBound	valeur jamais atteinte.
	/// \return un nombre de [0, inBound - 1]
	///
	virtual KUInt32 DistributionFast( KUInt32 inBound );
	
	///
	/// Génère un nombre aléatoire entier entre 0 et N-1.
	/// Ceci est la méthode lente sans erreur d'arrondi.
	///
	/// \param inBound	valeur jamais atteinte.
	/// \return un nombre de [0, inBound - 1]
	///
	virtual KUInt32 DistributionAccurate( KUInt32 inBound );
	
	///
	/// Génère un nombre aléatoire réel dans [x,y] avec une faible précision
	/// (par exemple 32 bits). On peut avoir inFirstBound < inSecondBound ou
	/// l'inverse.
	/// Par défaut, appelle la méthode Double_1 et effectue une règle de trois.
	///
	/// \param inFirstBound	premier argument
	/// \param inSecondBound second argument
	/// \return un nombre dans [inFirstBound,inSecondBound]
	///
	virtual double DistributionDouble_1(
						double inFirstBound,
						double inSecondBound );

	///
	/// Génère un nombre aléatoire booléen, \c true ou \c false.
	///
	/// \return \c true ou \c false.
	///
	virtual Boolean Coin( void );
};

#endif
		// _TRANDOMGENERATOR_H

// ===================================================================== //
// The New Testament offers the basis for modern computer coding theory, //
// in the form of an affirmation of the binary number system.            //
//                                                                       //
//         But let your communication be Yea, yea; nay, nay:             //
//         for whatsoever is more than these cometh of evil.             //
//                 -- Matthew 5:37                                       //
// ===================================================================== //
