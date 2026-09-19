// ==============================
// File:			TATACard.cp
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

#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <algorithm>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

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
TATACard::TATACard(const char* inImagePath, TLog* inLog)
: TPCMCIACard(inLog)
{
	mFilePath = strdup(inImagePath);
	mFile = fopen(inImagePath, "rb");
	if (mFile)
	{
		fseek(mFile, 0, SEEK_END);
		long fileSize = ftell(mFile);
		rewind(mFile);
		mData.resize(fileSize);
		fread(mData.data(), 1, fileSize, mFile);
		fclose(mFile);
		mFile = nullptr;
	}
}

// -------------------------------------------------------------------------- //
//  * ~TATACard( void )
// -------------------------------------------------------------------------- //
TATACard::~TATACard(void)
{
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
//  * GetVPCPins( void )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::GetVPCPins(void)
{
	if (GetLog())
	{
		GetLog()->LogLine("GetVPCPins");
	}

	return 0;
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
		theResult = 0x00;
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
		// theResult = 0x00;
		theResult = 0x20; // IOis8
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
		theResult = 0x0E;
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
		// Try reading 8 bits at a time from memory-mapped card
		//theResult = ReadFifoByte();
		// Try reading 16 bits
		KUInt8 lo = ReadFifoByte();
		KUInt8 hi = ReadFifoByte();
		theResult = (hi << 24) | (lo << 16) | (hi << 8) | lo;
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
		case 6:
			theResult = 0xA0; //
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
	} else if (inOffset == 0x0101) { // 0x0202
		// 202h: Configuration Option Register 2
		// Read: changed:1, SigChg:1, IOis8:1, 0, 0, PwrDwn:1, Int:1, 0
		// Write: 0, SigChg:1, IOis8:1, 0, 0, PwrDwn:1, 0, 0
	} else if (inOffset == 0x0102) { // 0x0204
		// 204h: Pin Replcemant Register
		// Read: 0, 0, CRdy/~Bsy:1, CWProt:1, 1, 1, RRdy/~Bsy:1, RWProt:1
		// Write: 0, 0, CRdy/~Bsy:1, CWProt:1, 0, 0, MRdy/~Bsy:1, MWProt:1
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
	switch (inOffset)
	{
		case 0x01:
			FesturesReg = inValue;
			break;
		case 0x02:
			SectorCountReg = inValue;
			break;
		case 0x03:
			SectorNumberReg = inValue;
			break;
		case 0x04:
			CylinderLowReg = inValue;
			break;
		case 0x05:
			CylinderHighReg = inValue;
			break;
		case 0x06:
			DriveHeadReg = inValue;
			break;
		case 0x07:
			CommandReg = inValue;
			StartCommand(inValue);
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
		case State::Class1SetBusy:
			mState = State::Idle;
			return 0x80; // kStatusReg_BSY
		case State::Class2SetBusy:
			mState = State::Class2DataReady;
			return 0x80; // kStatusReg_BSY
		case State::Class2DataReady:
			return 0x58; // kStatusReg_RDY | kStatusReg_DSC | kStatusReg_DRQ
	}
	return 0x50; // kStatusReg_RDY | kStatusReg_DSC
}

KUInt8 TATACard::ReadFifoByte(void)
{
	if (mState != State::Class2DataReady || mFifoPos >= mFifo.size())
		return 0;
	KUInt8 theResult = mFifo[mFifoPos++];
	if (mFifoPos >= mFifo.size())
		mState = State::Idle; // DRQ drops after the last byte
	return theResult;
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

void
TATACard::BuildIdentifyData(void)
{
	// SDP5A-5: 5MB, 160 cylinders, 2 heads, 32 sectors per track
	const KUInt16 kCylinders = 160;
	const KUInt16 kHeads = 2;
	const KUInt16 kSectorsPerTrack = 32;
	const KUInt32 kSectors = kCylinders * kHeads * kSectorsPerTrack; // 10240

	mFifo.assign(512, 0);
	mFifoPos = 0;

	SetIdentifyWord(mFifo, 0, 0x848A);	// CompactFlash, removable, no fixed disk
	SetIdentifyWord(mFifo, 1, kCylinders);
	SetIdentifyWord(mFifo, 3, kHeads);
	SetIdentifyWord(mFifo, 5, 0x0240); // unformatted bytes per sector
	SetIdentifyWord(mFifo, 6, kSectorsPerTrack);
	SetIdentifyWord(mFifo, 7, kSectors>>16);
	SetIdentifyWord(mFifo, 8, kSectors&0xFFFF);
	SetIdentifyString(mFifo, 10, 10, "4206");			// serial number
	SetIdentifyWord(mFifo, 20, 0x0002);	 // Buffer type
	SetIdentifyWord(mFifo, 21, 0x0002);	 // Buffer size (512)
	SetIdentifyWord(mFifo, 22, 0x0004);	 // # of ECC bytes on read/write long
	SetIdentifyString(mFifo, 23, 4, "1.0");		// firmware revision
	SetIdentifyString(mFifo, 27, 20, "SDP5A-5");	// model number
	SetIdentifyWord(mFifo, 47, 0x0001);	// max sectors per READ/WRITE MULTIPLE
	SetIdentifyWord(mFifo, 49, 0x0200);	// LBA supported, DMA not supported
	SetIdentifyWord(mFifo, 51, 0x0100);	// PIO timing
	SetIdentifyWord(mFifo, 53, 0x0001);	// words 54-58 are valid
	SetIdentifyWord(mFifo, 54, kCylinders);
	SetIdentifyWord(mFifo, 55, kHeads);
	SetIdentifyWord(mFifo, 56, kSectorsPerTrack);
	SetIdentifyWord(mFifo, 57, kSectors & 0xFFFF);
	SetIdentifyWord(mFifo, 58, kSectors >> 16);
	SetIdentifyWord(mFifo, 59, 0x0100);	// words 60-61 are valid
	SetIdentifyWord(mFifo, 60, kSectors & 0xFFFF);	// total LBA sectors
	SetIdentifyWord(mFifo, 61, kSectors >> 16);
}

void
TATACard::BuildSectorData(void)
{
	//const KUInt16 kCylinders = 160;
	const KUInt16 kHeads = 2;
	const KUInt16 kSectorsPerTrack = 32;
	const KUInt16 kSectorSize = 512;

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
	KUInt8 head = DriveHeadReg & 0x0F;
	KUInt32 lba = 0;
	if (DriveHeadReg & 0x40) {
		// LBA mode
		lba = ((head & 0x0F) << 24) | (CylinderHighReg << 16) | (CylinderLowReg << 8) | SectorNumberReg;
	} else {
		// CHS mode
		KUInt32 cylinder = (CylinderHighReg << 8) | CylinderLowReg;
		lba = SectorNumberReg + (head * kSectorsPerTrack) + (cylinder * kHeads * kSectorsPerTrack);
	}
	// Now lba contains the logical block address of the sector to read.
	size_t offset = (size_t) lba * kSectorSize;

	// Fill mFifo with the 512 bytes of the sector data.
	mFifo.assign(512, 0);
	mFifoPos = 0;

	if (offset < mData.size() - kSectorSize) {
		std::copy_n(mData.begin() + offset, kSectorSize, mFifo.begin());
	}

}

void
TATACard::StartCommand(KUInt8 inCommand)
{
	fprintf(stderr, "WriteCommand( 0x%.2X )\n", (unsigned int) inCommand);
	switch (inCommand) {
		case 0xEC: // Identify Drive Class 1
			fprintf(stderr, "::      Identify Drive\n");
			BuildIdentifyData();
			mState = State::Class2SetBusy;
			break;
		case 0x20: // Read Sectors
			fprintf(stderr, "::      Read Sectors\n");
			BuildSectorData();
			mState = State::Class2SetBusy; // fake it for now
			break;

		// TODO:
		// kReadVerifySectorsCmd = 0x40
		// kSetFeaturesCmd = 0xef
		// kStandbyImmediateCmd = 0xE0
		// kWriteSectorsCmd = 0x30
		default:
			break;
	}
}

// ============================================================================ //
// TODO (status: test card mounts, Identify Drive and Read Sectors work)
// ============================================================================ //
//
// 1. Missing commands. The host driver only needs these four. Add them to the
//    switch in StartCommand(). Use State::Class1SetBusy for the ones without a
//    data phase; it exists already but nothing sets it yet.
//
//    0x40 kReadVerifySectorsCmd  Class 1. No data. Check that the LBA range is
//                                inside the image, then finish like a read
//                                without DRQ.
//    0xEF kSetFeaturesCmd        Class 1. No data. The feature number is in the
//                                Features register (FesturesReg, sic; rename
//                                it). Accept and ignore, e.g. 0x01/0x81 =
//                                enable/disable 8-bit mode, 0x02/0x82 = write
//                                cache. Check what the host sends.
//    0xE0 kStandbyImmediateCmd   Class 1. No data. Just report ready.
//    0x30 kWriteSectorsCmd       Class 2, DATA IN direction, see 2.
//
//    Unknown commands are silently ignored right now, so the host would poll
//    Status forever. Answer them with ERR: Status 0x51 (RDY|DSC|ERR), Error
//    register 0x04 (ABRT). ReadMemB needs a case 1 for the Error register.
//
// 2. Write Sectors (0x30).
//    - There is no write path for the data register yet. WriteMemB() and
//      WriteMem() need the same decode as the read side: registers 0, 8, 9 and
//      the 0x400-0x7FF window. Note that ReadMem() only handles offset 0 and
//      packs two FIFO bytes into all four lanes, so WriteMem() needs to mirror
//      that: take the 16-bit value from the matching lanes.
//    - New state (e.g. Class2WriteReady): BSY, then DRQ with BSY clear (0x58)
//      while the host pushes 512 bytes into mFifo. After the last byte: BSY,
//      copy mFifo to the image at lba*512, then Idle.
//    - Factor the LBA/CHS decode out of BuildSectorData() into its own helper,
//      it is needed for read, write and verify.
//
// 3. Make writes stick: map the image instead of reading it into mData.
//    K/Misc/TMappedFile does this already (open(2) flags, O_RDWR gives
//    PROT_READ|PROT_WRITE with MAP_SHARED, and it writes the file back on
//    destruction if mmap is not available). Replace mFilePath/mFile/mData by a
//    TMappedFile, use GetBuffer()/GetSize() where mData is used, and open with
//    O_RDWR. Open read-only as a fallback if the image is not writable, and
//    answer Write Sectors with a write fault (Status 0x71, Error 0x04) in that
//    case. Remove the fopen/fread code in the constructor and the fclose in the
//    destructor.
//
// 4. Bugs seen while reading BuildSectorData().
//    - The range check `offset < mData.size() - kSectorSize` wraps around if
//      the image is smaller than 512 bytes, and it skips the last sector. Use
//      `offset + kSectorSize <= size`.
//    - Sector Count (register 2) is ignored, so only one sector is ever
//      delivered. 0 means 256. Refill the FIFO from ReadFifoByte() when it runs
//      dry, advance the LBA, and only go Idle after the last sector. The
//      register should count down to 0 when the command succeeds.
//    - An LBA past the end of the card should end with ERR and Error = 0x10
//      (IDNF), not with a sector of zeros.
//    - StartCommand() still says "fake it for now" for Read Sectors, the state
//      handling should be the same as for Identify Drive once the above is done.
//    - The Identify data and BuildSectorData() both hardcode the geometry
//      (160/2/32). Move it into shared constants, or derive it from the image
//      size.
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
// 6. Untested: SRST (Device Control bit 2) should return to Idle and clear the
//    FIFO. The Configuration Option Register SRESET (0x200, bit 7) should do
//    the same. The host writes 0x80 and then 0x00 to it after Identify Drive.
// ============================================================================ //
