// ==============================
// Fichier:			UMemoryTests.cp
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
//
// Créé le:			8/3/2005
// Tabulation:		4 espaces
//
// Copyright:		© 2005 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: UMemoryTests.cp 151 2006-01-13 16:15:33Z paul $
// ===========

#include <K/Defines/KDefinitions.h>
#include "UMemoryTests.h"

// ANSI C & POSIX
#if !TARGET_OS_WIN32
	#include <unistd.h>
#endif

// K
#include <K/Defines/UByteSex.h>

// Einstein
#include "Emulator/Log/TLog.h"
#include "Emulator/TMemory.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#if TARGET_OS_WIN32
	#define kTempFlashPath "c:/EinsteinTests.flash"
#else
	#define kTempFlashPath "/tmp/EinsteinTests.flash"
#endif

// -------------------------------------------------------------------------- //
//  * FlashTest( TLog* )
// -------------------------------------------------------------------------- //
void
UMemoryTests::FlashTest( TLog* inLog )
{
	KUInt8* romBuffer = (KUInt8*) malloc(TMemoryConsts::kLowROMEnd);
	TMemory theMem( inLog, (KUInt8*) romBuffer, kTempFlashPath );
	int index;

	if (theMem.WriteToFlash32Bits(
			0x00112233, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	if (theMem.WriteToFlash32Bits(
			0x44556677, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 + 4 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1+4");
	}
	if (theMem.WriteToFlash32Bits(
			0x8899AABB, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 + 8 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1+8");
	}
	if (theMem.WriteToFlash32Bits(
			0xCCDDEEFF, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 + 12 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1+C");
	}

	Boolean fault = false;
	for (index = 0; index < 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( TMemoryConsts::kFlashBank1 + (4*index), fault );
		if (fault)
		{
			inLog->FLogLine("A fault occurred reading at %i", index);
		}
		inLog->FLogLine("%i: %.8X", index, (unsigned int) theWord);
	}

	KUInt8 theByte;
	for (index = 0; index < 16; index++)
	{
		if (theMem.ReadBP( TMemoryConsts::kFlashBank1 + index, theByte ))
		{
			inLog->FLogLine("A fault occurred reading at %i", index);
		}
		inLog->FLogLine("%i: %.2X", index, (unsigned int) theByte);
	}

	if (theMem.WriteToFlash16Bits(
			0xFFEEDDCC, 0xFFFF0000, TMemoryConsts::kFlashBank1 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	if (theMem.WriteToFlash16Bits(
			0xBBAA9988, 0xFFFF0000, TMemoryConsts::kFlashBank1 + 4 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	if (theMem.WriteToFlash16Bits(
			0x77665544, 0xFFFF0000, TMemoryConsts::kFlashBank1 + 8 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	if (theMem.WriteToFlash16Bits(
			0x33221100, 0xFFFF0000, TMemoryConsts::kFlashBank1 + 12 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	for (index = 0; index < 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( TMemoryConsts::kFlashBank1 + (4*index), fault );
		if (fault)
		{
			inLog->FLogLine("A fault occurred reading at %i", index);
		}
		inLog->FLogLine("%i: %.8X", index, (unsigned int) theWord);
	}

	if (theMem.WriteToFlash16Bits(
			0x33221100, 0x0000FFFF, TMemoryConsts::kFlashBank1 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	if (theMem.WriteToFlash16Bits(
			0x77665544, 0x0000FFFF, TMemoryConsts::kFlashBank1 + 4 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	if (theMem.WriteToFlash16Bits(
			0xBBAA9988, 0x0000FFFF, TMemoryConsts::kFlashBank1 + 8 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	if (theMem.WriteToFlash16Bits(
			0xFFEEDDCC, 0x0000FFFF, TMemoryConsts::kFlashBank1 + 12 ))
	{
		inLog->LogLine("A fault occurred writing to flash at bank1");
	}
	for (index = 0; index < 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( TMemoryConsts::kFlashBank1 + (4*index), fault );
		if (fault)
		{
			inLog->FLogLine("A fault occurred reading at %i", index);
		}
		inLog->FLogLine("%i: %.8X", index, (unsigned int) theWord);
	}

	(void) ::unlink( kTempFlashPath );
	::free( romBuffer );
}

// ========================================================= //
// You are in a maze of little twisting passages, all alike. //
// ========================================================= //
