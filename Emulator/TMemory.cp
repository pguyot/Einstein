// ==============================
// File:			TMemory.cp
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

#include <K/Defines/KDefinitions.h>
#include "TMemory.h"

// POSIX & ANSI C
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#if !TARGET_OS_WIN32
	#include <sys/time.h>
	#include <unistd.h>
#endif

// K
#include <K/Streams/TStream.h>
#include <K/Defines/UByteSex.h>

// Einstein
#include "TARMProcessor.h"
#include "TInterruptManager.h"
#include "TDMAManager.h"
#include "Serial/TSerialPortManager.h"
#include "TEmulator.h"
#include "ROM/TROMImage.h"
#include "Log/TLog.h"
#include "JIT/JIT.h"
#include "PCMCIA/TATACard.h"
#include "PCMCIA/TLinearCard.h"
#include "PCMCIA/TPCMCIAController.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#define debugFlash 0

const int TMemory::kSerialNumberCRC[256] =
	{
		0x00, 0x8C, 0x46, 0xCA, 0x23, 0xAF, 0x65, 0xE9,
		0x9D, 0x11, 0xDB, 0x57, 0xBE, 0x32, 0xF8, 0x74,
		0xC2, 0x4E, 0x84, 0x08, 0xE1, 0x6D, 0xA7, 0x2B,
		0x5F, 0xD3, 0x19, 0x95, 0x7C, 0xF0, 0x3A, 0xB6,
		0x61, 0xED, 0x27, 0xAB, 0x42, 0xCE, 0x04, 0x88,
		0xFC, 0x70, 0xBA, 0x36, 0xDF, 0x53, 0x99, 0x15,
		0xA3, 0x2F, 0xE5, 0x69, 0x80, 0x0C, 0xC6, 0x4A,
		0x3E, 0xB2, 0x78, 0xF4, 0x1D, 0x91, 0x5B, 0xD7,
		0xBC, 0x30, 0xFA, 0x76, 0x9F, 0x13, 0xD9, 0x55,
		0x21, 0xAD, 0x67, 0xEB, 0x02, 0x8E, 0x44, 0xC8,
		0x7E, 0xF2, 0x38, 0xB4, 0x5D, 0xD1, 0x1B, 0x97,
		0xE3, 0x6F, 0xA5, 0x29, 0xC0, 0x4C, 0x86, 0x0A,
		0xDD, 0x51, 0x9B, 0x17, 0xFE, 0x72, 0xB8, 0x34,
		0x40, 0xCC, 0x06, 0x8A, 0x63, 0xEF, 0x25, 0xA9,
		0x1F, 0x93, 0x59, 0xD5, 0x3C, 0xB0, 0x7A, 0xF6,
		0x82, 0x0E, 0xC4, 0x48, 0xA1, 0x2D, 0xE7, 0x6B,
		0x5E, 0xD2, 0x18, 0x94, 0x7D, 0xF1, 0x3B, 0xB7,
		0xC3, 0x4F, 0x85, 0x09, 0xE0, 0x6C, 0xA6, 0x2A,
		0x9C, 0x10, 0xDA, 0x56, 0xBF, 0x33, 0xF9, 0x75,
		0x01, 0x8D, 0x47, 0xCB, 0x22, 0xAE, 0x64, 0xE8,
		0x3F, 0xB3, 0x79, 0xF5, 0x1C, 0x90, 0x5A, 0xD6,
		0xA2, 0x2E, 0xE4, 0x68, 0x81, 0x0D, 0xC7, 0x4B,
		0xFD, 0x71, 0xBB, 0x37, 0xDE, 0x52, 0x98, 0x14,
		0x60, 0xEC, 0x26, 0xAA, 0x43, 0xCF, 0x05, 0x89,
		0xE2, 0x6E, 0xA4, 0x28, 0xC1, 0x4D, 0x87, 0x0B,
		0x7F, 0xF3, 0x39, 0xB5, 0x5C, 0xD0, 0x1A, 0x96,
		0x20, 0xAC, 0x66, 0xEA, 0x03, 0x8F, 0x45, 0xC9,
		0xBD, 0x31, 0xFB, 0x77, 0x9E, 0x12, 0xD8, 0x54,
		0x83, 0x0F, 0xC5, 0x49, 0xA0, 0x2C, 0xE6, 0x6A,
		0x1E, 0x92, 0x58, 0xD4, 0x3D, 0xB1, 0x7B, 0xF7,
		0x41, 0xCD, 0x07, 0x8B, 0x62, 0xEE, 0x24, 0xA8,
		0xDC, 0x50, 0x9A, 0x16, 0xFF, 0x73, 0xB9, 0x35
	};

#define min(a,b) (a) < (b) ? (a) : (b)

// -------------------------------------------------------------------------- //
//  * TMemory( TLog*, KUInt8*, const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TMemory::TMemory(
			TLog* inLog,
			KUInt8* inROMImageBuffer,
			const char* inFlashPath,
			KUInt32 inRAMSize /* = 4194304 */ )
	:
		mProcessor( nil ),
		mLog( inLog ),
		mFlash( inLog, inFlashPath, NULL ),
		mROMImagePtr( inROMImageBuffer ),
		mRAM( nil ),
		mRAMSize( inRAMSize ),
		mRAMEnd( TMemoryConsts::kRAMStart + inRAMSize ),
		mMMU( this ),
		mJIT( this, &mMMU ),
		mBankCtrlRegister( 0 ),
		mInterruptManager( 0 ),
		mDMAManager( 0 ),
		mExternalPort( 0 ),
		mInfraredPort( 0 ),
		mBuiltInExtraPort( 0 ),
		mModemPort( 0 ),
		mSerialNumberIx( 64 ),
		mBPCount( 0 ),
		mWPCount( 0 )
{
	Init();
}

// -------------------------------------------------------------------------- //
//  * TMemory( TLog*, TROMImage*, const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TMemory::TMemory(
			TLog* inLog,
			TROMImage* inROMImage,
			const char* inFlashPath,
			KUInt32 inRAMSize /* = 4194304 */ )
	:
		mProcessor( nil ),
		mLog( inLog ),
		mFlash( inLog, inFlashPath, inROMImage ),
		mROMImagePtr( inROMImage->GetPointer() ),
		mRAM( nil ),
		mRAMSize( inRAMSize ),
		mRAMEnd( TMemoryConsts::kRAMStart + inRAMSize ),
		mMMU( this ),
		mJIT( this, &mMMU ),
		mBankCtrlRegister( 0 ),
		mInterruptManager( 0 ),
		mDMAManager( 0 ),
		mExternalPort( 0 ),
		mInfraredPort( 0 ),
		mBuiltInExtraPort( 0 ),
		mModemPort( 0 ),
		mSerialNumberIx( 64 ),
		mBPCount( 0 ),
		mWPCount( 0 )
{
	Init();
}

// -------------------------------------------------------------------------- //
//  * ~TMemory( void )
// -------------------------------------------------------------------------- //
TMemory::~TMemory( void )
{
	if (mRAM)
	{
		::free( mRAM );
	}
	
	int socketsIx;
	for (socketsIx = 0; socketsIx < kNbSockets; socketsIx++)
	{
		if (mPCMCIACtrls[socketsIx])
		{
			delete mPCMCIACtrls[socketsIx];
		}
	}
}

// -------------------------------------------------------------------------- //
//  * SetEmulator( TEmulator* )
// -------------------------------------------------------------------------- //
void
TMemory::SetEmulator( TEmulator* inEmulator )
{
	if (inEmulator)
	{
		mInterruptManager = inEmulator->GetInterruptManager();
		mDMAManager = inEmulator->GetDMAManager();
		mExternalPort = inEmulator->GetExternalSerialPort();
		mInfraredPort = inEmulator->GetInfraredSerialPort();
		mBuiltInExtraPort = inEmulator->GetBuiltInExtraSerialPort();
		mModemPort = inEmulator->GetModemSerialPort();
		mEmulator = inEmulator;

		int socketIx;
		for (socketIx = 0; socketIx < kNbSockets; socketIx++)
		{
			mPCMCIACtrls[socketIx] =
				new TPCMCIAController( mLog, mEmulator, socketIx );
		}
		
		ComputeSerialNumber( inEmulator->GetNewtonID() );
	} else {
		mInterruptManager = nil;
		mDMAManager = nil;
		mExternalPort = nil;
		mInfraredPort = nil;
		mBuiltInExtraPort = nil;
		mModemPort = nil;
		mEmulator = nil;

		int socketIx;
		for (socketIx = 0; socketIx < kNbSockets; socketIx++)
		{
			if (mPCMCIACtrls[socketIx])
			{
				delete mPCMCIACtrls[socketIx];
				mPCMCIACtrls[socketIx] = NULL;
			}
		}
	}
}

