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
#include "JIT/TJITPerformance.h"
#include "Network/TNetworkManager.h"
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
			TNetworkManager* inNetworkManager,
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
		mNetworkManager( inNetworkManager ),
		mSoundManager( inSoundManager ),
		mScreenManager( inScreenManager ),
		mLog( inLog ),
		mMonitor( NULL ),
		mRunning( false ),
		mPaused( false ),
		mBPHalted( false )
{
	mInterruptManager = new TInterruptManager(inLog, &mProcessor);
#ifdef JIT_PERFORMANCE
	branchDestCount.SetEmulator(this);
	branchLinkDestCount.SetEmulator(this);
#endif
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

	mNetworkManager->SetInterruptManager( mInterruptManager );
	mNetworkManager->SetMemory( &mMemory );
	
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
          
		printf("%s\n", theString);
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
TEmulator::SaveState( const char* inPath )
{
	// Open the file for writing.
	TStream* theStream = new TFileStream( inPath, "wb" );
	theStream->Version(1);
	theStream->PutInt32BE('EINI');
	theStream->PutInt32BE('SNAP');
	theStream->PutInt32BE(theStream->Version());
	TransferState( theStream );
	delete theStream;
}

// -------------------------------------------------------------------------- //
//  * LoadState( const char* inPath ) const
// -------------------------------------------------------------------------- //
void
TEmulator::LoadState( const char* inPath )
{
	KUInt32 id, type;
	
	// Open the file for Reading.
	TStream* theStream = new TFileStream( inPath, "rb" );
	id = theStream->GetInt32BE();
	if (id!='EINI') {
		fprintf(stderr, "This is not a file created by Einstein!\n");
		return;
	}
	type = theStream->GetInt32BE();
	if (type!='SNAP') {
		fprintf(stderr, "This is not an Einstein State file!\n");
		return;
	}
	theStream->Version(theStream->GetInt32BE());
	if (theStream->Version()!=1) {
		fprintf(stderr, "This Einstein State file is not supported. Please upgarde your Einstein version.\n");
		return;
	}
	TransferState( theStream );
	delete theStream;
}


// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TEmulator::TransferState( TStream* inStream )
{
	// First, save the memory.
	mMemory.TransferState( inStream );
	
	// Then the CPU.
	mProcessor.TransferState( inStream );
	
	// And the interrupt manager.
	mInterruptManager->TransferState( inStream );
  
	// And the interrupt manager.
	mDMAManager->TransferState( inStream );
	
	// And the screen content.
	mScreenManager->TransferState( inStream );
	
	// Emulator specific stuff.
	inStream->TransferInt32ArrayBE(mNewtonID, 2);
	inStream->TransferInt32BE( mSignal );
	inStream->TransferInt32BE( mInterrupted );
	inStream->TransferInt32BE( mRunning );
	inStream->TransferInt32BE( mPaused );
	inStream->TransferInt32BE( mBPHalted );
	inStream->TransferInt16BE( mBPID );
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


void TEmulator::SetNewtonID(KUInt32 inID0, KUInt32 inID1) {
	mNewtonID[0] = inID0;
	mNewtonID[1] = inID1;
	mMemory.ComputeSerialNumber( GetNewtonID() );
}

// ====================================================================== //
// A computer without COBOL and Fortran is like a piece of chocolate cake //
// without ketchup and mustard.                                           //
// ====================================================================== //
