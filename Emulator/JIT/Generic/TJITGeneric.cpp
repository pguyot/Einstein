// ==============================
// File:			TJITGeneric.cp
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
#include "Emulator/TMemory.h"
#include "Emulator/JIT/Generic/TJITGeneric.h"
#include "Emulator/JIT/Generic/TJITGenericPage.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemoryConsts.h"

#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"

#include "Emulator/NativeCalls/TVirtualizedCallsPatches.h"

#if 0
//
// The following code is a test for cooperative multitaksing as it is
// used in NewtonOS. The goal is, to move the multitasking from the
// emulation into the Einstein code, accelerating the emultion
// and getting closer to simulation and finally native code.
//

#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>

void
assign(long a, int *b)
{
	*b = (int)a;
}

int
test()
{
	ucontext_t uc, back;
	size_t sz = 0x10000;
	int value = 0;
	
	getcontext(&uc);
	
	uc.uc_stack.ss_sp = mmap(0, sz,
							 PROT_READ | PROT_WRITE | PROT_EXEC,
							 MAP_PRIVATE | MAP_ANON, -1, 0);
	uc.uc_stack.ss_size = sz;
	uc.uc_stack.ss_flags = 0;
	
	uc.uc_link = &back;
	
	makecontext(&uc, (void(*)())assign, 2, 123L, &value);
	swapcontext(&back, &uc);
	
	KPrintf("done %d\n", value);
	
	return (0);
}

/*
 // The ARM command movs pc, lr is commonly used to return from 
 // an exception or an interrupt routine. Here, we try to find
 // all point that may require task switching.
 
 movs pc,lr
 
 0x00019428 SaveCPUStateAndStopSystem
 0x0038D504 ResumeImage
 0x0038D78C FIQCleanUp
 -- floating point:
 0x0038D87C _getFPSCR
 0x0038D884 _setFPSCR
 0x0038D894 _rint
 0x0038D8A4 _rintZ
 0x0038D8B4 _rintP
 0x0038D8C4 _rintM
 0x0039262C Unnamed_00391CAC (FP_UndefHandlers_Start)
 --
 0x00392CD0 IRQCleanUp
 0x00392FD4 IRQCleanUp
 0x0039306C IRQCleanUp
 0x00393110 IRQCleanUp
 // all DatAbortHandler code seems to ond in SWI. The faulty memory
 // access generates a task switch into a "Monitor". The Monitor
 // maps RAM into the location that generated the fault, then 
 // suspends itself. The original task is rescheduled to execute 
 // the failed operation again.
 0x00393944 DataAbortHandler
 0x00393B80 DataAbortHandler
 0x003ADA6C DataAbortHandler
 0x003ADB10 DataAbortHandler
 0x003ADBB0 DataAbortHandler
 0x003ADD78 SWI?
 */

#endif


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TJITGeneric( void )
// -------------------------------------------------------------------------- //
TJITGeneric::TJITGeneric(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf )
	:
		TJIT<TJITGeneric, TJITGenericPage>( inMemoryIntf, inMMUIntf )
{
}

// -------------------------------------------------------------------------- //
//  * ~TJITGeneric( void )
// -------------------------------------------------------------------------- //
TJITGeneric::~TJITGeneric( void )
{
}