// -------------------------------------------------------------------------- //
//  * GetDirectPointerToRAM( VAddr, KUInt8** )
// -------------------------------------------------------------------------- //
Boolean
TMemory::GetDirectPointerToRAM( VAddr inAddress, KUInt8** outPTR ) 
{
	PAddr theAddress;
	
	if (IsMMUEnabled())
	{
		if (TranslateR( inAddress, theAddress ))
		{
			if(mLog)
				mLog->LogLine("MMU Translation failed");
			return true;
		}
	} else {
		theAddress = inAddress;
	}
	
	if(theAddress >= TMemoryConsts::kRAMStart && theAddress < mRAMEnd)
	{ // goodie
		*outPTR = ((KUInt8*) (mRAMOffset + theAddress));
	} else {
		if(mLog)
			mLog->LogLine("Tried to load a buffer that is not in RAM");
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * GetDirectPointerToROMRAM( VAddr, const KUInt8** )
// -------------------------------------------------------------------------- //
Boolean
TMemory::GetDirectPointerToROMRAM( VAddr inAddress, const KUInt8** outPTR ) 
{
	PAddr theAddress;
	
	// Optimization: avoid translation when reading unprotected ROM
	if (IsMMUEnabled() && !IsPageInROM(inAddress))
	{
		if (TranslateR( inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress;
	}

	if (!(inAddress & TMemoryConsts::kROMEndMask))
	{
		*outPTR = ((KUInt8*) (mROMImagePtr + theAddress));
	} else if(theAddress >= TMemoryConsts::kRAMStart && theAddress < mRAMEnd) {
		// goodie
		*outPTR = ((const KUInt8*) (mRAMOffset + theAddress));
	} else {
		if(mLog)
			mLog->LogLine("Tried to load a buffer that is not in RAM or ROM");
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * FastReadBuffer( VAddr, const KUInt8** )
// -------------------------------------------------------------------------- //
Boolean
TMemory::FastReadBuffer( VAddr inAddress, KUInt32 inAmount, KUInt8* outBuffer )
{
	const KUInt8* pointer;
	KUInt8* dst = outBuffer;
	KUInt32 len = inAmount;
	KUInt32 addr = inAddress;

	if (addr & 0x3)
	{
		int bytes = min(addr & 0x3, len);
		// Quickly skip to aligned accesses
		while (bytes-- > 0)
		{
			KUInt8 byte;
			if (ReadB(addr++, byte)) return true;
			*dst++ = byte;
			len--;
		}
	}
	
	if (GetDirectPointerToROMRAM(addr, &pointer))
	{
		// Slower.
		int words = len / 4;
		KUInt32* dst32 = (KUInt32*) dst;
		while (words-- > 0)
		{
			KUInt32 word;
			if (ReadAligned(addr, word)) return true;
			*dst32++ = UByteSex_FromBigEndian( word );
			addr += 4;
		}
	} else {
		KUInt32 base = inAddress & TMemoryConsts::kMMUSmallestPageMask;
		KUInt32 maxCopy = base - addr + TMemoryConsts::kMMUSmallestPageSize;
		KUInt32 alignedLen = len &~ 0x3;

		do {
			KUInt32 amount = min(alignedLen, maxCopy);
#if TARGET_RT_LITTLE_ENDIAN
			{
				KUInt32 nbWords = amount / 4;
				KUInt32* dstCrsr = (KUInt32*) dst;
				KUInt32* srcCrsr = (KUInt32*) pointer;
				while (nbWords-- > 0)
				{
					*dstCrsr++ = UByteSex_ToBigEndian(*srcCrsr++);
				}
			}
#else
			(void) ::memcpy(dst, pointer, amount);
#endif
			alignedLen -= amount;
			len -= amount;
			dst += amount;
			addr += amount;
			if (alignedLen == 0)
			{
				break;
			}
			// Else, amount == maxCopy
			maxCopy = TMemoryConsts::kMMUSmallestPageSize;
			if (GetDirectPointerToROMRAM(addr, &pointer))
			{
				SetHardwareFault(addr);
				return true;
			}
		} while (true);
	}

	// Copy unaligned bits at the end.
	while (len-- > 0)
	{
		KUInt8 byte;
		if (ReadB(addr++, byte)) return true;
		*dst++ = byte;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * FastReadString( VAddr, char** )
// -------------------------------------------------------------------------- //
Boolean
TMemory::FastReadString( VAddr inAddress, char** outString )
{
	KUInt32 result_len = 1024;
	char* result = (char*) ::malloc(result_len);
	KUInt32 len = result_len;
	char* dst = result;
	KUInt32 addr = inAddress;

#if TARGET_RT_BIG_ENDIAN
	const KUInt8* pointer;
	if ((addr & 0x3) || GetDirectPointerToROMRAM(addr, &pointer))
	{
#endif
		while (true)
		{
			KUInt8 byte = 0;
			// Slower.
			while (len-- > 0)
			{
				if (ReadB(addr++, byte))
				{
					::free(result);
					return true;
				}
				*dst++ = (char) byte;
				if (byte == 0)
				{
					break;
				}
			}
			
			if (byte != 0)
			{
				// Resize.
				len = 1024;
				result_len += len;
				ssize_t offset = dst - result;
				result = (char*) ::realloc(result, result_len);
				dst = result + offset;
			} else {
				break;
			}
		}
#if TARGET_RT_BIG_ENDIAN
	} else {
		KUInt32 base = inAddress & TMemoryConsts::kMMUSmallestPageMask;
		KUInt32 maxCopy = base - addr + TMemoryConsts::kMMUSmallestPageSize;

		do {
			KUInt32 amount = min(len, maxCopy);
			char* last = ::strncpy(dst, (char*) pointer, amount);
			if (*last == '\0')
			{
				break;
			}
			len -= amount;
			if (len == 0)
			{
				// Resize.
				len = 1024;
				result_len += len;
				KUInt32 offset = dst - result;
				result = (char*) ::realloc(result, result_len);
				dst = result + offset;
			}
			// Else, amount == maxCopy
			dst += amount;
			maxCopy = TMemoryConsts::kMMUSmallestPageSize;
			addr += amount;
			if (GetDirectPointerToROMRAM(addr, &pointer))
			{
				SetHardwareFault(addr);
				return true;
			}
		} while (true);
	}
#endif

	*outString = result;
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * FastReadString( VAddr, KUInt32*, char* )
// -------------------------------------------------------------------------- //
Boolean
TMemory::FastReadString( VAddr inAddress, KUInt32* ioAmount, char* outString )
{
	char* dst = outString;
	KUInt32 orglen = *ioAmount;
	KUInt32 len = orglen;
	KUInt32 addr = inAddress;

#if TARGET_RT_BIG_ENDIAN
	const KUInt8* pointer;
	if ((addr & 0x3) || GetDirectPointerToROMRAM(addr, &pointer))
	{
#endif
		// Slower.
		while (len-- > 0)
		{
			KUInt8 byte;
			if (ReadB(addr++, byte)) return true;
			*dst++ = (char) byte;
			if (byte == 0)
			{
				break;
			}
		}
#if TARGET_RT_BIG_ENDIAN
	} else {
		KUInt32 base = inAddress & TMemoryConsts::kMMUSmallestPageMask;
		KUInt32 maxCopy = base - addr + TMemoryConsts::kMMUSmallestPageSize;

		do {
			KUInt32 amount = min(len, maxCopy);
			char* last = ::strncpy(dst, (char*) pointer, amount);
			if (*last == '\0')
			{
				break;
			}
			len -= amount;
			if (len == 0)
			{
				break;
			}
			// Else, amount == maxCopy
			dst += amount;
			maxCopy = TMemoryConsts::kMMUSmallestPageSize;
			addr += amount;
			if (GetDirectPointerToROMRAM(addr, &pointer))
			{
				SetHardwareFault(addr);
				return true;
			}
		} while (true);
	}
#endif

	*ioAmount = orglen - len;

	return false;
}

// -------------------------------------------------------------------------- //
//  * FastWriteBuffer( VAddr, const KUInt8** )
// -------------------------------------------------------------------------- //
Boolean
TMemory::FastWriteBuffer( VAddr inAddress, KUInt32 inAmount, const KUInt8* inBuffer )
{
	KUInt8* pointer;
	const KUInt8* src = inBuffer;
	KUInt32 len = inAmount;
	KUInt32 addr = inAddress;

	if (addr & 0x3)
	{
		// Quickly skip to aligned accesses
		int bytes = min(addr & 0x3, len);
		while (bytes-- > 0)
		{
			KUInt8 byte = *src++;
			if (WriteB(addr++, byte)) return true;
			len--;
		}
	}	
	if (GetDirectPointerToRAM(addr, &pointer))
	{
		// Slower.
		int words = len / 4;
		KUInt32* src32 = (KUInt32*) src;
		while (words-- > 0)
		{
			KUInt32 word = UByteSex_ToBigEndian(*src32++);
			if (WriteAligned(addr, word)) return true;
			addr += 4;
		}
	} else {
		KUInt32 base = inAddress & TMemoryConsts::kMMUSmallestPageMask;
		KUInt32 maxCopy = base - addr + TMemoryConsts::kMMUSmallestPageSize;
		KUInt32 alignedLen = len &~ 0x3;

		do {
			KUInt32 amount = min(alignedLen, maxCopy);
#if TARGET_RT_LITTLE_ENDIAN
			{
				KUInt32 nbWords = amount / 4;
				KUInt32* dstCrsr = (KUInt32*) pointer;
				KUInt32* srcCrsr = (KUInt32*) src;
				while (nbWords-- > 0)
				{
					*dstCrsr++ = UByteSex_ToBigEndian(*srcCrsr++);
				}
			}
#else
			(void) ::memcpy(pointer, src, amount);
#endif
			alignedLen -= amount;
			len -= amount;
			src += amount;
			addr += amount;
			if (alignedLen == 0)
			{
				break;
			}
			// Else, amount == maxCopy
			maxCopy = TMemoryConsts::kMMUSmallestPageSize;
			if (GetDirectPointerToRAM(addr, &pointer))
			{
				SetHardwareFault(addr);
				return true;
			}
		} while (true);
	}

	// Copy unaligned bits at the end.
	while (len-- > 0)
	{
		KUInt8 byte = *src++;
		if (WriteB(addr++, byte)) return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * FastWriteString( VAddr, KUInt32*, const char* )
// -------------------------------------------------------------------------- //
Boolean
TMemory::FastWriteString( VAddr inAddress, KUInt32* ioAmount, const char* inString )
{
	const char* src = inString;
	KUInt32 orglen = *ioAmount;
	KUInt32 len = orglen;
	KUInt32 addr = inAddress;

#if TARGET_RT_BIG_ENDIAN
	KUInt8* pointer;
	if ((addr & 0x3) || GetDirectPointerToRAM(addr, &pointer))
	{
#endif
		// Slower.
		while (len-- > 0)
		{
			KUInt8 byte = *src++;
			if (WriteB(addr++, byte)) return true;
			if (byte == 0)
			{
				break;
			}
		}
#if TARGET_RT_BIG_ENDIAN
	} else {
		KUInt32 base = inAddress & TMemoryConsts::kMMUSmallestPageMask;
		KUInt32 maxCopy = base - addr + TMemoryConsts::kMMUSmallestPageSize;

		do {
			KUInt32 amount = min(len, maxCopy);
			char* last = ::strncpy((char*) pointer, src, amount);
			if (*last == '\0')
			{
				break;
			}
			len -= amount;
			if (len == 0)
			{
				break;
			}
			// Else, amount == maxCopy
			src += amount;
			maxCopy = TMemoryConsts::kMMUSmallestPageSize;
			addr += amount;
			if (GetDirectPointerToRAM(addr, &pointer))
			{
				SetHardwareFault(addr);
				return true;
			}
		} while (true);
	}
#endif
	
	*ioAmount = orglen - len;

	return false;
}

// -------------------------------------------------------------------------- //
//  * GetDirectPointerToPage( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32*
TMemory::GetDirectPointerToPage( KUInt32 inPAddr )
{
	if (!(inPAddr & TMemoryConsts::kROMEndMask))
	{
		return ((KUInt32*) ((KUIntPtr) mROMImagePtr + inPAddr));
	} else {
		return ((KUInt32*) (mRAMOffset + inPAddr));
	}
}

// -------------------------------------------------------------------------- //
//  * ReadInstruction( PAddr )
// -------------------------------------------------------------------------- //
KUInt32
TMemory::ReadInstruction( KUInt32 inBankNumber, KUInt32 inOffsetInBank )
{
	if (inBankNumber == 0)
	{
		return *((KUInt32*) ((KUIntPtr) mROMImagePtr + inOffsetInBank));
	} else /* (inBankNumber == 1) */ {
		return *((KUInt32*) (mRAM + inOffsetInBank));
	}
}

// -------------------------------------------------------------------------- //
//  * Read( VAddr, KUInt32& )
// -------------------------------------------------------------------------- //
Boolean
TMemory::Read( VAddr inAddress, KUInt32& outWord )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&1)) {
			fprintf(stderr, "Watchpoint 0x%08X read around 0x%08X\n", (unsigned int)inAddress, (unsigned int)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}
	
	PAddr theAddress;

	// Optimization: avoid translation when reading unprotected ROM
	if (IsMMUEnabled() && !IsPageInROM(inAddress))
	{
		if (TranslateR( inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress;
	}
	
	Boolean fault = false;
	outWord = ReadP( theAddress, fault );
	if (fault)
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * ReadAligned( VAddr, KUInt32& )
// -------------------------------------------------------------------------- //
Boolean
TMemory::ReadAligned( VAddr inAddress, KUInt32& outWord )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&1)) {
			fprintf(stderr, "Watchpoint 0x%08X read around 0x%08X\n", (unsigned int)inAddress, (unsigned int)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}

	PAddr theAddress;

	// Optimization: avoid translation when reading unprotected ROM
	if (IsMMUEnabled() && !IsPageInROM(inAddress))
	{
		if (TranslateR( inAddress & ~0x03, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress & ~0x03;
	}
	
	Boolean fault = false;
	outWord = ReadPAligned( theAddress, fault );
	if (fault)
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * ReadROMRAM( VAddr, KUInt32& )
// -------------------------------------------------------------------------- //
inline Boolean
TMemory::ReadROMRAM( VAddr inAddress, KUInt32& outWord )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&1)) {
			fprintf(stderr, "Watchpoint 0x%08X read around 0x%08X\n", (unsigned int)inAddress, (unsigned int)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}

	PAddr theAddress;

	// Optimization: avoid translation when reading unprotected ROM
	if (IsMMUEnabled() && ((inAddress < 0x00002000) || (inAddress & TMemoryConsts::kROMEndMask)))
	{
		if (TranslateR( inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress;
	}
	
	Boolean fault = false;
	outWord = ReadROMRAMP( theAddress, fault );
	if (fault)
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * ReadP( PAddr, Boolean& )
// -------------------------------------------------------------------------- //
KUInt32
TMemory::ReadP( PAddr inAddress, Boolean& outFault )
{
	if (!(inAddress & TMemoryConsts::kROMEndMask))
	{
		if (inAddress & 0x3)
		{
			// UNALIGNED ACCESS
			KUInt32 theValue =
				*((KUInt32*)
					((KUIntPtr) mROMImagePtr + (inAddress & 0xFFFFFFFC)));
			KUInt32 rotAmount = (inAddress & 0x3) * 8;
			return (theValue << rotAmount)
				| (theValue >> (32 - rotAmount));
		} else {
			return *((KUInt32*) ((KUIntPtr) mROMImagePtr + inAddress));
		}
	} else if (inAddress < TMemoryConsts::kRAMStart) {
		if (inAddress < TMemoryConsts::kFlashBank1) {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word: Bus error at P0x%.8X",
					(unsigned int) inAddress );
				// mEmulator->BreakInMonitor();
			}
			outFault = true;
			return false;
		} else if (inAddress < TMemoryConsts::kFlashBank1End) {
			KUInt32 theResult = mFlash.Read(
									inAddress - TMemoryConsts::kFlashBank1,
									0 );
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"Read flash (bank#1) at P0x%.8X = %.8X",
					(unsigned int) inAddress,
					(unsigned int) theResult );
			}
#endif
			return theResult;
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word access to unknown bank #1 at P0x%.8X",
					(unsigned int) inAddress );
			}
			// mEmulator->BreakInMonitor();
			return 0;
		}
	} else if (inAddress < mRAMEnd) {
		// RAM.
		if (inAddress & 0x3)
		{
			// UNALIGNED ACCESS
			KUInt32 theValue =
				*((KUInt32*)
					((KUIntPtr) mRAMOffset + (inAddress & 0xFFFFFFFC)));
			KUInt32 rotAmount = (inAddress & 0x3) * 8;
			return (theValue << rotAmount)
				| (theValue >> (32 - rotAmount));
		} else {
			return *((KUInt32*) ((KUIntPtr) mRAMOffset + inAddress));
		}
	} else if (inAddress < TMemoryConsts::kHardwareBase) {
		// Bank #2
		if (mLog)
		{
			mLog->FLogLine(
				"Read word access to unknown bank #2 at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		return 0;
	} else if (inAddress < TMemoryConsts::kExternalSerialBase) {
		// Bank #3
		if (inAddress == TMemoryConsts::kHdWr_04RAMSize) {
			KUInt32 thePageCount = (mRAMSize >> 16) & 0xFF;
			return
				(thePageCount << 24)
				| (thePageCount << 16)
				| thePageCount;
		} else if (inAddress == TMemoryConsts::kHdWr_08RAMSize) {
			return 0;
		} else if ((inAddress >= TMemoryConsts::kHdWr_DMAChan1Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan1End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan1Base) >> 13;
			KUInt32 reg = (inAddress & 0x1C00) >> 10;
			return mDMAManager->ReadChannel1Register( channel, reg );
		} else if ((inAddress >= TMemoryConsts::kHdWr_DMAChan2Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan2End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan2Base) >> 12;
			KUInt32 reg = (inAddress & 0x0C00) >> 10;
			return mDMAManager->ReadChannel2Register( channel, reg );
		} else if (inAddress == TMemoryConsts::kHdWr_DMAAssgmnt) {
			return mDMAManager->ReadChannelAssignmentRegister();
		} else if (inAddress == TMemoryConsts::kHdWr_DMAEnableStat) {
			return mDMAManager->ReadStatusRegister();
		} else if (inAddress == TMemoryConsts::kHdWr_DMAWordStat) {
			return mDMAManager->ReadWordStatusRegister();
		} else if (inAddress == TMemoryConsts::kHdWr_HighSpeedClck) {
			return TMemoryConsts::kHighSpeedClockVal;
		} else if (inAddress == TMemoryConsts::kHdWr_CalendarReg) {
			return mInterruptManager->GetRealTimeClock();
		} else if (inAddress == TMemoryConsts::kHdWr_AlarmReg) {
			return mInterruptManager->GetAlarm();
		} else if (inAddress == TMemoryConsts::kHdWr_Ticks) {
			return mInterruptManager->GetTimer();
		} else if (inAddress == TMemoryConsts::kHdWr_IntPresent) {
			return mInterruptManager->GetIntRaised();
		} else if (inAddress == TMemoryConsts::kHdWr_IntCtrlReg) {
			return mInterruptManager->GetIntCtrlReg();
		} else if (inAddress == TMemoryConsts::kHdWr_FIQMaskReg) {
			return mInterruptManager->GetFIQMask();
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg1) {
			return mInterruptManager->GetIntEDReg1();
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg2) {
			return mInterruptManager->GetIntEDReg2();
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg3) {
			return mInterruptManager->GetIntEDReg3();
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_RReg) {
			return mInterruptManager->GetGPIORaised();
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_EReg) {
			return mInterruptManager->GetGPIOCtrlReg();
		} else if (inAddress == TMemoryConsts::kHdWr_P0F18D400) {
			return 0xffffffff; // PCMCIA Door Locked?
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word access to unknown bank #3 at P0x%.8X",
					(unsigned int) inAddress );
			}
			// mEmulator->BreakInMonitor();
			return 0;
		}
	} else if (inAddress < TMemoryConsts::kSerialEnd) {
		// Serial bank
		if (mLog)
		{
			mLog->FLogLine(
				"Read word access to serial bank at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		return 0;
	} else if (inAddress < TMemoryConsts::kFlashBank2) {
		// Bank #4
		if (inAddress == TMemoryConsts::kHdWr_ExtDataAbt1) {
			return 0;
		} else if (inAddress == TMemoryConsts::kHdWr_ExtDataAbt3) {
			return 0;
		} else if (inAddress == TMemoryConsts::kHdWr_BankCtrlReg) {
			return mBankCtrlRegister;
		} else if (inAddress == TMemoryConsts::kROMSerialChip) {
			KUInt32 bit;
			if (mSerialNumberIx == 64)
			{
				bit = 0;
			} else if (mSerialNumberIx >= 32) {
				bit = mSerialNumber[0] >> (mSerialNumberIx - 32);
			} else {
				bit = mSerialNumber[1] >> mSerialNumberIx;
			}
			mSerialNumberIx = (mSerialNumberIx + 1) % 65;
			bit = bit & 0x1;
			return (bit << 1);
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word access to unknown bank #4 at P0x%.8X",
					(unsigned int) inAddress );
			}
			// mEmulator->BreakInMonitor();
			return 0;
		}
	} else if (inAddress < TMemoryConsts::kFlashBank2End) {
		KUInt32 theResult = mFlash.Read(
								inAddress - TMemoryConsts::kFlashBank2,
								1 );
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Read flash (bank#2) at P0x%.8X = %.8X",
				(unsigned int) inAddress,
				(unsigned int) theResult );
		}
#endif
		return theResult;
	} else if (inAddress < TMemoryConsts::kPCMCIA0Base) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read word access to unknown bank #5 at P0x%.8X",
				(unsigned int) inAddress );
		}
		return 0;
	} else if (inAddress < TMemoryConsts::kPCMCIA1Base) {
#if kNbSockets > 0
		return mPCMCIACtrls[0]->Read( inAddress - TMemoryConsts::kPCMCIA0Base );
#else
		return 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA2Base) {
#if kNbSockets > 1
		return mPCMCIACtrls[1]->Read( inAddress - TMemoryConsts::kPCMCIA1Base );
#else
		return 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3Base) {
#if kNbSockets > 2
		return mPCMCIACtrls[2]->Read( inAddress - TMemoryConsts::kPCMCIA2Base );
#else
		return 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3End) {
#if kNbSockets > 3
		return mPCMCIACtrls[3]->Read( inAddress - TMemoryConsts::kPCMCIA3Base );
#else
		return 0;
#endif
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"Read word: Bus error at P0x%.8X",
				(unsigned int) inAddress );
			// mEmulator->BreakInMonitor();
		}
		outFault = true;
		return 0;
	}
}

// -------------------------------------------------------------------------- //
//  * ReadPAligned( PAddr, Boolean& )
// -------------------------------------------------------------------------- //
KUInt32
TMemory::ReadPAligned( PAddr inAddress, Boolean& outFault )
{
	if (!(inAddress & TMemoryConsts::kROMEndMask))
	{
		return *((KUInt32*) ((KUIntPtr) mROMImagePtr + inAddress));
	} else if (inAddress < TMemoryConsts::kRAMStart) {
		if (inAddress < TMemoryConsts::kFlashBank1) {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word: Bus error at P0x%.8X",
					(unsigned int) inAddress );
				// mEmulator->BreakInMonitor();
			}
			outFault = true;
			return false;
		} else if (inAddress < TMemoryConsts::kFlashBank1End) {
			KUInt32 theResult = mFlash.Read(
									inAddress - TMemoryConsts::kFlashBank1,
									0 );
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"Read flash (bank#1) at P0x%.8X = %.8X",
					(unsigned int) inAddress,
					(unsigned int) theResult );
			}
#endif
			return theResult;
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word access to unknown bank #1 at P0x%.8X",
					(unsigned int) inAddress );
			}
			// mEmulator->BreakInMonitor();
			return 0;
		}
	} else if (inAddress < mRAMEnd) {
		// RAM.
		return *((KUInt32*) ((KUIntPtr) mRAMOffset + inAddress));
	} else if (inAddress < TMemoryConsts::kHardwareBase) {
		// Bank #2
		if (mLog)
		{
			mLog->FLogLine(
				"Read word access to unknown bank #2 at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		return 0;
	} else if (inAddress < TMemoryConsts::kExternalSerialBase) {
		// Bank #3
		if (inAddress == TMemoryConsts::kHdWr_04RAMSize) {
			KUInt32 thePageCount = (mRAMSize >> 16) & 0xFF;
			return
				(thePageCount << 24)
				| (thePageCount << 16)
				| thePageCount;
		} else if (inAddress == TMemoryConsts::kHdWr_08RAMSize) {
			return 0;
		} else if ((inAddress >= TMemoryConsts::kHdWr_DMAChan1Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan1End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan1Base) >> 13;
			KUInt32 reg = (inAddress & 0x1C00) >> 10;
			return mDMAManager->ReadChannel1Register( channel, reg );
		} else if ((inAddress >= TMemoryConsts::kHdWr_DMAChan2Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan2End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan2Base) >> 12;
			KUInt32 reg = (inAddress & 0x0C00) >> 10;
			return mDMAManager->ReadChannel2Register( channel, reg );
		} else if (inAddress == TMemoryConsts::kHdWr_DMAAssgmnt) {
			return mDMAManager->ReadChannelAssignmentRegister();
		} else if (inAddress == TMemoryConsts::kHdWr_DMAEnableStat) {
			return mDMAManager->ReadStatusRegister();
		} else if (inAddress == TMemoryConsts::kHdWr_DMAWordStat) {
			return mDMAManager->ReadWordStatusRegister();
		} else if (inAddress == TMemoryConsts::kHdWr_HighSpeedClck) {
			return TMemoryConsts::kHighSpeedClockVal;
		} else if (inAddress == TMemoryConsts::kHdWr_CalendarReg) {
			return mInterruptManager->GetRealTimeClock();
		} else if (inAddress == TMemoryConsts::kHdWr_AlarmReg) {
			return mInterruptManager->GetAlarm();
		} else if (inAddress == TMemoryConsts::kHdWr_Ticks) {
			return mInterruptManager->GetTimer();
		} else if (inAddress == TMemoryConsts::kHdWr_IntPresent) {
			return mInterruptManager->GetIntRaised();
		} else if (inAddress == TMemoryConsts::kHdWr_IntCtrlReg) {
			return mInterruptManager->GetIntCtrlReg();
		} else if (inAddress == TMemoryConsts::kHdWr_FIQMaskReg) {
			return mInterruptManager->GetFIQMask();
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg1) {
			return mInterruptManager->GetIntEDReg1();
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg2) {
			return mInterruptManager->GetIntEDReg2();
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg3) {
			return mInterruptManager->GetIntEDReg3();
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_RReg) {
			return mInterruptManager->GetGPIORaised();
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_EReg) {
			return mInterruptManager->GetGPIOCtrlReg();
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word access to unknown bank #3 at P0x%.8X",
					(unsigned int) inAddress );
			}
			// mEmulator->BreakInMonitor();
			return 0;
		}
	} else if (inAddress < TMemoryConsts::kSerialEnd) {
		// Serial bank
		if (mLog)
		{
			mLog->FLogLine(
				"Read word access to serial bank at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		return 0;
	} else if (inAddress < TMemoryConsts::kFlashBank2) {
		// Bank #4
		if (inAddress == TMemoryConsts::kHdWr_ExtDataAbt1) {
			return 0;
		} else if (inAddress == TMemoryConsts::kHdWr_ExtDataAbt3) {
			return 0;
		} else if (inAddress == TMemoryConsts::kHdWr_BankCtrlReg) {
			return mBankCtrlRegister;
		} else if (inAddress == TMemoryConsts::kROMSerialChip) {
			KUInt32 bit;
			if (mSerialNumberIx == 64)
			{
				bit = 0;
			} else if (mSerialNumberIx >= 32) {
				bit = mSerialNumber[0] >> (mSerialNumberIx - 32);
			} else {
				bit = mSerialNumber[1] >> mSerialNumberIx;
			}
			mSerialNumberIx = (mSerialNumberIx + 1) % 65;
			bit = bit & 0x1;
			return (bit << 1);
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Read word access to unknown bank #4 at P0x%.8X",
					(unsigned int) inAddress );
			}
			// mEmulator->BreakInMonitor();
			return 0;
		}
	} else if (inAddress < TMemoryConsts::kFlashBank2End) {
		KUInt32 theResult = mFlash.Read(
								inAddress - TMemoryConsts::kFlashBank2,
								1 );
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Read flash (bank#2) at P0x%.8X = %.8X",
				(unsigned int) inAddress,
				(unsigned int) theResult );
		}
