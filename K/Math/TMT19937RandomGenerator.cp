// ==============================
// Fichier:			TMT19937RandomGenerator.cp
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
// The Original Code is TMT19937RandomGenerator.cp.
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
//
// A C-program for MT19937, with initialization improved 2002/2/10.
// Coded by Takuji Nishimura and Makoto Matsumoto.
// This is a faster version by taking Shawn Cokus's optimization,
// Matthe Bellew's simplification, Isaku Wada's real version.
// 
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
// 
// 
// Any feedback is very welcome.
// http://www.math.keio.ac.jp/matumoto/emt.html
// email: matumoto@math.keio.ac.jp
// ===========
// $Id: TMT19937RandomGenerator.cp,v 1.2 2004/11/23 15:11:31 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Math/TMT19937RandomGenerator.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TMT19937RandomGenerator( KUInt32 )
// -------------------------------------------------------------------------- //
TMT19937RandomGenerator::TMT19937RandomGenerator( KUInt32 inSeed )
	:
		mLastState( &mState[N-1] )
{
	InitWithSeed( inSeed );
	ComputeNextStates();
}

// -------------------------------------------------------------------------- //
//  * TMT19937RandomGenerator( KUInt32*, int )
// -------------------------------------------------------------------------- //
TMT19937RandomGenerator::TMT19937RandomGenerator(
										KUInt32* inInitKey, int inKeyLength )
{
	// First initialize with some seed.
	InitWithSeed( 19650218UL );
	
	// Then initialize with the array.
	int i, j, k;
	i=1; j=0;
	k = (N>inKeyLength ? N : inKeyLength);
	for (; k; k--) {
		mState[i] = (mState[i] ^ ((mState[i-1] ^ (mState[i-1] >> 30)) * 1664525UL))
		  + inInitKey[j] + j; // non linear
		i++; j++;
		if (i>=N) { mState[0] = mState[N-1]; i=1; }
		if (j>=inKeyLength) j=0;
	}
	for (k=N-1; k; k--) {
		mState[i] = (mState[i] ^ ((mState[i-1] ^ (mState[i-1] >> 30)) * 1566083941UL))
		  - i; // non linear
		i++;
		if (i>=N) { mState[0] = mState[N-1]; i=1; }
	}

	mState[0] = 0x80000000UL; // MSB is 1; assuring non-zero initial array
	ComputeNextStates();
}

// -------------------------------------------------------------------------- //
//  * InitWithSeed( KUInt32 )
// -------------------------------------------------------------------------- //
void
TMT19937RandomGenerator::InitWithSeed( KUInt32 inSeed )
{
	// initializes mState[N] with a seed
    int indexStates;
    mState[0] = inSeed & 0xffffffffUL;
	for (indexStates=1; indexStates < N; indexStates++)
	{
		mState[indexStates] = (1812433253UL * (mState[indexStates-1] ^ (mState[indexStates-1] >> 30)) + indexStates); 
		// See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
		// In the previous versions, MSBs of the seed affect
		// only MSBs of the array mState[].
		// 2002/01/09 modified by Makoto Matsumoto
		mState[indexStates] &= 0xffffffffUL;  // for >32 bit machines
	}
}

// -------------------------------------------------------------------------- //
//  * ComputeNextStates( void )
// -------------------------------------------------------------------------- //
void
TMT19937RandomGenerator::ComputeNextStates( void )
{
	KUInt32* statesCursor = mState;
	int indexStates;

	mNextState = mState;
	
	for (indexStates=N-M+1; --indexStates; statesCursor++) 
		*statesCursor = statesCursor[M] ^ TWIST(statesCursor[0], statesCursor[1]);

	for (indexStates=M; --indexStates; statesCursor++) 
		*statesCursor = statesCursor[M-N] ^ TWIST(statesCursor[0], statesCursor[1]);

	*statesCursor = statesCursor[M-N] ^ TWIST(statesCursor[0], mState[0]);
}

// -------------------------------------------------------------------------- //
//  * Int_32( void )
// -------------------------------------------------------------------------- //
KUInt32
TMT19937RandomGenerator::Int_32(void)
{
	KUInt32 theResult;

	theResult = *mNextState++;
	if (mLastState == mNextState)
	{
		ComputeNextStates();
	}

	/* Tempering */
	theResult ^= (theResult >> 11);
	theResult ^= (theResult << 7) & 0x9d2c5680UL;
	theResult ^= (theResult << 15) & 0xefc60000UL;
	theResult ^= (theResult >> 18);

	return theResult;
}

// =============== //
// Please go away. //
// =============== //
