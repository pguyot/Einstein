// ==============================
// File:			Translate_HalfwordAndSignedDataTransferReg.cpp
// Project:			Einstein
//
// Copyright 2003-2007,2020 by Paul Guyot (pguyot@kallisys.net).
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

#ifdef JITTARGET_GENERIC

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"

#include "TJITGeneric_Macros.h"

#define IMPLEMENTATION	1
#include "TJITGeneric_HalfwordAndSignedDataTransferReg_template.t"
#undef IMPLEMENTATION

static JITFuncPtr HalfwordAndSignedDataTransferReg_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_HalfwordAndSignedDataTransferReg_template.t"
#undef TRANSLATION_ARRAY
};

void
Translate_HalfwordAndSignedDataTransferReg(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
    // Get the index.
	// -Cond-- 0  0  0  P  U  0  W  L  --Rn--- --Rd--- 0 0 0 0 1  S  H  1  --Rm--- Halfword and Signed Data Transfer (armv4)
    KUInt32 theIndex = (inInstruction & 0x00000060) >> 5;   //     SH
    theIndex |= (inInstruction & 0x00300000) >> 18;         //   WL00
    theIndex |= (inInstruction & 0x01800000) >> 19;         // PU0000
    PUSHFUNC(HalfwordAndSignedDataTransferReg_Funcs[theIndex]);
    PUSHVALUE(inInstruction);
    // Always push PC, in case we have a dataabort we'll need it.
    PUSHVALUE(inVAddr + 8);
}

void TJITGeneric_HalfwordAndSignedDataTransferReg_assertions( void );

void TJITGeneric_HalfwordAndSignedDataTransferReg_assertions( void )
{
	// Check that the array has the expected size.
    KCOMPILE_TIME_ASSERT_SIZE( HalfwordAndSignedDataTransferReg_Funcs, 0x04000 * sizeof(void*) );
}

#endif

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
