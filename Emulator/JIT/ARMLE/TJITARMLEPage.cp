// ==============================
// File:			TJITARMLEPage.cp
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
#include "TJITARMLEPage.h"

// Einstein
#include "TARMProcessor.h"

// For now, ARMLE heavily relies on Generic
#include "TJITGeneric_Macros.h"

#include "TJITGeneric_Test.h"
#include "TJITGeneric_Other.h"
#include "TJITGeneric_DataProcessingPSRTransfer.h"
#include "TJITGeneric_SingleDataTransfer.h"
#include "TJITGeneric_SingleDataSwap.h"
#include "TJITGeneric_Multiply.h"
#include "TJITGeneric_MultiplyAndAccumulate.h"
#include "TJITGeneric_BlockDataTransfer.h"

// Cf: JITARMLEGlue.dump
static const KUInt32 ExitJITNativeSequence_Code[] = {
	0xe58fe00c,	// str		lr, [pc, #12]
	0xe1a0e00f,	// mov		lr, pc
	0xe59ff000,	// ldr		pc, [pc, #0]
	0x00000000,	//< return_value
	0x00000000,	//< exit_jit_glue
	0x00000000	//< save_lr
};

static const KUInt32 ExitJITGlue[] = {
	0xe58fe02c,	// str		lr, [pc, #44]	; 54 <save_lr2>
	0xe59fe02c,	// ldr		lr, [pc, #44]	; 50 <exit_jit_zone>
	0xe8ae3fff,	// stmia	lr!, {r0-r9, sl, fp, ip, sp}
	0xe59f0020,	// ldr		r0, [pc, #32]	; 54 <save_lr2>
	0xe5901008,	// ldr		r1, [r0, #8]
	0xe8ae0002,	// stmia	lr!, {r1}
	0xe10f1000,	// mrs		r1, CPSR
	0xe8ae0002,	// stmia	lr!, {r1}
	0xe59ed000,	// ldr		sp, [lr]
	0xe8bd5ff2,	// ldmia	sp!, {r1-r9, sl, fp, ip, lr}
	0xe128f001,	// msr		CPSR_f, r1
	0xe5900000,	// ldr		r0, [r0]
	0xe1a0f00e,	// mov		pc, lr
	0x00000000,	//< save_lr2
	0x00000000	//< exit_jit_zone
};

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
/// Stats...
#define COLLECT_STATS_ON_PAGES 0

#ifdef COLLECT_STATS_ON_PAGES
static KUInt16 gMaxUnitsCount = 0;
#endif

// -------------------------------------------------------------------------- //
//  * TJITARMLEPage( void )
// -------------------------------------------------------------------------- //
TJITARMLEPage::TJITARMLEPage( void )
{
	mUnits = (JITUnit*) ::malloc(sizeof(JITUnit) * kDefaultUnitCount);
	mUnitCount = kDefaultUnitCount;
	mNativeCode = (KUInt32*) ::malloc(sizeof(KUInt32) * kDefaultNativeCount);
	mNativeCount = kDefaultNativeCount;
	
	// Create the exit sequence.
	mExitJITGlue = (KUInt32*) ::malloc(sizeof(ExitJITGlue));
	::memmove((void*) mExitJITGlue, (const void*) ExitJITGlue, sizeof(ExitJITGlue));
	mExitJITGlue[14] = (KUInt32) &mZone;
}

// -------------------------------------------------------------------------- //
//  * ~TJITARMLEPage( void )
// -------------------------------------------------------------------------- //
TJITARMLEPage::~TJITARMLEPage( void )
{
	::free(mUnits);
}

