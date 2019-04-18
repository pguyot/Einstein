// ==============================
// File:			Proteus.cpp
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
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


#include "Proteus.h"


using namespace NewtOS;


// --- class predeclarations

// --- typedefs

// --- function declaraitions

// --- classes


// --- ROM

//-/* 0x00000000-0x0071FC4C */ NewtonOS in ROM
// /* 0x00000000-0x000E57BC */

// QuickEnableInterrupt:

// /* 0x000E5868-0x001CC4A8 */

// MATT: StartScheduler
// /* 0x001CC514-0x0025215C */

// SwapInGlobals:
// /* 0x00252190-0x003AD750 */

// MATT: _SWI_Scheduler

// /* 0x003ADBB4-0x003AE4AC */
// SetAndClearBitsAtomic:
// /* 0x003AE4F8-0x0071FC4C */

// --- RExBlock

//-/* 0x0071FC4C-0x007EE048 */ NewtonOS ROM Extensions
// /* 0x0071FC4C-0x007EE048 */

// --- Jump Table (not used here)

//-/* 0x01a00000-0x01c1085C */


// ============================================================================== //
// I've ended up calling my cat Brexit. It wakes me up meowing like crazy every
// morning because it wants to go out, but as soon as I open the door, it just
// sits there undecided and then looks angry when I put it outside.
//   -- French Europe Minister Nathalie Loiseau
// ============================================================================== //
