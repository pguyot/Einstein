// ==============================
// File:			TEmulator.cp
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

#include <K/Defines/KDefinitions.h>
#include "TEmulator.h"

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#if !TARGET_OS_WIN32
	#include <unistd.h>
	#include <sys/time.h>
#endif

// K
#include <K/Streams/TStream.h>
#include <K/Streams/TFileStream.h>

// Einstein
#include "Log/TLog.h"
#include "JIT/JIT.h"
#include "Sound/TSoundManager.h"
#include "Screen/TScreenManager.h"
#include "PCMCIA/TPCMCIAController.h"
#include "PCMCIA/TLinearCard.h"
#include "Serial/TVoyagerSerialPort.h"
#include "TInterruptManager.h"
#include "TDMAManager.h"
#include "Platform/TPlatformManager.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
//#define kMyNewtonIDHigh		0x00000000
//#define kMyNewtonIDLow		0x020207A5
#define kMyNewtonIDHigh		0x00004E65
#define kMyNewtonIDLow		0x77746F6E

// -------------------------------------------------------------------------- //
//  * TEmulator( void )
// -------------------------------------------------------------------------- //
TEmulator::TEmulator(
			TLog* inLog,
			TROMImage* inROMImage,
			const char* inFlashPath,
			TSoundManager* inSoundManager,
			TScreenManager* inScreenManager,
			KUInt32 inRAMSize /* = 4194304 */ )
	:
		mMemory( inLog, inROMImage, inFlashPath, inRAMSize ),
		mProcessor( inLog, &mMemory ),
		mInterruptManager( nil ),
		mDMAManager( nil ),
		mPlatformManager( nil ),
		mExternalPort( nil ),
		mInfraredPort( nil ),
		mBuiltInExtraPort( nil ),
		mModemPort( nil ),
		mSoundManager( inSoundManager ),
		mScreenManager( inScreenManager ),
		mLog( inLog ),
		mMonitor( NULL ),
		mRunning( false ),
		mPaused( false ),
		mBPHalted( false )
{
	mInterruptManager = new TInterruptManager(inLog, &mProcessor);
	mDMAManager = new TDMAManager(inLog, &mMemory, mInterruptManager);
	mPlatformManager = new TPlatformManager( inLog, inScreenManager );
	mExternalPort = new TVoyagerSerialPort(
		inLog,
		TVoyagerSerialPort::kExternalSerialPort,
		mInterruptManager,
		mDMAManager);
	mInfraredPort = new TVoyagerSerialPort(
		inLog,
		TVoyagerSerialPort::kInfraredSerialPort,
		mInterruptManager,
		mDMAManager);
	mBuiltInExtraPort = new TVoyagerSerialPort(
		inLog,
		TVoyagerSerialPort::kBuiltInExtraSerialPort,
		mInterruptManager,
		mDMAManager);
	mModemPort = new TVoyagerSerialPort(
		inLog,
		TVoyagerSerialPort::kModemSerialPort,
		mInterruptManager,
		mDMAManager);

	mNewtonID[0] = kMyNewtonIDHigh;
	mNewtonID[1] = kMyNewtonIDLow;

	mMemory.SetEmulator( this );

	mSoundManager->SetInterruptManager( mInterruptManager );
	mSoundManager->SetMemory( &mMemory );
	mScreenManager->SetInterruptManager( mInterruptManager );
	mScreenManager->SetMemory( &mMemory );
	mScreenManager->SetPlatformManager( mPlatformManager );

	mPlatformManager->SetInterruptManager( mInterruptManager );
	mPlatformManager->SetMemory( &mMemory );
	
	mProcessor.SetEmulator( this );
}

// -------------------------------------------------------------------------- //
//  * ~TEmulator( void )
// -------------------------------------------------------------------------- //
TEmulator::~TEmulator( void )
{
	if (mInterruptManager)
		delete mInterruptManager;
	if (mDMAManager)
		delete mDMAManager;
	if (mPlatformManager)
		delete mPlatformManager;
	if (mExternalPort)
		delete mExternalPort;
	if (mInfraredPort)
		delete mInfraredPort;
	if (mBuiltInExtraPort)
		delete mBuiltInExtraPort;
	if (mModemPort)
		delete mModemPort;
}

