// ==============================
// File:			TEmulator.cp
// Project:			Einstein
//
// Copyright 2003-2022 by Paul Guyot (pguyot@kallisys.net).
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

#include "TEmulator.h"

// POSIX
#include <errno.h>
#include <exception>
#include <math.h>
#include <memory>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#if !TARGET_OS_WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

// K
#include <K/Streams/TFileStream.h>
#include <K/Streams/TRandomAccessStream.h>
#include <K/Streams/TStream.h>

// Einstein
#include "TDMAManager.h"
#include "TInterruptManager.h"
#include "Files/TFileManager.h"
#include "JIT/JIT.h"
#include "JIT/TJITStatistics.h"
#include "Log/TLog.h"
#include "Monitor/TMonitor.h"
#include "Network/TNetworkManager.h"
#include "PCMCIA/TLinearCard.h"
#include "PCMCIA/TPCMCIAController.h"
#include "Platform/TNewt.h"
#include "Platform/TPlatformManager.h"
#include "Screen/TScreenManager.h"
#include "Sound/TSoundManager.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
// #define kMyNewtonIDHigh		0x00000000
// #define kMyNewtonIDLow		0x020207A5
#define kMyNewtonIDHigh 0x00004E65
#define kMyNewtonIDLow 0x77746F6E

// Version of the state file written by SaveState(). Increment it whenever the
// data written by TransferState() changes, so old files are rejected.
// Version 2: run-control flags are no longer saved.
// Version 3: the platform manager (power state, event and buffer queues).
// Version 4: pending pen samples, sound interrupt masks, volume and buffers.
// Version 5: serial port DMA registers.
// Version 6: PCMCIA controller registers and card state.
static const KUInt32 kStateFileVersion = 6;

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
	KUInt32 inRAMSize /* = 4194304 */,
	TPrinterManager* inPrinterManager /* = nullptr */) :
		SerialPorts(this, inLog),
		mMemory(inLog, inROMImage, inFlashPath, inRAMSize),
		mProcessor(inLog, &mMemory),
		mNetworkManager(inNetworkManager),
		mPrinterManager(inPrinterManager),
		mSoundManager(inSoundManager),
		mScreenManager(inScreenManager),
		mLog(inLog)
{
	mInterruptManager = new TInterruptManager(inLog, &mProcessor);
#ifdef JIT_ENABLE_STATISTICS
	gJITStatistics.SetEmulator(this);
#endif
	mDMAManager = new TDMAManager(inLog, this, &mMemory, mInterruptManager);
	mPlatformManager = new TPlatformManager(inLog, inScreenManager);

	mNewtonID[0] = kMyNewtonIDHigh;
	mNewtonID[1] = kMyNewtonIDLow;

	mMemory.SetEmulator(this);

	mNetworkManager->SetInterruptManager(mInterruptManager);
	mNetworkManager->SetMemory(&mMemory);

	mSoundManager->SetInterruptManager(mInterruptManager);
	mSoundManager->SetMemory(&mMemory);

	mScreenManager->SetInterruptManager(mInterruptManager);
	mScreenManager->SetMemory(&mMemory);
	mScreenManager->SetPlatformManager(mPlatformManager);

	mPlatformManager->SetEmulator(this);
	mPlatformManager->SetInterruptManager(mInterruptManager);
	mPlatformManager->SetMemory(&mMemory);
	mPlatformManager->SetProcessor(&mProcessor);

	TNewt::SetEmulator(this);

	mProcessor.SetEmulator(this);
}

// -------------------------------------------------------------------------- //
//  * TEmulator( void )
// -------------------------------------------------------------------------- //
TEmulator::TEmulator(
	TLog* inLog,
	KUInt8* inROMImageBuffer,
	const char* inFlashPath,
	KUInt32 inRAMSize) :
		SerialPorts(this, inLog),
		mMemory(inLog, inROMImageBuffer, inFlashPath, inRAMSize),
		mProcessor(inLog, &mMemory),
		mLog(inLog)
{
	mInterruptManager = new TInterruptManager(inLog, &mProcessor);
#ifdef JIT_ENABLE_STATISTICS
	gJITStatistics.SetEmulator(this);
#endif
	mDMAManager = new TDMAManager(inLog, this, &mMemory, mInterruptManager);
	mPlatformManager = new TPlatformManager(inLog, nil);

	mNewtonID[0] = kMyNewtonIDHigh;
	mNewtonID[1] = kMyNewtonIDLow;

	mMemory.SetEmulator(this);

	mPlatformManager->SetEmulator(this);
	mPlatformManager->SetInterruptManager(mInterruptManager);
	mPlatformManager->SetMemory(&mMemory);
	mPlatformManager->SetProcessor(&mProcessor);

	mProcessor.SetEmulator(this);
}

