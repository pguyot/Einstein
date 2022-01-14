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

#if !TARGET_OS_WIN32
	#include <unistd.h>
#endif

// K
#include <K/Defines/UByteSex.h>

// Einstein
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/TMemory.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/JIT/JIT.h"
#include "Emulator/JIT/TJITPage.h"
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/Screen/TNullScreenManager.h"


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#if TARGET_OS_WIN32
	#define kTempFlashPath "c:/EinsteinTests.flash"
#else
	#define kTempFlashPath "/tmp/EinsteinTests.flash"
#endif

// -------------------------------------------------------------------------- //
//  * ExecuteInstruction( KUInt32, std::function<void (TARMProcessor& proc)> )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstruction( KUInt32 inInstruction, std::function<void (TARMProcessor& proc)> inTestFunction, TLog* inLog )
{
    KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
    ((KUInt32*) rom)[0] = inInstruction;
    TMemory theMem( inLog, rom, kTempFlashPath );
    TARMProcessor theProcessor( inLog, &theMem );
    theMem.GetJITObject()->Step( &theProcessor, 1 );
    (void) ::unlink( kTempFlashPath );
    ::free( rom );

    inTestFunction(theProcessor);
}

// -------------------------------------------------------------------------- //
//  * ExecuteInstruction( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstruction( const char* inHexWord, TLog* inLog )
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
	    ExecuteInstruction(theInstruction, [] (TARMProcessor& proc) {
	        proc.PrintRegisters();
	    }, inLog);
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteInstructionState1( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstructionState1( KUInt32 inInstruction, std::function<void (TARMProcessor& proc)> inTestFunction,  TLog* inLog )
{
    KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
    ((KUInt32*) rom)[0] = inInstruction;
    TMemory theMem( inLog, rom, kTempFlashPath );
    TARMProcessor theProcessor( inLog, &theMem );
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
    (void) ::unlink( kTempFlashPath );
    ::free( rom );

    inTestFunction(theProcessor);
}

// -------------------------------------------------------------------------- //
//  * ExecuteInstructionState1( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstructionState1( const char* inHexWord, TLog* inLog )
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
	    ExecuteInstructionState1(theInstruction, [] (TARMProcessor& proc) {
	        proc.PrintRegisters();
	    }, inLog);
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteInstructionState2( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstructionState2( KUInt32 inInstruction, std::function<void (TARMProcessor& proc)> inTestFunction,  TLog* inLog )
{
    KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
    ((KUInt32*) rom)[0] = inInstruction;
    TMemory theMem( inLog, rom, kTempFlashPath );
    TARMProcessor theProcessor( inLog, &theMem );
    theProcessor.SetRegister( 3, 0x00000020 );
    theProcessor.SetRegister( 12, 0xFFFFFFFF );
//		theProcessor.SetRegister( 15, 0x00000004 );
    theMem.GetJITObject()->Step( &theProcessor, 1 );
    theProcessor.PrintRegisters();
    (void) ::unlink( kTempFlashPath );
    ::free( rom );

    inTestFunction(theProcessor);
}

// -------------------------------------------------------------------------- //
//  * ExecuteInstructionState2( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteInstructionState2( const char* inHexWord, TLog* inLog )
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
	    ExecuteInstructionState2(theInstruction, [] (TARMProcessor& proc) {
	        proc.PrintRegisters();
	    }, inLog);
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteTwoInstructions( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteTwoInstructions( const char* inHexWords, TLog* inLog )
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
	    ExecuteTwoInstructions(theInstructions[0], theInstructions[1], [] (TARMProcessor& proc) {
	        proc.PrintRegisters();
	    }, inLog);
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteTwoInstructions( KUInt32, KUInt32, std::function<void (TARMProcessor&), TLog* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::ExecuteTwoInstructions( KUInt32 inInstruction1, KUInt32 inInstruction2, std::function<void (TARMProcessor& proc)> inTestFunction, TLog* inLog )
{
    KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
    ((KUInt32*) rom)[0] = inInstruction1;
    ((KUInt32*) rom)[1] = inInstruction2;
    TMemory theMem( inLog, rom, kTempFlashPath );
    TARMProcessor theProcessor( inLog, &theMem );
    theMem.GetJITObject()->Step( &theProcessor, 2 );
    (void) ::unlink( kTempFlashPath );
    ::free( rom );

    inTestFunction(theProcessor);
}

// -------------------------------------------------------------------------- //
//  * RunCode( const char*, TLog* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::RunCode( const char* inHexWords, TLog* inLog ) {
    RunCode(inHexWords, [] (TARMProcessor& proc) {
        proc.PrintRegisters();
    }, inLog);
}


// -------------------------------------------------------------------------- //
//  * RunCode( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::RunCode( const char* inHexWords, std::function<void (TARMProcessor& proc)> inTestFunction, TLog* inLog ) {
    KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
    KUInt32* theCodePtr = (KUInt32*) rom;
    int nbBytes;
    while (::sscanf(inHexWords, "%X %n", theCodePtr, &nbBytes) == 1) {
        inHexWords += nbBytes;
        theCodePtr++;
    }
    if (inLog) {
        inLog->FLogLine("Parsed %d instruction(s).", (int) ((theCodePtr - (KUInt32*)rom)));
    }

    TEmulator theEmulator(inLog, rom, kTempFlashPath);
    theEmulator.Run();
    theEmulator.GetProcessor()->PrintRegisters();
    (void) ::unlink( kTempFlashPath );
    ::free( rom );

    inTestFunction(*theEmulator.GetProcessor());
}



// -------------------------------------------------------------------------- //
//  * Step( const char* )
// -------------------------------------------------------------------------- //
void
UProcessorTests::Step( const char* inCount, TLog* inLog )
{
	KUInt32 count;
	if (inCount == nil)
	{
		(void) ::printf( "This test requires a number of steps in decimal.\n" );
	} else if (::sscanf( inCount, "%d", (unsigned int*) &count ) != 1) {
		(void) ::printf( "Can't parse number of steps (%s).\n", inCount );
	} else {
		// Load the ROM.
		TFlatROMImageWithREX theROM(
			"../../_Data_/717006", "../../_Data_/Einstein.rex");
		TNullSoundManager soundManager;
		TUsermodeNetwork networkManager(inLog);
		TNullScreenManager screenManager(inLog);
		TEmulator theEmulator(inLog, &theROM, kTempFlashPath, &soundManager, &screenManager, &networkManager);

		theEmulator.GetMemory()->GetJITObject()->Step( theEmulator.GetProcessor(), count );
		theEmulator.GetProcessor()->PrintRegisters();
		(void) ::unlink( kTempFlashPath );
	}
}

// ========================================================================== //
// APL is a mistake, carried through to perfection.  It is the language of    //
// the future for the programming techniques of the past: it creates a new    //
// generation of coding bums.                                                 //
//               -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5  //
// ========================================================================== //