#endif
		return theResult;
	} else if (inAddress < TMemoryConsts::kPCMCIA0Base) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read word access to unknown bank #5 at P0x%.8X",
				(unsigned int) inAddress );
		}
		return 0;
	} else if (inAddress < TMemoryConsts::kPCMCIA1Base) {
#if kNbSockets > 0
		return mPCMCIACtrls[0]->Read( inAddress - TMemoryConsts::kPCMCIA0Base );
#else
		return 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA2Base) {
#if kNbSockets > 1
		return mPCMCIACtrls[1]->Read( inAddress - TMemoryConsts::kPCMCIA1Base );
#else
		return 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3Base) {
#if kNbSockets > 2
		return mPCMCIACtrls[2]->Read( inAddress - TMemoryConsts::kPCMCIA2Base );
#else
		return 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3End) {
#if kNbSockets > 3
		return mPCMCIACtrls[3]->Read( inAddress - TMemoryConsts::kPCMCIA3Base );
#else
		return 0;
#endif
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"Read word: Bus error at P0x%.8X",
				(unsigned int) inAddress );
			// mEmulator->BreakInMonitor();
		}
		outFault = true;
		return 0;
	}
}

// -------------------------------------------------------------------------- //
//  * ReadROMRAMP( PAddr, Boolean& )
// -------------------------------------------------------------------------- //
#if !TARGET_OS_MAC
//inline
#endif
KUInt32
TMemory::ReadROMRAMP( PAddr inAddress, Boolean& outFault )
{
	if (!(inAddress & TMemoryConsts::kROMEndMask))
	{
		if (inAddress & 0x3)
		{
			// UNALIGNED ACCESS
			KUInt32 theValue =
				*((KUInt32*)
					((KUIntPtr) mROMImagePtr + (inAddress & 0xFFFFFFFC)));
			KUInt32 rotAmount = (inAddress & 0x3) * 8;
			return (theValue << rotAmount)
				| (theValue >> (32 - rotAmount));
		} else {
			return *((KUInt32*) ((KUIntPtr) mROMImagePtr + inAddress));
		}
	} else if (inAddress < TMemoryConsts::kRAMStart) {
		if (mLog)
		{
			mLog->FLogLine(
				"ReadROMRAM between ROM & RAM at P0x%.8X",
				(unsigned int) inAddress );
		}
		mMMU.SetHardwareFault( inAddress );
		outFault = true;
		return 0;
	} else if (inAddress < mRAMEnd) {
		// RAM.
		if (inAddress & 0x3)
		{
			// UNALIGNED ACCESS
			KUInt32 theValue =
				*((KUInt32*)
					((KUIntPtr) mRAMOffset + (inAddress & 0xFFFFFFFC)));
			KUInt32 rotAmount = (inAddress & 0x3) * 8;
			return (theValue << rotAmount)
				| (theValue >> (32 - rotAmount));
		} else {
			return *((KUInt32*) ((KUIntPtr) mRAMOffset + inAddress));
		}
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"ReadROMRAM after mRAMEnd at P0x%.8X",
				(unsigned int) inAddress );
		}
		mMMU.SetHardwareFault( inAddress );
		outFault = true;
		return 0;
	}
}

