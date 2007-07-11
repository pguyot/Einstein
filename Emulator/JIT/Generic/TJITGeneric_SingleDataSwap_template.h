// ==============================
// File:			TJITGeneric_SingleDataSwap_template.h
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

// #define FLAG_B:22
// Use of R15 for Rn, Rd and Rm is UNPREDICTABLE
// TODO: Make those instructions ATOMIC

SingleDataSwap_Template(FLAG_B, Rn, Rd, Rm)
#if IMPLEMENTATION
{
	POPPC();
	
#if Rn != 15 || Rd != 15 || Rm != 15
	TMemory* theMemoryInterface = ioCPU->GetMemory();
	TMemory::VAddr theAddress = (TMemory::VAddr) ioCPU->mCurrentRegisters[Rn];
#if FLAG_B
	// Swap byte quantity.
	KUInt8 theData;
	if (theMemoryInterface->ReadB( theAddress, theData ))
	{
		SETPC(GETPC());
		ioCPU->DataAbort();
		MMUCALLNEXT_AFTERSETPC;
	}
	
	// Write.
	if (theMemoryInterface->WriteB(
				theAddress, (KUInt8) (ioCPU->mCurrentRegisters[Rm] & 0xFF) ))
	{
		SETPC(GETPC());
		ioCPU->DataAbort();
		MMUCALLNEXT_AFTERSETPC;
	}
	
	// The remaining bits are filled with zeroes, as in LDRB.
	ioCPU->mCurrentRegisters[Rd] = theData;
#else	
	// Swap word quantity.
	KUInt32 theData;
	if (theMemoryInterface->Read( theAddress, theData ))
	{
		SETPC(GETPC());
		ioCPU->DataAbort();
		MMUCALLNEXT_AFTERSETPC;
	}
	
	// Write.
	if (theMemoryInterface->Write( theAddress, ioCPU->mCurrentRegisters[Rm] ))
	{
		SETPC(GETPC());
		ioCPU->DataAbort();
		MMUCALLNEXT_AFTERSETPC;
	}
	
	ioCPU->mCurrentRegisters[Rd] = theData;
#endif
#endif	
	CALLNEXTUNIT;
}
#endif
