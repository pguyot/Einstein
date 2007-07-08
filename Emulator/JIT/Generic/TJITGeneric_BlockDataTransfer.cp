// ==============================
// File:			TJITGeneric_BlockDataTransfer.cp
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

#include "TJITGeneric_Macros.h"

#define IMPLEMENTATION	1
#include "TJITGeneric_LDM1_template.t"
#include "TJITGeneric_LDM2_template.t"
#include "TJITGeneric_LDM3_template.t"
#include "TJITGeneric_STM1_template.t"
#include "TJITGeneric_STM2_template.t"
#undef IMPLEMENTATION

// P,U,W,Rn
static JITFuncPtr BlockDataTransfer_LDM1_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_LDM1_template.t"
#undef TRANSLATION_ARRAY
};
// P,U,Rn
static JITFuncPtr BlockDataTransfer_LDM2_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_LDM2_template.t"
#undef TRANSLATION_ARRAY
};
// P,U,W,Rn
static JITFuncPtr BlockDataTransfer_LDM3_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_LDM3_template.t"
#undef TRANSLATION_ARRAY
};
// P,U,W,Rn
static JITFuncPtr BlockDataTransfer_STM1_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_STM1_template.t"
#undef TRANSLATION_ARRAY
};
// P,U,Rn
static JITFuncPtr BlockDataTransfer_STM2_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_STM2_template.t"
#undef TRANSLATION_ARRAY
};

void
Translate_BlockDataTransfer(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	KUInt32 flag_pu =	(inInstruction & 0x01800000) >> (23 - 4);
	KUInt32 flag_puw =	flag_pu << 1 | ((inInstruction & 0x00200000) >> (21 - 4));
	KUInt32 Rn =		(inInstruction & 0x000F0000) >> 16;
	KUInt32 regList =	(inInstruction & 0x0000FFFF);
	KUInt32 nbRegs = CountBits(regList);
	// Different cases.
	if (inInstruction & 0x00100000)
	{
		// Load
		if (inInstruction & 0x00400000)
		{
			if (inInstruction & 0x00008000)
			{
				// LDM3
				PUSHFUNC(BlockDataTransfer_LDM3_Funcs[flag_puw | Rn]);
			} else {
				// LDM2
				PUSHFUNC(BlockDataTransfer_LDM2_Funcs[flag_pu | Rn]);
			}
		} else {
			// LDM1
			PUSHFUNC(BlockDataTransfer_LDM1_Funcs[flag_puw | Rn]);
		}
	} else {
		// Store
		if (inInstruction & 0x00400000)
		{
			// STM2
			PUSHFUNC(BlockDataTransfer_STM2_Funcs[flag_pu | Rn]);
		} else {
			// STM1
			PUSHFUNC(BlockDataTransfer_STM1_Funcs[flag_puw | Rn]);
		}
	}

	// Push the PC. We'll need it for data aborts.
	PUSHVALUE(inVAddr + 8);
	// Push the reg list and the number of registers.
	PUSHVALUE((nbRegs << 16) | regList);
}

void TJITGeneric_BlockDataTransfer_assertions( void );

void TJITGeneric_BlockDataTransfer_assertions( void )
{
	// Check that the array has the expected size.
    KCOMPILE_TIME_ASSERT_SIZE( BlockDataTransfer_LDM1_Funcs, 128 * sizeof(void*) );
    KCOMPILE_TIME_ASSERT_SIZE( BlockDataTransfer_LDM2_Funcs, 64 * sizeof(void*) );
    KCOMPILE_TIME_ASSERT_SIZE( BlockDataTransfer_LDM3_Funcs, 128 * sizeof(void*) );
    KCOMPILE_TIME_ASSERT_SIZE( BlockDataTransfer_STM1_Funcs, 128 * sizeof(void*) );
    KCOMPILE_TIME_ASSERT_SIZE( BlockDataTransfer_STM2_Funcs, 64 * sizeof(void*) );
}

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