// -------------------------------------------------------------------------- //
//  * ReadB( VAddr, KUInt8& )
// -------------------------------------------------------------------------- //
Boolean
TMemory::ReadB( VAddr inAddress, KUInt8& outByte )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&1)) {
			fprintf(stderr, "Watchpoint 0x%08X read around 0x%08X\n", (unsigned int)inAddress, (unsigned int)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}

	PAddr theAddress;

	if (IsMMUEnabled())
	{
		if (TranslateR( inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress;
	}
	
	if (ReadBP( theAddress, outByte ))
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * ReadBP( PAddr, KUInt8& )
// -------------------------------------------------------------------------- //
Boolean
TMemory::ReadBP( PAddr inAddress, KUInt8& outByte )
{
//	if (inAddress == 0x020237EA)
//	{
//		mEmulator->BreakInMonitor();
//	}
	if (!(inAddress & TMemoryConsts::kROMEndMask))
	{
#if TARGET_RT_LITTLE_ENDIAN
		// Swap the endianness of the address.
		outByte = *((KUInt8*) ((KUIntPtr) mROMImagePtr + (inAddress ^ 0x3)));
#else
		outByte = *((KUInt8*) ((KUIntPtr) mROMImagePtr + inAddress));
#endif
	} else if (inAddress < TMemoryConsts::kFlashBank1) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read byte: Bus error at P0x%.8X",
				(unsigned int) inAddress );
		}
		return true;
	} else if (inAddress < TMemoryConsts::kFlashBank1End) {
		outByte = mFlash.ReadB( inAddress - TMemoryConsts::kFlashBank1, 0 );
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Read flash (bank#1) at P0x%.8X = %.2X",
				(unsigned int) inAddress,
				(unsigned int) outByte );
		}
#endif
	} else if (inAddress < TMemoryConsts::kRAMStart) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read byte access to unknown bank #1 at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		outByte = 0;
	} else if (inAddress < mRAMEnd) {
		// RAM.
#if TARGET_RT_LITTLE_ENDIAN
		// Swap the endianness of the address.
		outByte = *((KUInt8*) (mRAMOffset + (inAddress ^ 0x3)));
#else
		outByte = *((KUInt8*) (mRAMOffset + inAddress));
#endif
	} else if (inAddress < TMemoryConsts::kHardwareBase) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read byte access to unknown bank #2 at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		outByte = 0;
	} else if (inAddress < TMemoryConsts::kExternalSerialBase) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read byte access to unknown bank #3 at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		outByte = 0;
	} else if (inAddress < TMemoryConsts::kSerialEnd) {
		if (inAddress < TMemoryConsts::kInfraredSerialBase)
		{
			outByte = mExternalPort->ReadRegister(
				inAddress - TMemoryConsts::kExternalSerialBase );
		} else if (inAddress < TMemoryConsts::kBuiltInSerialBase) {
			outByte = mInfraredPort->ReadRegister(
				inAddress - TMemoryConsts::kInfraredSerialBase );
		} else if (inAddress < TMemoryConsts::kModemSerialBase) {
			outByte = mBuiltInExtraPort->ReadRegister(
				inAddress - TMemoryConsts::kBuiltInSerialBase );
		} else {
			outByte = mModemPort->ReadRegister(
				inAddress - TMemoryConsts::kModemSerialBase );
		}
	} else if (inAddress < TMemoryConsts::kFlashBank2) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read byte access to unknown bank #4 at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		outByte = 0;
	} else if (inAddress < TMemoryConsts::kFlashBank2End) {
		outByte = mFlash.ReadB( inAddress - TMemoryConsts::kFlashBank2, 1 );
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Read flash (bank#2) at P0x%.8X = %.2X",
				(unsigned int) inAddress,
				(unsigned int) outByte );
		}
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA0Base) {
		if (mLog)
		{
			mLog->FLogLine(
				"Read byte access to unknown bank #5 at P0x%.8X",
				(unsigned int) inAddress );
		}
		// mEmulator->BreakInMonitor();
		outByte = 0;
	} else if (inAddress < TMemoryConsts::kPCMCIA1Base) {
#if kNbSockets > 0
		outByte = mPCMCIACtrls[0]->ReadB( inAddress - TMemoryConsts::kPCMCIA0Base );
#else
		outByte = 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA2Base) {
#if kNbSockets > 1
		outByte = mPCMCIACtrls[1]->ReadB( inAddress - TMemoryConsts::kPCMCIA1Base );
#else
		outByte = 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3Base) {
#if kNbSockets > 2
		outByte = mPCMCIACtrls[2]->ReadB( inAddress - TMemoryConsts::kPCMCIA2Base );
#else
		outByte = 0;
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3End) {
#if kNbSockets > 3
		outByte = mPCMCIACtrls[3]->ReadB( inAddress - TMemoryConsts::kPCMCIA3Base );
#else
		outByte = 0;
#endif
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"Read byte: Bus error at P0x%.8X",
				(unsigned int) inAddress );
		}
		return true;
	}
		
