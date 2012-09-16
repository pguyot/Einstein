// ==============================
// File:			PrecompiledPatches.h
// Project:			Einstein
//
// Copyright 2012 by Matthias Melcher (einstein@matthiasm.com).
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

#ifndef _PRECOMPILED_PATCHES_H
#define _PRECOMPILED_PATCHES_H


#include <K/Defines/KDefinitions.h>
#include "TROMImage.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"


#define PRECOMPILED_FUNCTION(address) \
JITInstructionProto(pcf_##address); \
TROMPatch p##address(address, pcf_##address, #address); \
JITInstructionProto(pcf_##address)

//CALLNEXTUNIT;
//SETPC(GETPC());
//MMUCALLNEXT_AFTERSETPC;

#endif

// ================================================================== //
// MAC user's dynamic debugging list evaluator?  Never heard of that. //
// ================================================================== //
