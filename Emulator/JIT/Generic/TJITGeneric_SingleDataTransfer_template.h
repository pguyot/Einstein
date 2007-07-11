// ==============================
// File:			TJITGeneric_SingleDataTransfer_template.h
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

// Use of PC for Rm is UNPREDICTABLE

#define FLAG_I	((BITS_FLAGS & 0x20) != 0)
#define FLAG_P	((BITS_FLAGS & 0x10) != 0)
#define FLAG_U	((BITS_FLAGS & 0x08) != 0)
#define FLAG_B	((BITS_FLAGS & 0x04) != 0)
#define FLAG_W	((BITS_FLAGS & 0x02) != 0)
#define FLAG_L	((BITS_FLAGS & 0x01) != 0)

#define WRITEBACK		(!FLAG_P || FLAG_W)
#define UNPRIVILEDGED	(!FLAG_P && FLAG_W)

SingleDataTransfer_Template(BITS_FLAGS, Rn, Rd)
#if IMPLEMENTATION
{
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	
	POPPC();

	TMemory* theMemoryInterface = ioCPU->GetMemory();
	KUInt32 offset;

#if FLAG_I
	if ((theInstruction & 0x00000FFF) >> 4)
	{
		// Shift.
		// PC should not be used as Rm.
		offset = GetShiftNoCarryNoR15( theInstruction, ioCPU->mCurrentRegisters, ioCPU->mCPSR_C );
	} else {
		offset = ioCPU->mCurrentRegisters[theInstruction & 0x0000000F];
	}
#else
	// Immediate
	offset = theInstruction & 0x00000FFF;
#endif
#if (Rn == 15)
	KUInt32 theAddress = GETPC();
#else
	KUInt32 theAddress = ioCPU->mCurrentRegisters[Rn];
#endif

#if FLAG_P
	#if FLAG_U
		// Add.
		theAddress += offset;
	#else
		// Substract.
		theAddress -= offset;
	#endif
#endif

#if UNPRIVILEDGED
	if (ioCPU->GetMode() != TARMProcessor::kUserMode)
	{
		// Access should be unprivileged.
		theMemoryInterface->SetPrivilege( false );
	}
#endif
	
#if FLAG_L
	// Load.
	#if FLAG_B
		// Byte
		KUInt8 theData;
		if (theMemoryInterface->ReadB( theAddress, theData ))
		{
			// No need to restore mMemory->SetPrivilege since
			// we'll access memory in privilege mode from now anyway.
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
	#else
		// Word
		KUInt32 theData;
		if (theMemoryInterface->Read( theAddress, theData ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
	#endif
		
	#if (Rd == 15)
		#if FLAG_B
			// UNPREDICTABLE
		#else
			// Clear high bits if required.
			// +4 for PREFETCH
			SETPC(theData + 4);
		#endif
	#else
		// Clear high bits if required.
		ioCPU->mCurrentRegisters[Rd] = theData;
	#endif
#else
	// Store.
	
	// If PC is Rd, the stored value is +12 instead of +8
	#if (Rd == 15)
		KUInt32 theValue = GETPC() + 4;
	#else
		KUInt32 theValue = ioCPU->mCurrentRegisters[Rd];
	#endif
	
	#if FLAG_B
		// Byte
		if (theMemoryInterface->WriteB(
				theAddress, (KUInt8) (theValue & 0xFF) ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
	#else
		// Word.
		if (theMemoryInterface->Write( theAddress, theValue ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
	#endif
#endif
	
#if WRITEBACK
	// Store the address to the base register.
	#if (Rn == 15)
		// UNPREDICTABLE CASE
	#else
		#if !FLAG_P
			#if FLAG_U
				// Add.
				ioCPU->mCurrentRegisters[Rn] = theAddress + offset;
			#else
				// Substract.
				ioCPU->mCurrentRegisters[Rn] = theAddress - offset;
			#endif
		#else
			ioCPU->mCurrentRegisters[Rn] = theAddress;
		#endif
	#endif
#endif

#if UNPRIVILEDGED
	if (ioCPU->GetMode() != TARMProcessor::kUserMode)
	{
		// Restore privilege access.
		theMemoryInterface->SetPrivilege( true );
	}
#endif

#if (Rd == 15) && FLAG_L
	FURTHERCALLNEXT_AFTERSETPC;
#else
	CALLNEXTUNIT;
#endif
}
#endif