// -------------------------------------------------------------------------- //
//  * Run( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Run( void )
{
	mRunning = true;
	mBPHalted = false;
	
	mInterruptManager->ResumeTimer();

	while (mRunning)
	{
		if (mPaused)
		{
			KUInt32 theCPSR = mProcessor.GetCPSR();
			mInterruptManager->WaitUntilInterrupt(
					!(theCPSR & TARMProcessor::kPSR_IBit),
					!(theCPSR & TARMProcessor::kPSR_FBit) );
			mPaused = false;
			if (!mRunning)
			{
				break;
			}
		}

		// Execute as many instructions as possible.
		if (!mInterrupted)
		{
			mSignal = true;
		}
		mMemory.GetJITObject()->Run( &mProcessor, &mSignal );
	}
	
	mInterruptManager->SuspendTimer();
}

// -------------------------------------------------------------------------- //
//  * Step( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Step( void )
{
	mRunning = true;
	mPaused = false;
	mBPHalted = false;

	mInterruptManager->ResumeTimer();
		
	// Execute 1 instruction
	mMemory.GetJITObject()->Step( &mProcessor, 1 );
	
	mInterruptManager->SuspendTimer();
}

// -------------------------------------------------------------------------- //
//  * DebuggerUND( KUInt32 )
// -------------------------------------------------------------------------- //
void
TEmulator::DebuggerUND( KUInt32 inPAddr )
{
	// If we have a monitor, stop. Otherwise, we'll continue (the OS will
	// very likely restart).
	BreakInMonitor();

	// Just log the string.
	if (mLog)
	{
		// Extract the string.
		KUInt8 theString[512];
		(void) ::sprintf( (char*) theString, "DebuggerUND: " );
		KUInt32 index = ::strlen( (const char*) theString);
		KUInt32 theAddress = inPAddr + 4;
		do {
			if (mMemory.ReadBP( theAddress++, theString[index] ))
			{
				theString[index] = 0;
				break;
			}
		} while (theString[index++] != 0);
		
		mLog->LogLine( (const char*) theString );
	}
}

// -------------------------------------------------------------------------- //
//  * BreakInMonitor( void )
// -------------------------------------------------------------------------- //
void
TEmulator::BreakInMonitor( void )
{
	if (mMonitor)
	{
		mSignal = false;
		mRunning = false;	
	}
}

// -------------------------------------------------------------------------- //
//  * SaveState( const char* inPath ) const
// -------------------------------------------------------------------------- //
void
TEmulator::SaveState( const char* inPath ) const
{
	// Open the file for writing.
	TStream* theStream = new TFileStream( inPath, "w" );
	SaveState( theStream );
	delete theStream;
}

// -------------------------------------------------------------------------- //
//  * SaveState( TStream* ) const
// -------------------------------------------------------------------------- //
void
TEmulator::SaveState( TStream* inStream ) const
{
	// First, save the memory.
	mMemory.SaveState( inStream );
	
	// Then the CPU.
	mProcessor.SaveState( inStream );
	
	// And the interrupt manager.
	mInterruptManager->SaveState( inStream );
	
	// Emulator specific stuff.
	inStream->PutInt32BE( mRunning );
	inStream->PutInt32BE( mPaused );
	inStream->PutInt32BE( mBPHalted );
	inStream->PutInt16BE( mBPID );
}

// -------------------------------------------------------------------------- //
//  * LoadState( TStream* )
// -------------------------------------------------------------------------- //
void
TEmulator::LoadState( TStream* inStream )
{
	// First, load the memory.
	mMemory.LoadState( inStream );

	// Then the CPU.
	mProcessor.LoadState( inStream );
	
	// And the interrupt manager.
	mInterruptManager->LoadState( inStream );

	// Emulator specific stuff.
	mRunning = inStream->GetInt32BE();
	mPaused = inStream->GetInt32BE();
	mBPHalted = inStream->GetInt32BE();
	mBPID = inStream->GetInt16BE();
}

// -------------------------------------------------------------------------- //
//  * InsertCard( void )
// -------------------------------------------------------------------------- //
void
TEmulator::InsertCard( void )
{
	if (mLog)
	{
		mLog->LogLine( "Insert card" );
	}
	mMemory.GetPCMCIAController(0)->InsertCard( new TLinearCard( 6 * 1024 * 1024 ) );
}

// -------------------------------------------------------------------------- //
//  * Stop( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Stop( void )
{
	mSignal = false;
	mRunning = false;
	mPaused = false;
	mInterruptManager->WakeEmulatorThread();
}

// -------------------------------------------------------------------------- //
//  * Quit( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Quit( void )
{
	Stop();
}

// ====================================================================== //
// A computer without COBOL and Fortran is like a piece of chocolate cake //
// without ketchup and mustard.                                           //
// ====================================================================== //
