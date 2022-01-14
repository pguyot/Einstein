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
#include "Emulator/JIT/JIT.h"

// Einstein
#include "Emulator/TARMProcessor.h"

#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"

void
Translate_BlockDataTransfer(
	JITPageClass* inPage,
	KUInt16* ioUnitCrsr,
	KUInt32 inInstruction,
	KUInt32 inVAddr);

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