// -------------------------------------------------------------------------- //
//  * ~TEmulator( void )
// -------------------------------------------------------------------------- //
TEmulator::~TEmulator(void)
{
	if (mInterruptManager)
		delete mInterruptManager;
	if (mDMAManager)
		delete mDMAManager;
	if (mPlatformManager)
		delete mPlatformManager;
}

// -------------------------------------------------------------------------- //
//  * Run( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Run(void)
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
				!(theCPSR & TARMProcessor::kPSR_FBit));
			mPaused = false;
			if (!mRunning)
			{
				break;
			}
		}

		// Execute as many instructions as possible.
		if (!mInterrupted)
		{
			mSignal.store(true);
		}
		// We can insert a try....catch block here to trace all CPU mode changes
		mMemory.GetJITObject()->Run(&mProcessor, &mSignal);
	}

	mInterruptManager->SuspendTimer();

	// FIXME: The code below may be harmful when we call the emulator through the monitor!
	// Instead, the caller of this function, or of TMonitor::run() should call the Quit function.
	if (mCallOnQuit)
		mCallOnQuit();

	// end the thread that runs the emulation
}

// -------------------------------------------------------------------------- //
//  * Step( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Step(void)
{
	mRunning = true;
	mPaused = false;
	mBPHalted = false;

	mInterruptManager->ResumeTimer();

	// Execute 1 instruction
	mMemory.GetJITObject()->Step(&mProcessor, 1);

	mInterruptManager->SuspendTimer();
}

// -------------------------------------------------------------------------- //
//  * SystemBootUND( KUInt32 )
// -------------------------------------------------------------------------- //
void
TEmulator::SystemBootUND(KUInt32 inPAddr)
{
	(void) inPAddr;
	// Just log the string.
	if (mLog)
	{
		KUInt8 theString[] = "SystemBoot";
		KPrintf("%s\n", theString);
		mLog->LogLine((const char*) theString);
	}
}

// -------------------------------------------------------------------------- //
//  * DebuggerUND( KUInt32 )
// -------------------------------------------------------------------------- //
void
TEmulator::DebuggerUND(KUInt32 inPAddr)
{
	// If we have a monitor, stop. Otherwise, we'll continue (the OS will
	// very likely restart).
	BreakInMonitor();

	// Just log the string.
	if (mLog)
	{
		// Extract the string.
		KUInt8 theString[512];
		(void) ::snprintf((char*) theString, 511, "DebuggerUND: ");
		ssize_t index = ::strlen((const char*) theString);
		KUInt32 theAddress = inPAddr + 4;
		do
		{
			if (mMemory.ReadBP(theAddress++, theString[index]))
			{
				theString[index] = 0;
				break;
			}
		} while (theString[index++] != 0);

		KPrintf("%s\n", theString);
		mLog->LogLine((const char*) theString);
	}
}

