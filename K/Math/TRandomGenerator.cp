// ==============================
// Fichier:			TRandomGenerator.cp
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
// The Original Code is TRandomGenerator.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2003-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
//					avec les éléments suivants en annexe A:
// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
// 
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
// 
//   3. The names of its contributors may not be used to endorse or promote 
//      products derived from this software without specific prior written 
//      permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ===========
// $Id: TRandomGenerator.cp,v 1.3 2005/02/14 20:55:05 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Math/TRandomGenerator.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TRandomGenerator( void )
// -------------------------------------------------------------------------- //
TRandomGenerator::TRandomGenerator( void )
{
}

// -------------------------------------------------------------------------- //
//  * ~TRandomGenerator( void )
// -------------------------------------------------------------------------- //
TRandomGenerator::~TRandomGenerator( void )
{
}

// -------------------------------------------------------------------------- //
//  * Int_31( void )
// -------------------------------------------------------------------------- //
KUInt32
TRandomGenerator::Int_31( void )
{
	return (KUInt32)(Int_32() >> 1);
}

// -------------------------------------------------------------------------- //
//  * Double_1( void )
// -------------------------------------------------------------------------- //
double
TRandomGenerator::Double_1(void)
{
	return (double)(Int_32()) * (1.0/4294967295.0); 
	// divided by 2^32-1
}

// -------------------------------------------------------------------------- //
//  * Double_2( void )
// -------------------------------------------------------------------------- //
double
TRandomGenerator::Double_2(void)
{
	return (double)(Int_32()) * (1.0/4294967296.0); 
	// divided by 2^32
}

// -------------------------------------------------------------------------- //
//  * Double_3( void )
// -------------------------------------------------------------------------- //
double
TRandomGenerator::Double_3(void)
{
	return ((double)(Int_32()) + 0.5) * (1.0/4294967296.0); 
	// divided by 2^32
}

// -------------------------------------------------------------------------- //
//  * Double_53( void )
// -------------------------------------------------------------------------- //
double
TRandomGenerator::Double_53(void) 
{
	KUInt32 a = Int_32() >> 5, b = Int_32() >> 6; 
	return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
// These real versions are due to Isaku Wada, 2002/01/09 added

// -------------------------------------------------------------------------- //
//  * DistributionFast( KUInt32 inBound )
// -------------------------------------------------------------------------- //
KUInt32
TRandomGenerator::DistributionFast( KUInt32 inBound )
{
	return (KUInt32) (Double_2() * inBound);
} 

// -------------------------------------------------------------------------- //
//  * DistributionAccurate( KUInt32 inBound )
// -------------------------------------------------------------------------- //
KUInt32
TRandomGenerator::DistributionAccurate( KUInt32 inBound )
{
	KUInt32 theMask = 1;
	KUInt32 theBound = inBound;
	while (theBound)
	{
		theMask <<= 1;
		theBound >>= 1;
	}
	
	theMask -= 1;

	do {
		// Loop until we find a number smaller than inBound.
		KUInt32 theNumber = Int_32() & theMask;
		if (theNumber < inBound)
		{
			return theNumber;
		}
	} while (true);
}

// -------------------------------------------------------------------------- //
//  * DistributionDouble_1( double inFirstBound, double inSecondBound )
// -------------------------------------------------------------------------- //
double
TRandomGenerator::DistributionDouble_1(
						double inFirstBound,
						double inSecondBound )
{
	// Si inFirstBound > inSecondBound, theRange > 0, theRange * Double_1() > 0,
	// le résultat est entre inFirstBound et inSecondBound.
	// Sinon, theRange < 0, theRange * Double_1() < 0 et le résultat est aussi
	// compris entre les deux bornes.
	double theRange = inFirstBound - inSecondBound;
	return (theRange * Double_1()) + inSecondBound;
}

// -------------------------------------------------------------------------- //
//  * Coin( void )
// -------------------------------------------------------------------------- //
Boolean
TRandomGenerator::Coin( void )
{
	return ((Int_32() & 1) != 0);
} 

// ======================================================================= //
// The trouble with computers is that they do what you tell them, not what //
// you want.                                                               //
//                 -- D. Cohen                                             //
// ======================================================================= //
