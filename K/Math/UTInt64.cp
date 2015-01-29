// ==============================
// Fichier:			UTInt64.cp
// Projet:			K
// 
// CrŽŽ le:			10/9/2003
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
// The Original Code is UTInt64.cp.
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
// $Id: UTInt64.cp,v 1.6 2007/07/11 01:36:29 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Math/UTInt64.h>

#ifdef _MSC_VER
	#define LLU(x) x##ui64
#else
	#define LLU(x) x##LLU
#endif

// -------------------------------------------------------------------------- //
//  * GetHi( const KUInt64& )
// -------------------------------------------------------------------------- //
KUInt32
UTInt64::GetHi( const KUInt64& in64BitsWord )
{
	#if HAS_C99_LONGLONG
		return (KUInt32) (in64BitsWord >> 32);
	#else
		return in64BitsWord.GetHi();
	#endif
}

// -------------------------------------------------------------------------- //
//  * GetHi( const KSInt64& )
// -------------------------------------------------------------------------- //
KSInt32
UTInt64::GetHi( const KSInt64& in64BitsWord )
{
	#if HAS_C99_LONGLONG
		return (KSInt32) (in64BitsWord >> 32);
	#else
		return in64BitsWord.GetHi();
	#endif
}

// -------------------------------------------------------------------------- //
//  * GetLo( const KUInt64& )
// -------------------------------------------------------------------------- //
KUInt32
UTInt64::GetLo( const KUInt64& in64BitsWord )
{
	#if HAS_C99_LONGLONG
		return (KUInt32) (in64BitsWord);
	#else
		return in64BitsWord.GetLo();
	#endif
}

// -------------------------------------------------------------------------- //
//  * GetLo( const KSInt64& )
// -------------------------------------------------------------------------- //
KUInt32
UTInt64::GetLo( const KSInt64& in64BitsWord )
{
	#if HAS_C99_LONGLONG
		return (KUInt32) (in64BitsWord);
	#else
		return in64BitsWord.GetLo();
	#endif
}

// -------------------------------------------------------------------------- //
//  * ToKUInt32( const KSInt64& )
// -------------------------------------------------------------------------- //
KUInt32
UTInt64::ToKUInt32( const KSInt64& in64BitsWord )
{
	#if HAS_C99_LONGLONG
		return (KUInt32) in64BitsWord;
	#else
		return in64BitsWord.GetHi();
	#endif
}

// -------------------------------------------------------------------------- //
//  * ToKUInt32( const KUInt64& )
// -------------------------------------------------------------------------- //
KUInt32
UTInt64::ToKUInt32( const KUInt64& in64BitsWord )
{
	#if HAS_C99_LONGLONG
		return (KUInt32) in64BitsWord;
	#else
		return in64BitsWord.GetHi();
	#endif
}

// -------------------------------------------------------------------------- //
//  * ToKSInt32( const KSInt64& )
// -------------------------------------------------------------------------- //
KSInt32
UTInt64::ToKSInt32( const KSInt64& in64BitsWord )
{
	#if HAS_C99_LONGLONG
		return (KSInt32) in64BitsWord;
	#else
		KSInt32 theResult = (KSInt32) (in64BitsWord.GetLo() & 0x7FFFFFFF);
		if (in64BitsWord.GetHi() & 0x80000000)
		{
			theResult |= 0x80000000;
		}
		
		return theResult;
	#endif
}

// -------------------------------------------------------------------------- //
//  * SetHi( KUInt64*, const KUInt32 )
// -------------------------------------------------------------------------- //
void
UTInt64::SetHi( KUInt64* io64BitsWord, const KUInt32 in32BitsValue )
{
	#if HAS_C99_LONGLONG
		*io64BitsWord &= (KUInt64) LLU(0x00000000FFFFFFFF);
		*io64BitsWord |= (((KUInt64) in32BitsValue) << 32);
	#else
		io64BitsWord->SetHi( in32BitsValue );
	#endif
}

