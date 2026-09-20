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

// Implementation based on the "SanDisk CompactFlash Memory Card Product Manual"
// from 1999. It covers SanDisk compact flash cards from SDCFB-4 to SDCFB-96.

/*

 ATA Cards store data via a disk controller in a conventional file system.
 Newton needs data to be executable in place. To solve this Paul Guyot's ATA
 driver reserves some space and maps the currently needed parts of an ATA card
 into this memory.

 ATA cards are written in a RAW format by the driver. The first 0x0200 bytes
 start with 'Newt' and hold the Newton store partition map. One ATA card can hold
 multiple store volumes.

 Partition Map:
 		ULong				fSignature;				// 'Newt' FourCC
		ULong				fVersion;				// 3
		ULong				fNumberOfSectorsInPartitionMap; // 1
		ULong				fIndexOfThisSector;
		ULong				fTotalNumberOfEntries;
		UShort				fNumberOfEntriesInThisSector;
		UShort				fReserved_01;
		ULong				fReserved_02;
		ULong				fReserved_03;

 SNSCPartitionEntry
		UShort				fType;
		UShort				fFlags;
		ULong				fStartSector;
		ULong				fSize;
		ULong				fReserved;

 The first store entry starts at 0x0200 in our sample with the 'Stor' FourCC.
 So a sector is 0x200 bytes = 512 bytes?

 SHeader
	ULong	fSignature;						///< doit �tre 'Stor' (53746F72)
	ULong	fVersion;						///< 4: cf plus bas.
	ULong	fLength;						///< 0x27ff (*512 = 5MB): en secteur, total
	ULong	fMapFirstSector;				///< 1: d�but de la carte, depuis le
											///< d�but du magasin (normalement,
											///< 1)
	ULong	fTransactionTableFirstSector;	///< 0x16: D�but de la carte des
											///< transactions.
	ULong	fTranslationTableFirstSector;	///< 0: D�but de la carte des
											///< traduction.
	ULong	fSeparateTranTableFirstSector;	///< 0x17: D�but de la carte des
											///< traduction.
	ULong	fRootID;						///< 0x300: Root Object ID
	UShort	fFlags;							///< 0: drapeaux (magasin unique sur
											///< une partition seulement).
	UShort	fUnused; 						///< 0
	ULong	fPoolSize;						///< 0x1ff: nombre de secteurs r�serv�s
											///< (� la fin).

 So reading an ATA card needs to go through translation tables to retreive the
 soups as they are stored. Converting that into a PCMCIA card layout is
 non-trivial. We do not have information or source code for the Linear Card
 driver.

 Alternatively, the interface in this file may be enough if it implements
 the ATA commands that the driver sends via PCMCIA adresses and translate
 those into raw file reads and writes. The original ATA driver would then
 do the rest. (So basically and umulation of an ATA device inside a driver that
 emulates a linear card inside a Newton emulator)

 http://www.kallisys.com/files/newton/ATA/ATA-Support-1.0-SourceCode.img.bin

*/

#include "TATACard.h"

// Einstein
#include "Emulator/Log/TLog.h"
#include "TPCMCIAController.h"

#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
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

// Larger images are clipped. The rest of the file is left alone, but can't be reached.
static const uint64_t kMaxImageSize = 128ULL * 1024 * 1024;

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
		0xdf, // SanDIsk PC Card ATA
		0x01,
	0x20, 4, // CISTPL_MANFID Tuple code
		0x45, // SanDisk manufacturer ID
		0x00, //
		0x01, // SanDsik SDP deries
		0x04, // SanDisk PC Card ATA
	0x2c, 23, // CISTPL_VERS_1 Tuple code
		0x04, // TPLLV1_MAJOR Tuple data
		0x01, // TPLLV1_MINOR Tuple data
		'S', 'u', 'n', 'D', 'i', 's', 'k', 0x00,
		'S', 'D', 'P', 0x00,
		'5', '/', '3', ' ', '0', '.', '6', 0x00,
		0xff, // End of tuple
	// 0x80, 3, ... // Vendor specific tuple code
	0x21, 2, // CISTPL_FUNCID
		0x04, // Disk Function
		0x01, // Install at POST
	0x22, 2, // CISTPL_FUNCE
		0x01, // Function extension data
		0x01, // PC-Car ATA
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

