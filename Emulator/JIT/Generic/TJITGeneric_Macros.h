// ==============================
// File:			TJITGeneric_Macros.h
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

#ifndef _TJITGENERIC_MACROS_H
#define _TJITGENERIC_MACROS_H

#define JITInstructionProto(name) \
	JITUnit* name(JITUnit* ioUnit, TARMProcessor* ioCPU)

// Those really access R15 (for now).
#define SETPC(newval) \
	ioCPU->mCurrentRegisters[TARMProcessor::kR15] = (newval)
#define THEPC \
	ioCPU->mCurrentRegisters[TARMProcessor::kR15]
#define GETPC() \
	thePC

#define PUSHVALUE(value) 									\
	inPage->PushUnit(ioUnitCrsr, value)
#define POPVALUE(lref) 										\
	{														\
		lref = ioUnit[1].fValue;							\
		ioUnit++;											\
	}
#define PUSHFUNC(func)										\
	inPage->PushUnit(ioUnitCrsr, func)


// Set this value to 0 if you want to measure the performance of individual
// functions at the cost of lowering overall performance. This is not working
// perfectly well (NewtonOS will not boot all the way), but gives some
// rough results
#if 1

#define EXECUTENEXTUNIT \
	return ioUnit[1].fFuncPtr(&ioUnit[1], ioCPU)

#define CALLNEXTUNIT \
	return ioUnit[1].fFuncPtr(&ioUnit[1], ioCPU)

#define CALLUNIT(offset) \
	return ioUnit[offset].fFuncPtr(&ioUnit[offset], ioCPU)

#else

#define EXECUTENEXTUNIT \
	THEPC += 4; return &ioUnit[1]

#define CALLNEXTUNIT \
	THEPC += 4; return &ioUnit[1]

#define CALLUNIT(offset) \
	THEPC += 4; return &ioUnit[offset]

#endif


#define MMUCALLNEXT_AFTERSETPC \
	{														\
		TMemory* theMemIntf = ioCPU->GetMemory();			\
		return theMemIntf->GetJITObject()->GetJITUnitForPC( \
			ioCPU, theMemIntf, THEPC );						\
	}

#define MMUCALLNEXT(pc) \
	{														\
		TMemory* theMemIntf = ioCPU->GetMemory();			\
		SETPC(pc);											\
		return theMemIntf->GetJITObject()->GetJITUnitForPC( \
			ioCPU, theMemIntf, pc );						\
	}

#define MMUSMARTCALLNEXT(pc) \
	{														\
		static KSInt32 ioUnitOffset = kOffsetUnknown;		\
	again:													\
		if (ioUnitOffset<kOffsetUnknown) {					\
			CALLUNIT(ioUnitOffset);							\
		} else {											\
			TMemory* theMemIntf = ioCPU->GetMemory();		\
			if (ioUnitOffset!=kNotTheSamePage) {			\
				ioUnitOffset = theMemIntf->GetJITObject()->GetJITUnitDelta(ioCPU, theMemIntf, ioUnit, pc ); \
				goto again;									\
			}												\
			SETPC(pc);										\
			return theMemIntf->GetJITObject()->GetJITUnitForPC( \
				ioCPU, theMemIntf, pc );					\
		}													\
	}

#define POPPC() \
	KUInt32 thePC; \
	POPVALUE(thePC)

// TODO: fix FURTHERCALLNEXT
#define FURTHERCALLNEXT(pc)			MMUCALLNEXT(pc)
#define FURTHERCALLNEXT_AFTERSETPC	MMUCALLNEXT_AFTERSETPC
#define CALLNEXT_SAVEPC				{}

/*
#define FURTHERCALLNEXT(pc) \
	KUInt32 currentPC = pc;									\
	if (((savedPC - 8) & JITPageClass::kPageMask)		\
		== ((currentPC - 4) & JITPageClass::kPageMask))	\
	{														\
		KSInt32 delta = currentPC - savedPC;				\
		KSInt32 delta4 = delta / 4;							\
		return &ioUnit[delta4 + 1];							\
	} else {												\
		MMUCALLNEXT(pc);									\
	}
*/

#ifdef JIT_PERFORMANCE
#define COUNTHIT(counter, address) counter.hit(address);
#else
#define COUNTHIT(counter, address) 
#endif

