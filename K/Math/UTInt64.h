// ==============================
// Fichier:			UTInt64.h
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
// The Original Code is UTInt64.h.
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
// $Id: UTInt64.h,v 1.3 2004/11/23 15:11:31 paul Exp $
// ===========

#ifndef _UTINT64_H
#define _UTINT64_H

#include <K/Defines/KDefinitions.h>

///
/// Classe pour des utilitaires avec les entiers sur 64 bits.
/// Fonctionne sur des \c unsigned \c long \c long (i.e. des KUInt64 natifs),
/// des \c signed \c long \c long (i.e. des KSInt64 natifs) ou sur des
/// \c TUInt64 et des \c TSInt64.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	UUInt64Test
///
class UTInt64
{
public:
	///
	/// Accesseur sur les 32 bits de poids fort.
	///
	/// \param in64BitsWord		mot dont on veut les 32 bits de poids fort.
	/// \return les 32 bits de poids fort
	///
	static KUInt32	GetHi( const KUInt64& in64BitsWord );
	
	///
	/// Accesseur sur les 32 bits de poids fort.
	///
	/// \param in64BitsWord		mot dont on veut les 32 bits de poids fort.
	/// \return les 32 bits de poids fort
	///
	static KSInt32	GetHi( const KSInt64& in64BitsWord );
	
	///
	/// Accesseur sur les 32 bits de poids faible.
	///
	/// \param in64BitsWord		mot dont on veut les 32 bits de poids faible.
	/// \return les 32 bits de poids faible
	///
	static KUInt32	GetLo( const KUInt64& in64BitsWord );
	
	///
	/// Accesseur sur les 32 bits de poids faible.
	///
	/// \param in64BitsWord		mot dont on veut les 32 bits de poids faible.
	/// \return les 32 bits de poids faible
	///
	static KUInt32	GetLo( const KSInt64& in64BitsWord );
	
	///
	/// Accesseur sur les 32 bits de poids faible.
	///
	/// \param in64BitsWord		mot dont on veut les 32 bits de poids faible.
	/// \return les 32 bits de poids faible
	///
	static KUInt32	ToKUInt32( const KSInt64& in64BitsWord );
	
	///
	/// Accesseur sur les 32 bits de poids faible.
	///
	/// \param in64BitsWord		mot dont on veut les 32 bits de poids faible.
	/// \return les 32 bits de poids faible
	///
	static KUInt32	ToKUInt32( const KUInt64& in64BitsWord );
		
	///
	/// Conversion en 32 bits avec signe.
	///
	/// \param in64BitsWord		mot dont on veut une version 32 bits.
	/// \return (KSInt32) in64BitsWord
	///
	static KSInt32	ToKSInt32( const KSInt64& in64BitsWord );

	///
	/// Sélecteur sur les 32 bits de poids fort.
	///
	/// \param io64BitsWord		mot dont on veut changer les 32 bits de poids
	///							fort.
	/// \param in32BitsValue	32 bits de poids fort à changer.
	///
	static void		SetHi( KUInt64* io64BitsWord, const KUInt32 in32BitsValue );
	
	///
	/// Sélecteur sur les 32 bits de poids fort.
	///
	/// \param io64BitsWord		mot dont on veut changer les 32 bits de poids
	///							fort.
	/// \param in32BitsValue	32 bits de poids fort à changer.
	///
	static void		SetHi( KSInt64* io64BitsWord, const KSInt32 in32BitsValue );
	
	///
	/// Sélecteur sur les 32 bits de poids faible.
	///
	/// \param io64BitsWord		mot dont on veut changer les 32 bits de poids
	///							faible.
	/// \param in32BitsValue	32 bits de poids faible à changer.
	///
	static void		SetLo( KUInt64* io64BitsWord, const KUInt32 in32BitsValue );
	
	///
	/// Sélecteur sur les 32 bits de poids faible.
	///
	/// \param io64BitsWord		mot dont on veut changer les 32 bits de poids
	///							faible.
	/// \param in32BitsValue	32 bits de poids faible à changer.
	///
	static void		SetLo( KSInt64* io64BitsWord, const KUInt32 in32BitsValue );

	///
	/// Sélecteur sur les deux moitiés de 32 bits.
	///
	/// \param out64BitsWord	mot de 64 bits à changer
	/// \param inHiHalf			32 bits de poids fort.
	/// \param inLoHalf			32 bits de poids faible.
	///
	static void		Set(
						KUInt64* out64BitsWord,
						const KUInt32 inHiHalf,
						const KUInt32 inLoHalf );

	///
	/// Sélecteur sur les deux moitiés de 32 bits.
	///
	/// \param out64BitsWord	mot de 64 bits à changer
	/// \param inHiHalf			32 bits de poids fort.
	/// \param inLoHalf			32 bits de poids faible.
	///
	static void		Set(
						KSInt64* out64BitsWord,
						const KSInt32 inHiHalf,
						const KUInt32 inLoHalf );
	
	///
	/// Créateur à partir de deux fois 32 bits.
	///
	/// \param inHiHalf			32 bits de poids fort.
	/// \param inLoHalf			32 bits de poids faible.
	/// \return un entier de 64 bits (de type \c KUInt64)
	///
	static KUInt64	CreateKUInt64(
						const KUInt32 inHiHalf,
						const KUInt32 inLoHalf );
	
	///
	/// Créateur à partir de deux fois 32 bits.
	///
	/// \param inHiHalf			32 bits de poids fort.
	/// \param inLoHalf			32 bits de poids faible.
	/// \return un entier de 64 bits (de type \c KUInt64)
	///
	static KSInt64	CreateKSInt64(
						const KSInt32 inHiHalf,
						const KUInt32 inLoHalf );
	
	///
	/// Mutliplication suivie d'un modulo.
	///
	/// \param inArgOne	premier argument (facteur)
	/// \param inArgTwo	second argument (facteur)
	/// \param inModule	troisième argument (module)
	/// \return (inArgOne * inArgTwo) % inModule
	///
	static KUInt64 MultModulo(
		const KUInt64& inArgOne,
		const KUInt64& inArgTwo,
		const KUInt64& inModule );

	///
	/// Exponentiation avec modulo.
	///
	/// \param inBase		premier argument (base)
	/// \param inExponent	second argument (exposant)
	/// \param inModule		troisième argument (module)
	/// \return (inBase ^ inExponent) % inModule
	///
	static KUInt64 ExpModulo(
		const KUInt64& inBase,
		const KUInt64& inExponent,
		const KUInt64& inModule );

	///
	/// Division euclidienne avec reste.
	///
	/// \param outQuotient	quotient
	/// \param outRemainder	reste
	/// \param inDividend	dividende
	/// \param inDivisor	diviseur
	///
	static void EuclideanDivision(
		KUInt64& outQuotient,
		KUInt64& outRemainder,
		const KUInt64& inDividend,
		const KUInt64& inDivisor );
};

#endif
		// _UTINT64_H

// =========================================================================== //
//         A novice asked the master: "I perceive that one computer company is //
// much larger than all others.  It towers above its competition like a giant  //
// among dwarfs.  Any one of its divisions could comprise an entire business.  //
// Why is this so?"                                                            //
//         The master replied, "Why do you ask such foolish questions?  That   //
// company is large because it is so large.  If it only made hardware, nobody  //
// would buy it.  If it only maintained systems, people would treat it like a  //
// servant.  But because it combines all of these things, people think it one  //
// of the gods!  By not seeking to strive, it conquers without effort."        //
//                 -- Geoffrey James, "The Tao of Programming"                 //
// =========================================================================== //
