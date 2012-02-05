// ==============================
// File:			TJITGeneric_SingleDataTransfer.cp
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
#include <K/Tests/KDebug.h>
#include "JIT.h"

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"

#include "TJITGeneric_Macros.h"

#define IMPLEMENTATION	1
#include "TJITGeneric_SingleDataTransfer_template.t"
#undef IMPLEMENTATION

static JITFuncPtr SingleDataTransfer_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_SingleDataTransfer_template.t"
#undef TRANSLATION_ARRAY
};


#define DirectSingleDataTransfer(Rd) \
JITInstructionProto(DirectSingleDataTransfer_ ## Rd) { \
	KUInt32 theValue; \
	POPVALUE(theValue); \
	ioCPU->mCurrentRegisters[Rd] = theValue; \
	CALLNEXTUNIT; \
}
DirectSingleDataTransfer(0)
DirectSingleDataTransfer(1)
DirectSingleDataTransfer(2)
DirectSingleDataTransfer(3)
DirectSingleDataTransfer(4)
DirectSingleDataTransfer(5)
DirectSingleDataTransfer(6)
DirectSingleDataTransfer(7)
DirectSingleDataTransfer(8)
DirectSingleDataTransfer(9)
DirectSingleDataTransfer(10)
DirectSingleDataTransfer(11)
DirectSingleDataTransfer(12)
DirectSingleDataTransfer(13)
DirectSingleDataTransfer(14)
DirectSingleDataTransfer(15)


static JITFuncPtr DirectSingleDataTransfer_Funcs[16] = {
	DirectSingleDataTransfer_0,
	DirectSingleDataTransfer_1,
	DirectSingleDataTransfer_2,
	DirectSingleDataTransfer_3,
	DirectSingleDataTransfer_4,
	DirectSingleDataTransfer_5,
	DirectSingleDataTransfer_6,
	DirectSingleDataTransfer_7,
	DirectSingleDataTransfer_8,
	DirectSingleDataTransfer_9,
	DirectSingleDataTransfer_10,
	DirectSingleDataTransfer_11,
	DirectSingleDataTransfer_12,
	DirectSingleDataTransfer_13,
	DirectSingleDataTransfer_14,
	DirectSingleDataTransfer_15, // Don't! This would be a branch instruction!
};


void
Translate_SingleDataTransfer(
					JITPageClass* inPage,
					TMemory* inMemoryIntf,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// special handling for reading a word form ROM
	if ( (inInstruction & 0x0fff0000) == 0x059F0000 && (inInstruction&0x00000fff)+inVAddr<0x00800000) {
		// The instruction "ldr r2, =12345" is used quite often. It is relatively slow
		// because it needs to do a round trip through the MMU. With this shortcut, we avoid 
		// reading the ROM and gain 3% perfomrance.
		KUInt32 theIndex = (inInstruction & 0x0000F000) >> 12;
		KUInt32 theAddress = (inInstruction&0x00000fff)+inVAddr+8;
		KUInt32 theValue;
		PUSHFUNC(DirectSingleDataTransfer_Funcs[theIndex]);
		inMemoryIntf->Read(theAddress, theValue);
		PUSHVALUE(theValue);
	} else {
		// Get the index.
		KUInt32 theIndex = (inInstruction & 0x03FFF000) >> 12;
		PUSHFUNC(SingleDataTransfer_Funcs[theIndex]);
		PUSHVALUE(inInstruction);
		// Always push PC, in case we have a dataabort we'll need it.
		PUSHVALUE(inVAddr + 8);
	}
}

void TJITGeneric_SingleDataTransfer_assertions( void );

void TJITGeneric_SingleDataTransfer_assertions( void )
{
	// Check that the array has the expected size.
    KCOMPILE_TIME_ASSERT_SIZE( SingleDataTransfer_Funcs, 0x04000 * sizeof(void*) );
}

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
