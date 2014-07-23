// ==============================
// File:			TJITGenericRetarget.h
// Project:			Einstein
//
// Copyright 2014 by Matthias Melcher (einstein@matthiasm.com).
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

#ifndef _TJITGENERICRETARGET_H
#define _TJITGENERICRETARGET_H

#include <K/Defines/KDefinitions.h>

#include "TJITGenericPage.h"

#include <stdio.h>


class TMemory;


/**
 * Class for retargeting NewtonOS ARM code into "C".
 *
 * This class translates ARM commnds into something that can be
 * compiled by a C compiler. In its first iteration, we will
 * generate the same code that the JIT compiler would create,
 * using the same generic JIT commands.
 *
 * The next step will allow for function calls and returns
 * using the "C" stack in parallel to the emulated stack.
 *
 * Then, we will reintroduce fibers, allowing multiple parallel
 * stacks for different CPU states.
 *
 * When this works well, we can abandon the emulated stack and
 * stay in the "C" world for almost all of the time.
 *
 * Now that we are simulating the CPU, we can continue to also simulate (instaed
 * of emulate) the RAM.
 *
 * Once that is done, simplify the JIT commands and create "normal" C code
 */
class TJITGenericRetarget : public TJITGenericPage
{
public:
	/**
	 * Create a retargeting environment that writes to a text file.
	 */
	TJITGenericRetarget(TMemory *inMemory);
	
	/**
	 * Release all resources.
	 */
	~TJITGenericRetarget();
	
	/**
	 * Start output into text files.
	 */
	bool OpenFiles(const char *filepath, const char *filename);
	
	/**
	 * End output into text files.
	 */
	void CloseFiles();
	
	/**
	 * Translate an entire function.
	 */
	void TranslateFunction(KUInt32 inFirst, KUInt32 inLast, const char *inName, bool inSafeMode=true);
	
	/**
	 * Subroutine to translate an instruction.
	 *
	 * \param inInstruction	instruction to translate.
	 * \param inVAddr	address of the instruction.
	 */
	void Translate(KUInt32 inVAddr, KUInt32 inInstruction);
	
protected:
	
	/**
	 * Subroutine to put the beginning of the test into the target.
	 *
	 * \param the kind of test.
	 */
	void PutTestBegin( int inTest );
	
	/**
	 * Subroutine to put the end of the test into the target.
	 *
	 * \param the kind of test.
	 */
	void PutTestEnd( int inTest );
	
	void DoTranslate_00( KUInt32 inVAddr, KUInt32 inInstruction );
	void DoTranslate_01( KUInt32 inVAddr, KUInt32 inInstruction );
	void DoTranslate_10( KUInt32 inVAddr, KUInt32 inInstruction );
	void DoTranslate_11( KUInt32 inVAddr, KUInt32 inInstruction );
	
	void Translate_SingleDataTransfer( KUInt32 inVAddr, KUInt32 inInstruction );
	void Translate_BlockDataTransfer( KUInt32 inVAddr, KUInt32 inInstruction );
	void Translate_DataProcessingPSRTransfer(KUInt32 inVAddr, KUInt32 inInstruction);
	
	void TestOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 Rn, KUInt32 thePushedValue);
	void ArithmeticOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rn, KUInt32 Rd, KUInt32 thePushedValue);
	void LogicalOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rn, KUInt32 Rd, KUInt32 thePushedValue);
	void MoveOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rd, KUInt32 thePushedValue);
	
	void Translate_Branch(KUInt32 inVAddr, KUInt32 inInstruction);
	
	void Translate_BlockDataTransfer_LDM1(KUInt32 inVAddr, KUInt32 inInstruction);
	void Translate_BlockDataTransfer_LDM2(KUInt32 inVAddr, KUInt32 inInstruction);
	void Translate_BlockDataTransfer_LDM3(KUInt32 inVAddr, KUInt32 inInstruction);
	void Translate_BlockDataTransfer_STM1(KUInt32 inVAddr, KUInt32 inInstruction);
	void Translate_BlockDataTransfer_STM2(KUInt32 inVAddr, KUInt32 inInstruction);

	void JITUnitBegin();
	void JITUnitEnd();
	
	void PushSymbol(const char *);
	void PushValue(KUInt32);
	void PushValueHex(KUInt32);
	void PushAddress(KUInt32);
	
	TMemory *pMemory;
	FILE *pCOut;
	FILE *pHOut;
	KUInt32 pFunctionBegin;
	KUInt32 pFunctionEnd;
};


#if 0
#include "TJITPage.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include "TJITPerformance.h"
#endif

class TARMProcessor;
union JITUnit;
class TJITGeneric;

// Function.
typedef JITUnit* (*JITFuncPtr)(JITUnit* ioUnit, TARMProcessor* ioCPU);

typedef union JITUnit {
	KUIntPtr	fPtr;
	KUInt32		fValue;
	JITFuncPtr	fFuncPtr;
} JITUnit;

