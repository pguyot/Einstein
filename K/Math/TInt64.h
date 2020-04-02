// ==============================
// Fichier:			TInt64.h
// Projet:			K
// 
// Cr�� le:			21/2/2004
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
// The Original Code is TInt64.h.
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
// $Id: TInt64.h,v 1.4 2005/02/24 06:49:58 pguyot Exp $
// ===========

#ifndef _TINT64_H
#define _TINT64_H

#include <K/Defines/KDefinitions.h>

///
/// Classe de base pour les entiers de 64 bits cr��s � partir de deux entiers
/// de 32 bits.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
/// \test	aucun test d�fini.
///
class TInt64
{
public:
	/// La classe UUInt64 g�re le pont entre les formes natives et les objets C++.
	friend class UTInt64;

	///
	/// Constructeur � partir de deux entiers de 32 bits.
	///
	/// \param inHi		valeur de poids fort
	/// \param inLo		valeur de poids faible
	///
	inline TInt64( const KUInt32 inHi, const KUInt32 inLo )
		:
			mHi( inHi ), mLo( inLo )
		{
		}

	///
	/// Op�rateur d'assignation � partir d'un entier de 64 bits.
	///
	/// \param inCopy	valeur � assigner � \c this
	/// \return \c this
	///
	inline TInt64& operator = ( const TInt64& inCopy )
		{
			mHi = inCopy.GetHi();
			mLo = inCopy.GetLo();
			return *this;
		}

	///
	/// Et logique (32 bits)
	///
	/// \param inArgument	masque.
	/// \return \c this
	///
	TInt64& operator &= ( const KUInt32 inArgument );

	///
	/// Et logique (64 bits)
	///
	/// \param inArgument	masque.
	/// \return \c this
	///
	TInt64& operator &= ( const TInt64& inArgument );

	///
	/// Ou logique (32 bits)
	///
	/// \param inArgument	valeur � ajouter.
	/// \return \c this
	///
	TInt64& operator |= ( const KUInt32 inArgument );

	///
	/// Ou logique (64 bits)
	///
	/// \param inArgument	valeur � ajouter.
	/// \return \c this
	///
	TInt64& operator |= ( const TInt64& inArgument );

	///
	/// Ou logique exclusif (32 bits)
	///
	/// \param inArgument	valeur � ajouter.
	/// \return \c this
	///
	TInt64& operator ^= ( const KUInt32 inArgument );

	///
	/// Ou logique exclusif (64 bits)
	///
	/// \param inArgument	valeur � ajouter.
	/// \return \c this
	///
	TInt64& operator ^= ( const TInt64& inArgument );

	///
	/// N�gation
	///
	/// \return la n�gation, bit � bit, de \c this
	///
	TInt64 operator ~ ( void ) const;

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

	// Op�rateurs binaires.
	friend bool operator == (
		const TInt64& inArgOne,
		const TInt64& inArgTwo );
	friend bool operator != (
		const TInt64& inArgOne,
		const TInt64& inArgTwo );

// Bogue de ARMCpp qui ne g�re pas UTInt64::Get*
#ifndef forARM
protected:
#endif
	///
	/// Accesseur sur les 32 bits de poids fort.
	///
	/// \return les 32 bits de poids fort.
	///
	inline	KUInt32	GetHi( void ) const
		{
			return mHi;
		}

	///
	/// Accesseur sur les 32 bits de poids faible.
	///
	/// \return les 32 bits de poids faible.
	///
	inline	KUInt32	GetLo( void ) const
		{
			return mLo;
		}

	///
	/// S�lecteur sur les 32 bits de poids fort.
	///
	/// \param inHiHalf	nouveaux 32 bits de poids fort.
	///
	inline	void	SetHi( KUInt32 inHiHalf )
		{
			mHi = inHiHalf;
		}

	///
	/// S�lecteur sur les 32 bits de poids faible.
	///
	/// \param inLoHalf	nouveaux 32 bits de poids faible.
	///
	inline	void	SetLo( KUInt32 inLoHalf )
		{
			mLo = inLoHalf;
		}

#ifdef forARM
protected:
#endif
	///
	/// D�calage logique � droite d'un bit avec retenue.
	///
	/// \return \c true si le bit de poids faible �tait � un (retenue)
	///
	bool		ShiftRight( void );

	///
	/// D�calage logique � gauche d'un bit avec retenue.
	///
	/// \return \c true si le bit de poids fort �tait � un (retenue)
	///
	bool		ShiftLeft( void );

	/// \name Variables
	KUInt32	mHi;			///< 32 bits de poids fort
	KUInt32	mLo;			///< 32 bits de poids faible
};

///
/// Et logique (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne & inArgTwo
///
inline
TInt64 operator & (
		const TInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TInt64( inArgOne ) &= inArgTwo;
}

///
/// Et logique (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne & inArgTwo
///
inline
TInt64 operator & (
		const KUInt32 inArgOne,
		const TInt64& inArgTwo )
{
	return TInt64( inArgTwo ) &= inArgOne;
}

///
/// Et logique (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne & inArgTwo
///
inline
TInt64 operator & (
		const TInt64& inArgOne,
		const TInt64& inArgTwo )
{
	return TInt64( inArgOne ) &= inArgTwo;
}

///
/// Ou logique (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne | inArgTwo
///
inline
TInt64 operator | (
		const TInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TInt64( inArgOne ) |= inArgTwo;
}

///
/// Ou logique (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne | inArgTwo
///
inline
TInt64 operator | (
		const KUInt32 inArgOne,
		const TInt64& inArgTwo )
{
	return TInt64( inArgTwo ) |= inArgOne;
}

///
/// Ou logique (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne | inArgTwo
///
inline
TInt64 operator | (
		const TInt64& inArgOne,
		const TInt64& inArgTwo )
{
	return TInt64( inArgOne ) |= inArgTwo;
}

///
/// Ou logique exclusif (32 bits)
///
/// \param inArgOne	premier argument (64 bits)
/// \param inArgTwo	second argument (32 bits)
/// \return \c inArgOne ^ inArgTwo
///
inline
TInt64 operator ^ (
		const TInt64& inArgOne,
		const KUInt32 inArgTwo )
{
	return TInt64( inArgOne ) ^= inArgTwo;
}

///
/// Ou logique exclusif (32 bits)
///
/// \param inArgOne	premier argument (32 bits)
/// \param inArgTwo	second argument (64 bits)
/// \return \c inArgOne ^ inArgTwo
///
inline
TInt64 operator ^ (
		const KUInt32 inArgOne,
		const TInt64& inArgTwo )
{
	return TInt64( inArgTwo ) ^= inArgOne;
}

///
/// Ou logique exclusif (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne ^ inArgTwo
///
inline
TInt64 operator ^ (
		const TInt64& inArgOne,
		const TInt64& inArgTwo )
{
	return TInt64( inArgOne ) ^= inArgTwo;
}

///
/// �galit� (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne == inArgTwo
///
bool operator == ( const TInt64& inArgOne, const TInt64& inArgTwo );

///
/// Diff�rence (64 bits)
///
/// \param inArgOne	premier argument
/// \param inArgTwo	second argument
/// \return \c inArgOne != inArgTwo
///
bool operator != ( const TInt64& inArgOne, const TInt64& inArgTwo );


#endif
		// _TINT64_H

// =================================================================== //
// ... A booming voice says, "Wrong, cretin!", and you notice that you //
// have turned into a pile of dust.                                    //
// =================================================================== //
//
