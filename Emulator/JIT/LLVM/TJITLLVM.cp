// ==============================
// File:			TJITLLVM.cp
// Project:			Einstein
//
// Copyright 2013-2014 by Paul Guyot (pguyot@kallisys.net).
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

#ifdef JITTARGET_LLVM

// Einstein
#include "TMemory.h"
#include "TJITLLVMPage.h"
#include "TJITLLVM.h"
#include "TARMProcessor.h"
#include "TEmulator.h"
#include "TMemoryConsts.h"
#include "Emulator/NativeCalls/TVirtualizedCallsPatches.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>

using namespace llvm;

// -------------------------------------------------------------------------- //
// Constants
// -------------------------------------------------------------------------- //

// This is required before constructing the first TJITLLVM object.
__attribute__((constructor))
static void InitLLVM() {
	InitializeNativeTarget();
}

// -------------------------------------------------------------------------- //
//  * TJITLLVM( void )
// -------------------------------------------------------------------------- //
TJITLLVM::TJITLLVM(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf )
	:
		TJIT<TJITLLVM, TJITLLVMPage>( inMemoryIntf, inMMUIntf )
{
}

// -------------------------------------------------------------------------- //
//  * ~TJITLLVM( void )
// -------------------------------------------------------------------------- //
TJITLLVM::~TJITLLVM( void )
{
}

// -------------------------------------------------------------------------- //
//  * Run( TARMProcessor*, volatile KUInt32* )
// -------------------------------------------------------------------------- //
void
TJITLLVM::Run( TARMProcessor* ioCPU, volatile bool* inSignal )
{
	KUInt32* pcPtr = &ioCPU->mCurrentRegisters[TARMProcessor::kR15];
	TMemory* theMemoryInterface = ioCPU->mMemory;
	TEmulator* theEmulator = ioCPU->mEmulator;
	JITFuncPtr theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
	while (true)
	{
		while (*inSignal)
		{
			// Here we go, iterating...
			theJITFunc = theJITFunc(ioCPU, inSignal);
		}
		
		// We may have been signaled because there was an interrupt.
		if (theEmulator->IsInterrupted())
		{
			KUInt32 theInterrupt = ioCPU->mPendingInterrupts;
			theEmulator->AckInterrupt();
			if (theInterrupt & TARMProcessor::kResetInterrupt)
			{
				ioCPU->Reset();
				// Unmaskable.
				theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kFIQInterrupt) && !ioCPU->mCPSR_F) {
				ioCPU->FIQ();
				theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kIRQInterrupt) && !ioCPU->mCPSR_I) {
				ioCPU->IRQ();
				theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
			}
		} else {
			break;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * Step( TARMProcessor*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITLLVM::Step( TARMProcessor* ioCPU, KUInt32 count )
{
	KUInt32* pendingInterrupts = &ioCPU->mPendingInterrupts;
	KUInt32* pcPtr = &ioCPU->mCurrentRegisters[TARMProcessor::kR15];
	TMemory* theMemoryInterface = ioCPU->mMemory;
	while (count-- > 0)
	{
		bool signal = 0;
    	JITFuncPtr theJITFunc = GetJITFuncForSingleInstructionAtPC(ioCPU, theMemoryInterface, *pcPtr);
    	(void) theJITFunc(ioCPU, &signal);

		// Maybe there is an interrupt...
		KUInt32 theInterrupt = *pendingInterrupts;
		if (theInterrupt)
		{
			if (theInterrupt & TARMProcessor::kResetInterrupt)
			{
				ioCPU->Reset();
				// Unmaskable.
			} else if ((theInterrupt & TARMProcessor::kFIQInterrupt) && !ioCPU->mCPSR_F) {
				ioCPU->FIQ();
			} else if ((theInterrupt & TARMProcessor::kIRQInterrupt) && !ioCPU->mCPSR_I) {
				ioCPU->IRQ();
			}
		}
	} // while
}

// -------------------------------------------------------------------------- //
//  * GetJITFuncForPC( TARMProcessor*, TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVM::GetJITFuncForPC(
					TARMProcessor* ioCPU,
					TMemory* inMemoryInterface,
					KUInt32 inPC )
{
	// Get the page from the cache.
	KUInt32 pc = inPC - 4;
	TJITLLVMPage* thePage = GetPage(pc);

	if (thePage == NULL)	
	{
		// Let's manage the exception
		ioCPU->PrefetchAbort();
		
		// Redo the translation.
		pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15];
		return GetJITFuncForPC( ioCPU, inMemoryInterface, pc );
	}

	KUInt32 indexInPage = GetOffsetInPage(pc) / sizeof( KUInt32 );

	// Return the function.
	return thePage->GetJITFuncForOffset(inMemoryInterface, indexInPage);
}

// -------------------------------------------------------------------------- //
//  * GetJITFuncForSingleInstructionAtPC( TARMProcessor*, TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVM::GetJITFuncForSingleInstructionAtPC(
					TARMProcessor* ioCPU,
					TMemory* inMemoryInterface,
					KUInt32 inPC )
{
	// Get the page from the cache.
	KUInt32 pc = inPC - 4;
	TJITLLVMPage* thePage = GetPage(pc);

	if (thePage == NULL)	
	{
		// Let's manage the exception
		ioCPU->PrefetchAbort();
		
		// Redo the translation.
		pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15];
		return GetJITFuncForPC( ioCPU, inMemoryInterface, pc );
	}

	KUInt32 indexInPage = GetOffsetInPage(pc) / sizeof( KUInt32 );

	// Return the function.
	return thePage->GetJITFuncForSingleInstructionAtOffset(inMemoryInterface, indexInPage);
}

// -------------------------------------------------------------------------- //
//  * DoPatchROM(KUInt32* romPtr, const std::string& inMachineName)
// -------------------------------------------------------------------------- //
void
TJITLLVM::DoPatchROM(KUInt32* romPtr, const std::string& inMachineName) {
	TVirtualizedCallsPatches::DoPatchROM(romPtr, inMachineName);
}


#endif

// ============================================================================ //
// FORTRAN, "the infantile disorder", by now nearly 20 years old, is hopelessly //
// inadequate for whatever computer application you have in mind today: it is   //
// too clumsy, too risky, and too expensive to use.                             //
//                 -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5  //
// ============================================================================ //
