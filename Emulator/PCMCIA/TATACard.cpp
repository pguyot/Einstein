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

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TATACard( KUInt32 )
// -------------------------------------------------------------------------- //
TATACard::TATACard(KUInt32 inSize)
{
	(void) inSize;
}

// -------------------------------------------------------------------------- //
//  * ~TATACard( void )
// -------------------------------------------------------------------------- //
TATACard::~TATACard(void)
{
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
	if (GetLog())
	{
		GetLog()->FLogLine("ReadAttrB( %.8X )", (unsigned int) inOffset);
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
	if (GetLog())
	{
		GetLog()->FLogLine("ReadMem( %.8X )", (unsigned int) inOffset);
	}

	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMemB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadMemB(KUInt32 inOffset)
{
	if (GetLog())
	{
		GetLog()->FLogLine("ReadMemB( %.8X )", (unsigned int) inOffset);
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