// -------------------------------------------------------------------------- //
//  * SetHi( KSInt64*, const KSInt32 )
// -------------------------------------------------------------------------- //
void
UTInt64::SetHi( KSInt64* io64BitsWord, const KSInt32 in32BitsValue )
{
	#if HAS_C99_LONGLONG
		*io64BitsWord &= (KSInt64) LLU(0x00000000FFFFFFFF);
		*io64BitsWord |= (((KSInt64) in32BitsValue) << 32);
	#else
		io64BitsWord->SetHi( in32BitsValue );
	#endif
}

// -------------------------------------------------------------------------- //
//  * SetLo( KUInt64*, const KUInt32 )
// -------------------------------------------------------------------------- //
void
UTInt64::SetLo( KUInt64* io64BitsWord, const KUInt32 in32BitsValue )
{
	#if HAS_C99_LONGLONG
		*io64BitsWord &= (KUInt64) LLU(0xFFFFFFFF00000000);
		*io64BitsWord |= in32BitsValue;
	#else
		io64BitsWord->SetLo( in32BitsValue );
	#endif
}

// -------------------------------------------------------------------------- //
//  * SetLo( KSInt64*, const KUInt32 )
// -------------------------------------------------------------------------- //
void
UTInt64::SetLo( KSInt64* io64BitsWord, const KUInt32 in32BitsValue )
{
	#if HAS_C99_LONGLONG
		*io64BitsWord &= (KUInt64) LLU(0xFFFFFFFF00000000);
		*io64BitsWord |= in32BitsValue;
	#else
		io64BitsWord->SetLo( in32BitsValue );
	#endif
}

// -------------------------------------------------------------------------- //
//  * Set( KUInt64*, const KUInt32, const KUInt32 )
// -------------------------------------------------------------------------- //
void
UTInt64::Set(
			KUInt64* out64BitsWord,
			const KUInt32 inHiHalf,
			const KUInt32 inLoHalf )
{
	#if HAS_C99_LONGLONG
		*out64BitsWord = (((KUInt64) inHiHalf) << 32) | inLoHalf;
	#else
		out64BitsWord->SetHi( inHiHalf );
		out64BitsWord->SetLo( inLoHalf );
	#endif
}

// -------------------------------------------------------------------------- //
//  * Set( KSInt64*, const KSInt32, const KUInt32 )
// -------------------------------------------------------------------------- //
void
UTInt64::Set(
			KSInt64* out64BitsWord,
			const KSInt32 inHiHalf,
			const KUInt32 inLoHalf )
{
	#if HAS_C99_LONGLONG
		*out64BitsWord = (((KSInt64) inHiHalf) << 32) | inLoHalf;
	#else
		out64BitsWord->SetHi( inHiHalf );
		out64BitsWord->SetLo( inLoHalf );
	#endif
}

// -------------------------------------------------------------------------- //
//  * CreateKUInt64( const KUInt32, const KUInt32 )
// -------------------------------------------------------------------------- //
KUInt64
UTInt64::CreateKUInt64( const KUInt32 inHiHalf, const KUInt32 inLoHalf )
{
	#if HAS_C99_LONGLONG
		return (((KUInt64) inHiHalf) << 32) | inLoHalf;
	#else
		return TUInt64( inHiHalf, inLoHalf );
	#endif
}

// -------------------------------------------------------------------------- //
//  * CreateKSInt64( const KSInt32, const KUInt32 )
// -------------------------------------------------------------------------- //
KSInt64
UTInt64::CreateKSInt64( const KSInt32 inHiHalf, const KUInt32 inLoHalf )
{
	#if HAS_C99_LONGLONG
		return (((KSInt64) inHiHalf) << 32) | inLoHalf;
	#else
		return TSInt64( inHiHalf, inLoHalf );
	#endif
}

