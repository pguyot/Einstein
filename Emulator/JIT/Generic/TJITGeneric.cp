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
#include "JIT.h"

// Einstein
#include "TMemory.h"
#include "TJITGenericPage.h"
#include "TARMProcessor.h"
#include "TEmulator.h"
#include "TMemoryConsts.h"

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
		TJIT<TJITGenericPage>( inMemoryIntf, inMMUIntf )
{
}

// -------------------------------------------------------------------------- //
//  * ~TJITGeneric( void )
// -------------------------------------------------------------------------- //
TJITGeneric::~TJITGeneric( void )
{
}

// -------------------------------------------------------------------------- //
//  * Run( TARMProcessor*, volatile KUInt32* )
// -------------------------------------------------------------------------- //
void
TJITGeneric::Run( TARMProcessor* ioCPU, volatile KUInt32* inSignal )
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
		KUIntPtr theSavedValue;
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
	
	KSInt32 delta = nextUnit - inUnit;
	
	printf("Finding Delta at 0x%08x = %d\n", ioCPU->GetRegister(15), delta);
	if (delta<0 || delta>500)
		return kNotTheSamePage; // FIXME: kludge to avoid crash
	return delta;
}


// ============================================================================ //
// FORTRAN, "the infantile disorder", by now nearly 20 years old, is hopelessly //
// inadequate for whatever computer application you have in mind today: it is   //
// too clumsy, too risky, and too expensive to use.                             //
//                 -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5  //
// ============================================================================ //
