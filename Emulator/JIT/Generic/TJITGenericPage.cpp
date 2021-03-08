// ==============================
// File:			TJITGenericPage.cp
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
#include "Emulator/JIT/Generic/TJITGenericPage.h"

#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"

#include "Emulator/JIT/Generic/TJITGeneric_Test.h"
#include "Emulator/JIT/Generic/TJITGeneric_Other.h"
#include "Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.h"
#include "Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.h"
#include "Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.h"
#include "Emulator/JIT/Generic/TJITGeneric_Multiply.h"
#include "Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.h"
#include "Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.h"
#include "Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransfer.h"

#ifdef JIT_PERFORMANCE
#include "TJITPerformance.h"
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
/// Stats...
#undef COLLECT_STATS_ON_PAGES

#ifdef COLLECT_STATS_ON_PAGES
static KUInt16 gMaxUnitsCount = 0;
#endif

// -------------------------------------------------------------------------- //
//  * TJITGenericPage( void )
// -------------------------------------------------------------------------- //
TJITGenericPage::TJITGenericPage( void )
{
	mUnits = (JITUnit*) ::malloc(sizeof(JITUnit) * kDefaultUnitCount);
	mUnitCount = kDefaultUnitCount;
}

// -------------------------------------------------------------------------- //
//  * ~TJITGenericPage( void )
// -------------------------------------------------------------------------- //
TJITGenericPage::~TJITGenericPage( void )
{
	::free(mUnits);
}