//	if (inAddress == 0x0F1C4400)
//	{
//		mEmulator->BreakInMonitor();
//	}

	return false;
}

// -------------------------------------------------------------------------- //
//  * Write( VAddr, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::Write( VAddr inAddress, KUInt32 inWord )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&2)) {
			fprintf(stderr, "Watchpoint 0x%08X written around 0x%08X\n", (unsigned int)inAddress, (unsigned int)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}

	PAddr theAddress;

	if (IsMMUEnabled())
	{
		if (TranslateW( inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress;
	}
	
	if (WriteP( theAddress, inWord ))
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * WriteAligned( VAddr, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::WriteAligned( VAddr inAddress, KUInt32 inWord )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&2)) {
			fprintf(stderr, "Watchpoint 0x%08X written around 0x%08X\n", (unsigned int)inAddress, (unsigned int)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}
	
	PAddr theAddress;

	if (IsMMUEnabled())
	{
		if (TranslateW( inAddress &~ 0x03, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress &~ 0x03;
	}
	
	if (WritePAligned( theAddress, inWord ))
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * WriteRAM( VAddr, KUInt32 )
// -------------------------------------------------------------------------- //
inline Boolean
TMemory::WriteRAM( VAddr inAddress, KUInt32 inWord )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&2)) {
			fprintf(stderr, "Watchpoint 0x%08lX written around 0x%08lX\n", (unsigned long)inAddress, (unsigned long)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}
	
	PAddr theAddress;

	if (IsMMUEnabled())
	{
		if (TranslateW( inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress;
	}
	
	if (WriteRAMP( theAddress, inWord ))
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * WriteP( PAddr, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::WriteP( PAddr inAddress, KUInt32 inWord )
{
	if (inAddress < TMemoryConsts::kRAMStart)
	{
		if (inAddress < TMemoryConsts::kHighROMEnd)
		{
			if (mLog)
			{
				mLog->FLogLine(
					"Ignored write word access to ROM at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
				//mEmulator->BreakInMonitor();
			}
		} else if (inAddress < TMemoryConsts::kFlashBank1) {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word: Bus error at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			return true;
		} else if (inAddress < TMemoryConsts::kFlashBank1End) {
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"Write word to flash (bank#1) (ignored) at P0x%.8X = %.8X",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
#endif
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word access to unknown bank #1 at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			// mEmulator->BreakInMonitor();
		}
	} else if (inAddress < mRAMEnd) {
//        if (inAddress == 0x04098490)
//        {
//                mEmulator->BreakInMonitor();
//        }
		// RAM.
		if (inAddress & 0x3)
		{
			// UNALIGNED ACCESS
			KUInt32 rotAmount = (inAddress & 0x3) * 8;
			KUInt32 theValue = (inWord >> rotAmount)
				| (inWord << (32 - rotAmount));
			*((KUInt32*) (mRAMOffset + inAddress)) = theValue;
		} else {
			*((KUInt32*) (mRAMOffset + inAddress)) = inWord;
		}
		
		// Invalidate JIT.
		mJIT.Invalidate( inAddress );
	} else if (inAddress < TMemoryConsts::kHardwareBase) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write word access to unknown bank #2 at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
//		if (inWord == 0x00000411)
//			mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kExternalSerialBase) {
		if ((inAddress >= TMemoryConsts::kHdWr_DMAChan1Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan1End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan1Base) >> 13;
			KUInt32 reg = (inAddress & 0x1C00) >> 10;
			mDMAManager->WriteChannel1Register( channel, reg, inWord );
		} else if ((inAddress >= TMemoryConsts::kHdWr_DMAChan2Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan2End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan2Base) >> 12;
			KUInt32 reg = (inAddress & 0x0C00) >> 10;
			mDMAManager->WriteChannel2Register( channel, reg, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_DMAAssgmnt) {
			mDMAManager->WriteChannelAssignmentRegister( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_DMAEnableStat) {
			mDMAManager->WriteEnableRegister( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_DMADisable) {
			mDMAManager->WriteDisableRegister( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_CalendarReg) {
			mInterruptManager->SetRealTimeClock(inWord);
		} else if (inAddress == TMemoryConsts::kHdWr_AlarmReg) {
			mInterruptManager->SetAlarm(inWord);
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg0) {
			mInterruptManager->SetTimerMatchRegister( 0, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg1) {
			mInterruptManager->SetTimerMatchRegister( 1, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg2) {
			mInterruptManager->SetTimerMatchRegister( 2, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg3) {
			mInterruptManager->SetTimerMatchRegister( 3, inWord );
//			if (mLog)
//			{
//				mLog->FLogLine(
//					"Set scheduler timer to %.8X (now = %.8X)",
//					(unsigned int) inWord,
//					(unsigned int) mInterruptManager->GetTimer() );
//			}
		} else if (inAddress == TMemoryConsts::kHdWr_IntCtrlReg) {
			mInterruptManager->SetIntCtrlReg( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntClear) {
			mInterruptManager->ClearInterrupts( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_FIQMaskReg) {
			mInterruptManager->SetFIQMask( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg1) {
			mInterruptManager->SetIntEDReg1( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg2) {
			mInterruptManager->SetIntEDReg2( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg3) {
			mInterruptManager->SetIntEDReg3( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_EReg) {
			mInterruptManager->SetGPIOCtrlReg( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_CReg) {
			mInterruptManager->ClearGPIO( inWord );
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word access to unknown bank #3 at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			// mEmulator->BreakInMonitor();
		}
	} else if (inAddress < TMemoryConsts::kSerialEnd) {
		// Serial bank
		if (mLog)
		{
			mLog->FLogLine(
				"Write word access to serial bank at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord);
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kFlashBank2) {
		if (inAddress == TMemoryConsts::kHdWr_ExtDataAbt2) {
			// Ignore it.
		} else if (inAddress == TMemoryConsts::kHdWr_BankCtrlReg) {
			mBankCtrlRegister = inWord;
//			if (mLog)
//			{
//				mLog->FLogLine(
//					"Changed bank control register to %.8X",
//					(unsigned int) inWord );
//				mEmulator->BreakInMonitor();
//			}
		} else if (inAddress == TMemoryConsts::kROMSerialChip) {
			// Ignore it.
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word access to unknown bank #4 at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			// mEmulator->BreakInMonitor();
		}
	} else if (inAddress < TMemoryConsts::kFlashBank2End) {
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Write word to flash (bank#2) (ignored) at P0x%.8X = %.8X",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA0Base) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write word access to unknown bank #5 at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kPCMCIA1Base) {
#if kNbSockets > 0
		mPCMCIACtrls[0]->Write( inAddress - TMemoryConsts::kPCMCIA0Base, inWord );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA2Base) {
#if kNbSockets > 1
		mPCMCIACtrls[1]->Write( inAddress - TMemoryConsts::kPCMCIA1Base, inWord );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3Base) {
#if kNbSockets > 2
		mPCMCIACtrls[2]->Write( inAddress - TMemoryConsts::kPCMCIA2Base, inWord );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3End) {
#if kNbSockets > 3
		mPCMCIACtrls[3]->Write( inAddress - TMemoryConsts::kPCMCIA3Base, inWord );
#endif
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"Write word: Bus error at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * WritePAligned( PAddr, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::WritePAligned( PAddr inAddress, KUInt32 inWord )
{
	if (inAddress < TMemoryConsts::kRAMStart)
	{
		if (inAddress < TMemoryConsts::kHighROMEnd)
		{
			if (mLog)
			{
				mLog->FLogLine(
					"Ignored write word access to ROM at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
				//mEmulator->BreakInMonitor();
			}
		} else if (inAddress < TMemoryConsts::kFlashBank1) {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word: Bus error at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			return true;
		} else if (inAddress < TMemoryConsts::kFlashBank1End) {
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"Write word to flash (bank#1) (ignored) at P0x%.8X = %.8X",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
#endif
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word access to unknown bank #1 at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			// mEmulator->BreakInMonitor();
		}
	} else if (inAddress < mRAMEnd) {
//        if (inAddress == 0x04098490)
//        {
//                mEmulator->BreakInMonitor();
//        }
		// RAM.
		*((KUInt32*) (mRAMOffset + inAddress)) = inWord;
		
		// Invalidate JIT.
		mJIT.Invalidate( inAddress );
	} else if (inAddress < TMemoryConsts::kHardwareBase) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write word access to unknown bank #2 at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
//		if (inWord == 0x00000411)
//			mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kExternalSerialBase) {
		if ((inAddress >= TMemoryConsts::kHdWr_DMAChan1Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan1End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan1Base) >> 13;
			KUInt32 reg = (inAddress & 0x1C00) >> 10;
			mDMAManager->WriteChannel1Register( channel, reg, inWord );
		} else if ((inAddress >= TMemoryConsts::kHdWr_DMAChan2Base)
			&& (inAddress < TMemoryConsts::kHdWr_DMAChan2End)) {
			KUInt32 channel = (inAddress - TMemoryConsts::kHdWr_DMAChan2Base) >> 12;
			KUInt32 reg = (inAddress & 0x0C00) >> 10;
			mDMAManager->WriteChannel2Register( channel, reg, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_DMAAssgmnt) {
			mDMAManager->WriteChannelAssignmentRegister( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_DMAEnableStat) {
			mDMAManager->WriteEnableRegister( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_DMADisable) {
			mDMAManager->WriteDisableRegister( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_CalendarReg) {
			mInterruptManager->SetRealTimeClock(inWord);
		} else if (inAddress == TMemoryConsts::kHdWr_AlarmReg) {
			mInterruptManager->SetAlarm(inWord);
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg0) {
			mInterruptManager->SetTimerMatchRegister( 0, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg1) {
			mInterruptManager->SetTimerMatchRegister( 1, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg2) {
			mInterruptManager->SetTimerMatchRegister( 2, inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_MatchReg3) {
			mInterruptManager->SetTimerMatchRegister( 3, inWord );
//			if (mLog)
//			{
//				mLog->FLogLine(
//					"Set scheduler timer to %.8X (now = %.8X)",
//					(unsigned int) inWord,
//					(unsigned int) mInterruptManager->GetTimer() );
//			}
		} else if (inAddress == TMemoryConsts::kHdWr_IntCtrlReg) {
			mInterruptManager->SetIntCtrlReg( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntClear) {
			mInterruptManager->ClearInterrupts( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_FIQMaskReg) {
			mInterruptManager->SetFIQMask( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg1) {
			mInterruptManager->SetIntEDReg1( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg2) {
			mInterruptManager->SetIntEDReg2( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_IntEDReg3) {
			mInterruptManager->SetIntEDReg3( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_EReg) {
			mInterruptManager->SetGPIOCtrlReg( inWord );
		} else if (inAddress == TMemoryConsts::kHdWr_GPIO_CReg) {
			mInterruptManager->ClearGPIO( inWord );
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word access to unknown bank #3 at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			// mEmulator->BreakInMonitor();
		}
	} else if (inAddress < TMemoryConsts::kSerialEnd) {
		// Serial bank
		if (mLog)
		{
			mLog->FLogLine(
				"Write word access to serial bank at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord);
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kFlashBank2) {
		if (inAddress == TMemoryConsts::kHdWr_ExtDataAbt2) {
			// Ignore it.
		} else if (inAddress == TMemoryConsts::kHdWr_BankCtrlReg) {
			mBankCtrlRegister = inWord;
//			if (mLog)
//			{
//				mLog->FLogLine(
//					"Changed bank control register to %.8X",
//					(unsigned int) inWord );
//				mEmulator->BreakInMonitor();
//			}
		} else if (inAddress == TMemoryConsts::kROMSerialChip) {
			// Ignore it.
		} else {
			if (mLog)
			{
				mLog->FLogLine(
					"Write word access to unknown bank #4 at P0x%.8X (%.8X)",
					(unsigned int) inAddress,
					(unsigned int) inWord );
			}
			// mEmulator->BreakInMonitor();
		}
	} else if (inAddress < TMemoryConsts::kFlashBank2End) {
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Write word to flash (bank#2) (ignored) at P0x%.8X = %.8X",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA0Base) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write word access to unknown bank #5 at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kPCMCIA1Base) {
#if kNbSockets > 0
		mPCMCIACtrls[0]->Write( inAddress - TMemoryConsts::kPCMCIA0Base, inWord );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA2Base) {
#if kNbSockets > 1
		mPCMCIACtrls[1]->Write( inAddress - TMemoryConsts::kPCMCIA1Base, inWord );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3Base) {
#if kNbSockets > 2
		mPCMCIACtrls[2]->Write( inAddress - TMemoryConsts::kPCMCIA2Base, inWord );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3End) {
#if kNbSockets > 3
		mPCMCIACtrls[3]->Write( inAddress - TMemoryConsts::kPCMCIA3Base, inWord );
#endif
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"Write word: Bus error at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * WriteRAMP( PAddr, KUInt32 )
// -------------------------------------------------------------------------- //
inline Boolean
TMemory::WriteRAMP( PAddr inAddress, KUInt32 inWord )
{
	if (inAddress < TMemoryConsts::kRAMStart)
	{
		if (mLog)
		{
			mLog->FLogLine(
				"WriteRAM before TMemoryConsts::kRAMStart at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
		return true;
	} else if (inAddress < mRAMEnd) {
		// RAM.
		if (inAddress & 0x3)
		{
			// UNALIGNED ACCESS
			KUInt32 rotAmount = (inAddress & 0x3) * 8;
			KUInt32 theValue = (inWord >> rotAmount)
				| (inWord << (32 - rotAmount));
			*((KUInt32*) (mRAMOffset + inAddress)) = theValue;
		} else {
			*((KUInt32*) (mRAMOffset + inAddress)) = inWord;
		}
		
		// Invalidate JIT.
		mJIT.Invalidate( inAddress );
		return false;
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"WriteRAM after mRAMEnd at P0x%.8X (%.8X)",
				(unsigned int) inAddress,
				(unsigned int) inWord );
		}
		return true;
	}
}

// -------------------------------------------------------------------------- //
//  * WriteB( VAddr, KUInt8 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::WriteB( VAddr inAddress, KUInt8 inByte )
{
	int i;
	for (i=0; i<mWPCount; i++) {
		if ((mWatchpoints[i].fAddress==inAddress) && (mWatchpoints[i].fMode&2)) {
			fprintf(stderr, "Watchpoint 0x%08X written around 0x%08X\n", (unsigned int)inAddress, (unsigned int)mProcessor->mCurrentRegisters[15]);
			mEmulator->BreakInMonitor();
		}
	}
	
	PAddr theAddress;
	if (IsMMUEnabled())
	{
		if (TranslateW( inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = inAddress;
	}
	
	if (WriteBP( theAddress, inByte ))
	{
		mMMU.SetHardwareFault( inAddress );
		return true;
	}
	
//	if (inAddress == 0x0C105548)
//	{
//		mEmulator->BreakInMonitor();
//	}

	return false;
}

// -------------------------------------------------------------------------- //
//  * WriteBP( PAddr, KUInt8 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::WriteBP( PAddr inAddress, KUInt8 inByte )
{
	if (inAddress < TMemoryConsts::kHighROMEnd)
	{
		if (mLog)
		{
			mLog->FLogLine(
				"Ignored write byte access to ROM at P0x%.8X (%.2X)",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
	} else if (inAddress < TMemoryConsts::kFlashBank1) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte: Bus error at P0x%.8X (%.2X)",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
		return true;
	} else if (inAddress < TMemoryConsts::kFlashBank1End) {
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte to flash (bank#1) (ignored) at P0x%.8X = %.2X",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
#endif
	} else if (inAddress < TMemoryConsts::kRAMStart) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte access to unknown bank #1 at P0x%.8X (%.2X)",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < mRAMEnd) {
//        if (inAddress == 0x04098490)
//        {
//                mEmulator->BreakInMonitor();
//        }

		// RAM.
#if TARGET_RT_LITTLE_ENDIAN
		// Swap the endianness of the address.
		*((KUInt8*) (mRAMOffset + (inAddress ^ 0x3))) = inByte;
#else
		*((KUInt8*) (mRAMOffset + inAddress)) = inByte;
#endif		

		// Invalidate JIT.
		mJIT.Invalidate( inAddress );
	} else if (inAddress < TMemoryConsts::kHardwareBase) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte access to unknown bank #2 at P0x%.8X (%.2X)",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kExternalSerialBase) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte access to unknown bank #3 at P0x%.8X = %.2X",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kSerialEnd) {
		if (inAddress < TMemoryConsts::kInfraredSerialBase)
		{
			mExternalPort->WriteRegister(
				inAddress - TMemoryConsts::kExternalSerialBase, inByte );
		} else if (inAddress < TMemoryConsts::kBuiltInSerialBase) {
			mInfraredPort->WriteRegister(
				inAddress - TMemoryConsts::kInfraredSerialBase, inByte );
		} else if (inAddress < TMemoryConsts::kModemSerialBase) {
			mBuiltInExtraPort->WriteRegister(
				inAddress - TMemoryConsts::kBuiltInSerialBase, inByte );
		} else {
			mModemPort->WriteRegister(
				inAddress - TMemoryConsts::kModemSerialBase, inByte );
		}
	} else if (inAddress < TMemoryConsts::kFlashBank2) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte access to unknown bank #4 at P0x%.8X = %.2X",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kFlashBank2End) {
#if debugFlash
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte to flash (bank#2) (ignored) at P0x%.8X = %.2X",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA0Base) {
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte access to unknown bank #5 at P0x%.8X (%.2X)",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
		// mEmulator->BreakInMonitor();
	} else if (inAddress < TMemoryConsts::kPCMCIA1Base) {
#if kNbSockets > 0
		mPCMCIACtrls[0]->WriteB( inAddress - TMemoryConsts::kPCMCIA0Base, inByte );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA2Base) {
#if kNbSockets > 1
		mPCMCIACtrls[1]->WriteB( inAddress - TMemoryConsts::kPCMCIA1Base, inByte );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3Base) {
#if kNbSockets > 2
		mPCMCIACtrls[2]->WriteB( inAddress - TMemoryConsts::kPCMCIA2Base, inByte );
#endif
	} else if (inAddress < TMemoryConsts::kPCMCIA3End) {
#if kNbSockets > 3
		mPCMCIACtrls[3]->WriteB( inAddress - TMemoryConsts::kPCMCIA3Base, inByte );
#endif
	} else {
		if (mLog)
		{
			mLog->FLogLine(
				"Write byte: Bus error at P0x%.8X (%.2X)",
				(unsigned int) inAddress,
				(unsigned int) inByte );
		}
		return true;
	}
	
//	if ((inAddress == 0x0F240000) || (inAddress == 0x0F240800))
//	{
//		mEmulator->BreakInMonitor();
//	}

	return false;
}

// -------------------------------------------------------------------------- //
//  * TranslateAndCheckFlashAddress( KUInt32, PAddr*)
// -------------------------------------------------------------------------- //
Boolean
TMemory::TranslateAndCheckFlashAddress( KUInt32 inAddress, PAddr* outAddress )
{
	PAddr theAddress;

	if (IsMMUEnabled())
	{
		if (TranslateW( (VAddr) inAddress, theAddress ))
		{
			return true;
		}
	} else {
		theAddress = (PAddr) inAddress;
	}
	
	if (((theAddress < TMemoryConsts::kFlashBank1) || (theAddress > TMemoryConsts::kFlashBank1End))
		&& ((theAddress < TMemoryConsts::kFlashBank2) || (theAddress > TMemoryConsts::kFlashBank2End)))
	{
		mEmulator->BreakInMonitor();
		return true;
	}
	
	if (outAddress)
	{
		*outAddress = theAddress;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * WriteToFlash32Bits( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::WriteToFlash32Bits( KUInt32 inWord, KUInt32 inMask, KUInt32 inAddress )
{
	PAddr theAddress = 0;
	
	if (TranslateAndCheckFlashAddress( inAddress, &theAddress ))
	{
		if (mLog)
		{
			mLog->FLogLine(
				"Out of bounds flash access at P0x%.8X (V0x%.8X)",
				(unsigned int) theAddress,
				(unsigned int) inAddress );
		}
		return true;
	}

#if debugFlash
	if (mLog)
	{
		mLog->FLogLine(
			"WriteToFlash32Bits (word = %.8X, mask = %.8X, addr = V0x%.8X : P0x%.8X)",
			(unsigned int) inWord,
			(unsigned int) inMask,
			(unsigned int) inAddress,
			(unsigned int) theAddress );
	}
#endif

	KUInt32 theOffset;
	KUInt32 theBank;
	if (theAddress < TMemoryConsts::kFlashBank2)
	{
		theOffset = theAddress - TMemoryConsts::kFlashBank1;
		theBank = 0;
	} else {
		theOffset = theAddress - TMemoryConsts::kFlashBank2;
		theBank = 1;
	}
	
	mFlash.Write( inWord, inMask, theOffset, theBank );
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * WriteToFlash16Bits( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::WriteToFlash16Bits( KUInt32 inWord, KUInt32 inMask, KUInt32 inAddress )
{
	PAddr theAddress = 0;
	
	if (TranslateAndCheckFlashAddress( inAddress, &theAddress ))
	{
		if (mLog)
		{
			mLog->FLogLine(
				"Out of bounds flash access at P0x%.8X (V0x%.8X)",
				(unsigned int) theAddress,
				(unsigned int) inAddress );
		}
		return true;
	}

#if debugFlash
	if (mLog)
	{
		mLog->FLogLine(
			"WriteToFlash16Bits (word = %.8X, mask = %.8X, addr = V0x%.8X : P0x%.8X)",
			(unsigned int) inWord,
			(unsigned int) inMask,
			(unsigned int) inAddress,
			(unsigned int) theAddress );
	}
#endif

	KUInt32 theOffset;
	KUInt32 theBank;
	if (theAddress < TMemoryConsts::kFlashBank2)
	{
		theOffset = (theAddress - TMemoryConsts::kFlashBank1) / 2;
		theBank = 0;
	} else {
		theOffset = (theAddress - TMemoryConsts::kFlashBank2) / 2;
		theBank = 1;
	}
	
	// if We do 16 bits, handle the swap.
	if (theAddress & 0x2)
	{
		// It's the lower 16 bits.
		mFlash.Write( inWord, inMask, theOffset &~ 0x1, theBank );	
	} else {
		// It's the higher 16 bits.
		mFlash.Write( (inWord << 16), (inMask << 16), theOffset &~ 0x1, theBank );	
	}
	
	return false;
}
// -------------------------------------------------------------------------- //
//  * EraseFlash( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::EraseFlash( KUInt32 inAddress, KUInt32 inBlockSize )
{
	PAddr theAddress = 0;
	
	if (TranslateAndCheckFlashAddress( inAddress, &theAddress ))
	{
		if (mLog)
		{
			mLog->FLogLine(
				"Out of bounds flash access at P0x%.8X (V0x%.8X)",
				(unsigned int) theAddress,
				(unsigned int) inAddress );
		}
		return true;
	}

#if debugFlash
	if (mLog)
	{
		mLog->FLogLine(
			"EraseFlash (addr = V0x%.8X : P0x%.8X, size = %.8X)",
			(unsigned int) inAddress,
			(unsigned int) theAddress,
			(unsigned int) inBlockSize );
	}
#endif

	// We assume that the whole block is within a single bank.

	KUInt32 theOffset;
	KUInt32 theBank;
	if (theAddress < TMemoryConsts::kFlashBank2)
	{
		theOffset = theAddress - TMemoryConsts::kFlashBank1;
		theBank = 0;
	} else {
		theOffset = theAddress - TMemoryConsts::kFlashBank2;
		theBank = 1;
	}
	
	mFlash.Erase( inBlockSize, theOffset, theBank );
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * ComputeSerialNumber( const KUInt32* )
// -------------------------------------------------------------------------- //
void
TMemory::ComputeSerialNumber( const KUInt32 inNewtonID[2] )
{
	int indexSerial = 8;
	unsigned int checksum = 0x100;
	KUInt32 serial = inNewtonID[1];
	for (indexSerial = 0; indexSerial < 32; indexSerial++)
	{
		if (serial & 0x1)
		{
			checksum |= 1;
		}
		if (checksum & 0x100)
		{
			checksum ^= 0x131;
		}
		checksum <<= 1;
		serial >>= 1;
	}
	serial = inNewtonID[0];
	for (; indexSerial < 48; indexSerial++)
	{
		if (serial & 0x1)
		{
			checksum |= 1;
		}
		if (checksum & 0x100)
		{
			checksum ^= 0x131;
		}
		checksum <<= 1;
		serial >>= 1;
	}
	
	int theCRC = kSerialNumberCRC[checksum >> 1];

	mSerialNumber[0] =
			(theCRC << 24)
		|	(inNewtonID[0] << 8)
		|	(inNewtonID[1] >> 24);
	mSerialNumber[1] =
			(inNewtonID[1] << 8)
		|	0x01;
}

// -------------------------------------------------------------------------- //
//  * ReadBreakpoint( VAddr, KUInt32& )
// -------------------------------------------------------------------------- //
Boolean
TMemory::ReadBreakpoint( VAddr inAddress, KUInt32& outWord )
{
	// Translate the address (priviledged).
	Boolean savedPrivilege = GetPrivilege();
	if (!savedPrivilege)
	{
		SetPrivilege( true );
	}
	
	PAddr theAddress;
	Boolean theResult = false;
	
	if (IsMMUEnabled())
	{
		theResult = TranslateR( inAddress, theAddress );
	} else {
		theAddress = inAddress;
	}
	if (!theResult)
	{
		// Iterate on the BP.
		theResult = true;
		SBreakpoint* endBP = &mBreakpoints[mBPCount];
		SBreakpoint* cursor;
		for (cursor = mBreakpoints; cursor < endBP; cursor++)
		{
			if (cursor->fAddress == theAddress)
			{
				theResult = false;
				outWord = cursor->fOriginalValue;
				break;
			}
		}
	}
	
	if (!savedPrivilege)
	{
		SetPrivilege(false);
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * SetBreakpoint( VAddr, KUInt16 )
// -------------------------------------------------------------------------- //
Boolean
TMemory::SetBreakpoint( VAddr inAddress, KUInt16 inID )
{
	// Translate the address (priviledged).
	Boolean savedPrivilege = GetPrivilege();
	if (!savedPrivilege)
	{
		SetPrivilege(true);
	}
	
	PAddr theAddress;
	Boolean theResult = false;
	
	if (IsMMUEnabled())
	{
		theResult = TranslateR( inAddress, theAddress );
	} else {
		theAddress = inAddress;
	}
	do {
		if (theResult)
		{
			break;
		}
		
		// Iterate on the BP to find it.
		Boolean weAlreadyHaveIt = false;
		SBreakpoint* endBP = &mBreakpoints[mBPCount];
		SBreakpoint* cursor;
		for (cursor = mBreakpoints; cursor < endBP; cursor++)
		{
			if (cursor->fAddress == theAddress)
			{
				weAlreadyHaveIt = true;
				break;
			}
		}
		
		if (weAlreadyHaveIt)
		{
			break;
		}
		
		// Add it.
		Boolean fault = false;
		KUInt32 originalValue =	ReadROMRAMP( theAddress, fault );
		if (fault)
		{
			break;
		}

		KUInt32 theValue = 0xE1200070;
		theValue |= inID & 0x000F;
		theValue |= (inID & 0xFFF0) << 4;

		if (!(theAddress & TMemoryConsts::kROMEndMask))
		{
			*((KUInt32*) ((KUIntPtr) mROMImagePtr + theAddress)) = theValue;

			// Invalidate JIT.
			mJIT.Invalidate( theAddress );
		} else {
			theResult = WriteP( theAddress, theValue );

			if (theResult)
			{
				break;
			}
		}
		
		KUInt32 theBPCount = mBPCount + 1;
		mBreakpoints = (SBreakpoint*) ::realloc( mBreakpoints,
			sizeof(SBreakpoint) * theBPCount);
		mBPCount = theBPCount;
		mBreakpoints[theBPCount - 1].fAddress = theAddress;
		mBreakpoints[theBPCount - 1].fOriginalValue = originalValue;
		mBreakpoints[theBPCount - 1].fBPValue = theValue;
	} while (false);
	
	if (!savedPrivilege)
	{
		SetPrivilege(false);
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * ClearBreakpoint( VAddr )
// -------------------------------------------------------------------------- //
Boolean
TMemory::ClearBreakpoint( VAddr inAddress )
{
	// Translate the address (priviledged).
	Boolean savedPrivilege = GetPrivilege();
	if (!savedPrivilege)
	{
		SetPrivilege(true);
	}
	
	PAddr theAddress;
	Boolean theResult = false;
	
	if (IsMMUEnabled())
	{
		theResult = TranslateR( inAddress, theAddress );
	} else {
		theAddress = inAddress;
	}
	if (!theResult)
	{
		// Iterate on the BP.
		theResult = true;
		SBreakpoint* endBP = &mBreakpoints[mBPCount];
		SBreakpoint* cursor;
		for (cursor = mBreakpoints; cursor < endBP; cursor++)
		{
			if (cursor->fAddress == theAddress)
			{
				theResult = false;
				
				KUInt32 theOriginalValue = cursor->fOriginalValue;
				
				(void) ::memmove(
					cursor, &cursor[1],
					(endBP - cursor) * sizeof (SBreakpoint) );
				KUInt32 theBPCount = mBPCount - 1;
				mBreakpoints = (SBreakpoint*) ::realloc( mBreakpoints,
					sizeof(SBreakpoint) * theBPCount);
				mBPCount = theBPCount;
				
				if (!(theAddress & TMemoryConsts::kROMEndMask))
				{
					*((KUInt32*) ((KUIntPtr) mROMImagePtr + theAddress))
						= theOriginalValue;

					// Invalidate JIT.
					mJIT.Invalidate( theAddress );
				} else {
					theResult = WriteRAMP( theAddress, theOriginalValue );
				}
				
				break;
			}
		}
	}
	
	if (!savedPrivilege)
	{
		SetPrivilege(false);
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * DisableBreakpoint( VAddr )
// -------------------------------------------------------------------------- //
Boolean
TMemory::DisableBreakpoint( VAddr inAddress )
{
	// Translate the address (priviledged).
	Boolean savedPrivilege = GetPrivilege();
	if (!savedPrivilege)
	{
		SetPrivilege(true);
	}
	
	PAddr theAddress;
	Boolean theResult = false;
	
	if (IsMMUEnabled())
	{
		theResult = TranslateR( inAddress, theAddress );
	} else {
		theAddress = inAddress;
	}
	if (!theResult)
	{
		// Iterate on the BP.
		theResult = true;
		SBreakpoint* endBP = &mBreakpoints[mBPCount];
		SBreakpoint* cursor;
		for (cursor = mBreakpoints; cursor < endBP; cursor++)
		{
			if (cursor->fAddress == theAddress)
			{
				theResult = false;
				
				KUInt32 theOriginalValue = cursor->fOriginalValue;
				
				if (!(theAddress & TMemoryConsts::kROMEndMask))
				{
					*((KUInt32*) ((KUIntPtr) mROMImagePtr + theAddress))
						= theOriginalValue;

					// Invalidate JIT.
					mJIT.Invalidate( theAddress );
				} else {
					theResult = WriteRAMP( theAddress, theOriginalValue );
				}
				break;
			}
		}
	}
	
	if (!savedPrivilege)
	{
		SetPrivilege(false);
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * EnableBreakpoint( VAddr )
// -------------------------------------------------------------------------- //
Boolean
TMemory::EnableBreakpoint( VAddr inAddress)
{
	// Translate the address (priviledged).
	Boolean savedPrivilege = GetPrivilege();
	if (!savedPrivilege)
	{
		SetPrivilege(true);
	}
	
	PAddr theAddress;
	Boolean theResult = false;
	
	if (IsMMUEnabled())
	{
		theResult = TranslateR( inAddress, theAddress );
	} else {
		theAddress = inAddress;
	}
	if (!theResult)
	{
		// Iterate on the BP.
		theResult = true;
		SBreakpoint* endBP = &mBreakpoints[mBPCount];
		SBreakpoint* cursor;
		for (cursor = mBreakpoints; cursor < endBP; cursor++)
		{
			if (cursor->fAddress == theAddress)
			{
				theResult = false;
				
				if (!(theAddress & TMemoryConsts::kROMEndMask))
				{
					*((KUInt32*) ((KUIntPtr) mROMImagePtr + theAddress))
						= cursor->fBPValue;

					// Invalidate JIT.
					mJIT.Invalidate( theAddress );
				} else {
					theResult = WriteRAMP( theAddress, cursor->fBPValue );
				}
				
				break;
			}
		}
	}
	
	if (!savedPrivilege)
	{
		SetPrivilege(true);
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * TransferState( TStream* ) const
// -------------------------------------------------------------------------- //
void
TMemory::TransferState( TStream* inStream )
{
	// Invalidate the JIT cache.
	mJIT.InvalidateTLB();
	
	// The various registers.
	inStream->TransferInt32BE( mRAMSize );
	inStream->TransferInt32BE( mRAMEnd );
	inStream->TransferInt32BE( mBankCtrlRegister );
	inStream->TransferInt32BE( mBPCount );
	
	// The ROM.
	inStream->TransferInt32ArrayBE( (KUInt32*) mROMImagePtr, 0x01000000 / sizeof( KUInt32 ) );

	// The RAM
	if (inStream->IsReading()) {
		mRAM = (KUInt8*) ::realloc( mRAM, mRAMSize );
		mRAMOffset = ((KUIntPtr) mRAM) - TMemoryConsts::kRAMStart;
	}
	inStream->TransferInt32ArrayBE( (KUInt32*) mRAM, mRAMSize / sizeof( KUInt32 ) );

	// The breakpoints.
	if (inStream->IsReading()) {
		KUInt32 BPSize = sizeof(SBreakpoint) * mBPCount;
		mBreakpoints = (SBreakpoint*) ::realloc( mBreakpoints, BPSize );
	}
	KUInt32 indexBP;
	for (indexBP = 0; indexBP < mBPCount; indexBP++)
	{
		inStream->TransferInt32BE( mBreakpoints[indexBP].fAddress );
		inStream->TransferInt32BE( mBreakpoints[indexBP].fOriginalValue );
		inStream->TransferInt32BE( mBreakpoints[indexBP].fBPValue );
	}

	// The MMU
	mMMU.TransferState( inStream );

	// The flash.
	mFlash.TransferState( inStream );

	// Invalidate the JIT cache.
	mJIT.InvalidateTLB();
}


// -------------------------------------------------------------------------- //
//  * Init( void )
// -------------------------------------------------------------------------- //
void
TMemory::Init( void )
{
	int socketsIx;
	for (socketsIx = 0; socketsIx < kNbSockets; socketsIx++)
	{
		mPCMCIACtrls[socketsIx] = NULL;
	}
	mRAM = (KUInt8*) ::calloc( 1, mRAMSize );	// Default is 4 MB
	mRAMOffset = ((KUIntPtr) mRAM) - TMemoryConsts::kRAMStart;	// Difference between our RAM base address and a real Newton's
	mBreakpoints = (SBreakpoint*) ::malloc( 1 );
	mSerialNumber[0] = 0;
	mSerialNumber[1] = 0;
	mWPCount = 0;
	mWatchpoints = (struct SWatchpoint*)::calloc(kMaxWatchpoints, sizeof(struct SWatchpoint));
}


Boolean TMemory::AddWatchpoint(VAddr inAddr, KUInt8 inMode)
{
	// if there is already a wp, replace it
	int i;
	for (i=0; i<mWPCount; i++) {
		if (mWatchpoints[i].fAddress==inAddr) {
			mWatchpoints[i].fAddress = inAddr;
			mWatchpoints[i].fMode = inMode;
			return false;
		}
	}
	// if there is no more room for wp's, fail.
	if (mWPCount==kMaxWatchpoints) return true;
	// add a wp to the end of the list
	mWatchpoints[mWPCount].fAddress = inAddr;
	mWatchpoints[mWPCount].fMode = inMode;
	mWPCount++;
	return false;
}


Boolean TMemory::ClearWatchpoint(VAddr inAddr)
{
	// find the wp
	int i;
	for (i=0; i<mWPCount; i++) {
		if (mWatchpoints[i].fAddress==inAddr) {
			// move all following wp's one position back
			memmove(mWatchpoints+i, mWatchpoints+i+1, (kMaxWatchpoints-i-1)*sizeof(SWatchpoint));
			mWPCount--;
			return false;
		}
	}
	return true;
}

Boolean TMemory::GetWatchpoint( int inIndex, VAddr &outAddress, KUInt8 &outMode )
{
	if (inIndex>=mWPCount) return true;
	outAddress = mWatchpoints[inIndex].fAddress;
	outMode = mWatchpoints[inIndex].fMode;
	return false;
}


// ========================================================================== //
// If I have seen farther than others, it is because I was standing on the    //
// shoulders of giants.                                                       //
//                 -- Isaac Newton                                            //
//                                                                            //
// In the sciences, we are now uniquely priviledged to sit side by side with  //
// the giants on whose shoulders we stand.                                    //
//                 -- Gerald Holton                                           //
//                                                                            //
// If I have not seen as far as others, it is because giants were standing on //
// my shoulders.                                                              //
//                 -- Hal Abelson                                             //
//                                                                            //
// Mathematicians stand on each other's shoulders.                            //
//                 -- Gauss                                                   //
//                                                                            //
// Mathemeticians stand on each other's shoulders while computer scientists   //
// stand on each other's toes.                                                //
//                 -- Richard Hamming                                         //
//                                                                            //
// It has been said that physicists stand on one another's shoulders.  If     //
// this is the case, then programmers stand on one another's toes, and        //
// software engineers dig each other's graves.                                //
//                 -- Unknown                                                 //
// ========================================================================== //
