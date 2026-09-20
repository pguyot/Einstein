// ==============================
// File:			TATACard.cpp
// Project:			Einstein
//
// Copyright 2003-2026 by Paul Guyot (pguyot@kallisys.net).
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

 ATA card emulation

 Newton needs data on a PC Card to be executable in place. To store more than
 that, Paul Guyot's ATA driver talks to a real ATA card and maps the parts it
 needs into memory. We emulate that ATA card, so the original driver does the
 rest (partition map, stores, ...):
 http://www.kallisys.com/files/newton/ATA/ATA-Support-1.0-SourceCode.img.bin

 The card is backed by a raw image of 512 byte sectors, read into memory. Images
 larger than 128MB are clipped. Every sector that is written is written to the
 file right away, so a crash of the emulator loses nothing.

 The card is a memory mapped PC Card with four configuration registers in
 attribute space (0x200...0x206), the ATA task file is in memory space:

 Offset   Read              Write
 0        Even data         Even data
 1        Error             Features
 2        Sector Count      Sector Count     0 = 256, counts down to 0 when done
 3        Sector Number     Sector Number    LBA 7-0
 4        Cylinder Low      Cylinder Low     LBA 15-8
 5        Cylinder High     Cylinder High    LBA 23-16
 6        Drive/Head        Drive/Head       LBA 27-24, bit 6: LBA, bit 4: drive
 7        Status            Command
 8, 9     Even/Odd data     Even/Odd data
 D        Error             Features
 E        Alt Status        Device Control   bit 2: SRST, bit 1: nIEN
 400-7FF  Data              Data

 Byte addresses are flipped within each 32 bit word: memory space offsets are
 XORed with 3, attribute space offsets are (offset / 2) ^ 1. The data register
 is 16 bits wide, a 32 bit access moves two bytes, in bits 31-16.

 A command is started by writing the Command register. Commands without data
 show BSY once and are done. Commands with data show BSY, then DRQ until the
 host has read or written all 512 bytes of a sector. Sector Count is counted
 down after every sector, so an error leaves the number of sectors that are
 left in it, and the LBA registers point to the sector that failed.

 Not implemented: drive 1, interrupts (the host polls, nIEN is ignored), I/O
 mode (Conf in the Configuration Option Register), power down, DMA, and the
 multiple sector commands.

*/

#include "TATACard.h"

// Einstein
#include "TPCMCIAController.h"
#include "Emulator/Log/TLog.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

static const size_t kSectorSize = 512;

// Larger images are clipped, the rest of the file is left alone.
static const uint64_t kMaxImageSize = 128ULL * 1024 * 1024;

// ATA commands
static const KUInt8 kReadSectorsCmd = 0x20;
static const KUInt8 kWriteSectorsCmd = 0x30;
static const KUInt8 kReadVerifySectorsCmd = 0x40;
static const KUInt8 kStandbyImmediateAltCmd = 0x94;
static const KUInt8 kStandbyImmediateCmd = 0xE0;
static const KUInt8 kIdentifyDriveCmd = 0xEC;
static const KUInt8 kSetFeaturesCmd = 0xEF;

// Status register
static const KUInt8 kStatusReg_BSY = 0x80;
static const KUInt8 kStatusReg_RDY = 0x40;
static const KUInt8 kStatusReg_DWF = 0x20; // Write fault
static const KUInt8 kStatusReg_DSC = 0x10;
static const KUInt8 kStatusReg_DRQ = 0x08;
static const KUInt8 kStatusReg_ERR = 0x01;

// Error register
static const KUInt8 kErrorReg_OK = 0x01; // Diagnostic result after a reset
static const KUInt8 kErrorReg_ABRT = 0x04; // Command aborted
static const KUInt8 kErrorReg_IDNF = 0x10; // Sector not found

// Device Control register
static const KUInt8 kDeviceControl_nIEN = 0x02;
static const KUInt8 kDeviceControl_SRST = 0x04;

// Configuration Option Register
static const KUInt8 kConfigOption_SRESET = 0x80;
static const KUInt8 kConfigOption_ConfMask = 0x3F; // Only 0 (memory mapped) is supported

// Configuration and Status Register, the bits the host can write
static const KUInt8 kConfigStatus_IOis8 = 0x20;
static const KUInt8 kConfigStatus_WriteMask = 0x64; // SigChg, IOis8, PwrDwn

// Pin Replacement Register
static const KUInt8 kPinReplacement_RWProt = 0x01;
static const KUInt8 kPinReplacement_RRdy = 0x02;
static const KUInt8 kPinReplacement_AlwaysSet = 0x0C;

