// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_TestOp_template.h
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

// SUB, RSB, ADD, ADC, SBC, RSC
// -- Imm, ImmC, NoShift, Regular
/*
#define MODE Imm
#define MODE NoShift
#define MODE Regular
#define MODE ImmC
x
Rn

#define OP	TST
#define OP	TEQ
#define OP	CMP
#define OP	CMN
*/

#if OP != TST && OP != TEQ && OP != CMP && OP != CMN
	#error "Please define OP to be TST, TEQ, CMP or CMN"
#endif
#if TST == TEQ || TST == CMP || TST == CMN || TEQ == CMP || TEQ == CMN \
	|| CMP == CMN
	#error "Please define TST, TEQ, CMP and CMN to be different"
#endif
#if MODE != Imm && MODE != ImmC && MODE != NoShift && MODE != Regular
	#error "Please define MODE to be Imm, ImmC, NoShift or Regular"
#endif
#if Imm == ImmC || Imm == NoShift || Imm == Regular || ImmC == NoShift || ImmC == Regular || NoShift == Regular
	#error "Please define Imm, ImmC, NoShift and Regular to be different"
#endif

TestOp(OP, MODE, Rn)
#if IMPLEMENTATION
{
#if (MODE == Imm) || (MODE == ImmC)
	KUInt32 Opnd2;
	POPVALUE(Opnd2);
	#if (Rn == 15)
	POPPC();
	#endif
#elif MODE == NoShift
	KUInt32 Rm;
	POPVALUE(Rm);
	KUInt32 Opnd2;
	POPPC();
	if (Rm == 15)
	{
		Opnd2 = GETPC();
	} else {
		Opnd2 = ioCPU->mCurrentRegisters[Rm];
	}
#elif OP == TST || OP == TEQ
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	Boolean carry = false;
	POPPC();
	KUInt32 Opnd2 = GetShift( ioCPU, theInstruction, &carry, GETPC() );
#else
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	POPPC();
	KUInt32 Opnd2 = GetShiftNoCarry( ioCPU, theInstruction, ioCPU->mCPSR_C, GETPC() );
#endif
#if (Rn == 15)
	KUInt32 Opnd1 = GETPC();
#else
	KUInt32 Opnd1 = ioCPU->mCurrentRegisters[Rn];
#endif
#if OP == TST
	const KUInt32 theResult = Opnd1 & Opnd2;
#elif OP == TEQ
	const KUInt32 theResult = Opnd1 ^ Opnd2;
#elif OP == CMP
	const KUInt32 theResult = Opnd1 - Opnd2;
#elif OP == CMN
	const KUInt32 theResult = Opnd1 + Opnd2;
#endif
#if OP == TST || OP == TEQ
	#if (MODE == NoShift) || (MODE == Imm)
		SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
	#elif (MODE == ImmC)
		SetCPSRBitsForLogicalOp( ioCPU, theResult, Opnd2 & 0x80000000 );
	#else
		SetCPSRBitsForLogicalOp( ioCPU, theResult, carry );
	#endif
#elif (OP == CMP) || (OP == CMN)
	const KUInt32 Negative1 = Opnd1 & 0x80000000;
	const KUInt32 Negative2 = Opnd2 & 0x80000000;
	const KUInt32 NegativeR = theResult & 0x80000000;
	#if OP == CMP
		SetCPSRBitsForArithmeticOp(
			ioCPU,
			theResult,
			(Negative1 && !Negative2)
			|| (Negative1 && !NegativeR)
			|| (!Negative2 && !NegativeR),
			(Negative1 && !Negative2 && !NegativeR)
			|| (!Negative1 && Negative2 && NegativeR));
	#elif OP == CMN
		SetCPSRBitsForArithmeticOp(
			ioCPU,
			theResult,
			(Negative1 && Negative2)
			|| ((Negative1 || Negative2) && !NegativeR),
			(Negative1 == Negative2)
			&& (Negative1 != NegativeR));
	#endif
#endif
	CALLNEXTUNIT;
}
#endif
