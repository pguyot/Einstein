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
#include "Emulator/JIT/JIT.h"

#ifdef JITTARGET_GENERIC

// Einstein
#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"
#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"
#include "Monitor/TSymbolList.h"

#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"

/*
 Floating point math emulation
 
 The SA110 processor has no floating point coprocessor. It emulates FP 
 instructions in software. Einstein then emulates the emulation. I am sure you
 can see where I am going.
 
 In order to increase speed, it would be nice to write new JIT code that 
 generates native code for all FP instructions, just as it does for ARM integer
 instructions. Even though floating point is used sparsely in the ROM, this 
 should still make for a bit of an acceleration.
 
 All FP registers are stored at __fp_regs (0x0C105A5C). There are 8 registers.
 Most instructions are 'd', some are 's' precission. Registeres are stored
 in their own format and converted to and from IEEE when copied to RAM or to
 registers.
*/


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
//  * CallNativePatch
//
//  This function allows for quick calls into host-native "C" and "C++" 
//  functions without patching more than a single instruction in the 
//  original code. 
//
//  Implementing host-native calls using a stub will allow direct calls
//  from other native functions, allowing full execution speed without 
//  any register emulation.
// -------------------------------------------------------------------------- //
JITInstructionProto(CallPatchNative)
{
	KUInt32 patchIndex;
	POPVALUE(patchIndex);
	TJITGenericPatchObject *patch = TJITGenericPatchManager::GetPatchAt(patchIndex);
	if (!patch) {
		fprintf(stderr, "ERROR in %s %d: no patch found for index %d\n",
				__FILE__, __LINE__, (int)patchIndex);
		CALLNEXTUNIT; // returns from this function
	}
	ioUnit = patch->Call(ioUnit, ioCPU);
	if (ioUnit) {
		CALLNEXTUNIT;
	} else {
		MMUCALLNEXT_AFTERSETPC;
	}
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
//  * Translate a native code injection
// -------------------------------------------------------------------------- //
KUInt32
Translate_PatchNativeCall(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
    KUInt32 ix = TJITGenericPatchObject::GetIndex(inInstruction);
    TJITGenericPatchObject* patchObject = TJITGenericPatchManager::GetPatchAt(ix);
    if (patchObject == NULL) {
        // This patch doesn't exist, we're probably trying to translate data.
        return inInstruction;
    }
    PUSHFUNC(CallPatchNative);
    PUSHVALUE(ix);
    return patchObject->GetOriginalInstruction();
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
	// Extension for native calls:
	// -Cond-- 1  1  1  1  1  0  ---------------------index----------------------- Call Native Code at Index
	// -Cond-- 1  1  1  1  1  1  ---------------------index----------------------- Call Injection at Index
	if (inInstruction & 0x02000000)
	{
		if (inInstruction & 0x01000000)
		{
			if (TJITGenericPatchObject::IsNativeCall(inInstruction)) {
				// quick host native call
				PUSHFUNC(CallPatchNative);
				PUSHVALUE(TJITGenericPatchObject::GetIndex(inInstruction));
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
	
	POPNIL();
	
	// MMUCALLNEXT()
	TMemory *theMemIntf = ioCPU->GetMemory();
	SETPC(theNewPC);
	JITUnit *nextUnit = theMemIntf->GetJITObject()
		->GetJITUnitForPC(ioCPU, theMemIntf, theNewPC);
	
	// now change the JIT command to the final fast branch
	ioUnit[ 0].fValue = (KUInt32)(nextUnit - ioUnit);
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
	POPNIL();
	
	// set the link register
	ioCPU->mCurrentRegisters[14] = theNewLR;
	
	// MMUCALLNEXT()
	TMemory *theMemIntf = ioCPU->GetMemory();
	SETPC(theNewPC);
	JITUnit *nextUnit = theMemIntf->GetJITObject()
		->GetJITUnitForPC(ioCPU, theMemIntf, theNewPC);
	
	// now change the JIT command to the final fast branch
	ioUnit[ 0].fValue = (KUInt32)(nextUnit - ioUnit);
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
//  * SystemBootUND
// -------------------------------------------------------------------------- //
JITInstructionProto(SystemBootUND)
{
	// 0xE6000010
	KUInt32 thePAddress;
	POPVALUE(thePAddress);
	// Set the PC before jumping to the handler....
	POPPC();
	SETPC(GETPC());
	ioCPU->GetEmulator()->SystemBootUND( thePAddress );
	
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
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
	
	// Don't execute next function.
	MMUCALLNEXT_AFTERSETPC;
}

// -------------------------------------------------------------------------- //
//  * TapFileCntlUND
// -------------------------------------------------------------------------- //
JITInstructionProto(TapFileCntlUND)
{
	// 0xE6000810
	KUInt32 thePAddress;
	POPVALUE(thePAddress);
	// Set the PC before jumping to the handler....
	POPPC();
	SETPC(GETCALLER() + 4);
	ioCPU->GetEmulator()->TapFileCntlUND( thePAddress );
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

#endif

// ========================================================= //
// As of next week, passwords will be entered in Morse code. //
// ========================================================= //
