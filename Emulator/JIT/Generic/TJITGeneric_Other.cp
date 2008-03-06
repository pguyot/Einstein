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
#include "JIT.h"

// Einstein
#include "TARMProcessor.h"
#include "TEmulator.h"
#include "TROMImage.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include "TJITPerformance.h"
#endif

#include "TJITGeneric_Macros.h"

#include <Albert/sys/einstein.h>

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
//  * CallHostNative
//
//  This function allows for quick calls into host-native "C" and "C++" 
//  functions without patching more than a single instruction in the 
//  original code. 
//
//  Implementing host-native calls using a stub will allow direct calls
//  from other native functions, allowing full execution speed without 
//  any register emulation.
//
//  This code is limited to MSVC on WIN32 until other platforms are verified.
// -------------------------------------------------------------------------- //
JITInstructionProto(CallHostNative)
{
	// Set the PC before jumping to the handler....
	KUInt32 callIndex;
	POPVALUE(callIndex);
  JITFuncPtr stub = TROMPatch::albertStub(callIndex);
  //printf("Albert: %3d:0x%08x\n", (int)callIndex, stub);
	if (!stub) {
		CALLNEXTUNIT;
	}
  Albert::CPUInterface = ioCPU;
	return stub(ioUnit, ioCPU);
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
					JITPageClass* inPage,
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
			if (inInstruction & 0x00800000) {
				// quick host native call
				PUSHFUNC(CallHostNative);
				PUSHVALUE(inInstruction & 0x007fffff);
				return; // do not push the current PC
			} else {
				// SWI.
				PUSHFUNC(SWI);
			}
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
	COUNTHIT(branchDestCount, theNewPC)
	
	// Branch.
	MMUCALLNEXT(theNewPC);
}

// -------------------------------------------------------------------------- //
//  * Branch within page using a know JITUnit delta.
// -------------------------------------------------------------------------- //
JITInstructionProto(BranchWithinPage)
{
	KUInt32 theNewPC;
	POPVALUE(theNewPC);
	COUNTHIT(branchDestCount, theNewPC)

	KSInt32 theDelta;
	POPVALUE(theDelta);

	// Branch.
	SETPC(theNewPC);
	return ioUnit+theDelta;
}

// -------------------------------------------------------------------------- //
//  * Branch within a page - find the delta first.
// -------------------------------------------------------------------------- //
JITInstructionProto(BranchWithinPageFindDelta)
{
	KUInt32 theNewPC;
	POPVALUE(theNewPC);
	COUNTHIT(branchDestCount, theNewPC)

	KSInt32 theDelta;
	POPVALUE(theDelta);
	
	// MMUCALLNEXT()
	TMemory *theMemIntf = ioCPU->GetMemory();
	SETPC(theNewPC);
	JITUnit *nextUnit = theMemIntf->GetJITObject()
		->GetJITUnitForPC(ioCPU, theMemIntf, theNewPC);

	// now change the JIT command to the final fast branch
	ioUnit[ 0].fValue = nextUnit - ioUnit;
	ioUnit[-2].fFuncPtr = BranchWithinPage;
	return nextUnit;
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
	COUNTHIT(branchLinkDestCount, theNewPC)

	// BL
	ioCPU->mCurrentRegisters[14] = theNewLR;
	MMUCALLNEXT(theNewPC);
}

// -------------------------------------------------------------------------- //
//  * Branch with link within page using a know JITUnit delta.
// -------------------------------------------------------------------------- //
JITInstructionProto(BranchWithLinkWithinPage)
{
	KUInt32 theNewLR;
	POPVALUE(theNewLR);
	KUInt32 theNewPC;
	POPVALUE(theNewPC);
	COUNTHIT(branchLinkDestCount, theNewPC)
	KSInt32 theDelta;
	POPVALUE(theDelta);

	// BL
	ioCPU->mCurrentRegisters[14] = theNewLR;
	SETPC(theNewPC);
	return ioUnit+theDelta;
}

// -------------------------------------------------------------------------- //
//  * Branch with link within a page - find the delta first.
// -------------------------------------------------------------------------- //
JITInstructionProto(BranchWithLinkWithinPageFindDelta)
{
	KUInt32 theNewLR;
	POPVALUE(theNewLR);
	KUInt32 theNewPC;
	POPVALUE(theNewPC);
	COUNTHIT(branchLinkDestCount, theNewPC)
	KSInt32 theDelta;
	POPVALUE(theDelta);

	// set the link register
	ioCPU->mCurrentRegisters[14] = theNewLR;

	// MMUCALLNEXT()
	TMemory *theMemIntf = ioCPU->GetMemory();
	SETPC(theNewPC);
	JITUnit *nextUnit = theMemIntf->GetJITObject()
		->GetJITUnitForPC(ioCPU, theMemIntf, theNewPC);

	// now change the JIT command to the final fast branch
	ioUnit[ 0].fValue = nextUnit - ioUnit;
	ioUnit[-3].fFuncPtr = BranchWithLinkWithinPage;
	return nextUnit;
}


// -------------------------------------------------------------------------- //
//  * Translate_Branch
// -------------------------------------------------------------------------- //
void
Translate_Branch(
					JITPageClass* inPage,
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
	// optimize here:
	// if (theOffsetInPage < kPageSize)
	// TODO: Matt: I am not 100% sure that the decission to optimize is correct. I
	//    do take a 4 byte offset of the target address into account, however I am 
	//    not entirely sure about the current PC
	if (inInstruction & 0x01000000)
	{
		// optimizing branches with link within pages gained only 1% speed
		if ( (inVAddr+delta>=inPage->GetVAddr()) && (inVAddr+delta<inPage->GetVAddr()+inPage->kPageSize) ) 
		{
			PUSHFUNC(BranchWithLinkWithinPageFindDelta);
			// The new LR
			PUSHVALUE(inVAddr + 4);	
			// The new PC
			PUSHVALUE(inVAddr + delta + 4);
			// The branch offset in ioUnits, to be calculated later
			PUSHVALUE(0xffffffff);
		} else {
			PUSHFUNC(BranchWithLink);
			// The new LR
			PUSHVALUE(inVAddr + 4);	
			// The new PC
			PUSHVALUE(inVAddr + delta + 4);
		}
	} else {
		// optimizing branches within pages gave us a 10% speed increase
		if ( (inVAddr+delta>=inPage->GetVAddr()) && (inVAddr+delta<inPage->GetVAddr()+inPage->kPageSize) ) 
		{
			PUSHFUNC(BranchWithinPageFindDelta);
			// The new PC
			PUSHVALUE(inVAddr + delta + 4);
			// The branch offset in ioUnits, to be calculated later
			PUSHVALUE(0xffffffff);
		} else {
			PUSHFUNC(Branch);
			// The new PC
			PUSHVALUE(inVAddr + delta + 4);
		}
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
