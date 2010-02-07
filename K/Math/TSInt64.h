// ==============================
// Fichier:			TSInt64.h
// Projet:			K
// 
// Créé le:			21/2/2004
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
// The Original Code is TSInt64.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TSInt64.h,v 1.3 2004/11/23 15:11:31 paul Exp $
// ===========

#ifndef _TSINT64_H
#define _TSINT64_H

#include <K/Defines/KDefinitions.h>
#include <K/Math/TInt64.h>

///
/// Classe pour un entier signé sur 64 bits.
/// Cette classe permet d'avoir SInt64 sur les plateformes n'ayant
/// pas \c signed \c long \c long mais gérant du 32 bits.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test défini.
///
class TSInt64
	:
		public TInt64
{
public:
	/// La classe UUInt64 gère le pont entre KSInt64 natif et TSInt64.
	friend class UTInt64;

	///
	/// Constructeur à partir de deux entiers de 32 bits.
	///
	/// \param inHi		valeur de poids fort
	/// \param inLo		valeur de poids faible
	///
	inline TSInt64( const KSInt32 inHi, const KUInt32 inLo )
		:
			TInt64( (KUInt32) inHi, inLo )
		{
		}
	
	///
	/// Constructeur à partir d'un entier de 32 bits signé.
	///
	/// \param inLo		valeur de poids faible
	///
	inline TSInt64( const KSInt32 inLo )
		:
			TInt64( 0, (KUInt32) inLo )
		{
			if (inLo < 0)
			{
				SetHi( 0xFFFFFFFF );
			}
		}

	///
	/// Constructeur à partir d'un entier de 32 bits non signé.
	///
	/// \param inLo		valeur de poids faible
	///
	inline TSInt64( const KUInt32 inLo = 0 )
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
	inline TSInt64& operator = ( const KSInt32 inLo )
		{
			SetLo( (KUInt32) inLo );
			if (inLo < 0)
			{
				SetHi( 0xFFFFFFFF );
			} else {
				SetHi( 0 );
			}

			return *this;
		}

	///
	/// Opération d'incrément (32 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TSInt64& operator += ( const KSInt32 inArgument );
	
	///
	/// Opération d'incrément (64 bits)
	///
	/// \param inArgument	valeur à ajouter.
	/// \return \c this
	///
	TSInt64& operator += ( const TSInt64& inArgument );
	
	///
	/// Opération de décrément (32 bits)
	///
	/// \param inArgument	valeur à retrancher.
	/// \return \c this
	///
	TSInt64& operator -= ( const KSInt32 inArgument );

	///
	/// Opération de décrément (64 bits)
	///
	/// \param inArgument	valeur à retrancher.
	/// \return \c this
	///
	TSInt64& operator -= ( const TSInt64& inArgument );

	///
	/// Opération de division (32 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TSInt64& operator /= ( const KSInt32 inArgument );
	
	///
	/// Opération de division (64 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TSInt64& operator /= ( const TSInt64& inArgument );

	///
	/// Opération de reste par division euclidienne (32 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TSInt64& operator %= ( const KSInt32 inArgument );

	///
	/// Opération de reste par division euclidienne (64 bits)
	///
	/// \param inArgument	diviseur.
	/// \return \c this
	///
	TSInt64& operator %= ( const TSInt64& inArgument );

	///
	/// Opération de multiplication (32 bits)
	///
	/// \param inArgument	facteur.
	/// \return \c this
	///
	TSInt64& operator *= ( const KSInt32 inArgument );

	///
	/// Opération de multiplication (64 bits)
	///
	/// \param inArgument	facteur.
	/// \return \c this
	///
	TSInt64& operator *= ( const TSInt64& inArgument );

	///
	/// Décalage gauche.
	///
	/// \param inArgument	nombre de décalages.
	/// \return \c this
	///
	TSInt64& operator <<= ( const int inArgument );

	///
	/// Décalage droit.
	///
	/// \param inArgument	nombre de décalages.
	/// \return \c this
	///
	TSInt64& operator >>= ( const int inArgument );

	///
	/// Conversion (avec perte) en 32 bits.
	///
	/// \return les 32 bits de poids faible plus le signe.
	///
#if 0
	inline operator KSInt32 ( void ) const
		{
			KSInt32 theResult = GetLo();
			if (GetHi() & 0x80000000)
			{
				theResult |= 0x80000000;
			} else {
				theResult &= 0x7FFFFFFF;
			}
			
			return theResult;
		}
#endif

	// Opérateurs binaires.
	friend bool operator == (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );
	friend bool operator == (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator != (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );
	friend bool operator != (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator <= (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );
	friend bool operator <= (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator <= (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator >= (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );
	friend bool operator >= (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator >= (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator < (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );
	friend bool operator < (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator < (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator > (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );
	friend bool operator > (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo );
	friend bool operator > (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo );
	friend TSInt64 operator / (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );
	friend TSInt64 operator / (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo );

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
		TSInt64& outQuotient,
		TSInt64& outRemainder,
		const TSInt64& inDividend,
		const TSInt64& inDivisor );

	///
	/// Addition avec retenue.
	///
	/// \param inArgument	entier à ajouter
	/// \return \c true si la somme dépasse 2^64
	///
	bool		Add( const TSInt64& inArgument );
};

///
/// Addition (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne + inArgTwo
///
inline
TSInt64 operator + (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo )
{
	return TSInt64( inArgOne ) += inArgTwo;
}

///
/// Addition (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne + inArgTwo
///
inline
TSInt64 operator + (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo )
{
	// L'addition est commutative.
	return TSInt64( inArgTwo ) += inArgOne;
}

///
/// Addition (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne + inArgTwo
///
inline
TSInt64 operator + (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo )
{
	return TSInt64( inArgOne ) += inArgTwo;
}

///
/// Soustraction (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne - inArgTwo
///
inline
TSInt64 operator - (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo )
{
	return TSInt64( inArgOne ) -= inArgTwo;
}

///
/// Soustraction (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne - inArgTwo
///
inline
TSInt64 operator - (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo )
{
	// La soustraction n'est pas commutative.
	return TSInt64( inArgOne ) -= inArgTwo;
}

///
/// Soustraction (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne - inArgTwo
///
inline
TSInt64 operator - (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo )
{
	return TSInt64( inArgOne ) -= inArgTwo;
}

///
/// Égalité (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne == inArgTwo
///
bool operator == ( const TSInt64& inArgOne, const KSInt32 inArgTwo );

///
/// Égalité (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne == inArgTwo
///
bool operator == ( const KSInt32 inArgOne, const TSInt64& inArgTwo );

///
/// Différence (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne != inArgTwo
///
bool operator != ( const TSInt64& inArgOne, const KSInt32 inArgTwo );

///
/// Différence (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne != inArgTwo
///
bool operator != ( const KSInt32 inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne <= inArgTwo
///
bool operator <= ( const TSInt64& inArgOne, const KSInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne <= inArgTwo
///
bool operator <= ( const KSInt32 inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne <= inArgTwo
///
bool operator <= ( const TSInt64& inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne >= inArgTwo
///
bool operator >= ( const TSInt64& inArgOne, const KSInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne >= inArgTwo
///
bool operator >= ( const KSInt32 inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne >= inArgTwo
///
bool operator >= ( const TSInt64& inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne < inArgTwo
///
bool operator < ( const TSInt64& inArgOne, const KSInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne < inArgTwo
///
bool operator < ( const KSInt32 inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne < inArgTwo
///
bool operator < ( const TSInt64& inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne > inArgTwo
///
bool operator > ( const TSInt64& inArgOne, const KSInt32 inArgTwo );

///
/// Comparaison (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne > inArgTwo
///
bool operator > ( const KSInt32 inArgOne, const TSInt64& inArgTwo );

///
/// Comparaison (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne > inArgTwo
///
bool operator > ( const TSInt64& inArgOne, const TSInt64& inArgTwo );

///
/// Division entière (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne / inArgTwo
///
TSInt64 operator / (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );

///
/// Division entière (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne / inArgTwo
///
TSInt64 operator / (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo );

///
/// Modulo (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne % inArgTwo
///
TSInt64 operator % (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo );

///
/// Modulo (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne % inArgTwo
///
TSInt64 operator % (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo );

///
/// Multiplication (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne * inArgTwo
///
inline
TSInt64 operator * (
		const TSInt64& inArgOne,
		const KSInt32 inArgTwo )
{
	return TSInt64( inArgOne ) *= inArgTwo;
}

///
/// Multiplication (32 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne * inArgTwo
///
inline
TSInt64 operator * (
		const KSInt32 inArgOne,
		const TSInt64& inArgTwo )
{
	// La multiplication est commutative.
	return TSInt64( inArgTwo ) *= inArgOne;
}

///
/// Multiplication (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne * inArgTwo
///
inline
TSInt64 operator * (
		const TSInt64& inArgOne,
		const TSInt64& inArgTwo )
{
	return TSInt64( inArgOne ) *= inArgTwo;
}

///
/// Décalage arithmétique gauche.
///
/// \param inArgument	premier argument
/// \param inShiftCount	décalage
/// \return \c inArgument << inShiftCount
///
inline
TSInt64 operator << (
		const TSInt64& inArgument,
		const int inShiftCount )
{
	return TSInt64( inArgument ) <<= inShiftCount;
}

///
/// Décalage arithmétique droit.
///
/// \param inArgument	premier argument
/// \param inShiftCount	décalage
/// \return \c inArgument << inShiftCount
///
inline
TSInt64 operator >> (
		const TSInt64& inArgument,
		const int inShiftCount )
{
	return TSInt64( inArgument ) >>= inShiftCount;
}

#endif
		// _TSINT64_H

// ============================================================================= //
// Help stamp out Mickey-Mouse computer interfaces -- Menus are for Restaurants! //
// ============================================================================= //
