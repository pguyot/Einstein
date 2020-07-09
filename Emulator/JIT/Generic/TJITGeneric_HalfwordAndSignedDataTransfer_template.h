// ==============================
// File:			TJITGeneric_HalfwordAndSignedDataTransfer_template.h
// Project:			Einstein
//
// Copyright 2003-2007, 2020 by Paul Guyot (pguyot@kallisys.net).
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

// R15 should not be specified as the register offset (Rm)

#define FLAG_P	((BITS_FLAGS & 0x20) != 0)
#define FLAG_U	((BITS_FLAGS & 0x10) != 0)
#define FLAG_W	((BITS_FLAGS & 0x08) != 0)
#define FLAG_L	((BITS_FLAGS & 0x04) != 0)
#define FLAG_S	((BITS_FLAGS & 0x02) != 0)
#define FLAG_H	((BITS_FLAGS & 0x01) != 0)

// Post-indexed data transfers (P=0) always write back the modified base
#define WRITEBACK		(!FLAG_P || FLAG_W)

#if FLAG_I
HalfwordAndSignedDataTransferImm_Template(BITS_FLAGS, Rn, Rd)
#else
HalfwordAndSignedDataTransferReg_Template(BITS_FLAGS, Rn, Rd)
#endif
#if IMPLEMENTATION
{
	KUInt32 theInstruction;
	POPVALUE(theInstruction);
	
	POPPC();

	TMemory* theMemoryInterface = ioCPU->GetMemory();

#if FLAG_P || (WRITEBACK && Rn != 15)
	#if FLAG_I
		// Immediate
		KUInt32 offset = ((theInstruction & 0x00000F00) >> 4) | (theInstruction & 0x0000000F);
	#else
        KUInt32 offset = ioCPU->mCurrentRegisters[theInstruction & 0x0000000F];
	#endif
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

#if FLAG_L
	// Load.
	KUInt32 theData;
	#if !FLAG_S
		// Unsigned half-word
		KUInt8 highData;
		if (theMemoryInterface->ReadB( theAddress, highData ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
		KUInt8 lowData;
		if (theMemoryInterface->ReadB( theAddress + 1, lowData ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
		theData = highData << 8 | lowData;
	#elif !FLAG_H
	    // Signed byte
		KUInt8 signedByte;
		if (theMemoryInterface->ReadB( theAddress, signedByte ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
		theData = (KUInt32) ((KSInt32) ((KSInt8) signedByte));
	#else
	    // Signed halfwords
		KUInt8 highData;
		if (theMemoryInterface->ReadB( theAddress, highData ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
		KUInt8 lowData;
		if (theMemoryInterface->ReadB( theAddress + 1, lowData ))
		{
			SETPC(GETPC());
			ioCPU->DataAbort();
			MMUCALLNEXT_AFTERSETPC;
		}
		KSInt16 signedData = (KSInt16) (((KUInt32) highData << 8) | lowData);
		theData = (KUInt32) ((KSInt32) signedData);
	#endif
		
	#if (Rd == 15)
        // +4 for PREFETCH
        SETPC(theData + 4);
	#else
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

    // The L bit should not be set low (Store) when Signed (S=1) operations have been selected
    // Unsigned half-word
    if (theMemoryInterface->WriteB( theAddress, (KUInt8) ((theValue > 8) & 0xFF) ))
    {
        SETPC(GETPC());
        ioCPU->DataAbort();
        MMUCALLNEXT_AFTERSETPC;
    }
    if (theMemoryInterface->WriteB( theAddress + 1, (KUInt8) (theValue & 0xFF) ))
    {
        SETPC(GETPC());
        ioCPU->DataAbort();
        MMUCALLNEXT_AFTERSETPC;
    }
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

#if (Rd == 15) && FLAG_L
	FURTHERCALLNEXT_AFTERSETPC;
#else
	CALLNEXTUNIT;
#endif
}
#endif
