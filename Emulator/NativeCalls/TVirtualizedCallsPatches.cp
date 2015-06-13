// ==============================
// File:			TVirtualizedCallsPatches.cp
// Project:			Einstein
//
// Copyright 2014 by Paul Guyot (pguyot@kallisys.net).
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

#include <K/Defines/KDefinitions.h>
#include "TVirtualizedCallsPatches.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
const KUInt32 kInvocation[4] = {
	0xE92D4000,	// stmdb    sp!, {lr}
	0xE59FE004,	// ldr      lr, 0x0000000C
	0xEE00EA10,	// mcr      10, 0, lr, cr0, cr0, {0}
	0xE8BD8000	// ldmia    sp!, {pc}
	// followed by address.
};

// These virtualization patches overwrite 4x 32-bit words in memory with the
// ARM instructions described by kInvocation, followed by 1x 32-bit word
// containing the enum value associated with the patch in
// k717006VirtualizationPatches or'ed with 0x80000000 (setting bit 31)
//
// TNativePrimitives::ExecuteNative() checks if bit 31 is set and calls into
// mVirtualizedCalls->Execute() if it is.

const KUInt32 k717006VirtualizationPatches[] = {
	// __rt_sdiv + 8, just after the divide by zero test.
	(0x0038CA10 + 8) / sizeof(KUInt32),	TVirtualizedCallsPatches::k__rt_sdiv,
	// __rt_udiv
	(0x0038C8FC + 8) / sizeof(KUInt32),	TVirtualizedCallsPatches::k__rt_udiv,
	// memmove
	//0x00382440 / sizeof(KUInt32),   TVirtualizedCallsPatches::kmemmove,
	// symcmp__FPcT1
	0x00358C9C / sizeof(KUInt32),	TVirtualizedCallsPatches::ksymcmp__FPcT1,
	0
};


// -------------------------------------------------------------------------- //
//  * DoPatchROM(KUInt32*, const std::string&)
// -------------------------------------------------------------------------- //
void
TVirtualizedCallsPatches::DoPatchROM(KUInt32* inROMPtr, const std::string& inMachineName) {
	const KUInt32* patches = nullptr;
	
	if (inMachineName == "717006") {
		patches = k717006VirtualizationPatches;
	}
	
	if (patches) {
		// Iterate on patches.
		while (*patches != 0) {
			// Patch.
			// Write all 5 words there.
			KUInt32 address = patches[0];
			
			inROMPtr[address++] = kInvocation[0];
			inROMPtr[address++] = kInvocation[1];
			inROMPtr[address++] = kInvocation[2];
			inROMPtr[address++] = kInvocation[3];
			
			// The last word in the patch is a fake instruction
			// which will be caught in TNativePrimitives::ExecuteNative()
			// because the high bit is set
			
			KUInt32 value = patches[1] | 0x80000000;
			inROMPtr[address] = value;
			
			patches += 2;
		}
	}
}
