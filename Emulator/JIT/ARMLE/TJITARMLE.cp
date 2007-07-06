// ==============================
// File:			TJITARMLE.cp
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
#include "TJITARMLE.h"

// Einstein
#include <TMemory.h>
#include <TJITARMLEPage.h>
#include "TARMProcessor.h"
#include "TEmulator.h"
#include "TMemoryConsts.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TJITARMLE( void )
// -------------------------------------------------------------------------- //
TJITARMLE::TJITARMLE(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf )
	:
		TJIT<TJITARMLEPage>( inMemoryIntf, inMMUIntf )
{
}

// -------------------------------------------------------------------------- //
//  * ~TJITARMLE( void )
// -------------------------------------------------------------------------- //
TJITARMLE::~TJITARMLE( void )
{
}

// -------------------------------------------------------------------------- //
//  * Run( TARMProcessor*, volatile KUInt32* )
// -------------------------------------------------------------------------- //
void
TJITARMLE::Run( TARMProcessor* ioCPU, volatile KUInt32* inSignal )
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
TJITARMLE::Step( TARMProcessor* ioCPU, KUInt32 count )
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
			theNextJITUnit->fFuncPtr = TJITARMLEPage::Halt;
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
TJITARMLE::GetJITUnitForPC(
					TARMProcessor* ioCPU,
					TMemory* inMemoryInterface,
					KUInt32 inPC )
{
	// Get the page from the cache.
	KUInt32 pc = inPC - 4;
	TJITARMLEPage* thePage = GetPage(pc);

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

// ==================================== //
// A CONS is an object which cares.     //
//                 -- Bernie Greenberg. //
// ==================================== //
