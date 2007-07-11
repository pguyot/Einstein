// ==============================
// File:			TJITGeneric_STM1_template.h
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

// Store multiple
// S=0, L=0

// Use of PC for Rn is UNPREDICTABLE.

STM1_Template(FLAG_P, FLAG_U, FLAG_W, Rn)
#if IMPLEMENTATION
{
	TMemory* theMemoryInterface = ioCPU->GetMemory();

	POPPC();

	KUInt32 theImmValue;
	POPVALUE(theImmValue);
	KUInt32 theRegList = theImmValue & 0xFFFF;
#if FLAG_W || !FLAG_U
	KUInt32 nbRegisters = theImmValue >> 16;
#endif
	KUInt32 curRegList = theRegList & 0x7FFF;
	// Rn == 15 -> UNPREDICTABLE
	KUInt32 baseAddress = ioCPU->mCurrentRegisters[Rn];

#if FLAG_W
	// Write back
	#if FLAG_U
		KUInt32 wbAddress = baseAddress + (nbRegisters * 4);
	#else
		KUInt32 wbAddress = baseAddress - (nbRegisters * 4);
	#endif
#endif

#if FLAG_U
	// Up.
	#if FLAG_P
		// Post: add 4.
		baseAddress += 4;
	#endif
#else
	// Down.
	baseAddress -= (nbRegisters * 4);

	#if !FLAG_P
		// Post: add 4.
		baseAddress += 4;
	#endif
#endif

	// Store.
	int indexReg = 0;
	while (curRegList)
	{
		if (curRegList & 1)
		{
			if (theMemoryInterface->WriteAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mCurrentRegisters[indexReg] ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}

		curRegList >>= 1;
		indexReg++;
	}
	if (theRegList & 0x8000)
	{
		// PC is special.
		// Stored value is PC + 12
		if (theMemoryInterface->WriteAligned(
				(TMemory::VAddr) baseAddress,
				GETPC() + 4 ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
	}

#if FLAG_W
	// Write back.
	// Rn == 15 -> UNPREDICTABLE
	ioCPU->mCurrentRegisters[Rn] = wbAddress;
#endif

	CALLNEXTUNIT;
}
#endif