// -------------------------------------------------------------------------- //
//  * GetShift( TARMProcessor*, KUInt32, Boolean* )
// -------------------------------------------------------------------------- //
inline KUInt32
GetShift(
				TARMProcessor* ioCPU,
				KUInt32 inShift,
				Boolean* outCarry,
				KUInt32 inPC )
{
	KUInt32 Shift = ((inShift & 0x00000FFF) >> 4);
	KUInt32 Rm = inShift & 0x0000000F;
	
	// Shift is not 0 here.	
	KUInt32 theResult;
	if (Rm == 15)
	{
		theResult = inPC;
	} else {
		theResult = ioCPU->mCurrentRegisters[Rm];
	}
	KUInt32 amount;
	KUInt32 registerBased = Shift & 0x1;   	// 0b000000001
	KUInt32 carry = 0;
	
	if (registerBased)
	{
		// Shift Register.
		// Shift amount is specified by bottom byte of Rs.
		// For all DataProcessing instructions with a shift by register,
		// specifying R15 for Rs (or Rd, Rm or Rn) has UNPREDICTABLE result.
		amount = ioCPU->mCurrentRegisters[Shift >> 4] & 0xFF;
	} else {
		// Shift Amount
		amount = Shift >> 3;
	}
	
	// Switch on the shift operation.
	switch ((Shift & 0x6) >> 1) // 0b00000110
	{
		case 0x0:
			// Logical Shift Left
			if (amount != 0)
			{
				// 3 cases: > 0, 32, > 32
				if (amount < 32)
				{
					carry = theResult & (1 << (32 - amount));
		
					theResult <<= amount;
				} else if (amount == 32) {
					// Bit 0 of Rm.
					carry = theResult & 1;
		
					theResult = 0;
				} else {
					theResult = 0;
				}
			} else if (!registerBased) {
				// LSL #0 is a special case where the carry out is the
				// C flag.
				carry = ioCPU->mCPSR_C;
			}
			break;
			
		case 0x1:
			// Logical Shift Right
			if (amount != 0)
			{
				// 3 cases: > 0, 32, > 32
				if (amount < 32)
				{
					carry = theResult & (1 << (amount - 1));
		
					theResult >>= amount;
				} else if (amount == 32) {
					// Bit 31 of Rm.
					carry = theResult & (1 << 31);
		
					theResult = 0;
				} else {
					theResult = 0;
				}
			} else if (!registerBased) {
				// LSR #0 is a special case meaning LSR #32
				// where the carry out is the bit 31 of Rm and
				// the result is 0.
				carry = theResult & 0x80000000;
				theResult = 0;
			}
			break;
			
		case 0x2:
			// Arithmetic Shift Right
			if ((amount == 0) && !registerBased)
			{
				// ASR #0 is a special case meaning ASR #32.
				amount = 32;
			}
			
			if (amount != 0)
			{
				if (amount < 32)
				{
					carry = theResult & ( 1 << (amount - 1) );
				
					if (theResult & 0x80000000)
					{
						theResult >>= amount;
						theResult |= (0xFFFFFFFF << (32 - amount));
					} else {
						theResult >>= amount;
					}
				} else  {
					// ASR by 32 or more is just like ASR #32.
					carry = theResult & 0x80000000;
					if (carry)
					{
						theResult = 0xFFFFFFFF;
					} else {
						theResult = 0;
					}
				}
			}
			break;
			
		case 0x3:
			// Rotate Right
			if (amount != 0)
			{
				if (registerBased && (amount > 32)) {
					// ROR by more than 32 are like ROR between 1 and 32.
					amount = ((amount - 1) & 0x1F) + 1;
				}
				
				carry = theResult & (1 << (32 - amount));
				theResult = (theResult >> amount)
					| (theResult << (32 - amount));
			} else if (!registerBased) {
				// ROR #0 actually is RRX
				carry = theResult & 0x1;
				if (ioCPU->mCPSR_C)
				{
					theResult = theResult >> 1 | 0x80000000;
				} else {
					theResult >>= 1;
				}
			}
			break;
	}
	
	if (carry)
	{
		*outCarry = true;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GetShiftNoCarry( KUInt32, KUInt32[32], Boolean )
// -------------------------------------------------------------------------- //
inline KUInt32
GetShiftNoCarry( TARMProcessor* ioCPU, KUInt32 inShift, Boolean inCPSR_C, KUInt32 inPC )
{
	KUInt32 Shift = ((inShift & 0x00000FFF) >> 4);
	KUInt32 Rm = inShift & 0x0000000F;
	
	// Shift is not 0 here.
	KUInt32 theResult;
	if (Rm == 15)
	{
		theResult = inPC;
	} else {
		theResult = ioCPU->mCurrentRegisters[Rm];
	}
	KUInt32 amount;
	KUInt32 registerBased = Shift & 0x1;   	// 0b000000001
	
	if (registerBased)
	{
		// Shift Register.
		// Shift amount is specified by bottom byte of Rs.
		// For all DataProcessing instructions with a shift by register,
		// specifying R15 for Rs (or Rd, Rm or Rn) has UNPREDICTABLE result.
		amount = ioCPU->mCurrentRegisters[Shift >> 4] & 0xFF;
	} else {
		// Shift Amount
		amount = Shift >> 3;
	}
	
	// Switch on the shift operation.
	switch ((Shift & 0x6) >> 1) // 0b00000110
	{
		case 0x0:
			// Logical Shift Left
			if (amount != 0)
			{
				if (amount < 32)
				{
					theResult <<= amount;
				} else {
					theResult = 0;
				}
//			} else if (!registerBased) {
				// LSL #0 is a special case where the carry out is the
				// C flag.
			}
			break;
			
		case 0x1:
			// Logical Shift Right
			if (amount != 0)
			{
				if (amount < 32)
				{
					theResult >>= amount;
				} else {
					theResult = 0;
				}
			} else if (!registerBased) {
				// LSR #0 is a special case meaning LSR #32
				// where the carry out is the bit 31 of Rm and
				// the result is 0.
				theResult = 0;
			}
			break;
			
		case 0x2:
			// Arithmetic Shift Right
			if ((amount == 0) && !registerBased)
			{
				// ASR #0 is a special case meaning ASR #32.
				amount = 32;
			}
			
			if (amount != 0)
			{
				if (amount < 32)
				{
					if (theResult & 0x80000000)
					{
						theResult >>= amount;
						theResult |= (0xFFFFFFFF << (32 - amount));
					} else {
					theResult >>= amount;
					}
				} else  {
					// ASR by 32 or more is just like ASR #32.
					if (theResult & 0x80000000)
					{
						theResult = 0xFFFFFFFF;
					} else {
						theResult = 0;
					}
				}
			}
			break;
			
		case 0x3:
			// Rotate Right
			if (amount != 0)
			{
				if (registerBased && (amount > 32)) {
					// ROR by more than 32 are like ROR between 1 and 32.
					amount = ((amount - 1) & 0x1F) + 1;
				}
				
				theResult = (theResult >> amount)
								| (theResult << (32 - amount));
			} else if (!registerBased) {
				// ROR #0 actually is RRX
				if (inCPSR_C)
				{
					theResult = theResult >> 1 | 0x80000000;
				} else {
					theResult >>= 1;
				}
			}
			break;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GetShiftNoCarryNoR15( KUInt32, KUInt32[32], Boolean )
// -------------------------------------------------------------------------- //
inline KUInt32
GetShiftNoCarryNoR15( KUInt32 inShift, KUInt32 inCurrentRegisters[16], Boolean inCPSR_C )
{
	KUInt32 Shift = ((inShift & 0x00000FFF) >> 5);
	KUInt32 Rm = inShift & 0x0000000F;
	
	// Shift is not 0 here.
	KUInt32 theResult;
	theResult = inCurrentRegisters[Rm];
	// Shift Amount
	KUInt32 amount = Shift >> 2;
	
	// Switch on the shift operation.
	switch (Shift & 0x3) // 0b0000011
	{
		case 0x0:
			// Logical Shift Left
			if (amount != 0)
			{
				if (amount < 32)
				{
					theResult <<= amount;
				} else {
					theResult = 0;
				}
			}
			break;
			
		case 0x1:
			// Logical Shift Right
			if (amount != 0)
			{
				if (amount < 32)
				{
					theResult >>= amount;
				} else {
					theResult = 0;
				}
			} else {
				// LSR #0 is a special case meaning LSR #32
				// where the carry out is the bit 31 of Rm and
				// the result is 0.
				theResult = 0;
			}
			break;
			
		case 0x2:
			// Arithmetic Shift Right
			if (amount == 0)
			{
				// ASR #0 is a special case meaning ASR #32.
				if (theResult & 0x80000000)
				{
					theResult = 0xFFFFFFFF;
				} else {
					theResult = 0;
				}
			} else {
				if (theResult & 0x80000000)
				{
					theResult >>= amount;
					theResult |= (0xFFFFFFFF << (32 - amount));
				} else {
					theResult >>= amount;
				}
			}
			break;
			
		case 0x3:
			// Rotate Right
			if (amount != 0)
			{
				theResult = (theResult >> amount)
								| (theResult << (32 - amount));
			} else {
				// ROR #0 actually is RRX
				if (inCPSR_C)
				{
					theResult = theResult >> 1 | 0x80000000;
				} else {
					theResult >>= 1;
				}
			}
			break;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * SetCPSRBitsForLogicalOp( TARMProcessor*, KUInt32, Boolean )
// -------------------------------------------------------------------------- //
inline void
SetCPSRBitsForLogicalOp( TARMProcessor* ioCPU, KUInt32 inResult, Boolean inCarry )
{
	if (inResult == 0)
	{
		ioCPU->mCPSR_Z = true;
		ioCPU->mCPSR_N = false;
	} else {
		ioCPU->mCPSR_Z = false;
		if (inResult & 0x80000000)
		{
			ioCPU->mCPSR_N = true;
		} else {
			ioCPU->mCPSR_N = false;
		}
	}
	
	ioCPU->mCPSR_C = inCarry;
}

// -------------------------------------------------------------------------- //
//  * SetCPSRBitsForLogicalOpLeaveCarry( TARMProcessor*, KUInt32 )
// -------------------------------------------------------------------------- //
inline void
SetCPSRBitsForLogicalOpLeaveCarry( TARMProcessor* ioCPU, KUInt32 inResult )
{
	if (inResult == 0)
	{
		ioCPU->mCPSR_Z = true;
		ioCPU->mCPSR_N = false;
	} else {
		ioCPU->mCPSR_Z = false;
		if (inResult & 0x80000000)
		{
			ioCPU->mCPSR_N = true;
		} else {
			ioCPU->mCPSR_N = false;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * SetCPSRBitsForArithmeticOp( TARMProcessor*, KUInt32, Boolean, Boolean )
// -------------------------------------------------------------------------- //
inline void
SetCPSRBitsForArithmeticOp(
					TARMProcessor* ioCPU,
					KUInt32 inResult,
					Boolean inCarry,
					Boolean inOverflow )
{
	if (inResult == 0)
	{
		ioCPU->mCPSR_Z = true;
		ioCPU->mCPSR_N = false;
	} else {
		ioCPU->mCPSR_Z = false;
		if (inResult & 0x80000000)
		{
			ioCPU->mCPSR_N = true;
		} else {
			ioCPU->mCPSR_N = false;
		}
	}
	ioCPU->mCPSR_C = inCarry;
	ioCPU->mCPSR_V = inOverflow;
}

// -------------------------------------------------------------------------- //
//  * CountBits( KUInt16 )
// -------------------------------------------------------------------------- //
inline KUInt32
CountBits( KUInt16 inWord )
{
	// http://www.caam.rice.edu/~dougm/twiddle/BitCount.html
	#define T unsigned
	#define ONES ((T)(-1))
	#define TWO(k) ((T)1 << (k))
	#define CYCL(k) (ONES/(1 + (TWO(TWO(k)))))
	#define BSUM(x,k) ((x)+=(x) >> TWO(k), (x) &= CYCL(k))
	inWord = (inWord & CYCL(0)) + ((inWord>>TWO(0)) & CYCL(0));
	inWord = (inWord & CYCL(1)) + ((inWord>>TWO(1)) & CYCL(1));
	BSUM(inWord,2);
	BSUM(inWord,3);
	
	return inWord;
}

#endif
		// _TJITGENERIC_MACROS_H

// ============================== //
// You're not Dave.  Who are you? //
// ============================== //
