// ==============================
// File:			TJITGeneric_LDM2_template.h
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

// User registers load multiple.
// S=1, L=1, W=0, REG_LIST & 0x8000 == 0

// Use of PC for Rn is UNPREDICTABLE.

LDM2_Template(FLAG_P, FLAG_U, Rn)
#if IMPLEMENTATION
{
	TMemory* theMemoryInterface = ioCPU->GetMemory();

	POPPC();

	KUInt32 theImmValue;
	POPVALUE(theImmValue);
	KUInt32 theRegList = theImmValue & 0xFFFF;
#if !FLAG_U
	KUInt32 nbRegisters = theImmValue >> 16;
#endif
	KUInt32 curRegList;
	KUInt32 bankRegList;

	if (ioCPU->GetMode() == TARMProcessor::kFIQMode)
	{
		curRegList = theRegList & 0xFF;
		bankRegList = theRegList & 0x7F00;
	} else {
		curRegList = theRegList & 0x1FFF;
		bankRegList = theRegList & 0x6000;
	}
	// Rn == 15 -> UNPREDICTABLE
	KUInt32 baseAddress = ioCPU->mCurrentRegisters[Rn];

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

	// Load.
	int indexReg = 0;
	while (curRegList)
	{
		if (curRegList & 1)
		{
			if (theMemoryInterface->ReadAligned(
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
	if (bankRegList)
	{
		if (bankRegList & 0x0100)
		{
			if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mR8_Bkup ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}
		if (bankRegList & 0x0200)
		{
			if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mR9_Bkup ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}
		if (bankRegList & 0x0400)
		{
			if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mR10_Bkup ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}
		if (bankRegList & 0x0800)
		{
			if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mR11_Bkup ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}
		if (bankRegList & 0x1000)
		{
			if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mR12_Bkup ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}
		if (bankRegList & 0x3000)
		{
			if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mR13_Bkup ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}
		if (bankRegList & 0x4000)
		{
			if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				ioCPU->mR14_Bkup ))
			{
				SETPC(GETPC());
				ioCPU->DataAbort();
				MMUCALLNEXT_AFTERSETPC;
			}
			baseAddress += 4;
		}
	}

	CALLNEXTUNIT;
}
#endif
