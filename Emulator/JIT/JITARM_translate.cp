// ==============================
// File:			JITARM_translate.cp
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
#include "JITARM.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * JITARM_translate( void )
// -------------------------------------------------------------------------- //
JITARM_translate::JITARM_translate( void )
{
}

// -------------------------------------------------------------------------- //
//  * ~JITARM_translate( void )
// -------------------------------------------------------------------------- //
JITARM_translate::~JITARM_translate( void )
{
}

// -------------------------------------------------------------------------- //
//  * Translate( TARMProcessor*, KUInt32, JITUnit* )
// -------------------------------------------------------------------------- //
// Determine if the instruction to translate is special (manipulates pc, lr,
// cpsr) or privileged.
JITUnit*
TJITGeneric::Translate(
				JITUnit* ioUnit,
				TJITGeneric* inJITObject,
				TARMProcessor* ioCPU )
{
	JITFuncPtr* theInstFuncPtr = &ioUnit->fSecondFuncPtr;
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15];
	KUInt32 bankNumber;
	KUInt32 offsetInBank;
	TMemory* theMemoryInterface = ioCPU->mMemory;
	(void) (theMemoryInterface->TranslateInstruction(
			(TMemory::VAddr) pc - 8,
			&bankNumber,
			&offsetInBank ));
	KUInt32 theInstruction =
		theMemoryInterface->ReadInstruction( bankNumber, offsetInBank );
	ioUnit->fInstruction = theInstruction;

	if (DoTranslateTest(theInstruction, &ioUnit->fFirstFuncPtr))
	{
		theInstFuncPtr = &ioUnit->fFirstFuncPtr;
	}

	switch ((theInstruction >> 26) & 0x3)				// 27 & 26
	{
		case 0x0:	// 00
			DoTranslate_00(theInstruction, theInstFuncPtr, ioUnit);
			break;
				
		case 0x1:	// 01
			DoTranslate_01(
					theInstruction,
					ioCPU,
					bankNumber,
					offsetInBank,
					ioUnit,
					theInstFuncPtr );
			break;
			
		case 0x2:	// 10
			DoTranslate_10(theInstruction, theInstFuncPtr, ioUnit);
			break;
	
		case 0x3:
			DoTranslate_11(theInstruction, theInstFuncPtr, ioUnit);
			break;
	} // switch 27 & 26
	
	// Call condition function.
	return ioUnit->fFirstFuncPtr( ioUnit, inJITObject, ioCPU );
}

// ======================================================================= //
// I've never been canoeing before, but I imagine there must be just a few //
// simple heuristics you have to remember...                               //
//                                                                         //
// Yes, don't fall out, and don't hit rocks.                               //
// ======================================================================= //
