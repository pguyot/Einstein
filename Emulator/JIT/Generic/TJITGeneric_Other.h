// ==============================
// File:			TJITGeneric_Other.h
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

#ifndef _TJITGENERIC_OTHER_H
#define _TJITGENERIC_OTHER_H

#include <K/Defines/KDefinitions.h>
#include "JIT.h"

// Einstein
#include "TARMProcessor.h"

#include "TJITGeneric_Macros.h"

KUInt32
Translate_Injection(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr );

KUInt32
Translate_SimulatorInjection(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr );

void
Translate_SWIAndCoproc(
					   JITPageClass* inPage,
					   KUInt16* ioUnitCrsr,
					   KUInt32 inInstruction,
					   KUInt32 inVAddr );

void
Translate_Branch(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr );

JITInstructionProto(DebuggerUND);
JITInstructionProto(UndefinedInstruction);
JITInstructionProto(SoftwareBreakpoint);

#endif
		// _TJITGENERIC_OTHER_H

// =========================== //
// System checkpoint complete. //
// =========================== //
