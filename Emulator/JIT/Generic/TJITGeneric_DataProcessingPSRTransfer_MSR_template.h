// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_MSR_template.h
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

/*
FIELDS_MASK

0x000F
0x00F0
0x0F00
0xF000

FLAG_R:22
MODE Imm
MODE Register
*/

// MSR
MSR(MODE, FLAG_R, FIELDS_MASK, Rm,
{
#if (MODE == Imm)
	KUInt32 Opnd2;
	POPVALUE(Opnd2);
#elif Rm == 15
	POPPC();
	const KUInt32 Opnd2 = GETPC();
#else
	const KUInt32 Opnd2 = ioCPU->mCurrentRegisters[Rm];
#endif
#if FLAG_R
	// SPSR
	if (ioCPU->GetMode() != TARMProcessor::kUserMode) {
		const KUInt32 oldValue = ioCPU->GetSPSR();
		ioCPU->SetSPSR((Opnd2 & FIELDS_MASK) | (oldValue & ~ FIELDS_MASK));
	}
#else
	// CPSR
	const KUInt32 oldValue = ioCPU->GetCPSR();
	if (ioCPU->GetMode() != TARMProcessor::kUserMode) {
		ioCPU->SetCPSR((Opnd2 & FIELDS_MASK) | (oldValue & ~ FIELDS_MASK));
	} else {
#if (FIELDS_MASK & 0xFF)
		ioCPU->SetCPSR((Opnd2 & 0xFF) | (oldValue & 0xFFFFFF00));
#endif
	}
#endif
	
	CALLNEXTUNIT;
})