// -------------------------------------------------------------------------- //
//  * TATACard( KUInt32 )
// -------------------------------------------------------------------------- //
TATACard::TATACard(const char* inImagePath)
{
	mFilePath = strdup(inImagePath);

	// Keep the file open, so we can write every sector as soon as it arrives.
	// A crash of the emulator can then not lose data.
	mFile = fopen(inImagePath, "r+b");
	if (!mFile)
	{
		mFile = fopen(inImagePath, "rb");
		mReadOnly = true;
	}
	if (mFile)
	{
		std::error_code theError;
		uint64_t fileSize = std::filesystem::file_size(inImagePath, theError);
		if (theError)
			fileSize = 0;
		uint64_t imageSize = std::min(fileSize, kMaxImageSize);
		imageSize -= imageSize % kSectorSize;
		if (imageSize != fileSize)
		{
			fprintf(stderr, "TATACard: using only the first %llu of %llu bytes of the image\n",
				(unsigned long long) imageSize, (unsigned long long) fileSize);
			if (GetLog())
			{
				GetLog()->FLogLine("TATACard: using only the first %llu of %llu bytes of the image",
					(unsigned long long) imageSize, (unsigned long long) fileSize);
			}
		}
		mData.resize((size_t) imageSize);
		size_t theRead = fread(mData.data(), 1, mData.size(), mFile);
		mData.resize(theRead - theRead % kSectorSize);
	}
	SetGeometry();
	Reset();
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

int
TATACard::Init(TPCMCIAController* inController)
{
	// This sets the log, the emulator and the controller, and IsInserted()
	// depends on it. TLinearCard does the same.
	int ret = TPCMCIACard::Init(inController);
	if (ret == -1)
		return ret;

	// Inserting a card powers it up, and the same card object may be inserted
	// again after it was removed: start over like a new card.
	mConfigOptionReg = 0;
	mConfigStatusReg = 0x20;
	mDeviceControlReg = 0x02;
	Reset();

	if (GetLog())
	{
		GetLog()->LogLine("Init");
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
	if (GetLog())
	{
		GetLog()->LogLine("GetVPCPins");
	}

	// The pins of a card that is ready and has a good battery, like TLinearCard.
	// (The controller adds the card detect pins.) If RDY/-BSY is low, the OS
	// waits for the card to become ready before it looks at it.
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
		GetLog()->FLogLine("SetVPCPins( %.4X )", (unsigned int) inPins);
	}
}

// -------------------------------------------------------------------------- //
//  * ReadAttr( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::ReadAttr(KUInt32 inOffset)
{
	fprintf(stderr, "ReadAttr( %.8X )\n", (unsigned int) inOffset);
	if (GetLog())
	{
		GetLog()->FLogLine("ReadAttr( %.8X )", (unsigned int) inOffset);
	}

	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadAttrB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadAttrB(KUInt32 inOffset)
{
	KUInt8 theResult = 0;

	inOffset = (inOffset / 2) ^ 1; // byte addresses are 32bit-flipped

	if (inOffset < sizeof(kDefaultCISData))	{
		theResult = kDefaultCISData[inOffset];
	} else if (inOffset == 0x0100) { // 0x0200
		// 200h: Configuration Option Register
		// Bit 7: 	SRESET
		// Bit 6: 	LevIREQ
		// Bit 5-0: Conf
		// Set SRESET to 1 sets card in reset state
		// LevIREQ: 1 = Level Mode Interrupt, 0 = Pulse Mode Interrupt
		// Conf: 0 = memory mapped (default)
		theResult = mConfigOptionReg;
	} else if (inOffset == 0x0101) { // 0x0202
		// 202h: Configuration Option Register 2
		// Read:
		// Bit 7: changed
		// Bit 6: SigChg
		// Bit 5: IOis8
		// Bit 4: 0
		// Bit 3: 0
		// Bit 2: PwrDwn
		// Bit 1: Int
		// Bit 0: 0
		// Write:
		// Bit 7: 0
		// Bit 6: SigChg
		// Bit 5: IOis8
		// Bit 4: 0
		// Bit 3: 0
		// Bit 2: PwrDwn
		// Bit 1: 0
		// Bit 0: 0
		// Changed (bit 7) and Int (bit 1) are always 0: we have no pin changes
		// and raise no interrupts.
		theResult = mConfigStatusReg;
	} else if (inOffset == 0x0102) { // 0x0204
		// 204h: Pin Replcemant Register
		// Read:
		// Bit 7: 0
		// Bit 6: 0
		// Bit 5: CRdy/~Bsy
		// Bit 4: CWProt
		// Bit 3: 1
		// Bit 2: 1
		// Bit 1: RRdy/~Bsy
		// Bit 0: RWProt
		// Write:
		// Bit 7: 0
		// Bit 6: 0
		// Bit 5: CRdy/~Bsy
		// Bit 4: CWProt
		// Bit 3: 0
		// Bit 2: 0
		// Bit 1: MRdy/~Bsy
		// Bit 0: MWProt
		// Bits 3 and 2 always read as 1. RRdy/~Bsy is the state of the ready
		// pin. It is only low while the card is held in reset: the busy states
		// end when Status is read, so a host polling here must not see them.
		// RWProt is the write protect pin, set for an image that can't be
		// written. The two "changed" bits are always 0.
		theResult = 0x0C;
		if (mState != State::InReset)
			theResult |= 0x02;
		if (mReadOnly)
			theResult |= 0x01;
	} else if (inOffset == 0x0103) { // 0x0206
		// 206h: Socket and Copy register
		// Read: 0, 0, 0, Drive#:1, 0, 0, 0, 0
		theResult = 0x00;
	} else {
		theResult = 0;
	}

	fprintf(stderr, "ReadAttrB( %.8X ) -> %02X (%c)\n",
		(unsigned int) inOffset,
		(unsigned int) theResult,
		isprint(theResult) ? theResult : '.');

	if (GetLog())
	{
		GetLog()->FLogLine("ReadAttrB( %.8X )", (unsigned int) inOffset);
	}

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * ReadIO( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::ReadIO(KUInt32 inOffset)
{
	fprintf(stderr, "ReadIO( %.8X )\n", (unsigned int) inOffset);
	if (GetLog())
	{
		GetLog()->FLogLine("ReadIO( %.8X )", (unsigned int) inOffset);
	}

	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIOB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadIOB(KUInt32 inOffset)
{
	fprintf(stderr, "ReadIOB( %.8X )\n", (unsigned int) inOffset);
	if (GetLog())
	{
		GetLog()->FLogLine("ReadIOB( %.8X )", (unsigned int) inOffset);
	}

	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMem( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::ReadMem(KUInt32 inOffset)
{
	static int cnt = 0;

	KUInt32 theResult = 0;

	if (inOffset == 0) {
		// The actual mapping is 16bits wide.
		KUInt8 b0 = ReadFifoByte();
		KUInt8 b1 = ReadFifoByte();
		// Position the bytes correctly for a 32-bit read from a 16-bit wide mapping.
		theResult = (b0 << 24) | (b1 << 16);
	}


	fprintf(stderr, "ReadMem( %.8X ) -> 0x%08x, cnt=%d\n", (unsigned int) inOffset, theResult, cnt++);
	if (GetLog())
	{
		GetLog()->FLogLine("ReadMem( %.8X )", (unsigned int) inOffset);
	}

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * ReadMemB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadMemB(KUInt32 inOffset)
{
	// Memory mapped card 0x0000-0x000f and 0x0400-0x07ff
	//
	// -REG
	// | A10          Offset
	// | | A9-A4      |
	// | | | A3/2/1/0 | Read                Write
	// | | | |        | |                   |
	// 1 0 X 0 0 0 0  0 Even RD Data		Even WR Data 1
	// 1 0 X 0 0 0 1  1 Error				Features 2
	// 1 0 X 0 0 1 0  2 Sector Count 		Sector Count		0=256, counts down, will be zero when command was successful
	// 1 0 X 0 0 1 1  3 Sector No. 			Sector No.			LBA 7-0
	// 1 0 X 0 1 0 0  4 Cylinder Low 		Cylinder Low		LBA 15-8
	// 1 0 X 0 1 0 1  5 Cylinder High 		Cylinder High		LBA 23-16
	// 1 0 X 0 1 1 0  6 Select Card /Head 	Select Card/Head	LBA 27-24
	// 1 0 X 0 1 1 1  7 Status 				Command				Reading clears pending interrupt
	// 1 0 X 1 0 0 0  8 Dup. Even RD Data	Dup. Even WR Data 2
	// 1 0 X 1 0 0 1  9 Dup. Odd RD Data	Dup. Odd WR Data 2
	// 1 0 X 1 0 1 0  A
	// 1 0 X 1 0 1 1  B
	// 1 0 X 1 1 0 0  C
	// 1 0 X 1 1 0 1  D Dup. Error			Dup. Features 2
	// 1 0 X 1 1 1 0  E Alt Status 			Device Ctl
	// 1 0 X 1 1 1 1  F Drive Address 		Reserved			Don;t use.
	//
	// 1 1 X X X X 0  8 Even RD Data 		Even WR Data 3
	// 1 1 X X X X 1  9 Odd RD Data 		Odd WR Data 3
	// 	UChar	fUnused[0x400];

	// [1] Error Register Bits:
	// Bit 7 (BBK) This bit is set when a Bad Block is detected.
	// Bit 6 (UNC) This bit is set when an Uncorrectable Error is encountered.
	// Bit 5 This bit is 0.
	// Bit 4 (IDNF) The requested sector ID is in error or cannot be found.
	// Bit 3 This bit is 0.
	// Bit 2 (Abort) This bit is set if the command has been aborted because of a CompactFlash Memory Card status
	// condition: (Not Ready, Write Fault, etc.) or when an invalid command has been issued.
	// Bit 1 This bit is 0.
	// Bit 0 (AMNF) This bit is set in case of a general error.

	// [6] Drive Head register:
	// 	  Bit 5 Bit 4 (DRV) Bit 3 (HS3) Bit 2 (HS2) Bit 1 (HS1) Bit 0 (HS0)
	// Bit 7: This bit is set to 1.
	// Bit 6: LBA is a flag to select either Cylinder/Head/Sector (CHS) or
	// 		Logical Block Address Mode (LBA).
	// 		When LBA=0, Cylinder/Head/Sector mode is selected.
	// 		When LBA=1, Logical Block Address is selected. In Logical Block
	// 		Mode, the Logical Block Address is interpreted as follows:
	// 		LBA07-LBA00: Sector Number Register D7-D0.
	// 		LBA15-LBA08: Cylinder Low Register D7-D0.
	// 		LBA23-LBA16: Cylinder High Register D7-D0.
	// LBA27-LBA24: Drive/Head Register bits HS3-HS0.
	// Bit 5: This bit is set to 1.
	// Bit 4: DRV is the drive number. When DRV=0, drive (card) 0 is selected
	// 		When DRV=1, drive (card) 1 is selected. The CompactFlash Card is
	// 		set to be Card 0 or 1 using the copy field of the PCMCIA Socket &
	// 		Copy configuration register.
	// Bit 3-0: HS3-HS0 In LBA mode, this is bit 27-24 of the Logical Block Address.
	//		Else this is the head number.

	// [7] Status & Alternate Status Registers
	// Bit 7 (BUSY) The busy bit is set when the CompactFlash Memory Card has
	//		access to the command buffer and registers and the host is locked
	//		out from accessing the command register and buffer. No other bits
	//		in this register are valid when this bit is set to a 1.
	// Bit 6 (RDY) RDY indicates whether the device is capable of performing
	//		CompactFlash Memory Card operations. This bit is cleared at power
	//		up and remains cleared until the CompactFlash Card is ready to accept
	//		a command.
	// Bit 5 (DWF) This bit, if set, indicates a write fault has occurred.
	// Bit 4 (DSC) This bit is set when the CompactFlash Memory Card is ready.
	// Bit 3 (DRQ) The Data Request is set when the CompactFlash Memory Card
	// 		requires that information be transferred either to or from the host
	//		through the Data register.
	// Bit 2 (CORR) This bit is set when a Correctable data error has been
	//		encountered and the data has been corrected. This condition does not
	//		terminate a multi-sector read operation.
	// Bit 1 (IDX) This bit is always set to 0.
	// Bit 0 (ERR) This bit is set when the previous command has ended in some
	//		type of error. The bits in the Error register contain additional
	//		information describing the error.

	// [0e] Device Control Register
	// Bit 7-4 Bit This bit is an X (don't care).
	// Bit 3 (1) This bit is ignored by the CompactFlash Memory Card.
	// Bit 2 (SW Rst) This bit is set to 1 in order to force the CompactFlash
	//		Memory Card to perform an AT Disk controller Soft Reset operation.
	// Bit 1 (-IEn) The Interrupt Enable bit enables interrupts when the bit
	//		is 0. When the bit is 1, interrupts from the CompactFlash Memory
	//		Card are disabled. This bit also controls the Int bit in the
	//		Configuration and Status Register. This bit is set to 1 at power
	//		on and Reset.
	// Bit 0 (0) This bit is ignored by the CompactFlash Memory Card.

	// Start a command by writing the command register
	// Class 1: set BUSY within 400ns
	// Class 2: set Busy within 400ns, set DRQ within 700us, clear BUSY within 400ns
	// Class 3: set Busy within 400ns, set DRQ within 20ms, clear BUSY within 400ns

// Model Number: 				SDP5A-5
// Capacity:					5,242,880 bytes
// Sectors/Card (Max LBA+1):	10,240
// No. of Heads:				2
// No. of Sectors/Track:		32
// No. of Cylinders:			160

	// WriteAttrB( 00000103, 00 )
	// WriteAttrB( 00000100, 40 )
	// ReadAttrB( 00000102 ) -> 0E (.)
	// WriteMemB( 0000000B, 0A )
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000006 ) -> A0 (.)
	// WriteMemB( 00000001, 00 )
	// WriteMemB( 00000002, 00 )
	// WriteMemB( 00000003, 00 )
	// WriteMemB( 00000004, 00 )
	// WriteMemB( 00000005, 00 )
	// WriteMemB( 00000006, 00 )
	// WriteMemB( 00000007, EC )		Identify Drive Class 1, DH=Drive
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000007 ) -> 50 (P)
	// ReadMemB( 00000001 ) -> 00 (.)
	// ReadMemB( 00000002 ) -> 00 (.)
	// ReadMemB( 00000003 ) -> 00 (.)
	// ReadMemB( 00000004 ) -> 00 (.)
	// ReadMemB( 00000005 ) -> 00 (.)
	// ReadMemB( 00000006 ) -> A0 (.)
	// ReadMemB( 00000007 ) -> 50 (P)
	// WriteAttrB( 00000100, 80 )
	// WriteAttrB( 00000100, 00 )


	KUInt8 theResult = 0;

	inOffset = (inOffset) ^ 3; // byte addresses are 32bit-flipped
	if (inOffset >= 0x400 && inOffset < 0x800) {
		// Data window, the host may use it instead of registers 0, 8 and 9.
		theResult = ReadFifoByte();
	} else switch (inOffset) {
		default:
			theResult = 0x00;
			break;
		case 0: // Even data
		case 8: // Duplicate even data
		case 9: // Odd data
			theResult = ReadFifoByte();
			break;
		case 1: // Error (Features when written)
		case 0x0D: // Duplicate Error
			theResult = mErrorReg;
			break;
		case 2:
			theResult = mSectorCountReg;
			break;
		case 3:
			theResult = mSectorNumberReg;
			break;
		case 4:
			theResult = mCylinderLowReg;
			break;
		case 5:
			theResult = mCylinderHighReg;
			break;
		case 6: // Drive/Head, bits 7 and 5 always read as 1
			theResult = mDriveHeadReg | 0xA0;
			break;
		case 7: // Status
		case 0x0E: // Alt Status, same value; nothing to clear as we raise no interrupts
			theResult = ReadStatus();
			break;
	}
	// if (inOffset < mData.size())
	// {
	// 	theResult = mData[inOffset];
	// } else
	// {
	// 	theResult = 0;
	// }

	fprintf(stderr, "ReadMemB( %.8X ) -> %02X (%c)\n",
		(unsigned int) inOffset,
		(unsigned int) theResult,
		isprint(theResult) ? theResult : '.');
	if (GetLog())
	{
		GetLog()->FLogLine("ReadMemB( %.8X )", (unsigned int) inOffset);
	}

	return theResult;
}



// -------------------------------------------------------------------------- //
//  * WriteAttr( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteAttr(KUInt32 inOffset, KUInt32 inValue)
{
	fprintf(stderr, "WriteAttr( %.8X, %.8X )\n", (unsigned int) inOffset, (unsigned int) inValue);
	if (GetLog())
	{
		GetLog()->FLogLine("WriteAttr( %.8X, %.8X )",
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

	fprintf(stderr, "WriteAttrB( %.8X, %.2X )\n", (unsigned int) inOffset, (unsigned int) inValue);

	if (inOffset == 0x0100) { // 0x0200
		// 200h: Configuration Option Register
		// SRESET:1, LevIREQ:1, Conf:6
		// Set SRESET to 1 sets card in reset state
		// LevIREQ: 1 = Level Mode Interrupt, 0 = Pulse Mode Interrupt
		// Conf: 0 = memory mapped (default)
		bool wasInReset = IsInReset();
		mConfigOptionReg = inValue;
		if (inValue & 0x3F) {
			fprintf(stderr, "WriteAttrB: configuration %d is not supported, the card stays memory mapped\n",
				(int) (inValue & 0x3F));
		}
		UpdateReset(wasInReset);
	} else if (inOffset == 0x0101) { // 0x0202
		// 202h: Configuration Option Register 2
		// Read: changed:1, SigChg:1, IOis8:1, 0, 0, PwrDwn:1, Int:1, 0
		// Write: 0, SigChg:1, IOis8:1, 0, 0, PwrDwn:1, 0, 0
		// The power down request has no effect here.
		mConfigStatusReg = inValue & 0x64;
	} else if (inOffset == 0x0102) { // 0x0204
		// 204h: Pin Replcemant Register
		// Read: 0, 0, CRdy/~Bsy:1, CWProt:1, 1, 1, RRdy/~Bsy:1, RWProt:1
		// Write: 0, 0, CRdy/~Bsy:1, CWProt:1, 0, 0, MRdy/~Bsy:1, MWProt:1
		// Nothing to do, there are no pin changes to report or to mask.
	} else if (inOffset == 0x0103) { // 0x0206
		// 206h: Socket and Copy register
		// Read/Write: 0, 0, 0, Drive#:1, 0, 0, 0, 0
	} else {
	}

	if (GetLog())
	{
		GetLog()->FLogLine("WriteAttrB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue);
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIO( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteIO(KUInt32 inOffset, KUInt32 inValue)
{
	fprintf(stderr, "WriteIO( %.8X, %.8X )\n", (unsigned int) inOffset, (unsigned int) inValue);
	if (GetLog())
	{
		GetLog()->FLogLine("WriteIO( %.8X, %.8X )",
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
	fprintf(stderr, "WriteIOB( %.8X, %.2X )\n", (unsigned int) inOffset, (unsigned int) inValue);
	if (GetLog())
	{
		GetLog()->FLogLine("WriteIOB( %.8X, %.2X )",
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
	fprintf(stderr, "WriteMem( %.8X, %.8X )\n", (unsigned int) inOffset, (unsigned int) inValue);

	if (inOffset == 0) {
		// The actual mapping is 16bits wide, see ReadMem()
		WriteFifoByte((inValue >> 24) & 0xFF);
		WriteFifoByte((inValue >> 16) & 0xFF);
	}
	if (GetLog())
	{
		GetLog()->FLogLine("WriteMem( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue);
	}
}

// -------------------------------------------------------------------------- //
//  * WriteMemB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TATACard::WriteMemB(KUInt32 inOffset, KUInt8 inValue)
{
	inOffset = (inOffset) ^ 3; // byte addresses are 32bit-flipped

	fprintf(stderr, "WriteMemB( %.8X, %.2X )\n", (unsigned int) inOffset, (unsigned int) inValue);
	if (inOffset >= 0x400 && inOffset < 0x800) {
		// Data window, the host may use it instead of registers 0, 8 and 9.
		WriteFifoByte(inValue);
	} else switch (inOffset)
	{
		case 0x00: // Even data
		case 0x08: // Duplicate even data
		case 0x09: // Odd data
			WriteFifoByte(inValue);
			break;
		case 0x01:
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
		case 0x0E: // Device Control
			// Bit 2 (SRST) resets the card as long as it is set, bit 1 (nIEN)
			// disables interrupts, which we never raise anyway.
			{
				bool wasInReset = IsInReset();
				mDeviceControlReg = inValue;
				UpdateReset(wasInReset);
			}
			break;
		default:
			break;
	}
	if (GetLog())
	{
		GetLog()->FLogLine("WriteMemB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue);
	}
}

// =================================================== //
// There's got to be more to life than compile-and-go. //
// =================================================== //


// The host ignores every status bit but BSY while BSY is set, so DRQ must only
// be reported together with BSY clear.
KUInt8 TATACard::ReadStatus(void)
{
	switch (mState) {
		case State::Idle:
			return 0x50; // kStatusReg_RDY | kStatusReg_DSC
		case State::NoDataBusy:
			mState = State::Idle;
			return 0x80; // kStatusReg_BSY
		case State::DataReadBusy:
			mState = State::DataReadReady;
			return 0x80; // kStatusReg_BSY
		case State::DataReadReady:
			return 0x58; // kStatusReg_RDY | kStatusReg_DSC | kStatusReg_DRQ
		case State::DataWriteBusy:
			mState = State::DataWriteReady;
			return 0x80; // kStatusReg_BSY
		case State::DataWriteReady:
			return 0x58; // kStatusReg_RDY | kStatusReg_DSC | kStatusReg_DRQ
		case State::Error:
			return 0x51; // kStatusReg_RDY | kStatusReg_DSC | kStatusReg_ERR
		case State::WriteFault:
			return 0x71; // kStatusReg_RDY | kStatusReg_DWF | kStatusReg_DSC | kStatusReg_ERR
		case State::InReset:
			return 0x80; // kStatusReg_BSY
	}
	return 0x50; // kStatusReg_RDY | kStatusReg_DSC
}

KUInt8 TATACard::ReadFifoByte(void)
{
	if (mState != State::DataReadReady || mFifoPos >= mFifo.size())
		return 0;
	KUInt8 theResult = mFifo[mFifoPos++];
	if (mFifoPos >= mFifo.size()) {
		// DRQ drops after the last byte of a block.
		if (mCommandReg == 0x20) { // Read Sectors
			// One block is done. The count was 0 for 256 blocks, so it wraps to
			// 255 and only reaches 0 again after the last one.
			--mSectorCountReg;
			if (mSectorCountReg == 0) {
				mState = State::Idle;
			} else {
				// On error the registers keep the count of the blocks that are
				// left and the address of the block that failed.
				SetLBA(GetLBA() + 1);
				mState = BuildSectorData() ? State::DataReadBusy : State::Error;
			}
		} else {
			mState = State::Idle;
		}
	}
	return theResult;
}

void TATACard::WriteFifoByte(KUInt8 inByte)
{
	if (mState != State::DataWriteReady || mFifoPos >= mFifo.size())
		return;
	mFifo[mFifoPos++] = inByte;
	if (mFifoPos < mFifo.size())
		return;

	// A whole block has arrived (Write Sectors). Same register handling as for
	// reading: the count was 0 for 256 blocks, so it wraps to 255 and only
	// reaches 0 again after the last one. On error the registers keep the
	// count of the blocks that are left and the address of the block that failed.
	if (!WriteSectorData()) {
		mState = State::WriteFault;
		return;
	}
	--mSectorCountReg;
	if (mSectorCountReg == 0) {
		mState = State::Idle;
	} else {
		SetLBA(GetLBA() + 1);
		if (IsSectorValid()) {
			mFifoPos = 0;
			mState = State::DataWriteBusy;
		} else {
			mState = State::Error;
		}
	}
}

static void SetIdentifyWord(std::vector<KUInt8>& ioBuffer, int inWord, KUInt16 inValue)
{
	ioBuffer[inWord * 2] = inValue & 0xFF;
	ioBuffer[inWord * 2 + 1] = inValue >> 8;
}

// ATA strings are space padded and store the first character of each pair in
// the high byte of the word.
static void SetIdentifyString(std::vector<KUInt8>& ioBuffer, int inFirstWord, int inNumWords, const char* inString)
{
	size_t len = strlen(inString);
	for (int i = 0; i < inNumWords; ++i) {
		char c0 = (size_t) (i * 2) < len ? inString[i * 2] : ' ';
		char c1 = (size_t) (i * 2 + 1) < len ? inString[i * 2 + 1] : ' ';
		SetIdentifyWord(ioBuffer, inFirstWord + i, (KUInt16) (((KUInt8) c0 << 8) | (KUInt8) c1));
	}
}

// A few rotates and xors over the file name (without the directory), so every
// image gets its own serial number, and it stays the same when the image moves.
static KUInt32 FileNameChecksum(const char* inPath)
{
	std::string name = std::filesystem::path(inPath ? inPath : "").filename().string();
	KUInt32 sum = 0xa63e95f1;
	for (unsigned char c : name)
		sum = ((sum << 5) | (sum >> 27)) ^ c;
	return sum;
}

void
TATACard::BuildIdentifyData(void)
{
	mFifo.assign(kSectorSize, 0);
	mFifoPos = 0;

	// What CHS addressing can reach, this may be a bit less than mSectors.
	KUInt32 chsSectors = (KUInt32) mCylinders * mHeads * mSectorsPerTrack;

	SetIdentifyWord(mFifo, 0, 0x848A);	// CompactFlash, removable, no fixed disk
	SetIdentifyWord(mFifo, 1, mCylinders);
	SetIdentifyWord(mFifo, 3, mHeads);
	SetIdentifyWord(mFifo, 5, 0x0240); // unformatted bytes per sector
	SetIdentifyWord(mFifo, 6, mSectorsPerTrack);
	char serial[21];
	snprintf(serial, sizeof(serial), "%08X", (unsigned int) FileNameChecksum(mFilePath));
	SetIdentifyString(mFifo, 10, 10, serial);			// serial number
	SetIdentifyWord(mFifo, 20, 0x0002);	 // Buffer type
	SetIdentifyWord(mFifo, 21, 0x0002);	 // Buffer size (512)
	SetIdentifyWord(mFifo, 22, 0x0004);	 // # of ECC bytes on read/write long
	SetIdentifyString(mFifo, 23, 4, "1.0");		// firmware revision
	char model[41];
	snprintf(model, sizeof(model), "Einstein ATA %.1f", mSectors / 2048.0);	// size in MB
	SetIdentifyString(mFifo, 27, 20, model);	// model number
	SetIdentifyWord(mFifo, 47, 0x0001);	// max sectors per READ/WRITE MULTIPLE
	SetIdentifyWord(mFifo, 49, 0x0200);	// LBA supported, DMA not supported
	SetIdentifyWord(mFifo, 51, 0x0100);	// PIO timing
	SetIdentifyWord(mFifo, 53, 0x0001);	// words 54-58 are valid
	SetIdentifyWord(mFifo, 54, mCylinders);
	SetIdentifyWord(mFifo, 55, mHeads);
	SetIdentifyWord(mFifo, 56, mSectorsPerTrack);
	SetIdentifyWord(mFifo, 57, chsSectors & 0xFFFF);	// current capacity in sectors
	SetIdentifyWord(mFifo, 58, chsSectors >> 16);
	SetIdentifyWord(mFifo, 59, 0x0000);	// Multiple sector setting is not valid
	SetIdentifyWord(mFifo, 60, mSectors & 0xFFFF);	// total LBA sectors
	SetIdentifyWord(mFifo, 61, mSectors >> 16);
}

void
TATACard::Reset(void)
{
	mState = State::Idle;
	mFifo.clear();
	mFifoPos = 0;

	// After a reset the task file holds the ATA signature, and the error
	// register the result of the diagnostic: 1 = no error.
	mErrorReg = 0x01;
	mFeaturesReg = 0;
	mSectorCountReg = 1;
	mSectorNumberReg = 1;
	mCylinderLowReg = 0;
	mCylinderHighReg = 0;
	mDriveHeadReg = 0;
	mCommandReg = 0;
}

bool
TATACard::IsInReset(void) const
{
	return (mConfigOptionReg & 0x80) || (mDeviceControlReg & 0x04);
}

void
TATACard::UpdateReset(bool inWasInReset)
{
	bool isInReset = IsInReset();
	if (isInReset && !inWasInReset) {
		fprintf(stderr, "::      Reset\n");
		Reset();
		mState = State::InReset;
	} else if (!isInReset && inWasInReset) {
		// Busy for a moment, then ready. The host can now read the diagnostic
		// result from the error register.
		fprintf(stderr, "::      Reset done\n");
		mState = State::NoDataBusy;
	}
}

void
TATACard::SetGeometry(void)
{
	// Card sizes come in multiples of 128kB (256 sectors), so this usually
	// divides evenly. The 5MB SDP5A-5 has 160 cylinders, 2 heads and 32 sectors
	// per track. Keep those, and add heads for larger images until there are
	// at most 1024 cylinders, like a PC BIOS would want. At the 128MB limit
	// that is 8 heads.
	mSectors = (KUInt32) (mData.size() / kSectorSize);
	mHeads = 2;
	mSectorsPerTrack = 32;
	while (mSectors / (mHeads * mSectorsPerTrack) > 1024 && mHeads < 16)
		mHeads *= 2;
	mCylinders = mSectors / (mHeads * mSectorsPerTrack);
	if (mCylinders == 0) {
		// Less than one cylinder: one head, and one short track holds it all.
		// (Also avoids dividing by zero later if there is no image at all.)
		mHeads = 1;
		mSectorsPerTrack = mSectors == 0 ? 1 : mSectors;
		mCylinders = mSectors == 0 ? 0 : 1;
	}
}

KUInt32
TATACard::GetLBA(void)
{
	// 1 0 X 0 0 0 1  1 Error				Features 2
	// 1 0 X 0 0 1 0  2 Sector Count 		Sector Count		0=256, counts down, will be zero when command was successful
	// 1 0 X 0 0 1 1  3 Sector No. 			Sector No.			LBA 7-0
	// 1 0 X 0 1 0 0  4 Cylinder Low 		Cylinder Low		LBA 15-8
	// 1 0 X 0 1 0 1  5 Cylinder High 		Cylinder High		LBA 23-16
	// 1 0 X 0 1 1 0  6 Select Card /Head 	Select Card/Head	LBA 27-24
	// 1 0 X 0 1 1 1  7 Status 				Command				Reading clears pending interrupt
	// [6] Drive Head register:
	// 	  Bit 5 Bit 4 (DRV) Bit 3 (HS3) Bit 2 (HS2) Bit 1 (HS1) Bit 0 (HS0)
	// Bit 7: This bit is set to 1.
	// Bit 6: LBA is a flag to select either Cylinder/Head/Sector (CHS) or
	// 		Logical Block Address Mode (LBA).
	// 		When LBA=0, Cylinder/Head/Sector mode is selected.
	// 		When LBA=1, Logical Block Address is selected. In Logical Block
	// 		Mode, the Logical Block Address is interpreted as follows:
	// 		LBA07-LBA00: Sector Number Register D7-D0.
	// 		LBA15-LBA08: Cylinder Low Register D7-D0.
	// 		LBA23-LBA16: Cylinder High Register D7-D0.
	// LBA27-LBA24: Drive/Head Register bits HS3-HS0.
	// Bit 5: This bit is set to 1.
	// Bit 4: DRV is the drive number. When DRV=0, drive (card) 0 is selected
	// 		When DRV=1, drive (card) 1 is selected. The CompactFlash Card is
	// 		set to be Card 0 or 1 using the copy field of the PCMCIA Socket &
	// 		Copy configuration register.
	// Bit 3-0: HS3-HS0 In LBA mode, this is bit 27-24 of the Logical Block Address.
	//		Else this is the head number.
	KUInt32 head = mDriveHeadReg & 0x0F;
	if (mDriveHeadReg & 0x40) {
		// LBA mode
		return (head << 24) | (mCylinderHighReg << 16) | (mCylinderLowReg << 8) | mSectorNumberReg;
	}
	// CHS mode, sector numbers start at 1
	if (mSectorNumberReg == 0 || mSectorNumberReg > mSectorsPerTrack || head >= mHeads)
		return 0xFFFFFFFF;
	KUInt32 cylinder = (mCylinderHighReg << 8) | mCylinderLowReg;
	return (cylinder * mHeads + head) * mSectorsPerTrack + (mSectorNumberReg - 1);
}

void
TATACard::SetLBA(KUInt32 inLBA)
{
	if (mDriveHeadReg & 0x40) {
		// LBA mode
		mSectorNumberReg = inLBA & 0xFF;
		mCylinderLowReg = (inLBA >> 8) & 0xFF;
		mCylinderHighReg = (inLBA >> 16) & 0xFF;
		mDriveHeadReg = (mDriveHeadReg & 0xF0) | ((inLBA >> 24) & 0x0F);
	} else {
		// CHS mode
		KUInt32 cylinder = inLBA / (mSectorsPerTrack * mHeads);
		mSectorNumberReg = inLBA % mSectorsPerTrack + 1;
		mCylinderLowReg = cylinder & 0xFF;
		mCylinderHighReg = (cylinder >> 8) & 0xFF;
		mDriveHeadReg = (mDriveHeadReg & 0xF0) | ((inLBA / mSectorsPerTrack) % mHeads);
	}
}

bool
TATACard::IsSectorValid(void)
{
	uint64_t offset = (uint64_t) GetLBA() * kSectorSize;
	if (offset + kSectorSize > mData.size()) {
		mErrorReg = 0x10; // IDNF, the requested sector ID cannot be found
		return false;
	}
	return true;
}

bool
TATACard::BuildSectorData(void)
{
	mFifo.assign(kSectorSize, 0);
	mFifoPos = 0;

	if (!IsSectorValid())
		return false;
	uint64_t offset = (uint64_t) GetLBA() * kSectorSize;
	std::copy_n(mData.begin() + (size_t) offset, kSectorSize, mFifo.begin());
	return true;
}

bool
TATACard::WriteSectorData(void)
{
	uint64_t offset = (uint64_t) GetLBA() * kSectorSize;
	// The file first: if that fails, memory and file must not differ.
	if (!mFile || mReadOnly
		|| fseek(mFile, (long) offset, SEEK_SET) != 0
		|| fwrite(mFifo.data(), 1, kSectorSize, mFile) != kSectorSize
		|| fflush(mFile) != 0) {
		mErrorReg = 0x04; // Abort
		return false;
	}
	std::copy_n(mFifo.begin(), kSectorSize, mData.begin() + (size_t) offset);
	return true;
}

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

void
TATACard::VerifySectors(void)
{
	// Same register handling as Read Sectors: the count is 0 for 256 sectors,
	// it wraps to 255 after the first one and reaches 0 after the last one.
	// On error it holds the number of sectors left and the LBA registers the
	// address of the sector that failed. On success they hold the address of
	// the last sector.
	for (;;) {
		if (!IsSectorValid()) {
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

void
TATACard::StartCommand(KUInt8 inCommand)
{
	fprintf(stderr, "WriteCommand( 0x%.2X )\n", (unsigned int) inCommand);
	if (mState == State::InReset) {
		// The card is busy resetting and does not look at the command register.
		return;
	}
	mErrorReg = 0;
	switch (inCommand) {
		case 0xEC: // Identify Drive Class 1
			fprintf(stderr, "::      Identify Drive\n");
			BuildIdentifyData();
			mState = State::DataReadBusy;
			break;
		case 0x20: // Read Sectors
			fprintf(stderr, "::      Read Sectors\n");
			// Sector Count: 0 means 256. ReadFifoByte() loads the following
			// blocks and counts the register down to 0.
			mState = BuildSectorData() ? State::DataReadBusy : State::Error;
			break;
		case 0x40: // Read Verify Sectors Class 1
			fprintf(stderr, "::      Read Verify Sectors\n");
			VerifySectors();
			break;
		case 0xEF: // Set Features Class 1
			// The feature number is in mFeaturesReg. 8/16 bit transfers are
			// handled by ReadMemB()/ReadMem(), the rest (write cache, power
			// management, ...) is meaningless in the emulator. So accept
			// every feature and do nothing.
			fprintf(stderr, "::      Set Features( 0x%.2X )\n", (unsigned int) mFeaturesReg);
			mState = State::NoDataBusy;
			break;

		case 0xE0: // Standby Immediate Class 1
		case 0x94: // (alternate code for the same command)
			// Power management only. Every sector is already written to the
			// file, but this is a good moment to make sure it reached the disk.
			fprintf(stderr, "::      Standby Immediate\n");
			FlushImage();
			mState = State::NoDataBusy;
			break;
		case 0x30: // Write Sectors Class 2
			fprintf(stderr, "::      Write Sectors\n");
			// Sector Count: 0 means 256. WriteFifoByte() writes every block
			// that arrives and counts the register down to 0.
			if (mReadOnly) {
				mErrorReg = 0x04; // Abort
				mState = State::WriteFault;
			} else if (!IsSectorValid()) {
				mState = State::Error;
			} else {
				mFifo.assign(kSectorSize, 0);
				mFifoPos = 0;
				mState = State::DataWriteBusy;
			}
			break;
		default:
			// Don't leave the host polling forever: abort the command.
			fprintf(stderr, "::      Unknown command 0x%.2X, aborted\n", (unsigned int) inCommand);
			mErrorReg = 0x04; // Abort
			mState = State::Error;
			break;
	}
}

// mErrorReg bits:
// Bit 7 (BBK) This bit is set when a Bad Block is detected.
// Bit 6 (UNC) This bit is set when an Uncorrectable Error is encountered.
// Bit 5 This bit is 0.
// Bit 4 (IDNF) The requested sector ID is in error or cannot be found.
// Bit 3 This bit is 0.
// Bit 2 (Abort) This bit is set if the command has been aborted because of a CompactFlash Memory Card status
// condition: (Not Ready, Write Fault, etc.) or when an invalid command has been issued.
// Bit 1 This bit is 0.
// Bit 0 (AMNF) This bit is set in case of a general error.

// ============================================================================ //
// TODO (status: the four commands the host driver uses are implemented and
// tested in a harness, but not yet with the real driver: Identify Drive, Read
// Sectors, Read Verify, Set Features, Standby Immediate, Write Sectors)
// ============================================================================ //
//
// 1. Unknown commands are aborted: Status 0x51 (RDY|DSC|ERR), Error register
//    0x04 (ABRT), i.e. State::Error, cleared by the next command. Done.
//
// 2. Test Write Sectors with the real driver. The 16 bit path is verified.
//    Multi-sector writes go BSY -> DRQ between blocks.
//
// 3. Image handling (decided against TMappedFile). The image is read into mData
//    (clipped to kMaxImageSize = 128MB, rounded down to whole sectors, the
//    file is never truncated). mFile stays open "r+b". Every sector is written
//    with fwrite+fflush as soon as it has arrived, so a crash of the emulator
//    loses nothing, and Standby Immediate/the destructor also fsync. If the
//    image can't be opened for writing, mReadOnly is set and Write Sectors
//    answers with State::WriteFault (0x71, Error 0x04).
//    Open: report to the user (not only stderr) when an image is clipped or
//    read only.
//
// 4. Geometry. SetGeometry() makes up cylinders, heads and sectors per track
//    from the image size (2 heads x 32 sectors, more heads above 1024
//    cylinders, so 160/2/32 for the 5MB card and 1024/8/32 for 128MB). The
//    Newton driver reads only the total number of sectors (Identify words 60-61)
//    because we say LBA is supported. CHS addresses are decoded with 1-based
//    sector numbers, as the ATA spec says, but that is untested with the real
//    driver since it always uses LBA.
//
// 5. Clean up.
//    - Remove the fprintf(stderr, ...) tracing (or put it behind a DEBUG macro),
//      the ReadMem() "cnt" counter and the dead commented-out code.
//    - Move the register table and the Identify Drive trace from ReadMemB() into
//      the header comment or a notes block.
//    - Registers 1..5 in ReadMemB() should return what was written (or the
//      Error register for 1), not 0. Register 6 still returns a fixed 0xA0.
//    - ReadAttr()/ReadIO()/WriteIO() etc. are still the stubs from the template.
//    - Keep Paul's copyright header, but fix the file name in it (TATACard.cp).
//    - Only if the host ever enables interrupts (clears nIEN, bit 1 of the
//      Device Control register at 0x0E): call mPCMCIAController->RaiseInterrupt()
//      when DRQ is set. The host is polling at the moment, so this is not needed.
//
// 6. Both soft resets are implemented (Reset(), UpdateReset()): SRESET in the
//    Configuration Option Register (0x200 in attribute space) and SRST in the
//    Device Control register (0x0E). While either is set the status is BSY and
//    commands are ignored, when both are clear the card is BSY once more and
//    then ready. Untested with the real driver.
// ============================================================================ //
