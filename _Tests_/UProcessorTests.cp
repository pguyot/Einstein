// ==============================
// Fichier:			UProcessorTests.cp
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			27/7/2004
// Tabulation:		4 espaces
// 
// Copyright:		© 2004 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: UProcessorTests.cp 151 2006-01-13 16:15:33Z paul $
// ===========

#include <K/Defines/KDefinitions.h>
#include "UProcessorTests.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <unistd.h>

// K
#include <K/Defines/UByteSex.h>

// Einstein
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/Log/TStdOutLog.h"
#include "Emulator/TMemory.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/JIT/JIT.h"
#include "Emulator/JIT/TJITPage.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#define kTempFlashPath "/tmp/EinsteinTests.flash"

// -------------------------------------------------------------------------- //
//  * ExecuteInstruction( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstruction( const char* inHexWord )
{
	KUInt32 theInstruction;
	if (inHexWord == nil)
	{
		(void) ::printf( "This test requires an instruction in hexa.\n" );
	} else if (::sscanf(
					inHexWord,
					"%X",
					(unsigned int*) &theInstruction ) != 1) {
		(void) ::printf( "Can't parse instruction (%s).\n", inHexWord );
	} else {
		TStdOutLog theLog;
		KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
		((KUInt32*) rom)[0] = theInstruction;
		TMemory theMem( &theLog, rom, kTempFlashPath );
		TARMProcessor theProcessor( &theLog, &theMem );
		theMem.GetJITObject()->Step( &theProcessor, 1 );
		theProcessor.PrintRegisters();
		(void) ::unlink( kTempFlashPath );
		::free( rom );
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteInstructionState1( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstructionState1( const char* inHexWord )
{
	KUInt32 theInstruction;
	if (inHexWord == nil)
	{
		(void) ::printf( "This test requires an instruction in hexa.\n" );
	} else if (::sscanf(
					inHexWord,
					"%X",
					(unsigned int*) &theInstruction ) != 1) {
		(void) ::printf( "Can't parse instruction (%s).\n", inHexWord );
	} else {
		TStdOutLog theLog;
		KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
		((KUInt32*) rom)[0] = theInstruction;
		TMemory theMem( &theLog, rom, kTempFlashPath );
		TARMProcessor theProcessor( &theLog, &theMem );
		theProcessor.SetRegister( 0, 0x01020304 );
		theProcessor.SetRegister( 1, 0x05060708 );
		theProcessor.SetRegister( 2, 0x090A0B0C );
		theProcessor.SetRegister( 3, 0x0D0E0F10 );
		theProcessor.SetRegister( 4, 0x11121314 );
		theProcessor.SetRegister( 5, 0x15161718 );
		theProcessor.SetRegister( 6, 0x191A1B1C );
		theProcessor.SetRegister( 7, 0x1D1E1F20 );
		theProcessor.SetRegister( 8, 0x21222324 );
		theProcessor.SetRegister( 9, 0x25262728 );
		theProcessor.SetRegister( 10, 0x292A2B2C );
		theProcessor.SetRegister( 11, 0x2D2E2F30 );
		theProcessor.SetRegister( 12, 0x31323334 );
		theProcessor.SetRegister( 13, 0x35363738 );
		theProcessor.SetRegister( 14, 0x393A3B3C );
//		theProcessor.SetRegister( 15, 0x00000004 );
		theMem.GetJITObject()->Step( &theProcessor, 1 );
		theProcessor.PrintRegisters();
		(void) ::unlink( kTempFlashPath );
		::free( rom );
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteInstructionState2( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstructionState2( const char* inHexWord )
{
	KUInt32 theInstruction;
	if (inHexWord == nil)
	{
		(void) ::printf( "This test requires an instruction in hexa.\n" );
	} else if (::sscanf(
					inHexWord,
					"%X",
					(unsigned int*) &theInstruction ) != 1) {
		(void) ::printf( "Can't parse instruction (%s).\n", inHexWord );
	} else {
		TStdOutLog theLog;
		KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
		((KUInt32*) rom)[0] = theInstruction;
		TMemory theMem( &theLog, rom, kTempFlashPath );
		TARMProcessor theProcessor( &theLog, &theMem );
		theProcessor.SetRegister( 3, 0x00000020 );
		theProcessor.SetRegister( 12, 0xFFFFFFFF );
//		theProcessor.SetRegister( 15, 0x00000004 );
		theMem.GetJITObject()->Step( &theProcessor, 1 );
		theProcessor.PrintRegisters();
		(void) ::unlink( kTempFlashPath );
		::free( rom );
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteTwoInstructions( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteTwoInstructions( const char* inHexWords )
{
	KUInt32 theInstructions[2];
	if (inHexWords == nil)
	{
		(void) ::printf( "This test requires two instructions in hexa.\n" );
	} else if (::sscanf(
					inHexWords,
					"%X-%X",
					(unsigned int*) &theInstructions[0],
					(unsigned int*) &theInstructions[1] ) != 2) {
		(void) ::printf( "Can't parse instructions (%s).\n", inHexWords );
	} else {
		TStdOutLog theLog;
		KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
		((KUInt32*) rom)[0] = theInstructions[0];
		((KUInt32*) rom)[1] = theInstructions[1];
		TMemory theMem( &theLog, rom, kTempFlashPath );
		TARMProcessor theProcessor( &theLog, &theMem );
		theMem.GetJITObject()->Step( &theProcessor, 2 );
		theProcessor.PrintRegisters();
		(void) ::unlink( kTempFlashPath );
		::free( rom );
	}
}

// -------------------------------------------------------------------------- //
//  * Step( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::Step( const char* inCount )
{
	KUInt32 count;
	if (inCount == nil)
	{
		(void) ::printf( "This test requires a number of steps in decimal.\n" );
	} else if (::sscanf( inCount, "%d", (unsigned int*) &count ) != 1) {
		(void) ::printf( "Can't parse number of steps (%s).\n", inCount );
	} else {
		// Load the ROM.
		TStdOutLog theLog;
		TFlatROMImageWithREX theROM(
			"../../_Data_/717006", "../../_Data_/Einstein.rex", "717006" );
		TMemory theMem( &theLog, &theROM, kTempFlashPath );
		TARMProcessor theProcessor( &theLog, &theMem );
		TInterruptManager theInterruptManager( &theLog, &theProcessor );
		theMem.SetInterruptManager( &theInterruptManager );
		theMem.GetJITObject()->Step( &theProcessor, count );
		theProcessor.PrintRegisters();
		(void) ::unlink( kTempFlashPath );
	}
}

// ========================================================================== //
// APL is a mistake, carried through to perfection.  It is the language of    //
// the future for the programming techniques of the past: it creates a new    //
// generation of coding bums.                                                 //
//               -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5  //
// ========================================================================== //