// clang-format off
const KUInt8 TATACard::kDefaultCISData[] = {
	0x01, 4, // CISTPL_DEVICE Tuple code
		0xdf, // IO device, no WPS, ext speed
		0x72, // 700ns if no wait
		0x01, // 2kB address space
		0xff, // End of tuple
	0x1c, 4, // CISTPL_DEVICE_OC Tuple code
		0x03, // 3V, WAIT is used
		0xd9, // I/O device, no WPS, 250ns with wait
		0x01, // 2kB address space
		0xff, // End of tuple
	0x18, 2, // CISTPL_JEDEC_C Tuple code
		0xdf, // SanDisk PC Card ATA
		0x01,
	0x20, 4, // CISTPL_MANFID Tuple code
		0x45, // SanDisk manufacturer ID
		0x00, //
		0x01, // SanDisk SDP series
		0x04, // SanDisk PC Card ATA
	0x2c, 23, // CISTPL_VERS_1 Tuple code
		0x04, // TPLLV1_MAJOR Tuple data
		0x01, // TPLLV1_MINOR Tuple data
		'S', 'u', 'n', 'D', 'i', 's', 'k', 0x00,
		'S', 'D', 'P', 0x00,
		'5', '/', '3', ' ', '0', '.', '6', 0x00,
		0xff, // End of tuple
	0x21, 2, // CISTPL_FUNCID
		0x04, // Disk Function
		0x01, // Install at POST
	0x22, 2, // CISTPL_FUNCE
		0x01, // Function extension data
		0x01, // PC-Card ATA
	0x22, 3, // CISTPL_FUNCE
		0x02, // Function extension data
		0x0C, // Res:4, U:1, S:1, V:2
		0x0F, // Drive auto power control
	0x1a, 5, // CISTPL_CONF
		0x01, // Size of reserved field
		0x00, // Last entry in configuration table
		0x00, 0x02, // location of config registers (0x0200 in reg space)
		0x0f, // TPCC_RMSK
	0x1b, 11, // CISTPL_CE
		0xc0, // Memory mapped IO, default configuration
		0xc0, // Memory only, no Bvd or wProt
		0xa1, // Additional configuration entry TPCE_FS
		0x27, // voltages
		0x55, 0x4d, 0x5d, 0x75, // Voltages
		0x08, 0x00, // length of memspace is 2kB
		0x21, // TPCE_MI
	0x14, 0, // CISTPL_NO_LINK
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // End of tuple
};
// clang-format on

// -------------------------------------------------------------------------- //
//  * TATACard( const char* )
// -------------------------------------------------------------------------- //
TATACard::TATACard(const char* inImagePath)
{
	mFilePath = strdup(inImagePath);

	// The file stays open, so that every sector can be written when it arrives.
	// If we can't write the image, we can still read it.
	mFile = fopen(inImagePath, "r+b");
	if (!mFile)
	{
		mFile = fopen(inImagePath, "rb");
		mReadOnly = true;
	}
	if (mFile)
	{
		std::error_code theError;
		mFileSize = std::filesystem::file_size(inImagePath, theError);
		if (theError)
			mFileSize = 0;
		uint64_t theImageSize = std::min(mFileSize, kMaxImageSize);
		theImageSize -= theImageSize % kSectorSize;
		mData.resize((size_t) theImageSize);
		size_t theRead = fread(mData.data(), 1, mData.size(), mFile);
		mData.resize(theRead - theRead % kSectorSize);
	}
	SetGeometry();
	PowerOn();
}

// -------------------------------------------------------------------------- //
//  * ~TATACard( void )
// -------------------------------------------------------------------------- //
TATACard::~TATACard(void)
{
	FlushImage();
	if (mFilePath)
	{
		::free(mFilePath);
		mFilePath = nullptr;
	}
	if (mFile)
	{
		fclose(mFile);
		mFile = nullptr;
	}
}

// -------------------------------------------------------------------------- //
//  * Init( TPCMCIAController* )
// -------------------------------------------------------------------------- //
int
TATACard::Init(TPCMCIAController* inController)
{
	// This sets the log, the emulator and the controller, IsInserted() needs it.
	int ret = TPCMCIACard::Init(inController);
	if (ret == -1)
		return ret;

	// The same card can be inserted again after it was removed.
	PowerOn();

	if (GetLog())
	{
		if (!mFile)
		{
			GetLog()->FLogLine("TATACard: can't open \"%s\"", mFilePath);
		} else
		{
			GetLog()->FLogLine("TATACard: \"%s\", %u sectors%s",
				mFilePath, (unsigned int) mSectors, mReadOnly ? ", read only" : "");
			if (mFileSize != mData.size())
			{
				GetLog()->FLogLine("TATACard: using %llu of %llu bytes of the image",
					(unsigned long long) mData.size(), (unsigned long long) mFileSize);
			}
		}
	}
	return 0;
}

