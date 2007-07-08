// ==============================
// File:			TJITGeneric_SingleDataSwap.cp
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
#include "TJITGeneric_SingleDataSwap_template.t"
#undef IMPLEMENTATION

static JITFuncPtr SingleDataSwap_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_SingleDataSwap_template.t"
#undef TRANSLATION_ARRAY
};

void
Translate_SingleDataSwap(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Get the index.
	const KUInt32 theIndex =
		(inInstruction & 0x00400000) >> 10
		| (inInstruction & 0x000FF000) >> 8
		| (inInstruction & 0x0000000F);
	PUSHFUNC(SingleDataSwap_Funcs[theIndex]);
	// Push PC. We'll need it if we have a data abort.
	PUSHVALUE(inVAddr + 8);
}

void TJITGeneric_SingleDataSwap_assertions( void );

void TJITGeneric_SingleDataSwap_assertions( void )
{
	// Check that the array has the expected size.
    KCOMPILE_TIME_ASSERT_SIZE( SingleDataSwap_Funcs, 0x2000 * sizeof(void*) );
}

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