// -------------------------------------------------------------------------- //
//  * Init( KUInt32*, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITGenericPage::Init(
			TMemory* inMemoryIntf,
			KUInt32 inVAddr,
			KUInt32 inPAddr )
{
	TJITPage<TJITGeneric, TJITGenericPage>::Init( inMemoryIntf, inVAddr, inPAddr );
	KUInt32* thePointer = GetPointer();

	// Translate the page.
	KUInt32 indexInstr;
	KUInt32 theOffsetInPage = 0;
	KUInt16 unitCrsr = 0;
	for (indexInstr = 0; indexInstr < kInstructionCount; indexInstr++)
	{
		mUnitsTable[indexInstr] = unitCrsr;
		Translate(
		    inMemoryIntf,
			&unitCrsr,
			thePointer[indexInstr],
			inVAddr + theOffsetInPage );
		theOffsetInPage += 4;
	}

	PushUnit(&unitCrsr, TJITGenericPage::EndOfPage);
	PushUnit(&unitCrsr, inVAddr + theOffsetInPage + 4);	// PC + 8

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
TJITGenericPage::PushUnit(KUInt16* ioUnitCrsr, KUIntPtr inUnit)
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


#ifdef JIT_PERFORMANCE
JITInstructionProto(instrCount)
{
	KUInt32 pc;
	POPVALUE(pc);
	COUNTHIT(branchDestCount, pc)
	EXECUTENEXTUNIT;
}
#endif


// -------------------------------------------------------------------------- //
//  * Translate( JITUnit*, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITGenericPage::Translate(
				TMemory* inMemoryIntf,
				KUInt16* ioUnitCrsr,
				KUInt32 inInstruction,
				KUInt32 inVAddr )
{
#ifdef JIT_PERFORMANCE
	PushUnit(ioUnitCrsr, instrCount);
	PushUnit(ioUnitCrsr, inVAddr);
#endif

	// handle injections before anything else
	if (TJITGenericPatchObject::IsNativeInjection(inInstruction)) {
		inInstruction = Translate_PatchNativeCall(
											this,
											ioUnitCrsr,
											inInstruction,
											inVAddr);
	}
	
	int theTestKind = inInstruction >> 28;
	KUInt16 testUnitCrsr = *ioUnitCrsr;
	if ((theTestKind != kTestAL) && (theTestKind != kTestNV))
	{
		// We have a real test.
		// Save some room for it, but put a null for now.
		PushUnit(ioUnitCrsr, (KUIntPtr) 0L);
	}
	
	if (theTestKind != kTestNV)
	{
		switch ((inInstruction >> 26) & 0x3)				// 27 & 26
		{
			case 0x0:	// 00
				DoTranslate_00(
					ioUnitCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x1:	// 01
				DoTranslate_01(
					inMemoryIntf,
					ioUnitCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x2:	// 10
				DoTranslate_10(
					ioUnitCrsr,
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
		KUInt16 nextInstrCrsr = *ioUnitCrsr;
		PutTest(testUnitCrsr, nextInstrCrsr - testUnitCrsr, theTestKind);
	}
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
TJITGenericPage::PutTest(
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
void
TJITGenericPage::DoTranslate_00(
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing PSR Transfer
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	// -Cond-- 0  0  0  P  U  0  W  L  --Rn--- --Rd--- 0 0 0 0 1  S  H  1  --Rm--- Halfword and Signed Data Transfer (armv4)
	// -Cond-- 0  0  0  P  U  1  W  L  --Rn--- --Rd--- -Offst- 1  S  H  1  -Offst- Halfword and Signed Data Transfer (armv4)
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm--- Single Data Swap
	if ((inInstruction & 0x0FC000F0) == 0x90)
	{
		// Multiply
		if (inInstruction & 0x00200000)
		{
			Translate_MultiplyAndAccumulate(
				this,
				ioUnitCrsr,
				inInstruction,
				inVAddr);
		} else {
			Translate_Multiply(
				this,
				ioUnitCrsr,
				inInstruction,
				inVAddr);
		}
	} else if ((inInstruction & 0x0FB00FF0) == 0x01000090) {
		// Single Data Swap
		Translate_SingleDataSwap(
				this,
				ioUnitCrsr,
				inInstruction,
				inVAddr );
	} else if ((inInstruction & 0x0E400F90) == 0x90) {
		Translate_HalfwordAndSignedDataTransferReg(
				this,
				ioUnitCrsr,
				inInstruction,
				inVAddr);
	} else if ((inInstruction & 0x0E400090) == 0x00400090) {
		Translate_HalfwordAndSignedDataTransferImm(
				this,
				ioUnitCrsr,
				inInstruction,
				inVAddr);
	} else if ((inInstruction & 0xC000000) == 0) {
		Translate_DataProcessingPSRTransfer(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr );
	} else {
		PushUnit(ioUnitCrsr, UndefinedInstruction);
		PushUnit(ioUnitCrsr, inVAddr + 8);
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_01( JITUnit*, KUInt32, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
inline
void
TJITGenericPage::DoTranslate_01(
					TMemory* inMemoryIntf,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Single Data Transfer & Undefined
	if ((inInstruction & 0x02000010) == 0x02000010)
	{
// -Cond-- 0  1  1  -XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX- 1  -XXXXX-
          // DA WINNER
		if (inInstruction == 0xE6000010)
		{
			PushUnit(ioUnitCrsr, SystemBootUND);
			PushUnit(ioUnitCrsr, inVAddr - GetVAddr() + GetPAddr());
			PushUnit(ioUnitCrsr, inVAddr + 8);
		}
		else if (inInstruction == 0xE6000510)
		{
			PushUnit(ioUnitCrsr, DebuggerUND);
			PushUnit(ioUnitCrsr, inVAddr - GetVAddr() + GetPAddr());
			PushUnit(ioUnitCrsr, inVAddr + 8);
		}
		else if (inInstruction == 0xE6000810)
		{
			PushUnit(ioUnitCrsr, TapFileCntlUND);
			PushUnit(ioUnitCrsr, inVAddr - GetVAddr() + GetPAddr());
			PushUnit(ioUnitCrsr, inVAddr + 8);
		}
		else 
		{
			PushUnit(ioUnitCrsr, UndefinedInstruction);
			PushUnit(ioUnitCrsr, inVAddr + 8);
		}
	} else {
// -Cond-- 0  1  I  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------
		Translate_SingleDataTransfer(this, inMemoryIntf, ioUnitCrsr, inInstruction, inVAddr);
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_10( JITUnit*, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
inline void
TJITGenericPage::DoTranslate_10(
					KUInt16* ioUnitCrsr,
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
}

// -------------------------------------------------------------------------- //
//  * EndOfPage( JITUnit*, TARMProcessor* )
// -------------------------------------------------------------------------- //
JITUnit*
TJITGenericPage::EndOfPage(
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
TJITGenericPage::Halt(
				JITUnit* ioUnit,
				TARMProcessor* ioCPU )
{
	// Halt is used for single steps.
	// If we are here, then the previous instruction tried to continue the
	// execution with us. And as a consequence, the PC is the previous PC + 4.
	SETPC(THEPC + 4);
	return ioUnit;
}

#endif

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
