// ==============================
// Fichier:			TUInt64.cp
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
// The Original Code is TUInt64.cp.
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
// $Id: TUInt64.cp,v 1.5 2005/02/24 06:49:58 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Math/TUInt64.h>

// --------------------------------------------------------------------	//
//	* operator += ( const KUInt32 )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator += ( const KUInt32 inArgument )
{
	// Addition avec détection de la retenue.
	// Si on a retenue, la somme est plus petite que chacunes des opérandes.
	mLo += inArgument;
	if (inArgument > mLo)
	{
		mHi++;
	}
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator += ( const TUInt64& )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator += ( const TUInt64& inArgument )
{
	// Idem.
	KUInt32 ancienLo = mLo;
	mLo += inArgument.GetLo();
	if (ancienLo > mLo)
	{
		mHi++;
	}
	
	mHi += inArgument.GetHi();
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator -= ( const KUInt32 )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator -= ( const KUInt32 inArgument )
{
	// Soustraction avec détection de la retenue.
	// Si on a retenue, la différence est plus grande que chacunes des opérandes.
	mLo -= inArgument;
	if (inArgument < mLo)
	{
		mHi--;
	}
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator -= ( const TUInt64 )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator -= ( const TUInt64& inArgument )
{
	// Idem.
	KUInt32 ancienLo = mLo;
	mLo -= inArgument.GetLo();
	if (ancienLo < mLo)
	{
		mHi--;
	}
	
	mHi -= inArgument.GetHi();
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator /= ( const KUInt32 )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator /= ( const KUInt32 inArgument )
{
	*this = *this / inArgument;

	return *this;
}

// --------------------------------------------------------------------	//
//	* operator /= ( const TUInt64& )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator /= ( const TUInt64& inArgument )
{
	*this = *this / inArgument;

	return *this;
}

// --------------------------------------------------------------------	//
//	* operator %= ( const KUInt32 )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator %= ( const KUInt32 inArgument )
{
	*this = *this % inArgument;

	return *this;
}

// --------------------------------------------------------------------	//
//	* operator %= ( const TUInt64& )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator %= ( const TUInt64& inArgument )
{
	*this = *this % inArgument;

	return *this;
}

// --------------------------------------------------------------------	//
//	* operator *= ( const KUInt32 )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator *= ( const KUInt32 inArgument )
{
	// A la Russian Peasant
	// *this joue le rôle de l'entier de gauche.
	TUInt64 theResult = 0;
	KUInt32 theRightInt = inArgument;
	while (theRightInt > 0)
	{
		if (theRightInt & 0x01)
		{
			(void) (theResult += *this);
		}
		// On divise theRightInt par deux
		theRightInt >>= 1;
		// Et on multiplie l'entier de gauche par deux
		(void) ShiftLeft();
	}
	
	*this = theResult;
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator *= ( const TUInt64& )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator *= ( const TUInt64& inArgument )
{
	// Idem
	TUInt64 theResult = 0;
	TUInt64 theRightInt = inArgument;
	
	while (theRightInt > 0)
	{
		// On divise theRightInt par deux
		if (theRightInt.ShiftRight())
		{
			(void) (theResult += *this);
		}
		// Et on multiplie l'entier de gauche par deux
		(void) ShiftLeft();
	}
	
	*this = theResult;
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator >>= ( const int )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator >>= ( const int inArgument )
{
	if (inArgument < 32)
	{
		mLo >>= inArgument;
		mLo |= (mHi << (32 - inArgument));
		mHi >>= inArgument;
	} else if (inArgument == 32)
	{
		mLo = mHi;
		mHi = 0;
	} else if (inArgument < 64)
	{
		mLo = mHi >> (inArgument - 32);
		mHi = 0;
	} else {
		mHi = 0;
		mLo = 0;
	}
	
	return *this;
}

// --------------------------------------------------------------------	//
//	* operator <<= ( const int )
// --------------------------------------------------------------------	//
TUInt64&
TUInt64::operator <<= ( const int inArgument )
{
	if (inArgument < 32)
	{
		mHi <<= inArgument;
		mHi |= (mLo >> (32 - inArgument));
		mLo <<= inArgument;
	} else if (inArgument == 32)
	{
		mHi = mLo;
		mLo = 0;
	} else if (inArgument < 64)
	{
		mHi = mLo << (inArgument - 32);
		mLo = 0;
	} else {
		mHi = 0;
		mLo = 0;
	}

	return *this;
}

// --------------------------------------------------------------------	//
//	* EuclideanDivision( TUInt64&, TUInt64&, const TUInt64&, const ... )
// --------------------------------------------------------------------	//
void
TUInt64::EuclideanDivision(
		TUInt64& outQuotient,
		TUInt64& outRemainder,
		const TUInt64& inDividend,
		const TUInt64& inDivisor )
{
	// Cas faciles (pour s'en débarrasser dès maintenant).
	if (inDivisor == inDividend)
	{
		outQuotient = 1;
		outRemainder = 0;
		return;
	}
	
	if (inDivisor > inDividend)
	{	
		outQuotient = 0;
		outRemainder = inDividend;
		return;
	}

	TUInt64 leDiviseur = inDivisor;
	
	// On décale le diviseur de ce qu'il faut vers la gauche (sans dépasser)
	int decalages = 0;
	while (leDiviseur <= inDividend)
	{
		decalages++;

		if (leDiviseur.ShiftLeft())
			break;	// Overflow.
	}
	
	// On l'a fait une fois de trop.
	decalages--;
	leDiviseur = inDivisor;
	(void) (leDiviseur <<= decalages);

	// Puis on soustrait à chaque fois si possible (on a alors un 1 dans le quotient)
	outRemainder = inDividend;
	outQuotient = 0;
	
	for(; decalages >= 0; decalages--)
	{
		// On multiplie le quotient par 2
		(void) outQuotient.ShiftLeft();
		if (outRemainder >= leDiviseur)
		{
			// Soustraction.
			(void) (outRemainder -= leDiviseur);
			
			// On met un 1 dans le quotient.
			(void) (outQuotient |= 1);
		}
		// Et on divise le diviseur par 2
		(void) leDiviseur.ShiftRight();
	}
}

// --------------------------------------------------------------------	//
//	* Add( const TUInt64& )
// --------------------------------------------------------------------	//
bool
TUInt64::Add( const TUInt64& inArgument )
{
	KUInt32 ancienLo = mLo;
	KUInt32 ancienHi = mHi;
	mLo += inArgument.GetLo();
	if (ancienLo > mLo)
	{
		mHi++;
	}
	
	mHi += inArgument.GetHi();

	return (ancienHi > mHi);
}

// Opérateurs binaires (non inline)

// --------------------------------------------------------------------	//
//	* operator == ( const TUInt64&, KUInt32 )
// --------------------------------------------------------------------	//
bool
operator == ( const TUInt64& inArgOne, const KUInt32 inArgTwo )
{
	return (inArgOne.GetHi() == 0)
		&& (inArgOne.GetLo() == inArgTwo);
}

// --------------------------------------------------------------------	//
//	* operator == ( KUInt32, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator == ( const KUInt32 inArgOne, const TUInt64& inArgTwo )
{
	return (inArgTwo.GetHi() == 0)
		&& (inArgTwo.GetLo() == inArgOne);
}

// --------------------------------------------------------------------	//
//	* operator != ( const TUInt64&, KUInt32 )
// --------------------------------------------------------------------	//
bool
operator != ( const TUInt64& inArgOne, const KUInt32 inArgTwo )
{
	return (inArgOne.GetHi() != 0)
		|| (inArgOne.GetLo() != inArgTwo);
}

// --------------------------------------------------------------------	//
//	* operator != ( KUInt32, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator != ( const KUInt32 inArgOne, const TUInt64& inArgTwo )
{
	return (inArgTwo.GetHi() != 0)
		|| (inArgTwo.GetLo() != inArgOne);
}

// --------------------------------------------------------------------	//
//	* operator <= ( const TUInt64&, KUInt32 )
// --------------------------------------------------------------------	//
bool
operator <= ( const TUInt64& inArgOne, const KUInt32 inArgTwo )
{
	return (inArgOne.GetHi() == 0)
		&& (inArgOne.GetLo() <= inArgTwo);
}

// --------------------------------------------------------------------	//
//	* operator <= ( KUInt32, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator <= ( const KUInt32 inArgOne, const TUInt64& inArgTwo )
{
	return (inArgTwo.GetHi() > 0)
		|| (inArgTwo.GetLo() >= inArgOne);
}

// --------------------------------------------------------------------	//
//	* operator <= ( const TUInt64&, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator <= ( const TUInt64& inArgOne, const TUInt64& inArgTwo )
{
	return ((inArgOne.GetHi() < inArgTwo.GetHi())
		|| ((inArgOne.GetHi() == inArgTwo.GetHi())
			&& (inArgOne.GetLo() <= inArgTwo.GetLo())));
}

// --------------------------------------------------------------------	//
//	* operator >= ( const TUInt64&, KUInt32 )
// --------------------------------------------------------------------	//
bool
operator >= ( const TUInt64& inArgOne, const KUInt32 inArgTwo )
{
	return (inArgOne.GetHi() > 0)
		|| (inArgOne.GetLo() >= inArgTwo);
}

// --------------------------------------------------------------------	//
//	* operator >= ( KUInt32, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator >= ( const KUInt32 inArgOne, const TUInt64& inArgTwo )
{
	return (inArgTwo.GetHi() == 0)
		&& (inArgTwo.GetLo() <= inArgOne);
}

// --------------------------------------------------------------------	//
//	* operator >= ( const TUInt64&, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator >= ( const TUInt64& inArgOne, const TUInt64& inArgTwo )
{
	return ((inArgOne.GetHi() > inArgTwo.GetHi())
		|| ((inArgOne.GetHi() == inArgTwo.GetHi())
			&& (inArgOne.GetLo() >= inArgTwo.GetLo())));
}

// --------------------------------------------------------------------	//
//	* operator < ( const TUInt64&, KUInt32 )
// --------------------------------------------------------------------	//
bool
operator < ( const TUInt64& inArgOne, const KUInt32 inArgTwo )
{
	return (inArgOne.GetHi() == 0)
		&& (inArgOne.GetLo() < inArgTwo);
}

// --------------------------------------------------------------------	//
//	* operator < ( KUInt32, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator < ( const KUInt32 inArgOne, const TUInt64& inArgTwo )
{
	return (inArgTwo.GetHi() > 0)
		|| (inArgTwo.GetLo() > inArgOne);
}

// --------------------------------------------------------------------	//
//	* operator < ( const TUInt64&, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator < ( const TUInt64& inArgOne, const TUInt64& inArgTwo )
{
	return ((inArgOne.GetHi() < inArgTwo.GetHi())
		|| ((inArgOne.GetHi() == inArgTwo.GetHi())
			&& (inArgOne.GetLo() < inArgTwo.GetLo())));
}

// --------------------------------------------------------------------	//
//	* operator > ( const TUInt64&, KUInt32 )
// --------------------------------------------------------------------	//
bool
operator > ( const TUInt64& inArgOne, const KUInt32 inArgTwo )
{
	return (inArgOne.GetHi() > 0)
		|| (inArgOne.GetLo() > inArgTwo);
}

// --------------------------------------------------------------------	//
//	* operator > ( KUInt32, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator > ( const KUInt32 inArgOne, const TUInt64& inArgTwo )
{
	return (inArgTwo.GetHi() == 0)
		&& (inArgTwo.GetLo() < inArgOne);
}

// --------------------------------------------------------------------	//
//	* operator > ( const TUInt64&, const TUInt64& )
// --------------------------------------------------------------------	//
bool
operator > ( const TUInt64& inArgOne, const TUInt64& inArgTwo )
{
	return ((inArgOne.GetHi() > inArgTwo.GetHi())
		|| ((inArgOne.GetHi() == inArgTwo.GetHi())
			&& (inArgOne.GetLo() > inArgTwo.GetLo())));
}

// --------------------------------------------------------------------	//
//	* operator / ( const TUInt64&, const KUInt32 )
// --------------------------------------------------------------------	//
TUInt64
operator / ( const TUInt64& inEntierUn,
		const KUInt32 inEntierDeux )
{
	TUInt64 leQuotient, leReste;
	TUInt64::EuclideanDivision( leQuotient, leReste, inEntierUn, TUInt64( inEntierDeux ) );

	return leQuotient;	
}

// --------------------------------------------------------------------	//
//	* operator / ( const TUInt64&, const TUInt64& )
// --------------------------------------------------------------------	//
TUInt64
operator / ( const TUInt64& inEntierUn,
		const TUInt64& inEntierDeux )
{
	TUInt64 leQuotient, leReste;
	TUInt64::EuclideanDivision( leQuotient, leReste, inEntierUn, inEntierDeux );

	return leQuotient;	
}

// --------------------------------------------------------------------	//
//	* operator % ( const TUInt64&, const KUInt32 )
// --------------------------------------------------------------------	//
TUInt64
operator % ( const TUInt64& inEntierUn,
		const KUInt32 inEntierDeux )
{
	TUInt64 leQuotient, leReste;
	TUInt64::EuclideanDivision( leQuotient, leReste, inEntierUn, TUInt64( inEntierDeux ) );

	return leReste;	
}

// --------------------------------------------------------------------	//
//	* operator % ( const TUInt64&, const TUInt64& )
// --------------------------------------------------------------------	//
TUInt64
operator % ( const TUInt64& inEntierUn,
		const TUInt64& inEntierDeux )
{
	TUInt64 leQuotient, leReste;
	TUInt64::EuclideanDivision( leQuotient, leReste, inEntierUn, inEntierDeux );

	return leReste;	
}

// --------------------------------------------------------------------	//
//	* operator ~ ( void ) const
// --------------------------------------------------------------------	//
TUInt64
TUInt64::operator ~ ( void ) const
{
	return TUInt64( ~mHi, ~mLo );
}

// ========================================================================= //
// A recent study has found that concentrating on difficult off-screen       //
// objects, such as the faces of loved ones, causes eye strain in computer   //
// scientists.  Researchers into the phenomenon cite the added concentration //
// needed to "make sense" of such unnatural three dimensional objects.       //
// ========================================================================= //
