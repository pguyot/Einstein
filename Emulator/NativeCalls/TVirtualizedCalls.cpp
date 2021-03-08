// ==============================
// File:			TVirtualizedCalls.cp
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#include "TVirtualizedCalls.h"
#include "TVirtualizedCallsPatches.h"

// Einstein
#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/TMemoryConsts.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define min(a,b) (a) < (b) ? (a) : (b)

// -------------------------------------------------------------------------- //
//  * TVirtualizedCalls( void )
// -------------------------------------------------------------------------- //
TVirtualizedCalls::TVirtualizedCalls(
		TEmulator* inEmulator,
		TMemory* inMemoryIntf,
		TARMProcessor* inProcessor )
	:
		mEmulator( inEmulator ),
		mMemoryIntf( inMemoryIntf ),
		mProcessor( inProcessor )
{
}

// -------------------------------------------------------------------------- //
//  * ~TVirtualizedCalls( void )
// -------------------------------------------------------------------------- //
TVirtualizedCalls::~TVirtualizedCalls( void )
{
}

// -------------------------------------------------------------------------- //
//  * __rt_sdiv( void )
// -------------------------------------------------------------------------- //
inline void
TVirtualizedCalls::__rt_sdiv( void )
{
	KSInt32 divider = mProcessor->GetRegister(0);
	KSInt32 dividend = mProcessor->GetRegister(1);
	KSInt32 result = dividend / divider;
	KSInt32 remainder = dividend % divider;
	mProcessor->SetRegister(0, result);
	mProcessor->SetRegister(1, remainder);
}

// -------------------------------------------------------------------------- //
//  * __rt_udiv( void )
// -------------------------------------------------------------------------- //
inline void
TVirtualizedCalls::__rt_udiv( void )
{
	KUInt32 divider = mProcessor->GetRegister(0);
	KUInt32 dividend = mProcessor->GetRegister(1);
	KUInt32 result = dividend / divider;
	KUInt32 remainder = dividend % divider;
	mProcessor->SetRegister(0, result);
	mProcessor->SetRegister(1, remainder);
}

// -------------------------------------------------------------------------- //
//  * memmove_slow( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
inline void
TVirtualizedCalls::memmove_slow( KUInt32 inDst, KUInt32 inSrc, KUInt32 inLen )
{
	// Slow version of memmove, typically for memmove with hardware.
	KUInt32 len = inLen;
	KUInt32 src = inSrc;
	KUInt32 dst = inDst;
	TMemory* theMemoryIntf = mMemoryIntf;
	
//	if ((src & 0x3) || (dst & 0x3) || (len & 0x3))
//	{
//		KPrintf( "unaligned memmove( dst=%.8X, src=%.8X, len=%.8X )\n", inDst, inSrc, inLen );
//	} else {
//		KPrintf( "  aligned memmove( dst=%.8X, src=%.8X, len=%.8X )\n", inDst, inSrc, inLen );
//	}

	while (len > 3)
	{
		KUInt32 word;
		if (theMemoryIntf->Read( src, word ))
		{
			len = 0;
			break;
		}
		if (theMemoryIntf->Write( dst, word ))
		{
			len = 0;
			break;
		}
		
		src += 4;
		dst += 4;
		len -= 4;
	}

	while (len)
	{
		KUInt8 byte;
		if (theMemoryIntf->ReadB( src, byte ))
		{
			break;
		}
		if (theMemoryIntf->WriteB( dst, byte ))
		{
			break;
		}
		
		src += 1;
		dst += 1;
		len -= 1;
	}
}

// -------------------------------------------------------------------------- //
//  * memmove( void )
// -------------------------------------------------------------------------- //
// BROKEN ON LITTLE ENDIAN
inline void
TVirtualizedCalls::memmove( void )
{
	KUInt32 dst = mProcessor->GetRegister(0);
	KUInt32 src = mProcessor->GetRegister(1);
	KUInt32 len = mProcessor->GetRegister(2);
	TMemory* theMemoryIntf = mMemoryIntf;
	
//	memmove_slow(dst, src, len);

	do {
		if ((dst == src) || (len == 0))
		{
			break;
		}

		// Optimization: consider page by page.
		const KUInt8* srcPtr;
		if (theMemoryIntf->GetDirectPointerToROMRAM(src, &srcPtr))
		{
			memmove_slow(dst, src, len);
			break;
		}
		KUInt8* dstPtr;
		if (theMemoryIntf->GetDirectPointerToRAM(dst, &dstPtr))
		{
			memmove_slow(dst, src, len);
			break;
		}
		
		KUInt32 baseSrc = src & TMemoryConsts::kMMUSmallestPageMask;
		KUInt32 baseDst = dst & TMemoryConsts::kMMUSmallestPageMask;
		KUInt32 maxSrc = baseSrc - src + TMemoryConsts::kMMUSmallestPageSize;
		KUInt32 maxDst = baseDst - dst + TMemoryConsts::kMMUSmallestPageSize;
		
		do {
			KUInt32 amount = min(min(len, maxSrc), maxDst);
			(void) ::memmove( dstPtr, srcPtr, amount );
			len -= amount;
			if (len == 0)
			{
				break;
			}
			
			src += amount;
			dst += amount;
			if (amount == maxSrc)
			{
				if (theMemoryIntf->GetDirectPointerToROMRAM(src, &srcPtr))
				{
					theMemoryIntf->SetHardwareFault(src);
					break;
				}
				baseSrc += TMemoryConsts::kMMUSmallestPageSize;
				maxSrc = TMemoryConsts::kMMUSmallestPageSize;
			} else {
				maxSrc -= amount;
				srcPtr += amount;
			}
			if (amount == maxDst)
			{
				if (theMemoryIntf->GetDirectPointerToRAM(dst, &dstPtr))
				{
					theMemoryIntf->SetHardwareFault(dst);
					break;
				}
				baseDst += TMemoryConsts::kMMUSmallestPageSize;
				maxDst = TMemoryConsts::kMMUSmallestPageSize;
			} else {
				maxDst -= amount;
				dstPtr += amount;
			}
		} while (true);
	} while (false);
	
	// mProcessor->SetRegister(0, dst);
}