// -------------------------------------------------------------------------- //
//  * TapFileCntlUND( KUInt32 )
// -------------------------------------------------------------------------- //
void
TEmulator::TapFileCntlUND(KUInt32 inPAddr)
{
	(void) inPAddr;

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

	if (mFileManager)
	{
		KUInt32 command = mProcessor.GetRegister(TARMProcessor::kR0);
		KUInt32 args = mProcessor.GetRegister(TARMProcessor::kR1);

		if (command == do_sys_open)
		{
			KUInt32 filenameAddress;
			KUInt32 modeIdx = 0;
			mMemory.Read(args, filenameAddress);
			mMemory.Read(args + 4, modeIdx);

			ssize_t index = 0;
			KUInt8 filename[512];

			do
			{
				if (mMemory.ReadBP(filenameAddress++, filename[index]))
				{
					filename[index] = 0;
					break;
				}
			} while (filename[index++] != 0);
			filename[index] = 0;

			// XXX: make sure modeIdx doesn't exceed this.
			const char* modes[] = { "r", "rb", "r+", "r+b", "w", "wb", "w+", "w+b", "a", "ab", "a+", "a+b" };

			result = mFileManager->do_sys_open((const char*) filename, modes[modeIdx]);
			if (result > 0)
			{
				mProcessor.SetRegister(TARMProcessor::kR7, 8);
			}
		} else
		{
			// All other commands have a fp for arg1...
			KUInt32 fp = 0;
			mMemory.Read(args, fp);

			// Single argument calls
			if (command == do_sys_close)
			{
				result = mFileManager->do_sys_close(fp);
			} else if (command == do_sys_istty)
			{
				result = mFileManager->do_sys_istty(fp);
			} else if (command == do_sys_flen)
			{
				result = mFileManager->do_sys_flen(fp);
			}

			// Two argument calls
			else if (command == do_sys_set_input_notify || command == do_sys_seek)
			{
				KUInt32 arg2 = 0;
				mMemory.Read(args + 4, arg2);

				if (command == do_sys_set_input_notify)
				{
					result = mFileManager->do_sys_set_input_notify(fp, arg2);
				} else if (command == do_sys_seek)
				{
					result = mFileManager->do_sys_seek(fp, arg2);
				}
			}

			// Three argument calls
			else if (command == do_sys_read || command == do_sys_write)
			{
				KUInt32 bufAddress = 0;
				KUInt32 nbyte = 0;

				mMemory.Read(args + 4, bufAddress);
				mMemory.Read(args + 8, nbyte);

				if (command == do_sys_read)
				{
					char* buffer = (char*) ::calloc(nbyte, 1);
					KSInt32 amount = mFileManager->do_sys_read(fp, buffer, nbyte);
					// 0 if the call is successful.
					// The same value as nbyte if the call has failed and EOF is assumed.
					// A smaller value than nbyte if the call was partially successful. No error is assumed, but the buffer has not been filled.
					if (amount == -1)
					{
						result = nbyte;
					} else
					{
						if (amount == 0)
						{
							buffer[0] = 0x04; // end of transmission
							amount = 1;
						}

						result = nbyte - amount;
					}

					KSInt32 index = 0;
					while (index < amount)
					{
						if (mMemory.WriteB(bufAddress + index, buffer[index]))
						{
							break;
						}
						index++;
					}

					free(buffer);
				} else if (command == do_sys_write)
				{
					KUInt32 index = 0;
					KUInt8* buffer = (KUInt8*) malloc(nbyte);

					while (index < nbyte)
					{
						if (mMemory.ReadB(bufAddress + index, buffer[index]))
						{
							break;
						}
						index++;
					}

					KSInt32 amount = mFileManager->do_sys_write(fp, buffer, nbyte);

					// 0 if the call is successful
					// the number of bytes that are not written, if there is an error.
					if (amount == -1)
					{
						amount = 0;
					}
					result = nbyte - amount;
					free(buffer);
				}
			}

			// Unhandled :(
			else
			{
				KPrintf("unknown TapFileCntl command: 0x%02x\n", (unsigned) command);
				BreakInMonitor();
				result = -1;
			}
		}
	}

	mProcessor.SetRegister(TARMProcessor::kR0, result);
}

// -------------------------------------------------------------------------- //
//  * BreakInMonitor( const char* msg = NULL )
// -------------------------------------------------------------------------- //
void
TEmulator::BreakInMonitor(const char* msg)
{
	if (mMonitor)
	{
		mSignal.store(false);
		mRunning = false;
		mBPHalted = true;
		mBPID = 0;
		mInterruptManager->WakeEmulatorThread();
		if (msg != NULL)
			mMonitor->PrintLine(msg, 0);
	}
}

// -------------------------------------------------------------------------- //
//  * SaveState( const char* inPath ) const
// -------------------------------------------------------------------------- //
Boolean
TEmulator::SaveState(const char* inPath)
{
	try
	{
		// Open the file for writing.
		std::unique_ptr<TStream> theStream(new TFileStream(inPath, "wb"));
		theStream->Version(kStateFileVersion);
		theStream->PutInt32BE('EINI');
		theStream->PutInt32BE('SNAP');
		theStream->PutInt32BE(theStream->Version());
		TransferState(theStream.get());
	} catch (const std::exception& e)
	{
		KPrintf("Could not save the emulator state to %s (%s).\n", inPath, e.what());
		return false;
	}
	return true;
}

// -------------------------------------------------------------------------- //
//  * LoadState( const char* inPath ) const
// -------------------------------------------------------------------------- //
Boolean
TEmulator::LoadState(const char* inPath)
{
	try
	{
		// Open the file for Reading.
		std::unique_ptr<TStream> theStream(new TFileStream(inPath, "rb"));
		if (theStream->GetInt32BE() != 'EINI')
		{
			KPrintf("This is not a file created by Einstein!\n");
			return false;
		}
		if (theStream->GetInt32BE() != 'SNAP')
		{
			KPrintf("This is not an Einstein State file!\n");
			return false;
		}
		theStream->Version(theStream->GetInt32BE());
		if (theStream->Version() != kStateFileVersion)
		{
			KPrintf("This Einstein State file is not supported. Please upgarde your Einstein version.\n");
			return false;
		}
		TransferState(theStream.get());
	} catch (const std::exception& e)
	{
		// The file was cut off or could not be read. Part of the state may
		// have been loaded, so at least drop the translated code.
		mMemory.GetJITObject()->InvalidateAll();
		KPrintf("Could not load the emulator state from %s (%s).\n", inPath, e.what());
		return false;
	}
	return true;
}

// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TEmulator::TransferState(TStream* inStream)
{
	// Keep the serial driver threads from changing memory and registers
	// while we save or load. The guard resumes them when we leave, even if
	// reading the file fails with an exception.
	struct SResumeSerialPorts {
		TSerialPorts& fPorts;
		~SResumeSerialPorts() { fPorts.ResumeAll(); }
	};
	SerialPorts.SuspendAll();
	SResumeSerialPorts theResumeGuard { SerialPorts };

	// When writing a file, remember where each section starts.
	TRandomAccessStream* theFile = dynamic_cast<TRandomAccessStream*>(inStream);
	if (inStream->IsWriting())
		mStateSections.clear();
	auto StartSection = [&](const char* inName) {
		if (theFile && inStream->IsWriting())
			mStateSections.push_back({ inName, theFile->GetCursor() });
	};

	// First, save the memory.
	StartSection("memory (registers, ROM, RAM, breakpoints, MMU, flash)");
	mMemory.TransferState(inStream);

	// Then the CPU.
	StartSection("CPU and native primitives");
	mProcessor.TransferState(inStream);

	// And the interrupt manager.
	StartSection("interrupt manager");
	mInterruptManager->TransferState(inStream);

	// And the DMA manager.
	StartSection("DMA manager");
	mDMAManager->TransferState(inStream);

	// The DMA registers of the serial ports.
	StartSection("serial ports");
	SerialPorts.TransferState(inStream);

	// The PCMCIA controllers and the state of the inserted cards.
	StartSection("PCMCIA");
	for (int socketIx = 0; socketIx < kNbSockets; socketIx++)
	{
		TPCMCIAController* theController = mMemory.GetPCMCIAController(socketIx);
		if (theController)
			theController->TransferState(inStream);
	}

	// And the screen content.
	StartSection("screen");
	mScreenManager->TransferState(inStream);

	// The sound manager: interrupt masks and volume.
	StartSection("sound manager");
	mSoundManager->TransferState(inStream);

	// The platform manager: power state, pending events and buffers.
	StartSection("platform manager");
	mPlatformManager->TransferState(inStream);

	// Emulator specific stuff. The run-control flags (mRunning, mPaused, ...)
	// belong to the host thread, not to the emulated machine, and are not saved.
	StartSection("emulator (Newton ID)");
	inStream->TransferInt32ArrayBE(mNewtonID, 2);

	// Show the screen as on or off, as the loaded machine expects it.
	if (inStream->IsReading())
	{
		if (mPlatformManager->IsPowerOn())
			mScreenManager->PowerOnScreen();
		else
			mScreenManager->PowerOffScreen();
	}
}

// -------------------------------------------------------------------------- //
//  * Stop( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Stop(void)
{
	mSignal.store(false);
	mRunning = false;
	mPaused = false;
	mInterruptManager->WakeEmulatorThread();
}

// -------------------------------------------------------------------------- //
//  * Quit( void )
// -------------------------------------------------------------------------- //
void
TEmulator::Quit(void)
{
	Stop();
}

// -------------------------------------------------------------------------- //
//  * SetNewtonID(KUInt32 inID0, KUInt32 inID1)
// -------------------------------------------------------------------------- //
void
TEmulator::SetNewtonID(KUInt32 inID0, KUInt32 inID1)
{
	mNewtonID[0] = inID0;
	mNewtonID[1] = inID1;
	mMemory.ComputeSerialNumber(GetNewtonID());
}

/**
 * Set a callback function that is called when the emulator thread is no longer running.
 * This can be used by the host user interface manager (TCLIApp, TCocoaAppController, etc.)
 * to close the window and quit the app.
 * @param inCallback we can call any kind of function here
 */
void
TEmulator::CallOnQuit(std::function<void()> inCallback)
{
	mCallOnQuit = std::move(inCallback);
}

// ====================================================================== //
// A computer without COBOL and Fortran is like a piece of chocolate cake //
// without ketchup and mustard.                                           //
// ====================================================================== //
