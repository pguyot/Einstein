// ==============================
// Fichier:			TMT19937RandomGenerator.h
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
// The Original Code is TMT19937RandomGenerator.h.
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
// $Id: TMT19937RandomGenerator.h,v 1.2 2004/11/23 15:11:31 paul Exp $
// ===========

#ifndef _TMT19937RANDOMGENERATOR_H
#define _TMT19937RANDOMGENERATOR_H

#include <K/Defines/KDefinitions.h>
#include <K/Math/TRandomGenerator.h>

///
/// Classe pour le générateur aléatoire Mersenne Twister MT19937.
/// Ce générateur a été conçu par Takuji Nishimura et Makoto Matsumoto.
/// http://www.math.keio.ac.jp/~matumoto/emt.html
/// M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-dimensionally
/// equidistributed uniform pseudorandom number generator", ACM Trans. on
/// Modeling and Computer Simulation Vol. 8, No. 1, January pp.3-30 1998.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TMT19937RandomGenerator
	:
		public TRandomGenerator
{
public:
	///
	/// Constructeur à partir d'une graine.
	///
	/// \param inSeed	graine pour initialiser le générateur.
	///
	TMT19937RandomGenerator( KUInt32 inSeed = 5489UL );
	
	///
	/// Constructeur à partir d'un tableau.
	///
	/// \param inInitKey	tableau pour initialiser le générateur.
	/// \param inKeyLength	nombre d'éléments dans le tableau.
	///
	TMT19937RandomGenerator( KUInt32* inInitKey, int inKeyLength );

	///
	/// Génère un nombre aléatoire de 32 bits ([0, 0xFFFFFFFF])
	///
	/// \return un nombre entre 0 et 2^32-1
	///
	virtual KUInt32 Int_32( void );

private:
	///
	/// Initialisation à partir d'une graine.
	///
	/// \param inSeed	graine pour l'initialisation.
	///
	void	InitWithSeed( KUInt32 inSeed );
	
	///
	/// Calcul des états suivants.
	///
	void	ComputeNextStates( void );

	// Period parameters
	#define N 624
	#define M 397
	#define MATRIX_A 0x9908b0dfUL	// constant vector a
	#define UMASK 0x80000000UL		// most significant w-r bits
	#define LMASK 0x7fffffffUL		// least significant r bits
	#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
	#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

	/// \name Variables
	KUInt32			mState[N];	///< the array for the state vector
	KUInt32*		mNextState;	///< Pointeur sur l'état courant.
	KUInt32*		mLastState;	///< Pointeur sur le dernier état.
};

#endif
		// _TMT19937RANDOMGENERATOR_H

// =============================== //
// System going down in 5 minutes. //
// =============================== //