// -------------------------------------------------------------------------- //
//  * symcmp__FPcT1( void )
// -------------------------------------------------------------------------- //
inline void
TVirtualizedCalls::symcmp__FPcT1( void )
{
	KUInt32 s1 = mProcessor->GetRegister(0);
	KUInt32 s2 = mProcessor->GetRegister(1);
	TMemory* theMemoryIntf = mMemoryIntf;
	KUInt32 result = 0;
	
	// Read 4 by 4.
	KUInt32 buffer1;
	KUInt32 buffer2;
	do {
		if (theMemoryIntf->Read(s1, buffer1)) break;
		if (theMemoryIntf->Read(s2, buffer2)) break;
		
		// Equal fast.
		if (buffer1 == buffer2)
		{
			if ((!(buffer1 & 0xFF000000))
				|| !(buffer1 & 0x00FF0000)
				|| !(buffer1 & 0x0000FF00)
				|| !(buffer1 & 0x000000FF))
			{
				break;
			}
			s1 += 4;
			s2 += 4;
		} else {
			// Consider each character.
			KUInt8 char1 = buffer1 >> 24;
			KUInt8 char2 = buffer2 >> 24;
			if (char1 != char2)
			{
				if ((char1 >= 0x61) && (char1 <= 0x7A))
				{
					char1 -= 0x20;
				}
				if ((char2 >= 0x61) && (char2 <= 0x7A))
				{
					char2 -= 0x20;
				}
				if (char1 < char2)
				{
					result = (KUInt32) -1;
					break;
				} else if (char1 > char2) {
					result = 1;
					break;
				}
			} else if (char1 == 0) {
				break;
			}

			char1 = (buffer1 >> 16) & 0xFF;
			char2 = (buffer2 >> 16) & 0xFF;
			if (char1 != char2)
			{
				if ((char1 >= 0x61) && (char1 <= 0x7A))
				{
					char1 -= 0x20;
				}
				if ((char2 >= 0x61) && (char2 <= 0x7A))
				{
					char2 -= 0x20;
				}
				if (char1 < char2)
				{
					result = (KUInt32) -1;
					break;
				} else if (char1 > char2) {
					result = 1;
					break;
				}
			} else if (char1 == 0) {
				break;
			}

			char1 = (buffer1 >> 8) & 0xFF;
			char2 = (buffer2 >> 8) & 0xFF;
			if (char1 != char2)
			{
				if ((char1 >= 0x61) && (char1 <= 0x7A))
				{
					char1 -= 0x20;
				}
				if ((char2 >= 0x61) && (char2 <= 0x7A))
				{
					char2 -= 0x20;
				}
				if (char1 < char2)
				{
					result = (KUInt32) -1;
					break;
				} else if (char1 > char2) {
					result = 1;
					break;
				}
			} else if (char1 == 0) {
				break;
			}

			char1 = buffer1 & 0xFF;
			char2 = buffer2 & 0xFF;
			if (char1 != char2)
			{
				if ((char1 >= 0x61) && (char1 <= 0x7A))
				{
					char1 -= 0x20;
				}
				if ((char2 >= 0x61) && (char2 <= 0x7A))
				{
					char2 -= 0x20;
				}
				if (char1 < char2)
				{
					result = (KUInt32) -1;
					break;
				} else if (char1 > char2) {
					result = 1;
					break;
				}
			} else if (char1 == 0) {
				break;
			}

			s1 += 4;
			s2 += 4;
		}
	} while (true);

	mProcessor->SetRegister(0, result);
}


// -------------------------------------------------------------------------- //
//  * Execute( KUInt32 )
// -------------------------------------------------------------------------- //
void
TVirtualizedCalls::Execute( KUInt32 inInstruction )
{
	switch (inInstruction)
	{
		case TVirtualizedCallsPatches::k__rt_sdiv:
			__rt_sdiv();
			break;

		case TVirtualizedCallsPatches::k__rt_udiv:
			__rt_udiv();
			break;

		case TVirtualizedCallsPatches::kmemmove:
			memmove();
			break;

		case TVirtualizedCallsPatches::ksymcmp__FPcT1:
			symcmp__FPcT1();
			break;

		default:
			break;
	}
}



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
