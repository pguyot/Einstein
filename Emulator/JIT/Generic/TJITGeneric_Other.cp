// ==============================
// File:			TJITGeneric_Other.cp
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
#include "TJITGenericPage.h"

// Einstein
#include "TARMProcessor.h"
#include "TEmulator.h"

#include "TJITGeneric_Macros.h"

// R15_SAFE

// -------------------------------------------------------------------------- //
//  * SWI
// -------------------------------------------------------------------------- //
JITInstructionProto(SWI)
{
	// Set the PC before jumping to the handler....
	POPPC();
	SETPC(GETPC());
	
	ioCPU->DoSWI();
	
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
}

// -------------------------------------------------------------------------- //
//  * CoprocDataTransfer
// -------------------------------------------------------------------------- //
JITInstructionProto(CoprocDataTransfer)
{
	// Set the PC before jumping to the handler....
	POPPC();
	SETPC(GETPC());
	// All CP15 LDC and STC operations are undefined.
	// And we don't have another coproc anyway.
	ioCPU->DoUndefinedInstruction();
	
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
}

// -------------------------------------------------------------------------- //
//  * CoprocDataOperation
// -------------------------------------------------------------------------- //
JITInstructionProto(CoprocDataOperation)
{
	// Set the PC before jumping to the handler....
	POPPC();
	SETPC(GETPC());
	// All CP15 CDP operations are undefined.
	// And we don't have another coproc anyway.
	ioCPU->DoUndefinedInstruction();
	
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
}

// -------------------------------------------------------------------------- //
//  * CoprocRegisterTransfer
// -------------------------------------------------------------------------- //
JITInstructionProto(CoprocRegisterTransfer)
{
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	POPPC();
	SETPC(GETPC());
	KUInt32 CPNumber = (theInstruction & 0x00000F00) >> 8;
	
	if (CPNumber == 0xF)
	{
		ioCPU->SystemCoprocRegisterTransfer(theInstruction);
	} else if (CPNumber == 10) {
		// Native primitives.
		ioCPU->NativeCoprocRegisterTransfer(theInstruction);
	} else {
		ioCPU->DoUndefinedInstruction();
	}
	
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
}

// -------------------------------------------------------------------------- //
//  * Translate_SWIAndCoproc
// -------------------------------------------------------------------------- //
void
Translate_SWIAndCoproc(
					TJITGenericPage* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// -Cond-- 1  1  0  P  U  N  W  L  --Rn--- --CRd-- --CP#-- -----offset-------- Coproc Data Transfer
	// -Cond-- 1  1  1  0  --CP Opc--- --CRn-- --CRd-- --CP#-- ---CP--- 0  --CRm-- Coproc Data Operation
	// -Cond-- 1  1  1  0  -CPOpc-- L  --CRn-- --Rd--- --CP#-- ---CP--- 1  --CRm-- Coproc Register Transfer
	// -Cond-- 1  1  1  1  -----------------ignored by processor------------------ Software Interrupt
	if (inInstruction & 0x02000000)
	{
		if (inInstruction & 0x01000000)
		{
			// SWI.
			PUSHFUNC(SWI);
		} else {
			if (inInstruction & 0x00000010)
			{
				PUSHFUNC(CoprocRegisterTransfer);
				PUSHVALUE(inInstruction);
			} else {
				PUSHFUNC(CoprocDataOperation);
			}
		}
	} else {
		PUSHFUNC(CoprocDataTransfer);
	}

	// For all those methods, put the PC.
	PUSHVALUE(inVAddr + 8);
}

// -------------------------------------------------------------------------- //
//  * Branch
// -------------------------------------------------------------------------- //
JITInstructionProto(Branch)
{
	KUInt32 theNewPC;
	POPVALUE(theNewPC);
	
	// Branch.
	MMUCALLNEXT(theNewPC);
}

// -------------------------------------------------------------------------- //
//  * BranchWithLink
// -------------------------------------------------------------------------- //
JITInstructionProto(BranchWithLink)
{
	KUInt32 theNewLR;
	POPVALUE(theNewLR);
	KUInt32 theNewPC;
	POPVALUE(theNewPC);

	// BL
	ioCPU->mCurrentRegisters[14] = theNewLR;
	MMUCALLNEXT(theNewPC);
}

// -------------------------------------------------------------------------- //
//  * Translate_Branch
// -------------------------------------------------------------------------- //
void
Translate_Branch(
					TJITGenericPage* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// -Cond-- 1  0  1  L  ---------------------offset---------------------------- Branch

	// PC prefetch strategy: PC currently is 8 bytes ahead.
	// When we enter this method, it should be 4 bytes ahead.
	// The offset of the branch should be added to the PC and then PC will
	// point exactly to the next instruction.
	// E.g. here B here is coded EAFFFFFE, i.e. branch to offset -8.
	// So what we need to do, for this emulator, is to add 4 to the
	// final result.

	KUInt32 offset = (inInstruction & 0x007FFFFF) << 2;
	if (inInstruction & 0x00800000)
	{
		offset |= 0xFE000000;
	}
	KUInt32 delta = offset + 8;

	// KUInt32 theOffsetInPage = inOffsetInPage + delta;
	// TODO: optimize here.
	// if (theOffsetInPage < kPageSize)
	if (inInstruction & 0x01000000)
	{
		PUSHFUNC(BranchWithLink);
		// The new LR
		PUSHVALUE(inVAddr + 4);	
		// The new PC
		PUSHVALUE(inVAddr + delta + 4);
	} else {
		PUSHFUNC(Branch);
		// The new PC
		PUSHVALUE(inVAddr + delta + 4);
	}
}

// -------------------------------------------------------------------------- //
//  * DebuggerUND
// -------------------------------------------------------------------------- //
JITInstructionProto(DebuggerUND)
{
	// 0xE6000510
	KUInt32 thePAddress;
	POPVALUE(thePAddress);
	// Set the PC before jumping to the handler....
	POPPC();
	SETPC(GETPC());
	ioCPU->GetEmulator()->DebuggerUND( thePAddress );
	
	ioCPU->DoUndefinedInstruction();
		
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
}

// -------------------------------------------------------------------------- //
//  * UndefinedInstruction
// -------------------------------------------------------------------------- //
JITInstructionProto(UndefinedInstruction)
{
	// Set the PC before jumping to the handler....
	POPPC();
	SETPC(GETPC());
	ioCPU->DoUndefinedInstruction();
	
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
}

// -------------------------------------------------------------------------- //
//  * SoftwareBreakpoint
// -------------------------------------------------------------------------- //
JITInstructionProto(SoftwareBreakpoint)
{
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	POPPC();
	SETPC(GETPC());

	ioCPU->GetEmulator()->Breakpoint( theInstruction );

	// Don't execute next function.
	MMUCALLNEXT(GETPC());
}

// ========================================================= //
// As of next week, passwords will be entered in Morse code. //
// ========================================================= //
