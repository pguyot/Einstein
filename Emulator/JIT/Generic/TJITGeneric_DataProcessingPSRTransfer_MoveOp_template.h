// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_MoveOp_template.h
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

// MOV, MVN
// -- Imm, ImmC, NoShift, Regular
/*
#define MODE Imm
#define MODE NoShift
#define MODE Regular
#define MODE ImmC
x
Rd
x
FLAG_S

#define OP	MOV
#define OP	MVN
*/

MoveOp(OP, MODE, FLAG_S, Rd)
#if IMPLEMENTATION
{
#if (MODE == Imm) || (MODE == ImmC)
	KUInt32 Opnd2;
	POPVALUE(Opnd2);
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
#elif FLAG_S
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	POPPC();
	bool carry = false;
	const KUInt32 Opnd2 = GetShift( ioCPU, theInstruction, &carry, GETPC() );
#else
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	POPPC();
	const KUInt32 Opnd2 = GetShiftNoCarry( ioCPU, theInstruction, ioCPU->mCPSR_C, GETPC() );
#endif
#if (OP == MOV)
	const KUInt32 theResult = Opnd2;
#elif (OP == MVN)
	const KUInt32 theResult = ~ Opnd2;
#endif
#if Rd == 15
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
		#if (MODE == NoShift) || (MODE == Imm)
			SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
		#elif (MODE == ImmC)
			SetCPSRBitsForLogicalOp( ioCPU, theResult, Opnd2 & 0x80000000 );
		#else
			SetCPSRBitsForLogicalOp( ioCPU, theResult, carry );
		#endif
	#endif
#endif
#if Rd == 15
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
