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
class TSymbolList;

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
	TJITGenericRetarget(TMemory *inMemory, TSymbolList *inSymbolList);
	
	/**
	 * Release all resources.
	 */
	~TJITGenericRetarget();
	
	/**
	 * Start output into text files.
	 */
	bool OpenFiles(const char *filePathAndName);
	
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

	TMemory *pMemory;
	TSymbolList *pSymbolList;
	FILE *pCOut;
	FILE *pHOut;
	KUInt32 pFunctionBegin;
	KUInt32 pFunctionEnd;
};

#endif
// _TJITGENERICRETARGET_H

// ======================================== //
// Ask not for whom the <CONTROL-G> tolls. //
// ======================================== //