///
/// Class for a JIT page for a generic threaded emulation.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TJITGenericPage
:
public TJITPage< TJITGenericPage >
{
	
public:
	///
	/// Access from TJITGeneric
	///
	friend class TJITGeneric;
	
	///
	/// Default constructor.
	///
	TJITGenericPage( void );
	
	///
	/// Destructor.
	///
	~TJITGenericPage( void );
	
	///
	/// Init with the memory interface, a virtual address and a physical address.
	///
	void Init(
			  TMemory* inMemoryIntf,
			  KUInt32 inVAddr,
			  KUInt32 inPAddr );
	
	///
	/// Push a unit in the table, resizing the table if required.
	///
	void PushUnit(KUInt16* ioUnitCrsr, JITFuncPtr inUnit) {
		PushUnit(ioUnitCrsr, (KUIntPtr) inUnit);
	}
	
	///
	/// Push a unit in the table, resizing the table if required.
	///
	void PushUnit(KUInt16* ioUnitCrsr, KUIntPtr inUnit);
	
	///
	/// Get the unit for a given (instruction) offset.
	///
	inline JITUnit* GetJITUnitForOffset(KUInt32 inOffset) {
		return &mUnits[mUnitsTable[inOffset]];
	}
	
	///
	/// Subroutine to translate an instruction.
	///
	/// \param ioUnit	unit to translate.
	/// \param inPAddr	physical address of the instruction.
	/// \param inInstruction	instruction to translate.
	///
	void Translate(
				   TMemory* inMemoryIntf,
				   KUInt16* ioUnitCrsr,
				   KUInt32 inInstruction,
				   KUInt32 inVAddr );
	
private:
	/// Test bits.
	enum ETestKind {
		kTestEQ = 0x0,
		kTestNE = 0x1,
		kTestCS = 0x2,
		kTestCC = 0x3,
		kTestMI = 0x4,
		kTestPL = 0x5,
		kTestVS = 0x6,
		kTestVC = 0x7,
		kTestHI = 0x8,
		kTestLS = 0x9,
		kTestGE = 0xA,
		kTestLT = 0xB,
		kTestGT = 0xC,
		kTestLE = 0xD,
		kTestAL = 0xE,
		kTestNV = 0xF,
	};
	
	///
	/// End of page.
	///
	/// \param inInstruction	current instruction.
	///
	static JITUnit* EndOfPage(
							  JITUnit* ioUnit,
							  TARMProcessor* ioObject );
	
	///
	/// Halt (used for stepping).
	///
	/// \param inInstruction	current instruction.
	///
	static JITUnit* Halt(
						 JITUnit* ioUnit,
						 TARMProcessor* ioObject );
	
	///
	/// Subroutine to put the test in the units table.
	///
	/// \param inInstruction	instruction to translate.
	/// \param ioUnitCrsr		cursor in the unit table.
	/// \return the kind of test.
	///
	void PutTest(
				 KUInt16 inUnitCrsr,
				 unsigned char inDelta,
				 int inTest );
	
	///
	/// Subroutine to translate an instruction and replace the unit.
	/// Called when first two bits are 00.
	///
	/// \param inInstruction	instruction to translate.
	/// \param ioUnitCrsr		cursor in the unit table.
	/// \param ioUnit			unit to fill.
	///
	void DoTranslate_00(
						KUInt16* ioUnitCrsr,
						KUInt32 inInstruction,
						KUInt32 inVAddr );
	
	///
	/// Subroutine to translate an instruction and replace the unit.
	/// Called when first two bits are 01.
	///
	/// \param inInstruction	instruction to translate.
	/// \param ioUnitCrsr		cursor in the unit table.
	/// \param ioUnit			unit to fill.
	///
	void DoTranslate_01(
						TMemory* inMemoryIntf,
						KUInt16* ioUnitCrsr,
						KUInt32 inInstruction,
						KUInt32 inVAddr );
	
	///
	/// Subroutine to translate an instruction and replace the unit.
	/// Called when first two bits are 10.
	///
	/// \param inInstruction	instruction to translate.
	/// \param ioUnitCrsr		cursor in the unit table.
	/// \param ioUnit			unit to fill.
	///
	void DoTranslate_10(
						KUInt16* ioUnitCrsr,
						KUInt32 inInstruction,
						KUInt32 inVAddr );
	
	///
	/// Subroutine to translate an instruction and replace the unit.
	/// Called when first two bits are 11.
	///
	/// \param inInstruction	instruction to translate.
	/// \param ioUnitCrsr		cursor in the unit table.
	/// \param ioUnit			unit to fill.
	///
	void DoTranslate_11(
						KUInt16* ioUnitCrsr,
						KUInt32 inInstruction,
						KUInt32 inVAddr );
	
	/// \name Constants
	enum {
		kInstructionCount = (TJITPage< TJITGenericPage >::kPageSize / 4),
		kDefaultUnitCount = 3 * kInstructionCount,
		kUnitIncrement = 32,
	};
	
	/// \name Variables
	KUInt32			mUnitCount;	///< Total number of units in this page.
								///< This is initialized with a reasonable
								///< default and increased as required.
	KUInt16			mUnitsTable[kInstructionCount];
	///< Array with the index of a unit for a given
	///< address. This is used to find out the
	///< proper unit when jumping...
	JITUnit*		mUnits;		///< Array with all the units.
};

#endif

#endif
// _TJITGENERICPAGE_H

// ======================================== //
// Ask not for whom the <CONTROL-G> tolls. //
// ======================================== //