// -------------------------------------------------------------------------- //
//  * Run( TARMProcessor*, volatile Boolean* )
// -------------------------------------------------------------------------- //
void
TJITGeneric::Run( TARMProcessor* ioCPU, volatile Boolean* inSignal )
{
	volatile KUInt32* pendingInterrupts = &ioCPU->mPendingInterrupts;
	KUInt32* pcPtr = &ioCPU->mCurrentRegisters[TARMProcessor::kR15];
	TMemory* theMemoryInterface = ioCPU->mMemory;
	TEmulator* theEmulator = ioCPU->mEmulator;
	JITUnit* theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
	while (true)
	{
		while (*inSignal)
		{
			// Here we go, iterating...
			// *pcPtr is 4 bytes ahead of the instruction that will
			// actually be executed here
			theJITUnit = theJITUnit->fFuncPtr( theJITUnit, ioCPU );
		}
		
		// We may have been signaled because there was an interrupt.
		if (theEmulator->IsInterrupted())
		{
			KUInt32 theInterrupt = *pendingInterrupts;
			theEmulator->AckInterrupt();
			if (theInterrupt & TARMProcessor::kResetInterrupt)
			{
				ioCPU->Reset();
				// Unmaskable.
				theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kFIQInterrupt) && !ioCPU->mCPSR_F) {
				ioCPU->FIQ();
				theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kIRQInterrupt) && !ioCPU->mCPSR_I) {
				ioCPU->IRQ();
				theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
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
TJITGeneric::Step( TARMProcessor* ioCPU, KUInt32 count )
{
	KUInt32* pendingInterrupts = &ioCPU->mPendingInterrupts;
	KUInt32* pcPtr = &ioCPU->mCurrentRegisters[TARMProcessor::kR15];
	TMemory* theMemoryInterface = ioCPU->mMemory;
	JITUnit* theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
	while (count-- > 0)
	{
		// To make sure we execute only one instruction, insert a halt for the
		// next instruction.
		JITUnit* theNextJITUnit =
			GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr + 4 );
		KUIntPtr theSavedValue = 0;
		if (theNextJITUnit) {
			theSavedValue = theNextJITUnit->fPtr;
			theNextJITUnit->fFuncPtr = TJITGenericPage::Halt;
		}
		// Execute only one instruction.
		theJITUnit = theJITUnit->fFuncPtr( theJITUnit, ioCPU );
		if (theNextJITUnit) {
			theNextJITUnit->fPtr = theSavedValue;
		}

		// Maybe there is an interrupt...
		KUInt32 theInterrupt = *pendingInterrupts;
		if (theInterrupt)
		{
			if (theInterrupt & TARMProcessor::kResetInterrupt)
			{
				ioCPU->Reset();
				// Unmaskable.
				theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kFIQInterrupt) && !ioCPU->mCPSR_F) {
				ioCPU->FIQ();
				theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kIRQInterrupt) && !ioCPU->mCPSR_I) {
				ioCPU->IRQ();
				theJITUnit = GetJITUnitForPC( ioCPU, theMemoryInterface, *pcPtr );
			}
		}
	} // while
}

// -------------------------------------------------------------------------- //
//  * GetJITUnitForPC( KUInt32 inPC )
// -------------------------------------------------------------------------- //
JITUnit*
TJITGeneric::GetJITUnitForPC(
					TARMProcessor* ioCPU,
					TMemory* inMemoryInterface,
					KUInt32 inPC )
{
	// Get the page from the cache.
	KUInt32 pc = inPC - 4;
	TJITGenericPage* thePage = GetPage(pc);

	if (thePage == NULL)	
	{
		// Let's manage the exception
		ioCPU->PrefetchAbort();
		
		// Redo the translation.
		pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15];
		return GetJITUnitForPC( ioCPU, inMemoryInterface, pc );
	}

	KUInt32 indexInPage = GetOffsetInPage(pc) / sizeof( KUInt32 );

	// Return the unit.
	return thePage->GetJITUnitForOffset(indexInPage);
}


KSInt32
TJITGeneric::GetJITUnitDelta(
							 TARMProcessor* ioCPU,
							 TMemory* inMemoryInterface,
							 JITUnit* inUnit,
							 KUInt32 inPC)
{
	// Get the page from the cache.
	KUInt32 pc = inPC - 4;
	TJITGenericPage* theNextPage = GetPage(pc);
	if (theNextPage == NULL) {
		return kNotTheSamePage;
	}
	
	TJITGenericPage* theCurrentPage = GetPage(ioCPU->GetRegister(15));
	if (theCurrentPage!=theNextPage) {
		return kNotTheSamePage;
	}
	
	KUInt32 indexInPage = GetOffsetInPage(pc) / sizeof( KUInt32 );	
	JITUnit *nextUnit = theNextPage->GetJITUnitForOffset(indexInPage);
	
	KSInt32 delta = (KSInt32)(nextUnit - inUnit);
	return delta;
}

// -------------------------------------------------------------------------- //
//  * DoPatchROM(KUInt32* romPtr, const std::string& inMachineName)
// -------------------------------------------------------------------------- //
void
TJITGeneric::DoPatchROM(KUInt32* romPtr, KSInt32 inROMId)
{
	TJITGenericPatchManager::DoPatchROM(romPtr, inROMId);
	TVirtualizedCallsPatches::DoPatchROM(romPtr, inROMId);
}


#endif

// ============================================================================ //
// FORTRAN, "the infantile disorder", by now nearly 20 years old, is hopelessly //
// inadequate for whatever computer application you have in mind today: it is   //
// too clumsy, too risky, and too expensive to use.                             //
//                 -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5  //
// ============================================================================ //