// -------------------------------------------------------------------------- //
//  * Remove( void )
// -------------------------------------------------------------------------- //
void
TATACard::Remove()
{
	// Every sector was written when it arrived, but make sure it reached the disk.
	FlushImage();
	TPCMCIACard::Remove();
}

// -------------------------------------------------------------------------- //
//  * GetVPCPins( void )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::GetVPCPins(void)
{
	// A card that is ready and has a good battery, like TLinearCard. The
	// controller adds the card detect pins. If RDY/-BSY is low, the OS waits
	// for the card before it looks at it.
	KUInt32 thePins = TPCMCIAController::k1C00_Pin62_45 // BVD2, -SPKR, -DASP
		| TPCMCIAController::k1C00_Pin63_46; // BVD1, -STSCHG, -PDIAG
	if (mState != State::InReset)
		thePins |= TPCMCIAController::k1C00_Pin16_37; // RDY/-BSY
	return thePins;
}

// -------------------------------------------------------------------------- //
//  * SetVPCPins( KUInt32 )
// -------------------------------------------------------------------------- //
void
TATACard::SetVPCPins(KUInt32 inPins)
{
	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: SetVPCPins( %.4X )", (unsigned int) inPins);
	}
}

// -------------------------------------------------------------------------- //
//  * ReadAttr( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::ReadAttr(KUInt32 inOffset)
{
	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: unsupported ReadAttr( %.8X )", (unsigned int) inOffset);
	}
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadAttrB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadAttrB(KUInt32 inOffset)
{
	inOffset = (inOffset / 2) ^ 1; // byte addresses are 32bit-flipped

	if (inOffset < sizeof(kDefaultCISData))
		return kDefaultCISData[inOffset];

	switch (inOffset)
	{
		case 0x0100: // 0x0200 Configuration Option Register: SRESET:1, LevIREQ:1, Conf:6
			return mConfigOptionReg;
		case 0x0101: // 0x0202 Configuration and Status Register: Changed:1, SigChg:1, IOis8:1, 0:2, PwrDwn:1, Int:1, 0:1
			// Changed and Int are always 0, we have no pin changes and no interrupts.
			return mConfigStatusReg;
		case 0x0102: // 0x0204 Pin Replacement Register: 0:2, CRdy:1, CWProt:1, 1:2, RRdy:1, RWProt:1
			// RRdy is the ready pin. It is only low while the card is held in
			// reset, the busy states end when Status is read, and a host that
			// polls here must not wait for that. RWProt is the write protect
			// pin. The two "changed" bits are always 0.
			return kPinReplacement_AlwaysSet
				| (mState != State::InReset ? kPinReplacement_RRdy : 0)
				| (mReadOnly ? kPinReplacement_RWProt : 0);
		case 0x0103: // 0x0206 Socket and Copy Register: 0:3, Drive#:1, 0:4
			return 0; // Drive 0
	}
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIO( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::ReadIO(KUInt32 inOffset)
{
	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: unsupported ReadIO( %.8X )", (unsigned int) inOffset);
	}
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIOB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadIOB(KUInt32 inOffset)
{
	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: unsupported ReadIOB( %.8X )", (unsigned int) inOffset);
	}
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMem( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::ReadMem(KUInt32 inOffset)
{
	if (inOffset != 0)
	{
		if (GetLog())
		{
			GetLog()->FLogLine("TATACard: unsupported ReadMem( %.8X )", (unsigned int) inOffset);
		}
		return 0;
	}

	// The data register is 16 bits wide, the two bytes are in bits 31-16.
	KUInt32 b0 = ReadFifoByte();
	KUInt32 b1 = ReadFifoByte();
	return (b0 << 24) | (b1 << 16);
}

// -------------------------------------------------------------------------- //
//  * ReadMemB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadMemB(KUInt32 inOffset)
{
	inOffset = inOffset ^ 3; // byte addresses are 32bit-flipped

	// The data window is an alternative to registers 0, 8 and 9.
	if (inOffset >= 0x400 && inOffset < 0x800)
		return ReadFifoByte();

	switch (inOffset)
	{
		case 0x00: // Even data
		case 0x08: // Duplicate even data
		case 0x09: // Odd data
			return ReadFifoByte();
		case 0x01: // Error
		case 0x0D: // Duplicate error
			return mErrorReg;
		case 0x02:
			return mSectorCountReg;
		case 0x03:
			return mSectorNumberReg;
		case 0x04:
			return mCylinderLowReg;
		case 0x05:
			return mCylinderHighReg;
		case 0x06: // Drive/Head, bits 7 and 5 always read as 1
			return mDriveHeadReg | 0xA0;
		case 0x07: // Status
		case 0x0E: // Alt Status, the same value, there is no interrupt to clear
			return ReadStatus();
	}

	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: unhandled ReadMemB( %.8X )", (unsigned int) inOffset);
	}
	return 0;
}

// -------------------------------------------------------------------------- //
//  * WriteAttr( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteAttr(KUInt32 inOffset, KUInt32 inValue)
{
	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: unsupported WriteAttr( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue);
	}
}

// -------------------------------------------------------------------------- //
//  * WriteAttrB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteAttrB(KUInt32 inOffset, KUInt8 inValue)
{
	inOffset = (inOffset / 2) ^ 1; // byte addresses are 32bit-flipped

	switch (inOffset)
	{
		case 0x0100: { // 0x0200 Configuration Option Register: SRESET:1, LevIREQ:1, Conf:6
			bool wasInReset = IsInReset();
			mConfigOptionReg = inValue;
			UpdateReset(wasInReset);
			if ((inValue & kConfigOption_ConfMask) && GetLog())
			{
				GetLog()->FLogLine("TATACard: configuration %u is not supported, the card stays memory mapped",
					(unsigned int) (inValue & kConfigOption_ConfMask));
			}
			break;
		}
		case 0x0101: // 0x0202 Configuration and Status Register: 0:1, SigChg:1, IOis8:1, 0:2, PwrDwn:1, 0:2
			// The power down request has no effect.
			mConfigStatusReg = inValue & kConfigStatus_WriteMask;
			break;
		case 0x0102: // 0x0204 Pin Replacement Register, no pin changes to report or to mask
		case 0x0103: // 0x0206 Socket and Copy Register, only drive 0
			break;
		default:
			if (GetLog())
			{
				GetLog()->FLogLine("TATACard: unhandled WriteAttrB( %.8X, %.2X )",
					(unsigned int) inOffset,
					(unsigned int) inValue);
			}
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIO( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteIO(KUInt32 inOffset, KUInt32 inValue)
{
	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: unsupported WriteIO( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue);
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIOB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteIOB(KUInt32 inOffset, KUInt8 inValue)
{
	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: unsupported WriteIOB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue);
	}
}

// -------------------------------------------------------------------------- //
//  * WriteMem( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteMem(KUInt32 inOffset, KUInt32 inValue)
{
	if (inOffset != 0)
	{
		if (GetLog())
		{
			GetLog()->FLogLine("TATACard: unsupported WriteMem( %.8X, %.8X )",
				(unsigned int) inOffset,
				(unsigned int) inValue);
		}
		return;
	}

	// The data register is 16 bits wide, the two bytes are in bits 31-16.
	WriteFifoByte((inValue >> 24) & 0xFF);
	WriteFifoByte((inValue >> 16) & 0xFF);
}

// -------------------------------------------------------------------------- //
//  * WriteMemB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteMemB(KUInt32 inOffset, KUInt8 inValue)
{
	inOffset = inOffset ^ 3; // byte addresses are 32bit-flipped

	// The data window is an alternative to registers 0, 8 and 9.
	if (inOffset >= 0x400 && inOffset < 0x800)
	{
		WriteFifoByte(inValue);
		return;
	}

	switch (inOffset)
	{
		case 0x00: // Even data
		case 0x08: // Duplicate even data
		case 0x09: // Odd data
			WriteFifoByte(inValue);
			break;
		case 0x01: // Features
			mFeaturesReg = inValue;
			break;
		case 0x02:
			mSectorCountReg = inValue;
			break;
		case 0x03:
			mSectorNumberReg = inValue;
			break;
		case 0x04:
			mCylinderLowReg = inValue;
			break;
		case 0x05:
			mCylinderHighReg = inValue;
			break;
		case 0x06:
			mDriveHeadReg = inValue;
			break;
		case 0x07:
			mCommandReg = inValue;
			StartCommand(inValue);
			break;
		case 0x0E: { // Device Control, SRST resets the card as long as it is set
			bool wasInReset = IsInReset();
			mDeviceControlReg = inValue;
			UpdateReset(wasInReset);
			break;
		}
		default:
			if (GetLog())
			{
				GetLog()->FLogLine("TATACard: unhandled WriteMemB( %.8X, %.2X )",
					(unsigned int) inOffset,
					(unsigned int) inValue);
			}
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * ReadStatus( void )
// -------------------------------------------------------------------------- //
// The host ignores every status bit but BSY while BSY is set, so DRQ must only
// be reported together with BSY clear.
KUInt8
TATACard::ReadStatus(void)
{
	switch (mState)
	{
		case State::Idle:
			break;
		case State::NoDataBusy:
			mState = State::Idle;
			return kStatusReg_BSY;
		case State::DataReadBusy:
			mState = State::DataReadReady;
			return kStatusReg_BSY;
		case State::DataWriteBusy:
			mState = State::DataWriteReady;
			return kStatusReg_BSY;
		case State::DataReadReady:
		case State::DataWriteReady:
			return kStatusReg_RDY | kStatusReg_DSC | kStatusReg_DRQ;
		case State::Error:
			return kStatusReg_RDY | kStatusReg_DSC | kStatusReg_ERR;
		case State::WriteFault:
			return kStatusReg_RDY | kStatusReg_DWF | kStatusReg_DSC | kStatusReg_ERR;
		case State::InReset:
			return kStatusReg_BSY;
	}
	return kStatusReg_RDY | kStatusReg_DSC;
}

// -------------------------------------------------------------------------- //
//  * ReadFifoByte( void )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadFifoByte(void)
{
	if (mState != State::DataReadReady || mFifoPos >= mFifo.size())
		return 0;
	KUInt8 theResult = mFifo[mFifoPos++];
	if (mFifoPos >= mFifo.size())
	{
		if (mCommandReg == kReadSectorsCmd)
		{
			// One sector is done. The count was 0 for 256 sectors, so it wraps
			// to 255 and only reaches 0 again after the last one.
			--mSectorCountReg;
			if (mSectorCountReg == 0)
			{
				mState = State::Idle;
			} else
			{
				SetLBA(GetLBA() + 1);
				mState = BuildSectorData() ? State::DataReadBusy : State::Error;
			}
		} else
		{
			mState = State::Idle;
		}
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * WriteFifoByte( KUInt8 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteFifoByte(KUInt8 inByte)
{
	if (mState != State::DataWriteReady || mFifoPos >= mFifo.size())
		return;
	mFifo[mFifoPos++] = inByte;
	if (mFifoPos < mFifo.size())
		return;

	// A whole sector has arrived, count like ReadFifoByte() does.
	if (!WriteSectorData())
	{
		mState = State::WriteFault;
		return;
	}
	--mSectorCountReg;
	if (mSectorCountReg == 0)
	{
		mState = State::Idle;
	} else
	{
		SetLBA(GetLBA() + 1);
		if (IsSectorValid())
		{
			mFifoPos = 0;
			mState = State::DataWriteBusy;
		} else
		{
			mState = State::Error;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * SetIdentifyWord( std::vector<KUInt8>&, int, KUInt16 )
// -------------------------------------------------------------------------- //
static void
SetIdentifyWord(std::vector<KUInt8>& ioBuffer, int inWord, KUInt16 inValue)
{
	ioBuffer[inWord * 2] = inValue & 0xFF;
	ioBuffer[inWord * 2 + 1] = inValue >> 8;
}

// -------------------------------------------------------------------------- //
//  * SetIdentifyString( std::vector<KUInt8>&, int, int, const char* )
// -------------------------------------------------------------------------- //
// ATA strings are space padded and store the first character of each pair in
// the high byte of the word.
static void
SetIdentifyString(std::vector<KUInt8>& ioBuffer, int inFirstWord, int inNumWords, const char* inString)
{
	size_t theLength = strlen(inString);
	for (int i = 0; i < inNumWords; ++i)
	{
		char c0 = (size_t) (i * 2) < theLength ? inString[i * 2] : ' ';
		char c1 = (size_t) (i * 2 + 1) < theLength ? inString[i * 2 + 1] : ' ';
		SetIdentifyWord(ioBuffer, inFirstWord + i, (KUInt16) (((KUInt8) c0 << 8) | (KUInt8) c1));
	}
}

// -------------------------------------------------------------------------- //
//  * FileNameChecksum( const char* )
// -------------------------------------------------------------------------- //
// A few rotates and xors over the file name (without the directory). Every
// image gets its own serial number, and it stays the same when the image moves.
static KUInt32
FileNameChecksum(const char* inPath)
{
	std::string theName = std::filesystem::path(inPath ? inPath : "").filename().string();
	KUInt32 theSum = 0xa63e95f1;
	for (unsigned char c : theName)
		theSum = ((theSum << 5) | (theSum >> 27)) ^ c;
	return theSum;
}

// -------------------------------------------------------------------------- //
//  * BuildIdentifyData( void )
// -------------------------------------------------------------------------- //
void
TATACard::BuildIdentifyData(void)
{
	mFifo.assign(kSectorSize, 0);
	mFifoPos = 0;

	// What CHS addressing can reach, this may be a bit less than mSectors.
	KUInt32 theCHSSectors = (KUInt32) mCylinders * mHeads * mSectorsPerTrack;

	char theSerial[21];
	snprintf(theSerial, sizeof(theSerial), "%08X", (unsigned int) FileNameChecksum(mFilePath));
	char theModel[41];
	snprintf(theModel, sizeof(theModel), "Einstein ATA %.1fMB", mSectors / 2048.0); // size in MB

	SetIdentifyWord(mFifo, 0, 0x848A); // CompactFlash, removable, no fixed disk
	SetIdentifyWord(mFifo, 1, mCylinders);
	SetIdentifyWord(mFifo, 3, mHeads);
	SetIdentifyWord(mFifo, 5, 0x0240); // Unformatted bytes per sector
	SetIdentifyWord(mFifo, 6, mSectorsPerTrack);
	SetIdentifyString(mFifo, 10, 10, theSerial);
	SetIdentifyWord(mFifo, 20, 0x0002); // Buffer type
	SetIdentifyWord(mFifo, 21, 0x0002); // Buffer size (512)
	SetIdentifyWord(mFifo, 22, 0x0004); // Number of ECC bytes on read/write long
	SetIdentifyString(mFifo, 23, 4, "1.0"); // Firmware revision
	SetIdentifyString(mFifo, 27, 20, theModel);
	SetIdentifyWord(mFifo, 47, 0x0001); // Max sectors per READ/WRITE MULTIPLE
	SetIdentifyWord(mFifo, 49, 0x0200); // LBA supported, DMA not supported
	SetIdentifyWord(mFifo, 51, 0x0100); // PIO timing
	SetIdentifyWord(mFifo, 53, 0x0001); // Words 54-58 are valid
	SetIdentifyWord(mFifo, 54, mCylinders);
	SetIdentifyWord(mFifo, 55, mHeads);
	SetIdentifyWord(mFifo, 56, mSectorsPerTrack);
	SetIdentifyWord(mFifo, 57, theCHSSectors & 0xFFFF); // Current capacity in sectors
	SetIdentifyWord(mFifo, 58, theCHSSectors >> 16);
	SetIdentifyWord(mFifo, 59, 0x0000); // Multiple sector setting is not valid
	SetIdentifyWord(mFifo, 60, mSectors & 0xFFFF); // Total number of sectors (LBA)
	SetIdentifyWord(mFifo, 61, mSectors >> 16);
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TATACard::PowerOn(void)
{
	mDeviceControlReg = kDeviceControl_nIEN;
	mConfigOptionReg = 0;
	mConfigStatusReg = kConfigStatus_IOis8;
	Reset();
}

// -------------------------------------------------------------------------- //
//  * Reset( void )
// -------------------------------------------------------------------------- //
void
TATACard::Reset(void)
{
	mState = State::Idle;
	mFifo.clear();
	mFifoPos = 0;

	// The task file holds the ATA signature after a reset.
	mErrorReg = kErrorReg_OK;
	mFeaturesReg = 0;
	mSectorCountReg = 1;
	mSectorNumberReg = 1;
	mCylinderLowReg = 0;
	mCylinderHighReg = 0;
	mDriveHeadReg = 0;
	mCommandReg = 0;
}

// -------------------------------------------------------------------------- //
//  * IsInReset( void )
// -------------------------------------------------------------------------- //
bool
TATACard::IsInReset(void) const
{
	return (mConfigOptionReg & kConfigOption_SRESET) || (mDeviceControlReg & kDeviceControl_SRST);
}

// -------------------------------------------------------------------------- //
//  * UpdateReset( bool )
// -------------------------------------------------------------------------- //
void
TATACard::UpdateReset(bool inWasInReset)
{
	bool isInReset = IsInReset();
	if (isInReset && !inWasInReset)
	{
		if (GetLog())
		{
			GetLog()->LogLine("TATACard: reset");
		}
		Reset();
		mState = State::InReset;
	} else if (!isInReset && inWasInReset)
	{
		// Busy for a moment, then ready. The host can then read the
		// diagnostic result from the Error register.
		mState = State::NoDataBusy;
	}
}

// -------------------------------------------------------------------------- //
//  * SetGeometry( void )
// -------------------------------------------------------------------------- //
void
TATACard::SetGeometry(void)
{
	// Card sizes come in multiples of 128kB (256 sectors), so this usually
	// divides evenly. Start with 2 heads and 32 sectors per track (the 5MB
	// SDP5A-5 has 160 cylinders), and add heads until there are at most 1024
	// cylinders, like a PC BIOS would want. That makes 8 heads at 128MB.
	mSectors = (KUInt32) (mData.size() / kSectorSize);
	mHeads = 2;
	mSectorsPerTrack = 32;
	while (mSectors / (mHeads * mSectorsPerTrack) > 1024 && mHeads < 16)
		mHeads *= 2;
	mCylinders = mSectors / (mHeads * mSectorsPerTrack);
	if (mCylinders == 0)
	{
		// Less than a cylinder: one head, and one short track holds it all.
		// This also avoids dividing by zero if there is no image at all.
		mHeads = 1;
		mSectorsPerTrack = mSectors == 0 ? 1 : mSectors;
		mCylinders = mSectors == 0 ? 0 : 1;
	}
}

// -------------------------------------------------------------------------- //
//  * GetLBA( void )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::GetLBA(void)
{
	KUInt32 theHead = mDriveHeadReg & 0x0F;
	if (mDriveHeadReg & 0x40)
	{
		// LBA mode
		return (theHead << 24) | (mCylinderHighReg << 16) | (mCylinderLowReg << 8) | mSectorNumberReg;
	}
	// CHS mode, sector numbers start at 1
	if (mSectorNumberReg == 0 || mSectorNumberReg > mSectorsPerTrack || theHead >= mHeads)
		return 0xFFFFFFFF;
	KUInt32 theCylinder = (mCylinderHighReg << 8) | mCylinderLowReg;
	return (theCylinder * mHeads + theHead) * mSectorsPerTrack + (mSectorNumberReg - 1);
}

// -------------------------------------------------------------------------- //
//  * SetLBA( KUInt32 )
// -------------------------------------------------------------------------- //
void
TATACard::SetLBA(KUInt32 inLBA)
{
	if (mDriveHeadReg & 0x40)
	{
		// LBA mode
		mSectorNumberReg = inLBA & 0xFF;
		mCylinderLowReg = (inLBA >> 8) & 0xFF;
		mCylinderHighReg = (inLBA >> 16) & 0xFF;
		mDriveHeadReg = (mDriveHeadReg & 0xF0) | ((inLBA >> 24) & 0x0F);
	} else
	{
		// CHS mode
		KUInt32 theCylinder = inLBA / (mSectorsPerTrack * mHeads);
		mSectorNumberReg = inLBA % mSectorsPerTrack + 1;
		mCylinderLowReg = theCylinder & 0xFF;
		mCylinderHighReg = (theCylinder >> 8) & 0xFF;
		mDriveHeadReg = (mDriveHeadReg & 0xF0) | ((inLBA / mSectorsPerTrack) % mHeads);
	}
}

// -------------------------------------------------------------------------- //
//  * IsSectorValid( void )
// -------------------------------------------------------------------------- //
bool
TATACard::IsSectorValid(void)
{
	uint64_t theOffset = (uint64_t) GetLBA() * kSectorSize;
	if (theOffset + kSectorSize > mData.size())
	{
		mErrorReg = kErrorReg_IDNF;
		if (GetLog())
		{
			GetLog()->FLogLine("TATACard: sector %u is not in the image", (unsigned int) GetLBA());
		}
		return false;
	}
	return true;
}

// -------------------------------------------------------------------------- //
//  * BuildSectorData( void )
// -------------------------------------------------------------------------- //
bool
TATACard::BuildSectorData(void)
{
	mFifo.assign(kSectorSize, 0);
	mFifoPos = 0;

	if (!IsSectorValid())
		return false;
	uint64_t theOffset = (uint64_t) GetLBA() * kSectorSize;
	std::copy_n(mData.begin() + (size_t) theOffset, kSectorSize, mFifo.begin());
	return true;
}

// -------------------------------------------------------------------------- //
//  * WriteSectorData( void )
// -------------------------------------------------------------------------- //
bool
TATACard::WriteSectorData(void)
{
	uint64_t theOffset = (uint64_t) GetLBA() * kSectorSize;
	// The file first: if that fails, memory and file must not differ.
	if (!mFile || mReadOnly
		|| fseek(mFile, (long) theOffset, SEEK_SET) != 0
		|| fwrite(mFifo.data(), 1, kSectorSize, mFile) != kSectorSize
		|| fflush(mFile) != 0)
	{
		mErrorReg = kErrorReg_ABRT;
		if (GetLog())
		{
			GetLog()->FLogLine("TATACard: can't write sector %u", (unsigned int) GetLBA());
		}
		return false;
	}
	std::copy_n(mFifo.begin(), kSectorSize, mData.begin() + (size_t) theOffset);
	return true;
}

// -------------------------------------------------------------------------- //
//  * FlushImage( void )
// -------------------------------------------------------------------------- //
void
TATACard::FlushImage(void)
{
	if (!mFile || mReadOnly)
		return;
	fflush(mFile);
#if defined(_WIN32)
	_commit(_fileno(mFile));
#else
	fsync(fileno(mFile));
#endif
}

// -------------------------------------------------------------------------- //
//  * VerifySectors( void )
// -------------------------------------------------------------------------- //
void
TATACard::VerifySectors(void)
{
	// Count like ReadFifoByte() does. On success the LBA registers point to the
	// last sector, on error to the one that failed.
	for (;;)
	{
		if (!IsSectorValid())
		{
			mState = State::Error;
			return;
		}
		--mSectorCountReg;
		if (mSectorCountReg == 0)
			break;
		SetLBA(GetLBA() + 1);
	}
	mState = State::NoDataBusy;
}

// -------------------------------------------------------------------------- //
//  * StartCommand( KUInt8 )
// -------------------------------------------------------------------------- //
void
TATACard::StartCommand(KUInt8 inCommand)
{
	if (mState == State::InReset)
	{
		// The card is busy resetting and does not look at the command register.
		if (GetLog())
		{
			GetLog()->FLogLine("TATACard: command 0x%.2X ignored, the card is in reset", (unsigned int) inCommand);
		}
		return;
	}

	if (GetLog())
	{
		GetLog()->FLogLine("TATACard: command 0x%.2X, sector %u, count %u",
			(unsigned int) inCommand, (unsigned int) GetLBA(), (unsigned int) mSectorCountReg);
	}

	mErrorReg = 0;
	switch (inCommand)
	{
		case kIdentifyDriveCmd:
			BuildIdentifyData();
			mState = State::DataReadBusy;
			break;
		case kReadSectorsCmd:
			// The following sectors are loaded by ReadFifoByte().
			mState = BuildSectorData() ? State::DataReadBusy : State::Error;
			break;
		case kReadVerifySectorsCmd:
			VerifySectors();
			break;
		case kWriteSectorsCmd:
			// The sectors are written by WriteFifoByte().
			if (mReadOnly)
			{
				mErrorReg = kErrorReg_ABRT;
				mState = State::WriteFault;
				if (GetLog())
				{
					GetLog()->LogLine("TATACard: the image is read only");
				}
			} else if (!IsSectorValid())
			{
				mState = State::Error;
			} else
			{
				mFifo.assign(kSectorSize, 0);
				mFifoPos = 0;
				mState = State::DataWriteBusy;
			}
			break;
		case kSetFeaturesCmd:
			// 8/16 bit transfers work through ReadMemB()/ReadMem(), and the
			// other features (write cache, power management, ...) mean nothing
			// here. Accept them all, the feature number is in mFeaturesReg.
			mState = State::NoDataBusy;
			break;
		case kStandbyImmediateCmd:
		case kStandbyImmediateAltCmd:
			// Every sector is already written to the file, but make sure it
			// reached the disk.
			FlushImage();
			mState = State::NoDataBusy;
			break;
		default:
			// Abort, or the host would poll forever.
			mErrorReg = kErrorReg_ABRT;
			mState = State::Error;
			if (GetLog())
			{
				GetLog()->FLogLine("TATACard: unknown command 0x%.2X", (unsigned int) inCommand);
			}
			break;
	}
}

// =================================================== //
// There's got to be more to life than compile-and-go. //
// =================================================== //

