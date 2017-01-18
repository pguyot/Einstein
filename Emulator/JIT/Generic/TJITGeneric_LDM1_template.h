// ==============================
// File:			TJITGeneric_LDM1_template.h
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

// Load multiple
// PUSWL
// S=0, L=1

// Use of PC for Rn is UNPREDICTABLE.

LDM1_Template(FLAG_P, FLAG_U, FLAG_W, Rn)
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

	// Load.
#if 0
	// 5460 loops in debug mode
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
#else
	// 6070 loops in debug mode
	// Reading from RAM is really expensive. The LDM command reads usually multiple
	// words at once. We optimize by calculating the source address only once
	// per MMU page.
	Boolean isFirst = 1;
	int indexReg = 0;
	KUInt32 theAddress = 0;
	while (curRegList)
	{
		if (curRegList & 1)
		{
			if (isFirst) {		
				if (theMemoryInterface->IsMMUEnabled() && !theMemoryInterface->IsPageInROM(baseAddress))
				{
					if (theMemoryInterface->TranslateR( baseAddress & ~0x03, theAddress ))
					{
						SETPC(GETPC());
						ioCPU->DataAbort();
						MMUCALLNEXT_AFTERSETPC;
					}
				} else {
					theAddress = baseAddress & ~0x03;
				}
				
				Boolean fault = false;
				ioCPU->mCurrentRegisters[indexReg] = theMemoryInterface->ReadPAligned( theAddress, fault );
				if (fault)
				{
					SETPC(GETPC());
					ioCPU->DataAbort();
					MMUCALLNEXT_AFTERSETPC;
				}
				theAddress += 4;
				if (theAddress&0x000003ff) 
					isFirst = 0;
			} else {
				if ((theAddress&0xff000000) == 0x04000000) { // fast RAM write
					//fprintf(stderr, "+");
					ioCPU->mCurrentRegisters[indexReg] = *((KUInt32*) (theMemoryInterface->GetRAMOffset() + theAddress));
				} else {
					Boolean fault = false;
					ioCPU->mCurrentRegisters[indexReg] = theMemoryInterface->ReadPAligned( theAddress, fault );
					if (fault)
					{
						SETPC(GETPC());
						ioCPU->DataAbort();
						MMUCALLNEXT_AFTERSETPC;
					}
				}
				theAddress += 4;
				if (!(theAddress&0x000003ff))
					isFirst = 1;
			}
			baseAddress += 4;
		}
		
		curRegList >>= 1;
		indexReg++;
	}
#endif
	
	
	
	if (theRegList & 0x8000)
	{
		// PC is special.
		KUInt32 theValue;
		if (theMemoryInterface->ReadAligned(
				(TMemory::VAddr) baseAddress,
				theValue ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
		SETPC( theValue + 4 );   // Prefetch.
	}

#if FLAG_W
	// Write back.
	// Rn == 15 -> UNPREDICTABLE
	ioCPU->mCurrentRegisters[Rn] = wbAddress;
#endif

	if (theRegList & 0x8000)
	{
		MMUCALLNEXT_AFTERSETPC;
	} else {
		CALLNEXTUNIT;
	}
}
#endif
