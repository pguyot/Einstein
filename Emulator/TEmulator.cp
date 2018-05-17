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
#include "Serial/TSerialPortManager.h"
#if TARGET_OS_MAC
#include "Serial/TPipesSerialPortManager.h"
#include "Serial/TPtySerialPortManager.h"
#include "Serial/TBasiliskIISerialPortManager.h"
#endif
#include "TInterruptManager.h"
#include "TDMAManager.h"
#include "Platform/TPlatformManager.h"
#include "Files/TFileManager.h"

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
			KUInt32 inRAMSize, /* = 4194304 */
			TSerialPortManager* inExtrSerialPortManager)
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
	mDMAManager = new TDMAManager(inLog, this, &mMemory, mInterruptManager);
	mPlatformManager = new TPlatformManager( inLog, inScreenManager );

	if (inExtrSerialPortManager) {
		mExternalPort = inExtrSerialPortManager;
	} else {
		mExternalPort = new TSerialPortManager(
			inLog,
			TSerialPortManager::kExternalSerialPort);
	}
	mExternalPort->run(mInterruptManager, mDMAManager, &mMemory);

	mInfraredPort = new TSerialPortManager(
		inLog,
		TSerialPortManager::kInfraredSerialPort);
	mInfraredPort->run(mInterruptManager, mDMAManager, &mMemory);

	mBuiltInExtraPort = new TSerialPortManager(
		inLog,
		TSerialPortManager::kBuiltInExtraSerialPort);
	mBuiltInExtraPort->run(mInterruptManager, mDMAManager, &mMemory);

	mModemPort = new TSerialPortManager(
		inLog,
		TSerialPortManager::kModemSerialPort);
	mModemPort->run(mInterruptManager, mDMAManager, &mMemory);

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
//  * TEmulator( void )
// -------------------------------------------------------------------------- //
TEmulator::TEmulator(
					 TLog* inLog,
					 KUInt8* inROMImageBuffer,
					 const char* inFlashPath,
					 KUInt32 inRAMSize)
	:
		mMemory( inLog, inROMImageBuffer, inFlashPath, inRAMSize ),
		mProcessor( inLog, &mMemory ),
		mInterruptManager( nil ),
		mDMAManager( nil ),
		mPlatformManager( nil ),
		mExternalPort( nil ),
		mInfraredPort( nil ),
		mBuiltInExtraPort( nil ),
		mModemPort( nil ),
		mNetworkManager( nil ),
		mSoundManager( nil ),
		mScreenManager( nil ),
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
	mDMAManager = new TDMAManager(inLog, this, &mMemory, mInterruptManager);
	mPlatformManager = new TPlatformManager( inLog, nil );
	
	mNewtonID[0] = kMyNewtonIDHigh;
	mNewtonID[1] = kMyNewtonIDLow;
	
	mMemory.SetEmulator( this );
	
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
		// We can insert a try....catch block here to trace all CPU mode changes
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
//  * SystemBootUND( KUInt32 )
// -------------------------------------------------------------------------- //
void
TEmulator::SystemBootUND( KUInt32 inPAddr )
{
	// Just log the string.
	if (mLog)
	{
		KUInt8 theString[] = "SystemBoot";
		printf("%s\n", theString);
		mLog->LogLine( (const char*) theString );
	}
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
		ssize_t index = ::strlen( (const char*) theString);
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
//  * TapFileCntlUND( KUInt32 )
// -------------------------------------------------------------------------- //
void
TEmulator::TapFileCntlUND( KUInt32 inPAddr )
{
	enum {
		do_sys_open = 0x10,
		do_sys_close = 0x11,
		do_sys_istty = 0x12,
		do_sys_read = 0x13,
		do_sys_write = 0x14,
		do_sys_set_input_notify = 0x15,
		do_sys_seek = 0x16,
		do_sys_flen = 0x17,
	};

	KSInt32 result = -1;
	
	if (mFileManager) {
		KUInt32 command = mProcessor.GetRegister(TARMProcessor::kR0);
		KUInt32 args = mProcessor.GetRegister(TARMProcessor::kR1);

		if (command == do_sys_open) {
			KUInt32 filenameAddress;
			KUInt32 modeIdx = 0;
			mMemory.Read(args, filenameAddress);
			mMemory.Read(args + 4, modeIdx);
			
			ssize_t index = 0;
			KUInt8 filename[512];
			
			do {
				if (mMemory.ReadBP( filenameAddress++, filename[index] ))
				{
					filename[index] = 0;
					break;
				}
			} while (filename[index++] != 0);
			filename[index] = 0;
			
			// XXX: make sure modeIdx doesn't exceed this.
			const char *modes[] = { "r", "rb", "r+", "r+b", "w", "wb", "w+", "w+b", "a", "ab", "a+", "a+b" };
			
			result = mFileManager->do_sys_open((const char*)filename, modes[modeIdx]);
			if (result > 0) {
				mProcessor.SetRegister(TARMProcessor::kR7, 8);
			}
		}
		else {
			// All other commands have a fp for arg1...
			KUInt32 fp = 0;
			mMemory.Read(args, fp);
			
			// Single argument calls
			if (command == do_sys_close) {
				result = mFileManager->do_sys_close(fp);
			}
			else if (command == do_sys_istty) {
				result = mFileManager->do_sys_istty(fp);
			}
			else if (command == do_sys_flen) {
				result = mFileManager->do_sys_flen(fp);
			}
			
			// Two argument calls
			else if (command == do_sys_set_input_notify || command == do_sys_seek) {
				KUInt32 arg2 = 0;
				mMemory.Read(args + 4, arg2);
				
				if (command == do_sys_set_input_notify) {
					result = mFileManager->do_sys_set_input_notify(fp, arg2);
				}
				else if (command == do_sys_seek) {
					result = mFileManager->do_sys_seek(fp, arg2);
				}
			}
			
			// Three argument calls
			else if (command == do_sys_read || command == do_sys_write) {
				KUInt32 bufAddress = 0;
				KUInt32 nbyte = 0;
				
				mMemory.Read(args + 4, bufAddress);
				mMemory.Read(args + 8, nbyte);
				
				if (command == do_sys_read) {
					char *buffer = (char *)::calloc(nbyte, 1);
					KSInt32 amount = mFileManager->do_sys_read(fp, buffer, nbyte);
					// 0 if the call is successful.
					// The same value as nbyte if the call has failed and EOF is assumed.
					// A smaller value than nbyte if the call was partially successful. No error is assumed, but the buffer has not been filled.
					if (amount == -1) {
						result = nbyte;
					}
					else {
						if (amount == 0) {
							buffer[0] = 0x04; // end of transmission
							amount = 1;
						}

						result = nbyte - amount;
					}

					KSInt32 index = 0;
					while (index < amount) {
						if (mMemory.WriteB( bufAddress + index, buffer[index] )) {
							break;
						}
						index++;
					}

					free(buffer);
				}
				else if (command == do_sys_write) {
					KUInt32 index = 0;
					KUInt8 buffer[nbyte];
					
					while (index < nbyte) {
						if (mMemory.ReadB( bufAddress + index, buffer[index] )) {
							break;
						}
						index++;
					}
					
					KSInt32 amount = mFileManager->do_sys_write(fp, buffer, nbyte);
					
					// 0 if the call is successful
					// the number of bytes that are not written, if there is an error.
					if (amount == -1) {
						amount = 0;
					}
					result = nbyte - amount;
				}
			}
			
			// Unhandled :(
			else {
				fprintf(stderr, "unknown TapFileCntl command: 0x%02x\n", command);
				BreakInMonitor();
				result = -1;
			}
		}
	}

	mProcessor.SetRegister(TARMProcessor::kR0, result);
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
