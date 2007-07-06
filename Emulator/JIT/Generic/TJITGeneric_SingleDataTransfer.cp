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
#include "TJITGenericPage.h"

// Einstein
#include "TARMProcessor.h"

#include "TJITGeneric_Macros.h"

#define IMPLEMENTATION	1
#include "TJITGeneric_SingleDataTransfer_template.t"
#undef IMPLEMENTATION

static JITFuncPtr SingleDataTransfer_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_SingleDataTransfer_template.t"
#undef TRANSLATION_ARRAY
};

void
Translate_SingleDataTransfer(
					TJITGenericPage* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Get the index.
	KUInt32 theIndex = (inInstruction & 0x03FFF000) >> 12;
	PUSHFUNC(SingleDataTransfer_Funcs[theIndex]);
	PUSHVALUE(inInstruction);
	// Always push PC, in case we have a dataabort we'll need it.
	PUSHVALUE(inVAddr + 8);
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
