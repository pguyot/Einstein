// ==============================
// File:			UJITLLVMTests.cp
// Project:			Einstein
//
// Copyright 2014 by Paul Guyot (pguyot@kallisys.net).
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

#include <K/Defines/KDefinitions.h>
#include "JIT.h"

#ifdef JITTARGET_LLVM

#include "UJITLLVMTests.h"

// Einstein
#include "Emulator/TEmulator.h"
#include "Emulator/JIT/TJITPage.h"
#include "Emulator/JIT/LLVM/TJITLLVMTranslator.h"
#include "Emulator/Log/TLog.h"

// llvm
#include <llvm/IR/Module.h>

using namespace llvm;

// -------------------------------------------------------------------------- //
// Constants
// -------------------------------------------------------------------------- //

#if TARGET_OS_WIN32
#define kTempFlashPath "c:/EinsteinTests.flash"
#else
#define kTempFlashPath "/tmp/EinsteinTests.flash"
#endif

// -------------------------------------------------------------------------- //
//  * TranslateEntryPoint( const char*, TLog* )
// -------------------------------------------------------------------------- //
void
UJITLLVMTests::TranslateEntryPoint( const char* inHexWords, TLog* inLog ) {
	if (inHexWords == nil)
	{
		(void) ::printf( "This test requires code in hexa\n" );
	} else {
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

		TMemory theMem( inLog, rom, kTempFlashPath );
		TJITLLVMPage* page = theMem.GetJITObject()->GetPage(0);
		TJITLLVMTranslator tr(*page);
		
		Module* module = new Module("", getGlobalContext());
		(void) tr.TranslateEntryPoint(theMem, 0, module);
		
		std::string buffer;
		raw_string_ostream stream(buffer);
		module->print(stream, nullptr);
		inLog->LogLine(stream.str().c_str());

		(void) ::unlink( kTempFlashPath );
		::free( rom );
	}
}

// -------------------------------------------------------------------------- //
//  * TranslateInstruction( const char*, TLog* )
// -------------------------------------------------------------------------- //
void
UJITLLVMTests::TranslateInstruction( const char* inHexWord, TLog* inLog ) {
	KUInt32 theInstruction;
	if (inHexWord == nil)
	{
		(void) ::printf( "This test requires an instruction in hexa.\n" );
	} else if (::sscanf(inHexWord, "%X", (unsigned int*) &theInstruction ) != 1) {
		(void) ::printf( "Can't parse instruction (%s).\n", inHexWord );
	} else {
		KUInt8* rom = (KUInt8*) ::calloc( 8 * 1024 * 1024, 1 );
		((KUInt32*) rom)[0] = theInstruction;
		TMemory theMem( inLog, rom, kTempFlashPath );
		TJITLLVMPage* page = theMem.GetJITObject()->GetPage(0);
		TJITLLVMTranslator tr(*page);

		Module* module = new Module("", getGlobalContext());
		(void) tr.TranslateSingleInstruction(theMem, 0, module);
		
		std::string buffer;
		raw_string_ostream stream(buffer);
		module->print(stream, nullptr);
		inLog->LogLine(stream.str().c_str());
		
		(void) ::unlink( kTempFlashPath );
		::free( rom );
	}
}


#endif