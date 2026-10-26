// ==============================
// Fichier:			TCircleBuffer.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
//
// Créé le:			26/5/2005
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
TCircleBuffer::TCircleBuffer(KUIntPtr inInitialCapacity /* = 0 */) :
		mBuffer(nil),
		mProducerCrsr(0),
		mConsumerCrsr(0),
		mBufferSize(inInitialCapacity)
{
	mBuffer = (KUInt8*) ::malloc(inInitialCapacity);
}

// -------------------------------------------------------------------------- //
//  * ~TCircleBuffer( void )
// -------------------------------------------------------------------------- //
TCircleBuffer::~TCircleBuffer(void)
{
	if (mBuffer)
	{
		::free(mBuffer);
	}
}

// -------------------------------------------------------------------------- //
//  * Produce( const void*, KUIntPtr )
//		One byte always stays free: in a full buffer, the producer cursor would
//		be on the consumer cursor, and the buffer would look empty. Both
//		cursors are always below mBufferSize.
// -------------------------------------------------------------------------- //
void
TCircleBuffer::Produce(const void* inBuffer, KUIntPtr inAmount)
{
	// Enlarge the buffer if required.
	KUIntPtr amountUsed = AvailableBytes();
	if (inAmount >= mBufferSize - amountUsed)
	{
		KUIntPtr newSize = amountUsed + inAmount + 1;
		KUInt8* newBuffer = (KUInt8*) ::malloc(newSize);

		// Copy data, put unconsumed data at the beginning.
		(void) Peek(newBuffer, amountUsed);
		::free(mBuffer);
		mBuffer = newBuffer;
		mBufferSize = newSize;
		mConsumerCrsr = 0;
		mProducerCrsr = amountUsed;
	}

	// Copy data, to the end of the buffer and then from its beginning.
	KUIntPtr toEnd = mBufferSize - mProducerCrsr;
	if (inAmount > toEnd)
	{
		KUIntPtr fromBeginning = inAmount - toEnd;
		(void) ::memcpy(
			mBuffer + mProducerCrsr,
			inBuffer,
			toEnd);
		(void) ::memcpy(
			mBuffer,
			((const KUInt8*) inBuffer) + toEnd,
			fromBeginning);
		mProducerCrsr = fromBeginning;
	} else
	{
		(void) ::memcpy(
			mBuffer + mProducerCrsr,
			inBuffer,
			inAmount);
		mProducerCrsr += inAmount;
		if (mProducerCrsr == mBufferSize)
		{
			mProducerCrsr = 0;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * AvailableBytes();
// -------------------------------------------------------------------------- //
KUIntPtr
TCircleBuffer::AvailableBytes()
{
	KUIntPtr max = 0;
	if (mConsumerCrsr <= mProducerCrsr)
	{
		// ---C123P---
		max = mProducerCrsr - mConsumerCrsr;
	} else
	{
		// 456P---C123
		max = mBufferSize + mProducerCrsr - mConsumerCrsr;
	}
	return max;
}

// -------------------------------------------------------------------------- //
//  * Consume( void*, KUIntPtr )
// -------------------------------------------------------------------------- //
KUIntPtr
TCircleBuffer::Consume(void* outBuffer, KUIntPtr inAmount)
{
	KUIntPtr amount = Peek(outBuffer, inAmount);
	Discard(amount);
	return amount;
}

// -------------------------------------------------------------------------- //
//  * Peek( void*, KUIntPtr )
// -------------------------------------------------------------------------- //
KUIntPtr
TCircleBuffer::Peek(void* outBuffer, KUIntPtr inAmount)
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
			amount);
	} else
	{
		// 456P---C123
		KUIntPtr max = mBufferSize + mProducerCrsr - mConsumerCrsr;
		if (amount > max)
		{
			amount = max;
		}

		if (amount + mConsumerCrsr > mBufferSize)
		{
			KUIntPtr toEnd = mBufferSize - mConsumerCrsr;
			KUIntPtr fromBeginning = amount - toEnd;
			(void) ::memcpy(
				outBuffer,
				(const void*) (mBuffer + mConsumerCrsr),
				toEnd);
			(void) ::memcpy(
				((KUInt8*) outBuffer) + toEnd,
				(const void*) mBuffer,
				fromBeginning);
		} else
		{
			(void) ::memcpy(
				outBuffer,
				(const void*) (mBuffer + mConsumerCrsr),
				amount);
		}
	}

	return amount;
}

// -------------------------------------------------------------------------- //
//  * Discard( KUIntPtr )
// -------------------------------------------------------------------------- //
void
TCircleBuffer::Discard(KUIntPtr inAmount)
{
	KUIntPtr amount = inAmount;
	KUIntPtr max = AvailableBytes();

	if (amount > max)
	{
		amount = max;
	}

	if (amount + mConsumerCrsr >= mBufferSize)
	{
		mConsumerCrsr = amount - (mBufferSize - mConsumerCrsr);
	} else
	{
		mConsumerCrsr += amount;
	}
}

// ============================================================================== //
// Giving up on assembly language was the apple in our Garden of Eden:  Languages //
// whose use squanders machine cycles are sinful.  The LISP machine now permits   //
// LISP programmers to abandon bra and fig-leaf.                                  //
//                 -- Epigrams in Programming, ACM SIGPLAN Sept. 1982             //
// ============================================================================== //
