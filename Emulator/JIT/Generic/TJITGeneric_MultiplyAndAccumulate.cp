// ==============================
// File:			TJITGeneric_MultiplyAndAccumulate.cp
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

#include <K/Defines/KDefinitions.h>
#include <K/Tests/KDebug.h>
#include "JIT.h"

// Einstein
#include "TARMProcessor.h"

#include "TJITGeneric_Macros.h"
#include "TJITGeneric_Other.h"

// -------------------------------------------------------------------------- //
//  * MultiplyAndAccumulate
// -------------------------------------------------------------------------- //
JITInstructionProto(MultiplyAndAccumulate)
{
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	
	const KUInt32 Rd = (theInstruction & 0x000F0000) >> 16;
	const KUInt32 Rn = (theInstruction & 0x0000F000) >> 12;
	const KUInt32 Rs = (theInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = theInstruction & 0x0000000F;

	// Accumulate
	const KUInt32 theResult = (ioCPU->mCurrentRegisters[Rm] * ioCPU->mCurrentRegisters[Rs]) + ioCPU->mCurrentRegisters[Rn];
	ioCPU->mCurrentRegisters[Rd] = theResult;

	CALLNEXTUNIT;
}

JITInstructionProto(MultiplyAndAccumulate_S)
{
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	
	const KUInt32 Rd = (theInstruction & 0x000F0000) >> 16;
	const KUInt32 Rn = (theInstruction & 0x0000F000) >> 12;
	const KUInt32 Rs = (theInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = theInstruction & 0x0000000F;

	// Accumulate
	const KUInt32 theResult = (ioCPU->mCurrentRegisters[Rm] * ioCPU->mCurrentRegisters[Rs]) + ioCPU->mCurrentRegisters[Rn];
	ioCPU->mCurrentRegisters[Rd] = theResult;

	// We track status flags
	if(theResult & 0x80000000)
		ioCPU->mCPSR_N = 1;
	else
		ioCPU->mCPSR_N = 0;
	
	if(theResult == 0)
		ioCPU->mCPSR_Z = 1;
	else
		ioCPU->mCPSR_Z = 0;

	CALLNEXTUNIT;
}


void
Translate_MultiplyAndAccumulate(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Get the index.
	const KUInt32 flag_s = (inInstruction & 0x00100000) >> 20;
	const KUInt32 Rd = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rn = (inInstruction & 0x0000F000) >> 12;
	const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = inInstruction & 0x0000000F;
	if ((Rd == 15) || (Rn == 15) || (Rs == 15) || (Rm == 15))
	{
		PUSHFUNC(UndefinedInstruction);
		PUSHVALUE(inVAddr + 8);
	} else {
		if (flag_s)
		{
			PUSHFUNC(MultiplyAndAccumulate_S);
		} else {
			PUSHFUNC(MultiplyAndAccumulate);
		}
		PUSHVALUE(inInstruction);
	}
}

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
