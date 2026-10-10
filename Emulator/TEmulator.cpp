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
#include <K/Misc/CRC32.h>
#include <K/Streams/TMemoryStream.h>
#include <K/Streams/TRandomAccessStream.h>
#include <K/Streams/TResetStream.h>
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
#include "PCMCIA/TPCMCIACard.h"
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
// Version 7: read position of the serial number chip.
// Version 8: no ROM; kind of file (fast start or debug) and what it must match
//            (ROM, RAM size, flash, PCMCIA cards); flash only in debug files.
// Version 9: kind and image path of the inserted PCMCIA cards.
// Version 10: screen size must match and is no longer loaded.
// Version 11: CRC32 of the file at the end.
static const KUInt32 kStateFileVersion = 11;

// Longest PCMCIA card image path in a state file. Longer paths are saved
// empty, so the card is not inserted again at a fast start.
static const KUInt32 kMaxStatePathLength = 2048;

// Number of values written by TEmulator::GetStateIdentity().
static const size_t kStateIdentitySize = 5 + 2 * kNbSockets;

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
	Run(mStopCount);
}

// -------------------------------------------------------------------------- //
//  * Run( KUInt32 )
// -------------------------------------------------------------------------- //
void
TEmulator::Run(KUInt32 inStopCount)
{
	mRunning = true;
	// A Stop() between reading the count and setting mRunning would be lost.
	// Stop() counts first, so either we see the new count here, or the loop
	// sees mRunning == false.
	if (mStopCount != inStopCount)
		mRunning = false;
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
//  * WriteStateFile( const char*, const std::vector<KUInt8>& )
// -------------------------------------------------------------------------- //
// Write the state and a CRC32 of it. Every write and the close are checked,
// so a full disk can't leave a cut-off file that looks complete.
static Boolean
WriteStateFile(const char* inPath, const std::vector<KUInt8>& inData)
{
	FILE* theFile = ::fopen(inPath, "wb");
	if (theFile == nullptr)
		return false;
	KUInt32 theCRC = GetCRC32(inData.data(), (KUInt32) inData.size());
	KUInt8 theCRCBytes[4] = {
		(KUInt8) (theCRC >> 24), (KUInt8) (theCRC >> 16),
		(KUInt8) (theCRC >> 8), (KUInt8) theCRC
	};
	Boolean theResult = (::fwrite(inData.data(), 1, inData.size(), theFile) == inData.size())
		&& (::fwrite(theCRCBytes, 1, 4, theFile) == 4)
		&& (::fflush(theFile) == 0);
	if (::fclose(theFile) != 0)
		theResult = false;
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * ReadStateFile( const char*, std::vector<KUInt8>& )
// -------------------------------------------------------------------------- //
// Read a whole state file and check its CRC32, so a damaged or cut-off file
// is refused before anything is loaded. Returns nullptr if the file is fine,
// or what is wrong with it.
static const char*
ReadStateFile(const char* inPath, std::vector<KUInt8>& outData)
{
	outData.clear();
	FILE* theFile = ::fopen(inPath, "rb");
	if (theFile == nullptr)
		return "the file could not be opened";
	long theSize = -1;
	if (::fseek(theFile, 0, SEEK_END) == 0)
		theSize = ::ftell(theFile);
	Boolean theReadOK = (theSize >= 4) && (::fseek(theFile, 0, SEEK_SET) == 0);
	if (theReadOK)
	{
		outData.resize((size_t) theSize);
		theReadOK = (::fread(outData.data(), 1, outData.size(), theFile) == outData.size());
	}
	::fclose(theFile);
	if (!theReadOK)
	{
		outData.clear();
		return "the file is too short or could not be read";
	}
	size_t theDataSize = outData.size() - 4;
	const KUInt8* theCRCBytes = outData.data() + theDataSize;
	KUInt32 theSavedCRC = ((KUInt32) theCRCBytes[0] << 24) | ((KUInt32) theCRCBytes[1] << 16)
		| ((KUInt32) theCRCBytes[2] << 8) | (KUInt32) theCRCBytes[3];
	if (GetCRC32(outData.data(), (KUInt32) theDataSize) != theSavedCRC)
	{
		outData.clear();
		return "the file is damaged (checksum)";
	}
	outData.resize(theDataSize);
	return nullptr;
}

// -------------------------------------------------------------------------- //
//  * SaveState( const char* inPath ) const
// -------------------------------------------------------------------------- //
Boolean
TEmulator::SaveState(const char* inPath, EStateKind inKind)
{
	// Collect the state in memory, then write it to a temporary file and rename
	// it when everything was written, so a file with the final name is always
	// complete.
	std::string theTempPath = std::string(inPath) + ".tmp";
	TMemoryStream theMemoryStream;
	TStream* theStream = &theMemoryStream;
	try
	{
		theStream->Version(kStateFileVersion);
		theStream->PutInt32BE('EINI');
		theStream->PutInt32BE('SNAP');
		theStream->PutInt32BE(theStream->Version());

		// The kind of file, and what it must match to be loaded again.
		theStream->PutInt32BE(inKind);
		std::vector<KUInt32> theIdentity = GetStateIdentity();
		for (KUInt32 theValue : theIdentity)
			theStream->PutInt32BE(theValue);

		// The inserted cards, so a fast start can insert them again.
		for (int socketIx = 0; socketIx < kNbSockets; socketIx++)
		{
			TPCMCIAController* theController = mMemory.GetPCMCIAController(socketIx);
			TPCMCIACard* theCard = theController ? theController->CurrentCard() : nullptr;
			const char* thePath = (theCard && theCard->GetImagePath()) ? theCard->GetImagePath() : "";
			KUInt32 theLength = (KUInt32)::strlen(thePath);
			if (theLength > kMaxStatePathLength)
				theLength = 0;
			theStream->PutInt32BE(theCard ? theCard->GetStateTag() : 0);
			theStream->PutInt32BE(theLength);
			theStream->Write(thePath, &theLength);
		}

		theStream->TransferFlags((inKind == kDebugState) ? kStateIncludesFlash : 0);
		TransferState(theStream);
	} catch (const std::exception& e)
	{
		KPrintf("Could not save the emulator state to %s (%s).\n", inPath, e.what());
		return false;
	}
	if (!WriteStateFile(theTempPath.c_str(), theMemoryStream.GetData()))
	{
		(void) ::remove(theTempPath.c_str());
		KPrintf("Could not save the emulator state to %s (writing failed).\n", inPath);
		return false;
	}
#if TARGET_OS_WIN32
	(void) ::remove(inPath); // rename() does not replace files on Windows
#endif
	if (::rename(theTempPath.c_str(), inPath) != 0)
	{
		(void) ::remove(theTempPath.c_str());
		KPrintf("Could not save the emulator state to %s (rename failed).\n", inPath);
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
	std::vector<KUInt8> theData;
	const char* theError = ReadStateFile(inPath, theData);
	if (theError != nullptr)
	{
		KPrintf("Not loading the state from %s: %s.\n", inPath, theError);
		return false;
	}
	TMemoryStream theMemoryStream(std::move(theData));
	TStream* theStream = &theMemoryStream;
	try
	{
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

		// Check that the file belongs to this emulator before changing anything.
		KUInt32 theKind = theStream->GetInt32BE();
		if (theKind != kFastStartState && theKind != kDebugState)
		{
			KPrintf("Unknown kind of Einstein State file.\n");
			return false;
		}
		std::vector<KUInt32> theCurrent = GetStateIdentity();
		static const char* const kIdentityNames[] = {
			"ROM", "RAM size", "flash", "screen width", "screen height",
			"card in socket 0", "contents of the card in socket 0",
			"card in socket 1", "contents of the card in socket 1"
		};
		static_assert(sizeof(kIdentityNames) / sizeof(kIdentityNames[0]) == kStateIdentitySize,
			"Update kIdentityNames when GetStateIdentity() changes");
		Boolean theMatch = true;
		for (size_t i = 0; i < theCurrent.size(); i++)
		{
			KUInt32 theSaved = theStream->GetInt32BE();
			// The flash is only checked for fast start files. Debug files load it.
			if (i == 2 && theKind == kDebugState)
				continue;
			if (theSaved != theCurrent[i])
			{
				KPrintf("Not loading the state from %s: the %s changed.\n", inPath, kIdentityNames[i]);
				theMatch = false;
			}
		}
		if (!theMatch)
			return false;

		// Skip the inserted cards, see ReadStateCards().
		for (int socketIx = 0; socketIx < kNbSockets; socketIx++)
		{
			(void) theStream->GetInt32BE();
			KUInt32 theLength = theStream->GetInt32BE();
			theStream->CheckDataSize(theLength, kMaxStatePathLength, "the card image path length");
			std::vector<char> thePath(theLength);
			theStream->Read(thePath.data(), &theLength);
		}

		theStream->TransferFlags((theKind == kDebugState) ? kStateIncludesFlash : 0);
		TransferState(theStream);
	} catch (const std::exception& e)
	{
		// The contents of the file don't fit what we expect. Part of the state may
		// have been loaded, so at least drop the translated code.
		mMemory.GetJITObject()->InvalidateAll();
		KPrintf("Could not load the emulator state from %s (%s).\n", inPath, e.what());
		return false;
	}
	return true;
}

// -------------------------------------------------------------------------- //
//  * ReadStateCards( const char*, std::vector<SStateCard>& )
// -------------------------------------------------------------------------- //
Boolean
TEmulator::ReadStateCards(const char* inPath, std::vector<SStateCard>& outCards)
{
	outCards.clear();
	std::vector<KUInt8> theData;
	if (ReadStateFile(inPath, theData) != nullptr)
		return false;
	TMemoryStream theMemoryStream(std::move(theData));
	TStream* theStream = &theMemoryStream;
	try
	{
		if (theStream->GetInt32BE() != 'EINI' || theStream->GetInt32BE() != 'SNAP'
			|| theStream->GetInt32BE() != kStateFileVersion)
			return false;
		(void) theStream->GetInt32BE(); // kind of file
		for (size_t i = 0; i < kStateIdentitySize; i++)
			(void) theStream->GetInt32BE();
		for (int socketIx = 0; socketIx < kNbSockets; socketIx++)
		{
			SStateCard theCard;
			theCard.fTag = theStream->GetInt32BE();
			KUInt32 theLength = theStream->GetInt32BE();
			theStream->CheckDataSize(theLength, kMaxStatePathLength, "the card image path length");
			std::vector<char> thePath(theLength);
			theStream->Read(thePath.data(), &theLength);
			theCard.fImagePath.assign(thePath.data(), theLength);
			outCards.push_back(theCard);
		}
	} catch (const std::exception& e)
	{
		outCards.clear();
		return false;
	}
	return true;
}

// -------------------------------------------------------------------------- //
//  * ResetState( void )
// -------------------------------------------------------------------------- //
void
TEmulator::ResetState(void)
{
	// Walk the same tree as saving and loading. Every value that has a reset
	// value goes to its power-on state, all others are kept.
	TResetStream theStream;
	TransferState(&theStream);

	// The CPU takes the reset exception.
	mProcessor.Reset();
}

// -------------------------------------------------------------------------- //
//  * GetStateIdentity( void )
// -------------------------------------------------------------------------- //
std::vector<KUInt32>
TEmulator::GetStateIdentity()
{
	std::vector<KUInt32> theIdentity;
	theIdentity.push_back(mMemory.GetROMChecksum());
	theIdentity.push_back(mMemory.GetRAMSize());
	theIdentity.push_back(mMemory.GetFlashChecksum());
	theIdentity.push_back(mScreenManager->GetPortraitWidth());
	theIdentity.push_back(mScreenManager->GetPortraitHeight());
	for (int socketIx = 0; socketIx < kNbSockets; socketIx++)
	{
		TPCMCIAController* theController = mMemory.GetPCMCIAController(socketIx);
		TPCMCIACard* theCard = theController ? theController->CurrentCard() : nullptr;
		theIdentity.push_back(theCard ? theCard->GetStateTag() : 0);
		theIdentity.push_back(theCard ? theCard->GetContentsChecksum() : 0);
	}
	return theIdentity;
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

	// Show the screen as on or off, as the loaded (or reset) machine expects it.
	if (inStream->IsReading() || inStream->IsResetting())
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
	mStopCount++;
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