// --------------------------------------------------------------------	//
//	* MultModulo( const KUInt64&, const KUInt64&, const KUInt64& )
// --------------------------------------------------------------------	//
// Multiplication avec modulo
// Retourne Un * Deux mod Module
KUInt64
UTInt64::MultModulo(
	const KUInt64& inArgOne,
	const KUInt64& inArgTwo,
	const KUInt64& inModule )
{
	// Russian Peasant modifiŽ avec modulo.
	KUInt64 theLeftInt = inArgOne % inModule;
	KUInt64 theRightInt = inArgTwo % inModule;
	KUInt64 theResult = 0;
	
	while (theRightInt > 0)
	{
		// On divise theRightInt par deux
#if HAS_C99_LONGLONG
		bool theCarry = ((theRightInt & LLU(0x0000000000000001)) != 0);
		theRightInt >>= 1;
		if (theCarry)
#else
		if (theRightInt.ShiftRight())
#endif
		{
			// Gestion de l'overflow.
			// theResult est < inModule
			// theLeftInt est < inModule
			// Donc theResult + theLeftInt est < 2 * inModule
			// Si c'est >= inModule, je retranche inModule
#if HAS_C99_LONGLONG
			theResult += theLeftInt;
			if ((theResult < theLeftInt) || (theResult >= inModule))
#else
			if (theResult.Add( theLeftInt ) || (theResult >= inModule))
#endif
			{
				(void) (theResult -= inModule);
			}
		}
		// Et on multiplie l'entier de gauche par deux
		// Comme theLeftInt est < inModule, 
		// theLeftInt * 2 est < 2 * inModule
#if HAS_C99_LONGLONG
		theCarry = ((theLeftInt & LLU(0x8000000000000000)) != 0);
		theLeftInt <<= 1;
		if (theCarry || (theLeftInt >= inModule))
#else
		if (theLeftInt.ShiftLeft() || (theLeftInt >= inModule))
#endif
		{
			(void) (theLeftInt -= inModule);
			// Gestion de l'overflow:
			// theLeftInt est en fait 0x100000000 + theLeftInt
			// Donc theLeftInt - inModule est:
			// theLeftInt + (0x100000000 - inModule)
			// Il suffit donc de faire theLeftInt -= inModule
		}
	}
		
	return theResult;
}

// --------------------------------------------------------------------	//
//	* ExpModulo( const KUInt64&, const KUInt64&, const KUInt64& )
// --------------------------------------------------------------------	//
// Exponentiation avec modulo
// Retourne Base ^ Exposant mod Module
KUInt64
UTInt64::ExpModulo(
	const KUInt64& inBase,
	const KUInt64& inExponent,
	const KUInt64& inModule )
{
	// ItŽration sur les bits de E.
	// le rŽsultat est produit(base^(2^ki) modulo module) modulo module.
	// Donc, on calcule base^2ki ˆ chaque itŽration, on multiplie
	// avec le rŽsultat si le bit correspondant de E est ˆ 1.
	
	KUInt64 basePuissance2KiReduite = inBase % inModule; // i = 0
	KUInt64 theResult = 1;
	KUInt64 theExponent = inExponent;
	
	while (theExponent != 0)
	{
#if HAS_C99_LONGLONG
		bool theCarry = ((theExponent & LLU(0x0000000000000001)) != 0);
		theExponent >>= 1;
		if (theCarry)
#else
		if (theExponent.ShiftRight())
#endif
		{
			theResult =
				MultModulo( basePuissance2KiReduite, theResult, inModule );
		}
		
		basePuissance2KiReduite =
			MultModulo( basePuissance2KiReduite, basePuissance2KiReduite, inModule );
	}
	
	return theResult;
}

// --------------------------------------------------------------------	//
//	* EuclideanDivision( KUInt64&, KUInt64&, const KUInt64&, const ... )
// --------------------------------------------------------------------	//
void
UTInt64::EuclideanDivision(
		KUInt64& outQuotient,
		KUInt64& outRemainder,
		const KUInt64& inDividend,
		const KUInt64& inDivisor )
{
#if HAS_C99_LONGLONG
	outQuotient = inDividend / inDivisor;
	outRemainder = inDividend - (outQuotient * inDivisor);
#else
	TUInt64::EuclideanDivision(
		outQuotient, outRemainder, inDividend, inDivisor );
#endif
}

// =============================================== //
// There are never any bugs you haven't found yet. //
// =============================================== //
