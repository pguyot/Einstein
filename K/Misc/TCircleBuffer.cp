// ==============================
// Fichier:			TCircleBuffer.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// CrŽŽ le:			26/5/2005
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
// The Original Code is TCircleBuffer.cp.
// 
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2005 the
// Initial Developer. All Rights Reserved.
// 
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
// 
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TCircleBuffer.cp,v 1.3 2005/05/27 01:52:08 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TCircleBuffer.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <string.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TCircleBuffer( KUIntPtr )
// -------------------------------------------------------------------------- //
TCircleBuffer::TCircleBuffer( KUIntPtr inInitialCapacity /* = 0 */ )
	:
		mBuffer( nil ),
		mProducerCrsr( 0 ),
		mConsumerCrsr( 0 ),
		mBufferSize( inInitialCapacity )
{
	mBuffer = (KUInt8*) ::malloc( inInitialCapacity );
}

// -------------------------------------------------------------------------- //
//  * ~TCircleBuffer( void )
// -------------------------------------------------------------------------- //
TCircleBuffer::~TCircleBuffer( void )
{
	if (mBuffer)
	{
		::free( mBuffer );
	}
}

// -------------------------------------------------------------------------- //
//  * Produce( const void*, KUIntPtr )
// -------------------------------------------------------------------------- //
void
TCircleBuffer::Produce( const void* inBuffer, KUIntPtr inAmount )
{
	// Enlarge the buffer if required.
	KUIntPtr amountFree;
	if (mConsumerCrsr <= mProducerCrsr)
	{
		// ---C123P---
		amountFree = mBufferSize - (mProducerCrsr - mConsumerCrsr);
	} else {
		// 456P---C123
		amountFree = mProducerCrsr - mConsumerCrsr;
	}
	
	if (inAmount > amountFree)
	{
		KUIntPtr delta = inAmount - amountFree;
		KUInt8* newBuffer = (KUInt8*) ::malloc( mBufferSize + delta );
		
		// Copy data, put unconsumed data at the beginning.
		if (mConsumerCrsr <= mProducerCrsr)
		{
			// ---C123P--- -> C123P--------
			(void) ::memcpy(
				mBuffer,
				(const void*) (mBuffer + mConsumerCrsr),
				mProducerCrsr - mConsumerCrsr);
			mProducerCrsr -= mConsumerCrsr;
			mConsumerCrsr = 0;
		} else {
			// 456P---C123 -> C123456P-----
			(void) ::memcpy(
				mBuffer,
				(const void*) (mBuffer + mConsumerCrsr),
				mBufferSize - mConsumerCrsr);
			(void) ::memcpy(
				mBuffer + (mBufferSize - mConsumerCrsr),
				(const void*) mBuffer,
				mProducerCrsr);
			mProducerCrsr += mBufferSize - mConsumerCrsr;
			mConsumerCrsr = 0;
		}
		
		// Update buffer.
		mBufferSize += delta;
		::free(mBuffer);
		mBuffer = newBuffer;
	}
	
	// Copy data.
	if (mConsumerCrsr <= mProducerCrsr)
	{
		// ---C123P---
		if (inAmount + mProducerCrsr > mBufferSize)
		{
			KUIntPtr toEnd = mBufferSize - mProducerCrsr;
			KUIntPtr fromBeginning = inAmount - toEnd;
			(void) ::memcpy(
				mBuffer + mProducerCrsr,
				inBuffer,
				toEnd );
			(void) ::memcpy(
				mBuffer,
				((KUInt8*) inBuffer) + toEnd,
				fromBeginning );
			mProducerCrsr = fromBeginning;
		} else {
			(void) ::memcpy(
				mBuffer + mProducerCrsr,
				inBuffer,
				inAmount );
			mProducerCrsr += inAmount;
		}
	} else {
		// 456P---C123
		(void) ::memcpy(
			mBuffer + mProducerCrsr,
			inBuffer,
			inAmount );
		mProducerCrsr += inAmount;
	}
}

// -------------------------------------------------------------------------- //
//  * Consume( void*, KUIntPtr )
// -------------------------------------------------------------------------- //
KUIntPtr
TCircleBuffer::Consume( void* outBuffer, KUIntPtr inAmount )
{
	KUIntPtr amount = inAmount;
	
	// Copy data.
	if (mConsumerCrsr <= mProducerCrsr)
	{
		// ---C123P---
		KUIntPtr max = mProducerCrsr - mConsumerCrsr;
		if (amount > max)
		{
			amount = max;
		}

		(void) ::memcpy(
			outBuffer,
			(const void*) (mBuffer + mConsumerCrsr),
			amount );

		mConsumerCrsr += amount;
	} else {
		// 456P---C123
		KUIntPtr max = mBufferSize + mProducerCrsr - mConsumerCrsr;
		if (amount > max)
		{
			amount = max;
		}

		if (amount + mConsumerCrsr > mBufferSize)
		{
			KUIntPtr toEnd = mBufferSize - mConsumerCrsr;
			KUIntPtr fromBeginning = inAmount - toEnd;
			(void) ::memcpy(
				outBuffer,
				(const void*) (mBuffer + mConsumerCrsr),
				toEnd );
			(void) ::memcpy(
				((KUInt8*) outBuffer) + toEnd,
				(const void*) mBuffer,
				fromBeginning );
			mConsumerCrsr = fromBeginning;
		} else {
			(void) ::memcpy(
				outBuffer,
				(const void*) (mBuffer + mConsumerCrsr),
				inAmount );
			mConsumerCrsr += inAmount;
		}
	}
	
	return amount;
}

// ============================================================================== //
// Giving up on assembly language was the apple in our Garden of Eden:  Languages //
// whose use squanders machine cycles are sinful.  The LISP machine now permits   //
// LISP programmers to abandon bra and fig-leaf.                                  //
//                 -- Epigrams in Programming, ACM SIGPLAN Sept. 1982             //
// ============================================================================== //
