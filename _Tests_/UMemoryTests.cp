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
#include <unistd.h>

// K
#include <K/Defines/UByteSex.h>

// Einstein
#include "Emulator/Log/TStdOutLog.h"
#include "Emulator/TMemory.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#define kTempFlashPath "/tmp/EinsteinTests.flash"

// -------------------------------------------------------------------------- //
//  * ReadROMTest( void )
// -------------------------------------------------------------------------- //
void
UMemoryTests::ReadROMTest( void )
{
	KUInt8* romBuffer = (KUInt8*) malloc(TMemoryConsts::kLowROMEnd);
	TStdOutLog theLog;
	TMemory theMem( &theLog, romBuffer, kTempFlashPath );
	int index;
	for (index = 0; index < 8; index++)
	{
		romBuffer[index] = (index * 16) + index;
	}

#if TARGET_RT_LITTLE_ENDIAN
	// Swap the ROM the way it is swapped
	KUInt32* theROM = (KUInt32*) romBuffer;
	theROM[0] = UByteSex::Swap( theROM[0] );
	theROM[1] = UByteSex::Swap( theROM[1] );
#endif
	
	Boolean fault = false;
	for (index = 0; index <= 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( index, fault );
		if (fault)
		{
			(void) ::printf( "A fault occurred at %i\n", index  );
		}
		::printf( "%i: %.8X\n", index, (unsigned int) theWord );
	}
	
	KUInt8 theByte;
	for (index = 0; index < 8; index++)
	{
		if (theMem.ReadBP( index, theByte ))
		{
			(void) ::printf( "A fault occurred at %i\n", index  );
		}
		::printf( "%i: %.2X\n", index, (unsigned int) theByte );
	}

	(void) ::unlink( kTempFlashPath );
	::free( romBuffer );
}

// -------------------------------------------------------------------------- //
//  * ReadWriteRAMTest( void )
// -------------------------------------------------------------------------- //
void
UMemoryTests::ReadWriteRAMTest( void )
{
	TStdOutLog theLog;
	KUInt8* romBuffer = (KUInt8*) malloc(TMemoryConsts::kLowROMEnd);
	TMemory theMem( &theLog, (KUInt8*) romBuffer, kTempFlashPath );
	int index;

	if (theMem.WriteP( 0x04000000, 0x00112233 ))
	{
		(void) ::printf( "A fault occurred writing at 0x04000000\n"  );
	}
	if (theMem.WriteP( 0x04000004, 0x44556677 ))
	{
		(void) ::printf( "A fault occurred writing at 0x04000004\n"  );
	}

	Boolean fault = false;
	for (index = 0; index <= 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( 0x04000000 + index, fault );
		if (fault)
		{
			(void) ::printf( "A fault occurred reading at %i\n", index  );
		}
		::printf( "%i: %.8X\n", index, (unsigned int) theWord );
	}
	
	KUInt8 theByte;
	for (index = 0; index < 8; index++)
	{
		if (theMem.ReadBP( 0x04000000 + index, theByte ))
		{
			(void) ::printf( "A fault occurred reading at %i\n", index  );
		}
		::printf( "%i: %.2X\n", index, (unsigned int) theByte );
	}

	for (index = 0; index < 8; index++)
	{
		if (theMem.WriteBP( 0x04000000 + index, ((index + 8) * 16) + index + 8 ))
		{
			(void) ::printf( "A fault occurred writing at %i\n", index  );
		}
	}

	for (index = 0; index <= 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( 0x04000000 + index, fault );
		if (fault)
		{
			(void) ::printf( "A fault occurred reading at %i\n", index  );
		}
		::printf( "%i: %.8X\n", index, (unsigned int) theWord );
	}

	(void) ::unlink( kTempFlashPath );
	::free( romBuffer );
}

// -------------------------------------------------------------------------- //
//  * FlashTest( void )
// -------------------------------------------------------------------------- //
void
UMemoryTests::FlashTest( void )
{
	TStdOutLog theLog;
	KUInt8* romBuffer = (KUInt8*) malloc(TMemoryConsts::kLowROMEnd);
	TMemory theMem( &theLog, (KUInt8*) romBuffer, kTempFlashPath );
	int index;

	if (theMem.WriteToFlash32Bits(
			0x00112233, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	if (theMem.WriteToFlash32Bits(
			0x44556677, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 + 4 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1+4\n"  );
	}
	if (theMem.WriteToFlash32Bits(
			0x8899AABB, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 + 8 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1+8\n"  );
	}
	if (theMem.WriteToFlash32Bits(
			0xCCDDEEFF, 0xFFFFFFFF, TMemoryConsts::kFlashBank1 + 12 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1+C\n"  );
	}

	Boolean fault = false;
	for (index = 0; index < 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( TMemoryConsts::kFlashBank1 + (4*index), fault );
		if (fault)
		{
			(void) ::printf( "A fault occurred reading at %i\n", index  );
		}
		::printf( "%i: %.8X\n", index, (unsigned int) theWord );
	}
	
	KUInt8 theByte;
	for (index = 0; index < 16; index++)
	{
		if (theMem.ReadBP( TMemoryConsts::kFlashBank1 + index, theByte ))
		{
			(void) ::printf( "A fault occurred reading at %i\n", index  );
		}
		::printf( "%i: %.2X\n", index, (unsigned int) theByte );
	}

	if (theMem.WriteToFlash16Bits(
			0xFFEEDDCC, 0xFFFF0000, TMemoryConsts::kFlashBank1 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	if (theMem.WriteToFlash16Bits(
			0xBBAA9988, 0xFFFF0000, TMemoryConsts::kFlashBank1 + 4 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	if (theMem.WriteToFlash16Bits(
			0x77665544, 0xFFFF0000, TMemoryConsts::kFlashBank1 + 8 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	if (theMem.WriteToFlash16Bits(
			0x33221100, 0xFFFF0000, TMemoryConsts::kFlashBank1 + 12 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	for (index = 0; index < 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( TMemoryConsts::kFlashBank1 + (4*index), fault );
		if (fault)
		{
			(void) ::printf( "A fault occurred reading at %i\n", index  );
		}
		::printf( "%i: %.8X\n", index, (unsigned int) theWord );
	}

	if (theMem.WriteToFlash16Bits(
			0x33221100, 0x0000FFFF, TMemoryConsts::kFlashBank1 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	if (theMem.WriteToFlash16Bits(
			0x77665544, 0x0000FFFF, TMemoryConsts::kFlashBank1 + 4 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	if (theMem.WriteToFlash16Bits(
			0xBBAA9988, 0x0000FFFF, TMemoryConsts::kFlashBank1 + 8 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	if (theMem.WriteToFlash16Bits(
			0xFFEEDDCC, 0x0000FFFF, TMemoryConsts::kFlashBank1 + 12 ))
	{
		(void) ::printf( "A fault occurred writing to flash at bank1\n"  );
	}
	for (index = 0; index < 4; index++)
	{
		KUInt32 theWord = theMem.ReadP( TMemoryConsts::kFlashBank1 + (4*index), fault );
		if (fault)
		{
			(void) ::printf( "A fault occurred reading at %i\n", index  );
		}
		::printf( "%i: %.8X\n", index, (unsigned int) theWord );
	}

	(void) ::unlink( kTempFlashPath );
	::free( romBuffer );
}

// ========================================================= //
// You are in a maze of little twisting passages, all alike. //
// ========================================================= //
