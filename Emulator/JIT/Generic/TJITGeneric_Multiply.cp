// ==============================
// File:			TJITGeneric_Multiply.cp
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
#include "TJITGenericPage.h"

// Einstein
#include "TARMProcessor.h"

#include "TJITGeneric_Macros.h"
#include "TJITGeneric_Other.h"

#define IMPLEMENTATION	1
#include "TJITGeneric_Multiply_template.t"
#undef IMPLEMENTATION

static JITFuncPtr Multiply_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_Multiply_template.t"
#undef TRANSLATION_ARRAY
};

void
Translate_Multiply(
					TJITGenericPage* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Get the index.
	const KUInt32 flag_s = (inInstruction & 0x00100000) >> 20;
	const KUInt32 Rd = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = inInstruction & 0x0000000F;
	if ((Rd == 15) || (Rs == 15) || (Rm == 15))
	{
		PUSHFUNC(UndefinedInstruction);
		PUSHVALUE(inVAddr + 8);
	} else {
		const KUInt32 theIndex =
			(((((flag_s * 15) + Rd) * 15) + Rs) * 15) + Rm;
		PUSHFUNC(Multiply_Funcs[theIndex]);
	}
}

void TJITGeneric_Multiply_assertions( void );

void TJITGeneric_Multiply_assertions( void )
{
	// Check that the array has the expected size.
    KCOMPILE_TIME_ASSERT_SIZE(
    	Multiply_Funcs, 2 * 15 * 15 * 15 * sizeof(void*) );
}

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
