// ==============================
// Fichier:			tests.cp
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			31/1/2004
// Tabulation:		4 espaces
// 
// Copyright:		© 2004-2005 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: tests.cp 151 2006-01-13 16:15:33Z paul $
// ===========

#include <K/Defines/KDefinitions.h>

// ANSI C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tests
#include "tests.h"
#include "UProcessorTests.h"
#include "UScreenTests.h"
#include "UMemoryTests.h"
#include "UHostInfoTests.h"

// ------------------------------------------------------------------------- //
//  * main
// ------------------------------------------------------------------------- //
// Point d'entrée des tests pour une interface cli.
// Syntaxe:
//		EinsteinTests	<nom du test>
int
main( int argc, char* argv[] )
{
	const char* argument = nil;
	if (argc == 3)
	{
		argument = argv[2];
	} else if (argc != 2)
	{
		(void) ::printf( "Erreur de syntaxe.\n" );
		(void) ::printf( "%s <nom du test> [argument]\n", argv[0] );
		::exit(1);
	}
	
	test( argv[1], argument );
		
	return 0;
}

// ------------------------------------------------------------------------- //
//  * test( const char*, const char* )
// ------------------------------------------------------------------------- //
void
test( const char* inTestName, const char* inArgument )
{
	TStdOutLog theLog;
	if (::strcmp(inTestName, "execute-instruction") == 0) {
		// inArgument: instruction to execute.
		UProcessorTests::ExecuteInstruction( inArgument, &theLog );
	} else if (::strcmp(inTestName, "execute-instruction-state1") == 0) {
		// inArgument: instruction to execute.
		UProcessorTests::ExecuteInstructionState1( inArgument, &theLog );
	} else if (::strcmp(inTestName, "execute-instruction-state2") == 0) {
		// inArgument: instruction to execute.
		UProcessorTests::ExecuteInstructionState2( inArgument, &theLog );
	} else if (::strcmp(inTestName, "execute-two-instructions") == 0) {
		// inArgument: instructions to execute.
		UProcessorTests::ExecuteTwoInstructions( inArgument, &theLog );
	} else if (::strcmp(inTestName, "step") == 0) {
		// inArgument: number of steps to perform.
		UProcessorTests::Step( inArgument, &theLog );
	} else if (::strcmp(inTestName, "run-code") == 0) {
		// inArgument: code to execute.
		UProcessorTests::RunCode( inArgument, &theLog );
#ifndef TARGET_OS_MAC
	} else if (::strcmp(inTestName, "screen-x11") == 0) {
		UScreenTests::TestX11();
#endif
	} else if (::strcmp(inTestName, "memory-read-rom") == 0) {
		UMemoryTests::ReadROMTest(&theLog);
	} else if (::strcmp(inTestName, "memory-read-write-ram") == 0) {
		UMemoryTests::ReadWriteRAMTest(&theLog);
	} else if (::strcmp(inTestName, "flash") == 0) {
		UMemoryTests::FlashTest(&theLog);
	} else if (::strcmp(inTestName, "host-info") == 0) {
		UHostInfoTests::HostInfoTest(&theLog);
	} else {
		(void) ::printf( "%s is an unknown test.\n", inTestName );
	}
}

// ============================================================================ //
// Windows Airlines:                                                            //
// The terminal is very neat and clean, the attendants all very attractive, the //
// pilots very capable. The fleet of Learjets the carrier operates is immense.  //
// Your jet takes off without a hitch, pushing above the clouds, and at 20,000  //
// feet it explodes without warning.                                            //
// ============================================================================ //
