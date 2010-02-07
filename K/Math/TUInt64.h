// ==============================
// Fichier:			TUInt64.h
// Projet:			K
// 
// Créé le:			10/9/2003
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
// The Original Code is TUInt64.h.
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
// $Id: TUInt64.h,v 1.6 2005/05/26 06:52:04 pguyot Exp $
// ===========

#ifndef _TUINT64_H
#define _TUINT64_H

#include <K/Defines/KDefinitions.h>
#include <K/Math/TInt64.h>

///
/// Classe pour un entier non signé sur 64 bits.
/// Cette classe permet d'avoir UInt64 sur les plateformes n'ayant
/// pas \c unsigned \c long \c long mais gérant du 32 bits.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.6 $
///
/// \test	aucun test défini.
///
class TUInt64
	:
		public TInt64
{
public:
	/// La classe UTInt64 gère le pont entre KUInt64 natif et TUInt64.
	friend class UTInt64;

	///
	/// Constructeur à partir de deux entiers de 32 bits.
	///
	/// \param inHi		valeur de poids fort
	/// \param inLo		valeur de poids faible
	///
	inline TUInt64( const KUInt32 inHi, const KUInt32 inLo )
		:
			TInt64( inHi, inLo )
		{
		}
	
	///
	/// Constructeur à partir d'un entier de 32 bits.
	///
	/// \param inLo		valeur de poids faible
	///
	inline TUInt64( const KUInt32 inLo = 0 )
		:
			TInt64( 0, inLo )
		{
		}

	///
	/// Opérateur d'assignation à partir d'un entier de 32 bits.
	///
	/// \param inLo		valeur (poids faible)
	/// \return \c this
	///
	inline TUInt64& operator = ( const KUInt32 inLo )
		{
			mHi = 0;
			mLo = inLo;
			return *this;
		}
	
	///
	/// Opération d'incrément (32 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TUInt64& operator += ( const KUInt32 inArgument );
	
	///
	/// Opération d'incrément (64 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TUInt64& operator += ( const TUInt64& inArgument );
	
	///
	/// Opération de décrément (32 bits)
	///
	/// \param inArgument	valeur à retrancher.
	/// \return \c this
	///
	TUInt64& operator -= ( const KUInt32 inArgument );

	///
	/// Opération de décrément (64 bits)
	///
	/// \param inArgument	valeur à retrancher.
	/// \return \c this
	///
	TUInt64& operator -= ( const TUInt64& inArgument );

	///
	/// Opération de division (32 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TUInt64& operator /= ( const KUInt32 inArgument );
	
	///
	/// Opération de division (64 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TUInt64& operator /= ( const TUInt64& inArgument );

	///
	/// Opération de reste par division euclidienne (32 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TUInt64& operator %= ( const KUInt32 inArgument );

	///
	/// Opération de reste par division euclidienne (64 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TUInt64& operator %= ( const TUInt64& inArgument );

	///
	/// Opération de multiplication (32 bits)
	///
	/// \param inArgument	facteur.
	/// \return \c this
	///
	TUInt64& operator *= ( const KUInt32 inArgument );

	///
	/// Opération de multiplication (64 bits)
	///
	/// \param inArgument	facteur.
	/// \return \c this
	///
	TUInt64& operator *= ( const TUInt64& inArgument );

	///
	/// Décalage gauche.
	///
	/// \param inArgument	nombre de décalages.
	/// \return \c this
	///
	TUInt64& operator <<= ( const int inArgument );

	///
	/// Décalage droit.
	///
	/// \param inArgument	nombre de décalages.
	/// \return \c this
	///
	TUInt64& operator >>= ( const int inArgument );

	///
	/// Et logique (32 bits)
	///
	/// \param inArgument	masque.
	/// \return \c this
	///
	TUInt64& operator &= ( const KUInt32 inArgument )
		{
			(void) (TInt64::operator &= ( inArgument ));
			return *this;
		}

	///
	/// Et logique (64 bits)
	///
	/// \param inArgument	masque.
	/// \return \c this
	///
	TUInt64& operator &= ( const TInt64& inArgument )
		{
			(void) (TInt64::operator &= ( inArgument ));
			return *this;
		}

	///
	/// Ou logique (32 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TUInt64& operator |= ( const KUInt32 inArgument )
		{
			(void) (TInt64::operator |= ( inArgument ));
			return *this;
		}

	///
	/// Ou logique (64 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TUInt64& operator |= ( const TInt64& inArgument )
		{
			(void) (TInt64::operator |= ( inArgument ));
			return *this;
		}

	///
	/// Ou logique exclusif (32 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TUInt64& operator ^= ( const KUInt32 inArgument )
		{
			(void) (TInt64::operator ^= ( inArgument ));
			return *this;
		}

	///
	/// Ou logique exclusif (64 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TUInt64& operator ^= ( const TInt64& inArgument )
		{
			(void) (TInt64::operator ^= ( inArgument ));
			return *this;
		}

	///
	/// Négation
	///
	/// \return la négation, bit à bit, de \c this
	///
	TUInt64 operator ~ ( void ) const;

	///
	/// Conversion (avec perte) en 32 bits.
	///
	/// \return les 32 bits de poids faible.
	///
#if 0
	inline operator KUInt32 ( void ) const
		{
			return mLo;
		}
#endif

	// Opérateurs binaires.
	friend bool operator == (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend bool operator == (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator != (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend bool operator != (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator <= (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend bool operator <= (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator <= (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator >= (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend bool operator >= (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator >= (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator < (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend bool operator < (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator < (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator > (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend bool operator > (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo );
	friend bool operator > (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );
	friend TUInt64 operator / (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend TUInt64 operator / (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );
	friend TUInt64 operator % (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );
	friend TUInt64 operator % (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );

private:
	///
	/// Division euclidienne avec reste.
	///
	/// \param outQuotient	quotient
	/// \param outRemainder	reste
	/// \param inDividend	dividende
	/// \param inDivisor	diviseur
	///
	static void EuclideanDivision(
		TUInt64& outQuotient,
		TUInt64& outRemainder,
		const TUInt64& inDividend,
		const TUInt64& inDivisor );

	///
	/// Addition avec retenue.
	///
	/// \param inArgument	entier à ajouter
	/// \return \c true si la somme dépasse 2^64
	///
	bool		Add( const TUInt64& inArgument );
};

///
/// Addition (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne + inArgTwo
///
inline
TUInt64 operator + (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TUInt64( inArgOne ) += inArgTwo;
}

///
/// Addition (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne + inArgTwo
///
inline
TUInt64 operator + (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo )
{
	// L'addition est commutative.
	return TUInt64( inArgTwo ) += inArgOne;
}

///
/// Addition (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne + inArgTwo
///
inline
TUInt64 operator + (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgOne ) += inArgTwo;
}

///
/// Soustraction (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne - inArgTwo
///
inline
TUInt64 operator - (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TUInt64( inArgOne ) -= inArgTwo;
}

///
/// Soustraction (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne - inArgTwo
///
inline
TUInt64 operator - (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo )
{
	// La soustraction n'est pas commutative.
	return TUInt64( inArgOne ) -= inArgTwo;
}

///
/// Soustraction (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne - inArgTwo
///
inline
TUInt64 operator - (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgOne ) -= inArgTwo;
}

///
/// Égalité (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne == inArgTwo
///
bool operator == ( const TUInt64& inArgOne, const KUInt32 inArgTwo );

///
/// Égalité (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne == inArgTwo
///
bool operator == ( const KUInt32 inArgOne, const TUInt64& inArgTwo );

///
/// Différence (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne != inArgTwo
///
bool operator != ( const TUInt64& inArgOne, const KUInt32 inArgTwo );

///
/// Différence (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne != inArgTwo
///
bool operator != ( const KUInt32 inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne <= inArgTwo
///
bool operator <= ( const TUInt64& inArgOne, const KUInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne <= inArgTwo
///
bool operator <= ( const KUInt32 inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne <= inArgTwo
///
bool operator <= ( const TUInt64& inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne >= inArgTwo
///
bool operator >= ( const TUInt64& inArgOne, const KUInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne >= inArgTwo
///
bool operator >= ( const KUInt32 inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne >= inArgTwo
///
bool operator >= ( const TUInt64& inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne < inArgTwo
///
bool operator < ( const TUInt64& inArgOne, const KUInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne < inArgTwo
///
bool operator < ( const KUInt32 inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne < inArgTwo
///
bool operator < ( const TUInt64& inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne > inArgTwo
///
bool operator > ( const TUInt64& inArgOne, const KUInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne > inArgTwo
///
bool operator > ( const KUInt32 inArgOne, const TUInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne > inArgTwo
///
bool operator > ( const TUInt64& inArgOne, const TUInt64& inArgTwo );

///
/// Division entière (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne / inArgTwo
///
TUInt64 operator / (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );

///
/// Division entière (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne / inArgTwo
///
TUInt64 operator / (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );

///
/// Modulo (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne % inArgTwo
///
TUInt64 operator % (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo );

///
/// Modulo (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne % inArgTwo
///
TUInt64 operator % (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo );

///
/// Multiplication (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne * inArgTwo
///
inline
TUInt64 operator * (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TUInt64( inArgOne ) *= inArgTwo;
}

///
/// Multiplication (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne * inArgTwo
///
inline
TUInt64 operator * (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo )
{
	// La multiplication est commutative.
	return TUInt64( inArgTwo ) *= inArgOne;
}

///
/// Multiplication (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne * inArgTwo
///
inline
TUInt64 operator * (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgOne ) *= inArgTwo;
}

///
/// Décalage arithmétique gauche.
///
/// \param inArgument	premier argument
/// \param inShiftCount	décalage
/// \return \c inArgument << inShiftCount
///
inline
TUInt64 operator << (
		const TUInt64& inArgument,
		const int inShiftCount )
{
	return TUInt64( inArgument ) <<= inShiftCount;
}

///
/// Décalage arithmétique droit.
///
/// \param inArgument	premier argument
/// \param inShiftCount	décalage
/// \return \c inArgument << inShiftCount
///
inline
TUInt64 operator >> (
		const TUInt64& inArgument,
		const int inShiftCount )
{
	return TUInt64( inArgument ) >>= inShiftCount;
}

///
/// Et logique (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne & inArgTwo
///
inline
TUInt64 operator & (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TUInt64( inArgOne ) &= inArgTwo;
}

///
/// Et logique (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne & inArgTwo
///
inline
TUInt64 operator & (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgTwo ) &= inArgOne;
}

///
/// Et logique (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne & inArgTwo
///
inline
TUInt64 operator & (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgOne ) &= inArgTwo;
}

///
/// Ou logique (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne | inArgTwo
///
inline
TUInt64 operator | (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TUInt64( inArgOne ) |= inArgTwo;
}

///
/// Ou logique (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne | inArgTwo
///
inline
TUInt64 operator | (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgTwo ) |= inArgOne;
}

///
/// Ou logique (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne | inArgTwo
///
inline
TUInt64 operator | (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgOne ) |= inArgTwo;
}

///
/// Ou logique exclusif (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne ^ inArgTwo
///
inline
TUInt64 operator ^ (
		const TUInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TUInt64( inArgOne ) ^= inArgTwo;
}

///
/// Ou logique exclusif (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne ^ inArgTwo
///
inline
TUInt64 operator ^ (
		const KUInt32 inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgTwo ) ^= inArgOne;
}

///
/// Ou logique exclusif (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne ^ inArgTwo
///
inline
TUInt64 operator ^ (
		const TUInt64& inArgOne,
		const TUInt64& inArgTwo )
{
	return TUInt64( inArgOne ) ^= inArgTwo;
}

#endif
		// _TUINT64_H

// ============================================================================ //
// There has also been some work to allow the interesting use of macro names.   //
// For example, if you wanted all of your "creat()" calls to include read       //
// permissions for everyone, you could say                                      //
//                                                                              //
//         #define creat(file, mode)       creat(file, mode | 0444)             //
//                                                                              //
//         I would recommend against this kind of thing in general, since it    //
// hides the changed semantics of "creat()" in a macro, potentially far away    //
// from its uses.                                                               //
//         To allow this use of macros, the preprocessor uses a process that    //
// is worth describing, if for no other reason than that we get to use one of   //
// the more amusing terms introduced into the C lexicon.  While a macro is      //
// being expanded, it is temporarily undefined, and any recurrence of the macro //
// name is "painted blue" -- I kid you not, this is the official terminology    //
// -- so that in future scans of the text the macro will not be expanded        //
// recursively.  (I do not know why the color blue was chosen; I'm sure it      //
// was the result of a long debate, spread over several meetings.)              //
//                 -- From Ken Arnold's "C Advisor" column in Unix Review       //
// ============================================================================ //