// -------------------------------------------------------------------------- //
//  * Init( KUInt32*, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITARMLEPage::Init(
			TMemory* inMemoryIntf,
			KUInt32 inVAddr,
			KUInt32 inPAddr )
{
	TJITPage<TJITARMLEPage>::Init( inMemoryIntf, inVAddr, inPAddr );
	KUInt32* thePointer = GetPointer();

	// Translate the page.
	KUInt32 indexInstr;
	KUInt32 theOffsetInPage = 0;
	KUInt16 unitCrsr = 0;
	KUInt16 nativeCrsr = 0;
	Boolean previousIsNative = false;
	
	KUInt16 unitPtrsInNative[kInstructionCount];
	KUInt16 nativePtrsInUnits[kInstructionCount];
	KUInt16 unitPtrsInNativeCount = 0;
	KUInt16 nativePtrsInUnitsCount = 0;
	
	Boolean isNative = false;
	
	for (indexInstr = 0; indexInstr < kInstructionCount; indexInstr++)
	{
		KUInt32 theUnitCrsr = unitCrsr;
		KUInt32 theNativeCrsr = nativeCrsr;
		mUnitsTable[indexInstr] = theUnitCrsr;
		isNative = Translate(
							&unitCrsr,
							&nativeCrsr,
							thePointer[indexInstr],
							inVAddr + theOffsetInPage );
		if (isNative)
		{
			// Push the enter native glue.
			PushUnit(&unitCrsr, EnterNative);
			nativePtrsInUnits[nativePtrsInUnitsCount++] = unitCrsr;
			PushUnit(&unitCrsr, theNativeCrsr);
			PushUnit(&unitCrsr, (KUInt32) &mZone);
		} else if (previousIsNative) {
			// Push the exit native glue.
			PushNative(&nativeCrsr, ExitJITNativeSequence_Code[0]);
			PushNative(&nativeCrsr, ExitJITNativeSequence_Code[1]);
			PushNative(&nativeCrsr, ExitJITNativeSequence_Code[2]);
			unitPtrsInNative[unitPtrsInNativeCount++] = nativeCrsr;
			PushNative(&nativeCrsr, theUnitCrsr);
			PushNative(&nativeCrsr, (KUInt32) mExitJITGlue);
			PushNative(&nativeCrsr, 0);
		}
		previousIsNative = isNative;
		theOffsetInPage += 4;
	}

	if (isNative) {
		// Push the exit native glue.
		PushNative(&nativeCrsr, ExitJITNativeSequence_Code[0]);
		PushNative(&nativeCrsr, ExitJITNativeSequence_Code[1]);
		PushNative(&nativeCrsr, ExitJITNativeSequence_Code[2]);
		unitPtrsInNative[unitPtrsInNativeCount++] = nativeCrsr;
		PushNative(&nativeCrsr, unitCrsr);
		PushNative(&nativeCrsr, (KUInt32) mExitJITGlue);
		PushNative(&nativeCrsr, 0);
	}
	
	PushUnit(&unitCrsr, TJITARMLEPage::EndOfPage);
	PushUnit(&unitCrsr, inVAddr + theOffsetInPage + 4);	// PC + 8

	// Rebranch the native and unit pointers, now that the allocated blocks will
	// not move.
	KUInt32 indexPtrs;
	for (indexPtrs = 0; indexPtrs < unitPtrsInNativeCount; indexPtrs++) {
		KUInt32 theNativeIndex = unitPtrsInNative[indexPtrs];
		KUInt32 theUnitIndex = mNativeCode[theNativeIndex];
		mNativeCode[theNativeIndex] = (KUInt32) &mUnits[theUnitIndex];
	}
	for (indexPtrs = 0; indexPtrs < nativePtrsInUnitsCount; indexPtrs++) {
		KUInt32 theUnitIndex = nativePtrsInUnits[indexPtrs];
		KUInt32 theNativeIndex = mUnits[theUnitIndex].fValue;
		mUnits[theUnitIndex].fPtr = (KUIntPtr) &mNativeCode[theNativeIndex];
	}

#ifdef COLLECT_STATS_ON_PAGES
	if (unitCrsr > gMaxUnitsCount) {
		gMaxUnitsCount = unitCrsr;
		fprintf(stderr, "Max units count = %i\n", unitCrsr);
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * PushUnit( KUInt16*, KUIntPtr )
// -------------------------------------------------------------------------- //
void
TJITARMLEPage::PushUnit(KUInt16* ioUnitCrsr, KUIntPtr inUnit)
{
	// Can we push it?
	KUInt16 theCrsr = *ioUnitCrsr;
	if (theCrsr == mUnitCount) {
		// We need to resize the table.
		mUnitCount += kUnitIncrement;
		mUnits = (JITUnit*) ::realloc(mUnits, mUnitCount * sizeof(JITUnit));
	}
	mUnits[theCrsr++].fPtr = inUnit;
	*ioUnitCrsr = theCrsr;
}

// -------------------------------------------------------------------------- //
//  * PushNative( KUInt16*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITARMLEPage::PushNative(KUInt16* ioNativeCrsr, KUInt32 inInstruction)
{
	// Can we push it?
	KUInt16 theCrsr = *ioNativeCrsr;
	if (theCrsr == mNativeCount) {
		// We need to resize the table.
		mNativeCount += kNativeIncrement;
		mNativeCode = (KUInt32*) ::realloc(mNativeCode, mNativeCount * sizeof(KUInt32));
	}
	mNativeCode[theCrsr++] = inInstruction;
	*ioNativeCrsr = theCrsr;
}

// -------------------------------------------------------------------------- //
//  * Translate( JITUnit*, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TJITARMLEPage::Translate(
				KUInt16* ioUnitCrsr,
				KUInt16* ioNativeCrsr,
				KUInt32 inInstruction,
				KUInt32 inVAddr )
{
	Boolean isNative = false;
	int theTestKind = inInstruction >> 28;
	KUInt16 testUnitCrsr = *ioUnitCrsr;
	if ((theTestKind != kTestAL) && (theTestKind != kTestNV))
	{
		// We have a real test.
		// Save some room for it, but put a null for now.
		PushUnit(ioUnitCrsr, (KUInt32) 0L);
	}
	
	if (theTestKind != kTestNV)
	{
		switch ((inInstruction >> 26) & 0x3)				// 27 & 26
		{
			case 0x0:	// 00
				isNative = DoTranslate_00(
					ioUnitCrsr,
					ioNativeCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x1:	// 01
				isNative = DoTranslate_01(
					ioUnitCrsr,
					ioNativeCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x2:	// 10
				isNative = DoTranslate_10(
					ioUnitCrsr,
					ioNativeCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x3:	// 11
				Translate_SWIAndCoproc(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr);
				break;
		} // switch 27 & 26
	}
	
	// Finally put the test, based on the current unit crsr.
	if ((theTestKind != kTestAL) && (theTestKind != kTestNV))
	{
		if (isNative) {
			// Do not put the test since the function is native, and
			// revert the cursor.
			*ioUnitCrsr = testUnitCrsr;
		} else {
			KUInt16 nextInstrCrsr = *ioUnitCrsr;
			PutTest(testUnitCrsr, nextInstrCrsr - testUnitCrsr, theTestKind);
		}
	}
	
	// For now, we don't have any native instruction.
	return isNative;
}

// -------------------------------------------------------------------------- //
//  * PutTest( KUInt16*, KUInt32 )
// -------------------------------------------------------------------------- //
#define _template1(func, delta)	func ## delta
#define __PutTest_line(func, delta)										\
		case (delta):													\
			mUnits[inUnitCrsr].fFuncPtr = _template1(func, delta);		\
			break

// NOTICE: maximum number of units for an instruction is set to 6.....

#define __PutTest_packet(func)							\
		switch(inDelta) {								\
			__PutTest_line(func, 2);					\
			__PutTest_line(func, 3);					\
			__PutTest_line(func, 4);					\
			__PutTest_line(func, 5);					\
			__PutTest_line(func, 6);					\
			__PutTest_line(func, 7);					\
			default:									\
				fprintf(stderr, "Test overflow!\n");	\
				abort();								\
		}

inline void
TJITARMLEPage::PutTest(
				KUInt16 inUnitCrsr,
				unsigned char inDelta,
				int inTest )
{
	// Test the condition.
	switch (inTest)
	{
		// 0000 = EQ - Z set (equal)
		case kTestEQ:
			__PutTest_packet(TestEQ);
			break;
			
			// 0001 = NE - Z clear (not equal)
		case kTestNE:
			__PutTest_packet(TestNE);
			break;
	
			// 0010 = CS - C set (unsigned higher or same)
		case kTestCS:
			__PutTest_packet(TestCS);
			break;
			
			// 0011 = CC - C clear (unsigned lower)
		case kTestCC:
			__PutTest_packet(TestCC);
			break;
			
			// 0100 = MI - N set (negative)
		case kTestMI:
			__PutTest_packet(TestMI);
			break;
			
			// 0101 = PL - N clear (positive or zero)
		case kTestPL:
			__PutTest_packet(TestPL);
			break;
			
			// 0110 = VS - V set (overflow)
		case kTestVS:
			__PutTest_packet(TestVS);
			break;
			
			// 0111 = VC - V clear (no overflow)
		case kTestVC:
			__PutTest_packet(TestVC);
			break;
			
			// 1000 = HI - C set and Z clear (unsigned higher)
		case kTestHI:
			__PutTest_packet(TestHI);
			break;
			
			// 1001 = LS - C clear or Z set (unsigned lower or same)
		case kTestLS:
			__PutTest_packet(TestLS);
			break;
			
			// 1010 = GE - N set and V set, or N clear and V clear (greater or equal)
		case kTestGE:
			__PutTest_packet(TestGE);
			break;
			
			// 1011 = LT - N set and V clear, or N clear and V set (less than)
		case kTestLT:
			__PutTest_packet(TestLT);
			break;
			
			// 1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
		case kTestGT:
			__PutTest_packet(TestGT);
			break;
			
			// 1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
		case kTestLE:
			__PutTest_packet(TestLE);
			break;
			
			// 1110 = AL - always
		case kTestAL:
			// 1111 = NV - never
		case kTestNV:
		default:
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_00( KUInt32, JITFuncPtr*, JITUnit* )
// -------------------------------------------------------------------------- //
inline
Boolean
TJITARMLEPage::DoTranslate_00(
					KUInt16* ioUnitCrsr,
					KUInt16* ioNativeCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	Boolean isNative = false;
	
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing PSR Transfer
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm---
	if ((inInstruction & 0x020000F0) == 0x90)	// I=0 & 0b1001----
	{
		if (inInstruction & 0x01000000)
		{
			// Single Data Swap
			Translate_SingleDataSwap(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr );
		} else {
			// Multiply and multiply and accumulate are native.
			PushNative(ioNativeCrsr, inInstruction);
			isNative = true;
		}
	} else {
		Translate_DataProcessingPSRTransfer(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr );
	}

	return isNative;
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_01( JITUnit*, KUInt32, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
inline
Boolean
TJITARMLEPage::DoTranslate_01(
					KUInt16* ioUnitCrsr,
					KUInt16* ioNativeCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Single Data Transfer & Undefined
	if ((inInstruction & 0x02000010) == 0x02000010)
	{
// -Cond-- 0  1  1  -XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX- 1  -XXXXX-
	// DA WINNER
		if (inInstruction == 0xE6000510)
		{
			PushUnit(ioUnitCrsr, DebuggerUND);
			PushUnit(ioUnitCrsr, inVAddr - GetVAddr() + GetPAddr());
			PushUnit(ioUnitCrsr, inVAddr + 8);
		} else {
			PushUnit(ioUnitCrsr, UndefinedInstruction);
			PushUnit(ioUnitCrsr, inVAddr + 8);
		}
	} else {
// -Cond-- 0  1  I  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------
		Translate_SingleDataTransfer(this, ioUnitCrsr, inInstruction, inVAddr);
	}

	// No native instruction here.
	return false;
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_10( JITUnit*, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
inline Boolean
TJITARMLEPage::DoTranslate_10(
					KUInt16* ioUnitCrsr,
					KUInt16* ioNativeCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Block Data Transfer
	// Branch
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	// -Cond-- 1  0  1  L  ---------------------offset---------------------------- Branch
	if (inInstruction & 0x02000000)
	{
		Translate_Branch(this, ioUnitCrsr, inInstruction, inVAddr);
	} else {
		// Block Data Transfer.
		Translate_BlockDataTransfer(this, ioUnitCrsr, inInstruction, inVAddr);
	}
	
	// No native instruction here.
	return false;
}

// -------------------------------------------------------------------------- //
//  * EndOfPage( JITUnit*, TARMProcessor* )
// -------------------------------------------------------------------------- //
JITUnit*
TJITARMLEPage::EndOfPage(
				JITUnit* ioUnit,
				TARMProcessor* ioCPU )
{
	// PC shouldn't have been incremented.
	POPPC();

	// Don't execute next function.
	MMUCALLNEXT(GETPC());
}

// -------------------------------------------------------------------------- //
//  * Halt( JITUnit*, TARMProcessor* )
// -------------------------------------------------------------------------- //
JITUnit*
TJITARMLEPage::Halt(
				JITUnit* ioUnit,
				TARMProcessor* ioCPU )
{
	// Halt is used for single steps.
	// If we are here, then the previous instruction tried to continue the
	// execution with us. And as a consequence, the PC is the previous PC + 4.
	SETPC(THEPC + 4);
	return ioUnit;
}

// -------------------------------------------------------------------------- //
//  * EnterNative( JITUnit*, TARMProcessor* )
// -------------------------------------------------------------------------- //
JITUnit*
TJITARMLEPage::EnterNative(
				JITUnit* ioUnit,
				TARMProcessor* ioCPU )
{
	// We enter native code here.
	// Pop the address of the native instruction.
	KUInt32 theNativeFuncPtr;
	POPVALUE(theNativeFuncPtr);
	KUIntPtr theZonePtr;
	POPVALUE(theZonePtr);
	JITZone* theZone = (JITZone*) theZonePtr;
	
	// Setup the zone.
	// Hopefully, gcc will optimize this with a series of ldm/stm.
	theZone->fJITRegisters[0] = ioCPU->mCurrentRegisters[0];
	theZone->fJITRegisters[1] = ioCPU->mCurrentRegisters[1];
	theZone->fJITRegisters[2] = ioCPU->mCurrentRegisters[2];
	theZone->fJITRegisters[3] = ioCPU->mCurrentRegisters[3];
	theZone->fJITRegisters[4] = ioCPU->mCurrentRegisters[4];
	theZone->fJITRegisters[5] = ioCPU->mCurrentRegisters[5];
	theZone->fJITRegisters[6] = ioCPU->mCurrentRegisters[6];
	theZone->fJITRegisters[7] = ioCPU->mCurrentRegisters[7];
	theZone->fJITRegisters[8] = ioCPU->mCurrentRegisters[8];
	theZone->fJITRegisters[9] = ioCPU->mCurrentRegisters[9];
	theZone->fJITRegisters[10] = ioCPU->mCurrentRegisters[10];
	theZone->fJITRegisters[11] = ioCPU->mCurrentRegisters[11];
	theZone->fJITRegisters[12] = ioCPU->mCurrentRegisters[12];
	theZone->fJITRegisters[13] = ioCPU->mCurrentRegisters[13];
	theZone->fJITRegisters[14] = ioCPU->mCurrentRegisters[14];
	theZone->fJITCPSR = ioCPU->GetCPSR();

	// Enter the magic!
	theZone->fNativeAddr = theNativeFuncPtr;
	JITUnit* theUnit = EnterJITGlue(theZone);

	ioCPU->mCurrentRegisters[0] = theZone->fJITRegisters[0];
	ioCPU->mCurrentRegisters[1] = theZone->fJITRegisters[1];
	ioCPU->mCurrentRegisters[2] = theZone->fJITRegisters[2];
	ioCPU->mCurrentRegisters[3] = theZone->fJITRegisters[3];
	ioCPU->mCurrentRegisters[4] = theZone->fJITRegisters[4];
	ioCPU->mCurrentRegisters[5] = theZone->fJITRegisters[5];
	ioCPU->mCurrentRegisters[6] = theZone->fJITRegisters[6];
	ioCPU->mCurrentRegisters[7] = theZone->fJITRegisters[7];
	ioCPU->mCurrentRegisters[8] = theZone->fJITRegisters[8];
	ioCPU->mCurrentRegisters[9] = theZone->fJITRegisters[9];
	ioCPU->mCurrentRegisters[10] = theZone->fJITRegisters[10];
	ioCPU->mCurrentRegisters[11] = theZone->fJITRegisters[11];
	ioCPU->mCurrentRegisters[12] = theZone->fJITRegisters[12];
	ioCPU->mCurrentRegisters[13] = theZone->fJITRegisters[13];
	ioCPU->mCurrentRegisters[14] = theZone->fJITRegisters[14];
	ioCPU->mCPSR_N = (theZone->fJITCPSR & TARMProcessor::kPSR_NBit) != 0;
	ioCPU->mCPSR_Z = (theZone->fJITCPSR & TARMProcessor::kPSR_ZBit) != 0;
	ioCPU->mCPSR_C = (theZone->fJITCPSR & TARMProcessor::kPSR_CBit) != 0;
	ioCPU->mCPSR_V = (theZone->fJITCPSR & TARMProcessor::kPSR_VBit) != 0;
	ioCPU->mCPSR_I = (theZone->fJITCPSR & TARMProcessor::kPSR_IBit) != 0;
	ioCPU->mCPSR_F = (theZone->fJITCPSR & TARMProcessor::kPSR_FBit) != 0;
	ioCPU->mCPSR_T = (theZone->fJITCPSR & TARMProcessor::kPSR_TBit) != 0;
	
	// The PC is incorrect now, so we cannot return to the loop.
	// Let's call the next non-native function.
	return theUnit->fFuncPtr(theUnit, ioCPU);
}

// ============================================================================= //
// Ever wondered about the origins of the term "bugs" as applied to computer     //
// technology?  U.S. Navy Capt. Grace Murray Hopper has firsthand explanation.   //
// The 74-year-old captain, who is still on active duty, was a pioneer in        //
// computer technology during World War II.  At the C.W. Post Center of Long     //
// Island University, Hopper told a group of Long Island public school adminis-  //
// trators that the first computer "bug" was a real bug--a moth.  At Harvard     //
// one August night in 1945, Hopper and her associates were working on the       //
// "granddaddy" of modern computers, the Mark I.  "Things were going badly;      //
// there was something wrong in one of the circuits of the long glass-enclosed   //
// computer," she said.  "Finally, someone located the trouble spot and, using   //
// ordinary tweezers, removed the problem, a two-inch moth.  From then on, when  //
// anything went wrong with a computer, we said it had bugs in it."  Hopper      //
// said that when the veracity of her story was questioned recently, "I referred //
// them to my 1945 log book, now in the collection of the Naval Surface Weapons  //
// Center, and they found the remains of that moth taped to the page in          //
// question."                                                                    //
//                 [actually, the term "bug" had even earlier usage in           //
//                 regard to problems with radio hardware.  Ed.]                 //
// ============================================================================= //
