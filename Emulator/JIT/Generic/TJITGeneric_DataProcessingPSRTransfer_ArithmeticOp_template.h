// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp_template.h
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
Rd
x
Rn
x
FLAG_S

#define OP	SUB
#define OP	RSB
#define OP	ADD
#define OP	ADC
#define OP	SBC
#define OP	RSC
*/
#if OP != SUB && OP != RSB && OP != ADD && OP != ADC && OP != SBC && OP != RSC
	#error "Please define OP to be SUB, RSB, ADD, ADC, SBC or RSC"
#endif
#if SUB == RSB || SUB == ADD || SUB == ADC || SUB == SBC || SUB == RSC \
	|| RSB == ADD || RSB == ADC || RSB == SBC || RSB == RSC || ADD == ADC \
	|| ADD == SBC || ADD == RSC || ADC == SBC || ADC == RSC || SBC == RSC
	#error "Please define SUB, RSB, ADD, ADC, SBC and RSC to be different"
#endif
#if MODE != Imm && MODE != ImmC && MODE != NoShift && MODE != Regular
	#error "Please define MODE to be Imm, ImmC, NoShift or Regular"
#endif
#if Imm == ImmC || Imm == NoShift || Imm == Regular || ImmC == NoShift || ImmC == Regular || NoShift == Regular
	#error "Please define Imm, ImmC, NoShift and Regular to be different"
#endif

ArithmeticOp(OP, MODE, FLAG_S, Rn, Rd)
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
#if (OP == SUB)
	const KUInt32 theResult = Opnd1 - Opnd2;
#elif (OP == RSB)
	const KUInt32 theResult = Opnd2 - Opnd1;
#elif (OP == ADD)
	const KUInt32 theResult = Opnd1 + Opnd2;
#elif (OP == ADC)
	const KUInt32 theResult = Opnd1 + Opnd2 + ioCPU->mCPSR_C;
#elif (OP == SBC)
	const KUInt32 theResult = Opnd1 - Opnd2 - 1 + ioCPU->mCPSR_C;
#elif (OP == RSC)
	const KUInt32 theResult = Opnd2 - Opnd1 - 1 + ioCPU->mCPSR_C;
#endif
#if (Rd == 15)
	#if !FLAG_S
		CALLNEXT_SAVEPC;
	#endif
	SETPC(theResult + 4);
	#if FLAG_S
		ioCPU->SetCPSR( ioCPU->GetSPSR() );
	#endif
#else
	ioCPU->mCurrentRegisters[Rd] = theResult;
	#if FLAG_S
		const KUInt32 Negative1 = Opnd1 & 0x80000000;
		const KUInt32 Negative2 = Opnd2 & 0x80000000;
		const KUInt32 NegativeR = theResult & 0x80000000;
		#if (OP == SUB) || (OP == SBC)
			SetCPSRBitsForArithmeticOp(
				ioCPU,
				theResult,
				(Negative1 && !Negative2)
				|| (Negative1 && !NegativeR)
				|| (!Negative2 && !NegativeR),
				(Negative1 && !Negative2 && !NegativeR)
				|| (!Negative1 && Negative2 && NegativeR));
		#elif (OP == RSB) || (OP == RSC)
			SetCPSRBitsForArithmeticOp(
				ioCPU,
				theResult,
				(Negative2 && !Negative1)
				|| (Negative2 && !NegativeR)
				|| (!Negative1 && !NegativeR),
				(Negative2 && !Negative1 && !NegativeR)
				|| (!Negative2 && Negative1 && NegativeR));
		#elif (OP == ADD) || (OP == ADC)
			SetCPSRBitsForArithmeticOp(
				ioCPU,
				theResult,
				(Negative1 && Negative2)
				|| ((Negative1 || Negative2) && !NegativeR),
				(Negative1 == Negative2)
				&& (Negative1 != NegativeR));
		#endif
	#endif
#endif
#if (Rd == 15)
	#if FLAG_S
		MMUCALLNEXT_AFTERSETPC;
	#else
		FURTHERCALLNEXT_AFTERSETPC;
	#endif
#else
	CALLNEXTUNIT;
#endif
}
#endif
