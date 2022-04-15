// ==============================
// File:			TMonitor.cp
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

// Tell MSWindows to not generate a min/max macro
#define NOMINMAX

#include "TMonitor.h"
#include <K/Streams/TFileStream.h>

// C++11 and up
#include <algorithm>
#include <chrono>
#include <thread>

// ANSI C & POSIX
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#if TARGET_OS_WIN32
#include <Windows.h>
#include <assert.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#else
#include <strings.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

// Einstein
#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "Monitor/TSymbolList.h"
#include "Monitor/UDisasm.h"
#include "Emulator/Log/TBufferLog.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/Screen/TScreenManager.h"

#ifdef JITTARGET_GENERIC
//#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"
#endif

// -------------------------------------------------------------------------- //
// Constants
// -------------------------------------------------------------------------- //
const char* TMonitor::kEraseLine = "\033[K";
const char* const TMonitor::kRegisterNames[16] = {
	"R0 ",
	"R1 ",
	"R2 ",
	"R3 ",
	"R4 ",
	"R5 ",
	"R6 ",
	"R7 ",
	"R8 ",
	"R9 ",
	"R10",
	"R11",
	"R12",
	"R13",
	"LR ",
	"PC "
};

const char* const TMonitor::kModesNames[32] = {
	"?00", "?01", "?02", "?03", "?04", "?05", "?06", "?07",
	"?08", "?09", "?0A", "?0B", "?0C", "?0D", "?0E", "?0F",
	"usr", "fiq", "irq", "svc", "?14", "?15", "?16", "abt",
	"?18", "?19", "?1A", "und", "?1C", "?1D", "?1E", "sys"
};

// -------------------------------------------------------------------------- //
//  * TMonitor( TBufferLog*, TEmulator*, TSymbolList* )
// -------------------------------------------------------------------------- //
TMonitor::TMonitor(
	TBufferLog* inLog,
	TEmulator* inEmulator,
	TSymbolList* inSymbolList,
	const char* inROMPath) :
		TMonitorCore(inSymbolList),
		mEmulator(inEmulator),
		mProcessor(inEmulator->GetProcessor()),
		mInterruptManager(inEmulator->GetInterruptManager()),
		mLog(inLog),
		mHalted(true),
		mCommand(kNop),
		mFilename(nullptr),
		mLastScreenHalted(true),
		mROMPath(strdup(inROMPath))
{
	mMemory = inEmulator->GetMemory();

#if !TARGET_UI_FLTK
	if (::socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, mSocketPair) != 0)
	{
		(void) ::fprintf(stderr, "Error with socketpair: %i\n", errno);
		::abort();
	}

	mLog->BindWithRefreshSocket(mSocketPair[1]);
#endif

	CreateCondVarAndMutex();

#if !TARGET_UI_FLTK
	// Clear the terminal and go to the uppermost position.
	(void) ::printf("\033[1;1H");
	(void) ::printf("\033[2J");
#endif

	// Tell the emulator it's being monitored.
	inEmulator->SetMonitor(this);
}

// -------------------------------------------------------------------------- //
//  * ~TMonitor( void )
// -------------------------------------------------------------------------- //
TMonitor::~TMonitor()
{
	DeleteCondVarAndMutex();
	if (mFilename)
		free(mFilename);

#if !TARGET_UI_FLTK
	// Clear the terminal and go to the uppermost position.
	(void) ::printf("\033[1;1H");
	(void) ::printf("\033[2J");
#endif
}

// -------------------------------------------------------------------------- //
// Run( void )
// -------------------------------------------------------------------------- //
void
TMonitor::Run()
{
	// Acquire the mutex.
	AcquireMutex();

	// At first, we're halted.
	mHalted = true;

	// If the user put a script at /ROMPath/monitorrc, run it.
	// This is used to set the default path, breakpoints, etc.
	ExecuteStartupScript();

	if (mRunOnStartup)
	{
		mCommand = kRun;
		DrawScreen();
		RunEmulator();
	}

	Boolean loop = true;
	while (loop)
	{
		if (mCommand != kExit)
			DrawScreen();
		// Wait forever for a command.
		WaitOnCondVar();

		switch (mCommand)
		{
			case kNop:
				break;

			case kRun:
				RunEmulator();
				break;

			case kStep:
				StepEmulator();
				break;

			case kExit:
				loop = false;
				break;

			case kSaveState:
				SaveEmulatorState(mFilename);
				break;

			case kLoadState:
				LoadEmulatorState(mFilename);
				break;
		}
	}

	// Release the mutex.
	ReleaseMutex();
}

// -------------------------------------------------------------------------- //
// RunEmulator( void )
// -------------------------------------------------------------------------- //
void
TMonitor::RunEmulator()
{
	// We aren't stopped.
	mHalted = false;

#if !TARGET_UI_FLTK
	char someByte = 0;
	// Write a byte to the socket pair.
	(void) ::write(mSocketPair[1], &someByte, 1);
#endif

	// Get the PC.
	KUInt32 realPC = mProcessor->GetRegister(15) - 4;

	// Get the instruction.
	KUInt32 instruction;
	Boolean instructionIsBP = false;
	if (!mMemory->Read((TMemory::VAddr) realPC, instruction))
	{
		if ((instruction & 0xFFF000F0) == 0xE1200070)
		{
			instructionIsBP = true;
		}
	}

	while (true)
	{
		if (instructionIsBP)
		{
			// Disable, step, enable, run.
			(void) mMemory->DisableBreakpoint(realPC);
			mEmulator->Step();
			(void) mMemory->EnableBreakpoint(realPC);
			mEmulator->Run();
		} else
		{
			// Just run.
			mEmulator->Run();
		}

		// We're halted now. Check if it was because of a BP.
		if (mEmulator->IsBPHalted())
		{
			// Get back one instruction.
			realPC = mProcessor->GetRegister(15) - 4;
			mProcessor->SetRegister(15, realPC);
			realPC -= 4;
			instructionIsBP = true;

			// Process the breakpoint.
			if (ProcessBreakpoint(mEmulator->GetBPID(), realPC))
			{
				mLog->LogLine("break from breakpoint");
				break;
			}
		} else
		{
			break;
		}
	}

	mHalted = true;
#if !TARGET_UI_FLTK
	// Write a byte to the socket pair.
	(void) ::write(mSocketPair[1], &someByte, 1);
#endif
}

// -------------------------------------------------------------------------- //
// StepEmulator( void )
// -------------------------------------------------------------------------- //
void
TMonitor::StepEmulator()
{
	// Get the PC.
	KUInt32 realPC = mProcessor->GetRegister(15) - 4;

	// Get the instruction.
	KUInt32 instruction;
	Boolean instructionIsBP = false;
	if (!mMemory->Read((TMemory::VAddr) realPC, instruction))
	{
		if ((instruction & 0xFFF000F0) == 0xE1200070)
		{
			instructionIsBP = true;
		}
	}

	if (instructionIsBP)
	{
		// Disable, step, enable.
		(void) mMemory->DisableBreakpoint(realPC);
		mEmulator->Step();
		(void) mMemory->EnableBreakpoint(realPC);
	} else
	{
		// Just step.
		mEmulator->Step();
	}

#if !TARGET_UI_FLTK
	// Write a byte to the socket pair.
	char someByte = 0;
	(void) ::write(mSocketPair[1], &someByte, 1);
#endif
}

// -------------------------------------------------------------------------- //
// SaveEmulatorState( const char * )
// -------------------------------------------------------------------------- //
void
TMonitor::SaveEmulatorState(const char* inFilename)
{
	if (inFilename == 0)
	{
		inFilename = "/tmp/einstein.state";
	}
#if !TARGET_UI_FLTK
	char someByte = 0;
	mEmulator->SaveState(inFilename);
	(void) ::write(mSocketPair[1], &someByte, 1);
#endif
}

// -------------------------------------------------------------------------- //
// LoadEmulatroState( const char * )
// -------------------------------------------------------------------------- //
void
TMonitor::LoadEmulatorState(const char* inFilename)
{
	if (inFilename == 0)
	{
		inFilename = "/tmp/einstein.state";
	}
	if (TFileStream::Exists(inFilename))
	{
		mEmulator->LoadState(inFilename);
	}
	TScreenManager* screen = mEmulator->GetScreenManager();
	TScreenManager::SRect rect;
	rect.fLeft = 0;
	rect.fTop = 0;
	rect.fBottom = static_cast<KUInt16>(screen->GetScreenHeight() - 1);
	rect.fRight = static_cast<KUInt16>(screen->GetScreenWidth() - 1);
	screen->UpdateScreenRect(&rect);
#if !TARGET_UI_FLTK
	char someByte = 0;
	(void) ::write(mSocketPair[1], &someByte, 1);
#endif
}

// -------------------------------------------------------------------------- //
// SnapEmulatorState( const char * )
// -------------------------------------------------------------------------- //
void
TMonitor::SnapEmulatorState(const char* inFilename)
{
	// TODO: power emulator off
	TPlatformManager* pm = mEmulator->GetPlatformManager();
	if (pm->IsPowerOn())
		pm->SendPowerSwitchEvent();
	// TODO: save state
	// TODO: pause emulator
	// TODO: signal caller
	mEmulator->SaveState(inFilename);
#if !TARGET_UI_FLTK
	char someByte = 0;
	(void) ::write(mSocketPair[1], &someByte, 1);
#endif
}

// -------------------------------------------------------------------------- //
// LoadEmulatroState( const char * )
// -------------------------------------------------------------------------- //
void
TMonitor::RevertEmulatorState(const char* inFilename)
{
	// TODO: pause emulator
	// TODO: load state
	// TODO: bring emulator out of sleep state
	// TODO: signal caller
	mEmulator->LoadState(inFilename);
	TScreenManager* screen = mEmulator->GetScreenManager();
	TScreenManager::SRect rect;
	rect.fLeft = 0;
	rect.fTop = 0;
	rect.fBottom = static_cast<KUInt16>(screen->GetScreenHeight() - 1);
	rect.fRight = static_cast<KUInt16>(screen->GetScreenWidth() - 1);
	screen->UpdateScreenRect(&rect);
#if !TARGET_UI_FLTK
	char someByte = 0;
	(void) ::write(mSocketPair[1], &someByte, 1);
#endif
}

// -------------------------------------------------------------------------- //
// ProcessBreakpoint( KUInt16, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TMonitor::ProcessBreakpoint(KUInt16 inBPID, KUInt32 inBPAddr)
{
	Boolean stop = true;

	switch (inBPID)
	{
		case 0:
			// Permanent, loggable breakpoint.
			{
				char theLine[256];
				(void) ::sprintf(theLine, "Break at %.8X", (int) (inBPAddr));
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
			break;

		case 1:
			// Temporary breakpoint.
			(void) mMemory->ClearBreakpoint(inBPAddr);
			break;

		case 2:
			// Watch pc without any parameter.
			{
				char theLine[256];
				(void) ::sprintf(theLine, "Watch at %.8X", (int) (inBPAddr));
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
			stop = false;
			break;

		case 3:
			// Watch pc with 1 parameter.
			{
				char theLine[256];
				(void) ::sprintf(
					theLine,
					"Watch at %.8X [R0=%.8X]",
					(int) (inBPAddr),
					(unsigned int) mProcessor->GetRegister(0));
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
			stop = false;
			break;

		case 4:
			// Watch pc with 2 parameters.
			{
				char theLine[256];
				(void) ::sprintf(
					theLine,
					"Watch at %.8X [R0=%.8X,R1=%.8X]",
					(int) (inBPAddr),
					(unsigned int) mProcessor->GetRegister(0),
					(unsigned int) mProcessor->GetRegister(1));
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
			stop = false;
			break;

		case 5:
			// Watch pc with 3 parameters.
			{
				char theLine[256];
				(void) ::sprintf(
					theLine,
					"Watch at %.8X [R0=%.8X,R1=%.8X,R2=%.8X]",
					(int) (inBPAddr),
					(unsigned int) mProcessor->GetRegister(0),
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2));
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
			stop = false;
			break;
	}

	return stop;
}

// -------------------------------------------------------------------------- //
// Stop( void )
// -------------------------------------------------------------------------- //
void
TMonitor::Stop()
{
	mCommand = kExit;

	while (!mHalted)
	{
		mEmulator->Stop();
	}

	AcquireMutex();
	//  I have the mutex, so the loop is waiting.

	SignalCondVar();

	ReleaseMutex();
}

// little helper to return the prinatble version of any character
static char
cc(unsigned int v)
{
	v = v & 0xff;
	if (v < 32 || v > 126)
		return '.';
	return (char) v;
}

// -------------------------------------------------------------------------- //
// ExecuteStartupScript()
// -------------------------------------------------------------------------- //
Boolean
TMonitor::ExecuteStartupScript()
{
	bool theResult = true;
	char buf[2048];
	snprintf(buf, 2048, "%s/monitorrc", mROMPath);
	if (::access(buf, 4 /*R_OK*/) == 0)
	{
		theResult = ExecuteScript(buf);
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
// ExecuteCommand( const char* inCommand )
// -------------------------------------------------------------------------- //
Boolean
TMonitor::ExecuteCommand(const char* inCommand)
{
	Boolean theResult = true;
	int theArgInt, theArgInt2;
	char theLine[512];

	static KUInt32 lastDisStart = 0;

	if (inCommand[0] == '#')
	{ // script comment
	} else if ((::strcmp(inCommand, "run") == 0) // commands when the emulator is halted.

		|| (::strcmp(inCommand, "g") == 0))
	{
		if (mHalted)
		{
			mCommand = kRun;
			SignalCondVar();
		} else
		{
			PrintLine("The emulator is already running", MONITOR_LOG_ERROR);
		}
	} else if ((::strcmp(inCommand, "step") == 0)
		|| (::strcmp(inCommand, "") == 0))
	{
		if (mHalted)
		{
			// Print the current instruction.
			PrintCurrentInstruction();

			mCommand = kStep;
			SignalCondVar();
		} else if (::strcmp(inCommand, "") != 0)
		{
			PrintLine("The emulator is already running", MONITOR_LOG_ERROR);
		}
	} else if (::strncmp(inCommand, "save ", 5) == 0)
	{
		if (mHalted)
		{
			PrintLine("Saving emulator state", MONITOR_LOG_INFO);
			if (!mFilename)
				mFilename = (char*) malloc(2048);
			strcpy(mFilename, inCommand + 5);
			mCommand = kSaveState;
			SignalCondVar();
		} else
		{
			PrintLine("The emulator is running", MONITOR_LOG_ERROR);
		}
	} else if (::strncmp(inCommand, "load ", 5) == 0)
	{
		if (mHalted)
		{
			PrintLine("Loading emulator state", MONITOR_LOG_INFO);
			if (!mFilename)
				mFilename = (char*) malloc(2048);
			strcpy(mFilename, inCommand + 5);
			mCommand = kLoadState;
			SignalCondVar();
		} else
		{
			PrintLine("The emulator is running", MONITOR_LOG_ERROR);
		}
	} else if (::strcmp(inCommand, "snap") == 0)
	{
		if (!mHalted)
		{
			int i;
			TPlatformManager* pm = mEmulator->GetPlatformManager();
			if (pm->IsPowerOn())
			{
				PrintLine("Powering down", MONITOR_LOG_INFO);
				pm->SendPowerSwitchEvent();
				// wait a bit until we power down
				for (i = 300; i > 0; --i)
				{ // max. 3 seconds
					if (!pm->IsPowerOn())
						break;
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				if (i == 0)
				{
					PrintLine("ERROR: Failed to power down!\n", MONITOR_LOG_ERROR);
					// TODO: do something!
				}
			}
			PrintLine("Stopping the Emulator", MONITOR_LOG_INFO);
			mEmulator->Stop();
			// wait for the emulator to stop
			for (i = 10; i > 0; --i)
			{ // 1/10th of a second
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			PrintLine("Saving emulator snapshot", MONITOR_LOG_INFO);
			SaveEmulatorState();
			PrintLine("Emulator snapshot saved.", MONITOR_LOG_INFO);
		} else
		{
			PrintLine("The emulator is halted", MONITOR_LOG_ERROR);
		}
	} else if (::strcmp(inCommand, "revert") == 0 || ::strcmp(inCommand, "rev") == 0)
	{
		int i;
		TPlatformManager* pm = mEmulator->GetPlatformManager();
		if (pm->IsPowerOn())
		{
			PrintLine("Powering down", MONITOR_LOG_INFO);
			pm->SendPowerSwitchEvent();
			// wait a bit until we power down
			for (i = 300; i > 0; --i)
			{ // max. 3 seconds
				if (!pm->IsPowerOn())
					break;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			if (i == 0)
			{
				PrintLine("ERROR: Failed to power down!", MONITOR_LOG_ERROR);
				// TODO: do something!
			}
		}
		if (!mHalted)
		{
			PrintLine("Stopping the Emulator", MONITOR_LOG_INFO);
			mEmulator->Stop();
			// wait for the emulator to stop
			for (i = 10; i > 0; --i)
			{ // 1/10th of a second
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
		PrintLine("Loading emulator snapshot", MONITOR_LOG_INFO);
		LoadEmulatorState();
		PrintLine("Restarting the Emulator", MONITOR_LOG_INFO);
		if (mHalted)
		{
			mCommand = kRun;
			SignalCondVar();
		}
		// wait for the emulator to start
		for (i = 300; i > 0; --i)
		{ // 1/10th of a second
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		PrintLine("Powering up", MONITOR_LOG_INFO);
		if (!pm->IsPowerOn())
			pm->SendPowerSwitchEvent();
		PrintLine("Emulator snapshot restored.", MONITOR_LOG_INFO);
	} else if ((::strcmp(inCommand, "t") == 0)
		|| (::strcmp(inCommand, "trace") == 0))
	{
		// Is it a jump?
		Boolean putBPAndRun = false;

		KUInt32 instruction;
		KUInt32 realPC = mProcessor->GetRegister(15) - 4;
		if (!mMemory->Read((TMemory::VAddr) realPC, instruction))
		{
			if ((instruction & 0x0F000000) == 0x0B000000)
			{
				putBPAndRun = true;
			}
		}

		// Print the first instruction.
		PrintCurrentInstruction();

		if (putBPAndRun)
		{
			// Run until we reach the breakpoint.
			(void) mMemory->SetBreakpoint(realPC + 4, kTemporaryBP);
			mCommand = kRun;
			SignalCondVar();
		} else
		{
			// Step.
			mCommand = kStep;
			SignalCondVar();
		}
	} else if (::strcmp(inCommand, "mr") == 0)
	{
		if (mHalted)
		{
			(void) mMemory->SetBreakpoint(mProcessor->GetRegister(14), kTemporaryBP);
			mCommand = kRun;
			SignalCondVar();
		} else
		{
			PrintLine("The emulator is already running", MONITOR_LOG_ERROR);
		}
	} else if (::sscanf(inCommand, "pc=%X", &theArgInt) == 1)
	{
		if (mHalted)
		{
			mProcessor->SetRegister(15, theArgInt);
			(void) ::sprintf(
				theLine, "pc <- %.8X",
				(unsigned int) theArgInt);
			PrintLine(theLine, MONITOR_LOG_INFO);
		} else
		{
			PrintLine("Cannot change register, the emulator is running", MONITOR_LOG_ERROR);
		}
	} else if (::sscanf(inCommand, "r%i=%X", &theArgInt, &theArgInt2) == 2)
	{
		if (mHalted)
		{
			if ((theArgInt >= 0) && (theArgInt <= 15))
			{
				mProcessor->SetRegister(theArgInt, theArgInt2);
				(void) ::sprintf(
					theLine, "r%i <- %.8X",
					(int) theArgInt,
					(unsigned int) theArgInt2);
			} else
			{
				(void) ::sprintf(
					theLine, "Unknown register r%i",
					(int) theArgInt);
			}
			PrintLine(theLine, MONITOR_LOG_INFO);
		} else
		{
			PrintLine("Cannot change register, the emulator is running", MONITOR_LOG_ERROR);
		}
	} else if (::strcmp(inCommand, "mmu") == 0)
	{
		if (mHalted)
		{
			if (mMemory->IsMMUEnabled())
			{
				(void) ::sprintf(
					theLine, "TT = %.8X, AC = %.8X, FSR = %.8X, FAR = %.8X, AP = %s%s%s",
					(unsigned int) mMemory->GetTranslationTableBase(),
					(unsigned int) mMemory->GetDomainAccessControl(),
					(unsigned int) mMemory->GetFaultStatusRegister(),
					(unsigned int) mMemory->GetFaultAddressRegister(),
					mMemory->GetSystemProtection() ? "S" : "s",
					mMemory->GetROMProtection() ? "R" : "r",
					mMemory->GetPrivilege() ? "P" : "p");
				PrintLine(theLine, MONITOR_LOG_INFO);
			} else
			{
				PrintLine("MMU is disabled", MONITOR_LOG_INFO);
			}
		} else
		{
			PrintLine("Cannot play with MMU, the emulator is running", MONITOR_LOG_ERROR);
		}
	} else if (::sscanf(inCommand, "mmu %X", &theArgInt) == 1)
	{
		if (mHalted)
		{
			if (mMemory->IsMMUEnabled())
			{
				TMemory::PAddr physAddr;
				if (mMemory->TranslateR(
						(TMemory::VAddr) theArgInt,
						physAddr))
				{
					(void) ::sprintf(
						theLine, "An error occurred resolving V0x%.8X [%.8X]",
						(unsigned int) theArgInt,
						(unsigned int) mMemory->GetFaultStatusRegister());
					PrintLine(theLine, MONITOR_LOG_ERROR);
				} else
				{
					(void) ::sprintf(
						theLine, "V0x%.8X = P0x%.8X",
						(unsigned int) theArgInt,
						(unsigned int) physAddr);
					PrintLine(theLine, MONITOR_LOG_INFO);
				}
			} else
			{
				PrintLine("MMU is currently disabled", MONITOR_LOG_INFO);
			}
		} else
		{
			PrintLine("Cannot play with MMU, the emulator is running", MONITOR_LOG_ERROR);
		}
		// commands when the emulator is running
	} else if (::strcmp(inCommand, "stop") == 0)
	{
		if (!mHalted)
		{
			//(void) ::fprintf( stderr, "doing stop\n" );
			mEmulator->Stop();
		} else
		{
			PrintLine("Emulator is not running", MONITOR_LOG_ERROR);
		}
		// commands always available.
	} else if ((::sscanf(inCommand, "break %X", &theArgInt) == 1)
		|| (::sscanf(inCommand, "bp %X", &theArgInt) == 1))
	{
		if (mMemory->SetBreakpoint(theArgInt, kPermanentBP))
		{
			(void) ::sprintf(
				theLine, "Setting breakpoint at %.8X failed",
				(unsigned int) theArgInt);
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			(void) ::sprintf(
				theLine, "Breakpoint set at %.8X",
				(unsigned int) theArgInt);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "bc %X", &theArgInt) == 1)
	{
		if (mMemory->ClearBreakpoint(theArgInt))
		{
			(void) ::sprintf(
				theLine, "Clearing breakpoint at %.8X failed",
				(unsigned int) theArgInt);
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			(void) ::sprintf(
				theLine, "Breakpoint cleared at %.8X",
				(unsigned int) theArgInt);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "dl P%X", &theArgInt) == 1)
	{
		Boolean fault = false;
		KUInt32 theData = mMemory->ReadP(
			(TMemory::VAddr) theArgInt, fault);
		if (fault)
		{
			(void) ::sprintf(
				theLine, "Memory error when accessing %.8X [%.8X]",
				(unsigned int) theArgInt,
				(unsigned int) mMemory->GetFaultStatusRegister());
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			(void) ::sprintf(
				theLine, "P%.8X: %.8X",
				(unsigned int) theArgInt,
				(unsigned int) theData);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "dl %X", &theArgInt) == 1)
	{
		KUInt32 theData;
		if (mMemory->Read(
				(TMemory::VAddr) theArgInt, theData))
		{
			(void) ::sprintf(
				theLine, "Memory error when accessing %.8X [%.8X]",
				(unsigned int) theArgInt,
				(unsigned int) mMemory->GetFaultStatusRegister());
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			(void) ::sprintf(
				theLine, "%.8X: %.8X",
				(unsigned int) theArgInt,
				(unsigned int) theData);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "d %X", &theArgInt) == 1)
	{
		KUInt32 theData;
		if (mMemory->Read(
				(TMemory::VAddr) theArgInt, theData))
		{
			(void) ::sprintf(
				theLine, "Memory error when accessing %.8X [%.8X]",
				(unsigned int) theArgInt,
				(unsigned int) mMemory->GetFaultStatusRegister());
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			int i;
			for (i = 0; i < 16; i++)
			{
				PrintInstruction(theArgInt + 4 * i);
			}
		}
		lastDisStart = theArgInt + 4 * 16;
	} else if (strcmp(inCommand, "d") == 0)
	{
		int i;
		for (i = 0; i < 16; i++)
		{
			PrintInstruction(lastDisStart + 4 * i);
		}
		lastDisStart += 4 * 16;
	} else if (::sscanf(inCommand, "dm %X-%X", &theArgInt, &theArgInt2) == 2)
	{
		KUInt32 theData[4];
		while (((KUInt32) theArgInt) < ((KUInt32) theArgInt2))
		{
			if (mMemory->Read(
					(TMemory::VAddr) theArgInt, theData[0])
				|| mMemory->Read(
					(TMemory::VAddr) theArgInt + 4, theData[1])
				|| mMemory->Read(
					(TMemory::VAddr) theArgInt + 8, theData[2])
				|| mMemory->Read(
					(TMemory::VAddr) theArgInt + 12, theData[3]))
			{
				(void) ::sprintf(
					theLine, "Memory error when accessing %.8X [%.8X]",
					(unsigned int) theArgInt,
					(unsigned int) mMemory->GetFaultStatusRegister());
				PrintLine(theLine, MONITOR_LOG_ERROR);
			} else
			{
				(void) ::sprintf(
					theLine, "%.8X: %.8X %.8X %.8X %.8X",
					(unsigned int) theArgInt,
					(unsigned int) theData[0],
					(unsigned int) theData[1],
					(unsigned int) theData[2],
					(unsigned int) theData[3]);
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
			theArgInt += 16;
		}
	} else if (::sscanf(inCommand, "dis %X", &theArgInt) == 1)
	{
		KUInt32 addr = theArgInt;
		KUInt32 data;

		for (int i = 0; i < 16; ++i)
		{
			if (mMemory->Read((TMemory::VAddr) addr, data))
			{
				(void) ::sprintf(
					theLine, "Memory error when accessing %.8X [%.8X]",
					(unsigned int) theArgInt,
					(unsigned int) mMemory->GetFaultStatusRegister());
				PrintLine(theLine, MONITOR_LOG_ERROR);
				break;
			} else
			{
				char disasm[256];

				UDisasm::Disasm(disasm, 256, addr, data);
				::snprintf(theLine, sizeof(theLine), "     %08X   %s", (unsigned int) addr, disasm);
				PrintLine(theLine, MONITOR_LOG_INFO);
			}

			addr += 4;
		}
	} else if (::sscanf(inCommand, "dm %X", &theArgInt) == 1)
	{
		KUInt32 theData[4];
		KUInt32 last;
		for (last = theArgInt + 16 * 16; ((KUInt32) theArgInt) < last; theArgInt += 16)
		{
			if (mMemory->Read(
					(TMemory::VAddr) theArgInt, theData[0])
				|| mMemory->Read(
					(TMemory::VAddr) theArgInt + 4, theData[1])
				|| mMemory->Read(
					(TMemory::VAddr) theArgInt + 8, theData[2])
				|| mMemory->Read(
					(TMemory::VAddr) theArgInt + 12, theData[3]))
			{
				(void) ::sprintf(
					theLine, "Memory error when accessing %.8X [%.8X]",
					(unsigned int) theArgInt,
					(unsigned int) mMemory->GetFaultStatusRegister());
				PrintLine(theLine, MONITOR_LOG_ERROR);
			} else
			{
				(void) ::sprintf(
					theLine, "%.8X: %.8X %.8X %.8X %.8X %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
					(unsigned int) theArgInt,
					(unsigned int) theData[0],
					(unsigned int) theData[1],
					(unsigned int) theData[2],
					(unsigned int) theData[3],
					cc(theData[0] >> 24), cc(theData[0] >> 16), cc(theData[0] >> 8), cc(theData[0]),
					cc(theData[1] >> 24), cc(theData[1] >> 16), cc(theData[1] >> 8), cc(theData[1]),
					cc(theData[2] >> 24), cc(theData[2] >> 16), cc(theData[2] >> 8), cc(theData[2]),
					cc(theData[3] >> 24), cc(theData[3] >> 16), cc(theData[3] >> 8), cc(theData[3]));
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
		}
	} else if (::sscanf(inCommand, "dm P%X-%X", &theArgInt, &theArgInt2) == 2)
	{
		KUInt32 theData[4];
		Boolean fault = false;
		while (((KUInt32) theArgInt) < ((KUInt32) theArgInt2))
		{
			theData[0] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt, fault);
			theData[1] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt + 4, fault);
			theData[2] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt + 8, fault);
			theData[3] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt + 12, fault);
			if (fault)
			{
				(void) ::sprintf(
					theLine, "Memory error when accessing P%.8X [%.8X]",
					(unsigned int) theArgInt,
					(unsigned int) mMemory->GetFaultStatusRegister());
				PrintLine(theLine, MONITOR_LOG_ERROR);
			} else
			{
				(void) ::sprintf(
					theLine, "P%.8X: %.8X %.8X %.8X %.8X",
					(unsigned int) theArgInt,
					(unsigned int) theData[0],
					(unsigned int) theData[1],
					(unsigned int) theData[2],
					(unsigned int) theData[3]);
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
			theArgInt += 16;
		}
	} else if (::sscanf(inCommand, "dm P%X", &theArgInt) == 1)
	{
		KUInt32 theData[4];
		KUInt32 last;
		Boolean fault = false;
		for (last = theArgInt + 64; ((KUInt32) theArgInt) < last; theArgInt += 16)
		{
			theData[0] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt, fault);
			theData[1] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt + 4, fault);
			theData[2] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt + 8, fault);
			theData[3] = mMemory->ReadP(
				(TMemory::VAddr) theArgInt + 12, fault);
			if (fault)
			{
				(void) ::sprintf(
					theLine, "Memory error when accessing P%.8X [%.8X]",
					(unsigned int) theArgInt,
					(unsigned int) mMemory->GetFaultStatusRegister());
				fault = false;
				PrintLine(theLine, MONITOR_LOG_ERROR);
			} else
			{
				(void) ::sprintf(
					theLine, "P%.8X: %.8X %.8X %.8X %.8X",
					(unsigned int) theArgInt,
					(unsigned int) theData[0],
					(unsigned int) theData[1],
					(unsigned int) theData[2],
					(unsigned int) theData[3]);
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
		}
	} else if (::sscanf(inCommand, "sl %X %X", &theArgInt, &theArgInt2) == 2)
	{
		if (mMemory->Write(
				(TMemory::VAddr) theArgInt, theArgInt2))
		{
			(void) ::sprintf(
				theLine, "Memory error when writing at %.8X [%.8X]",
				(unsigned int) theArgInt,
				(unsigned int) mMemory->GetFaultStatusRegister());
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			(void) ::sprintf(
				theLine, "%.8X <- %.8X",
				(unsigned int) theArgInt,
				(unsigned int) theArgInt2);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "sl P%X %X", &theArgInt, &theArgInt2) == 2)
	{
		if (mMemory->WriteP(
				(TMemory::VAddr) theArgInt, theArgInt2))
		{
			(void) ::sprintf(
				theLine, "Memory error when writing at %.8X [%.8X]",
				(unsigned int) theArgInt,
				(unsigned int) mMemory->GetFaultStatusRegister());
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			(void) ::sprintf(
				theLine, "P%.8X <- %.8X",
				(unsigned int) theArgInt,
				(unsigned int) theArgInt2);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "raise %X", &theArgInt) == 1)
	{
		mInterruptManager->RaiseInterrupt(theArgInt);
		(void) ::sprintf(
			theLine, "IR |= %.8X",
			(int) theArgInt);
		PrintLine(theLine, MONITOR_LOG_INFO);
	} else if (::sscanf(inCommand, "gpio %X", &theArgInt) == 1)
	{
		mInterruptManager->RaiseGPIO(theArgInt);
		(void) ::sprintf(
			theLine, "RaiseGPIO %.8X",
			(int) theArgInt);
		PrintLine(theLine, MONITOR_LOG_INFO);
	} else if (::sscanf(inCommand, "watch %i %X", &theArgInt2, &theArgInt) == 2)
	{
		if ((theArgInt2 >= 0) && (theArgInt2 <= 3))
		{
			if (mMemory->SetBreakpoint(theArgInt, KUInt16(kWatch0BP + theArgInt2)))
			{
				(void) ::sprintf(
					theLine, "Setting breakpoint at %.8X failed",
					(unsigned int) theArgInt);
				PrintLine(theLine, MONITOR_LOG_ERROR);
			} else
			{
				(void) ::sprintf(
					theLine, "Watching execution at %.8X with %i parameters",
					(unsigned int) theArgInt,
					(int) theArgInt2);
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
		} else
		{
			(void) ::sprintf(
				theLine, "Cannot watch %i parameters (proper range: 0..3)",
				(int) theArgInt2);
			PrintLine(theLine, MONITOR_LOG_ERROR);
		}
	} else if (::strcmp(inCommand, "bt") == 0)
	{
		PrintBacktrace();
	} else if (::strncmp(inCommand, "bt ", 3) == 0)
	{
		PrintBacktrace(::atoi(inCommand + 3));
	} else if (::strcmp(inCommand, "log") == 0)
	{
		mLog->CloseLog();
	} else if (::sscanf(inCommand, "log %s", theLine) == 1)
	{
		mLog->OpenLog(theLine);
	} else if (::strcmp(inCommand, "disable log") == 0)
	{
		mLog->Disable();
	} else if (::strcmp(inCommand, "enable log") == 0)
	{
		mLog->Enable();
	} else if (::strcmp(inCommand, "help") == 0
		|| ::strcmp(inCommand, "h") == 0
		|| ::strcmp(inCommand, "?") == 0)
	{
		PrintHelp();
	} else if (::strncmp(inCommand, "help ", 5) == 0)
	{
		ExecuteHelpCommand(inCommand + 5);
	} else if (::strncmp(inCommand, "h ", 2) == 0)
	{
		ExecuteHelpCommand(inCommand + 2);
	} else if (::strncmp(inCommand, "? ", 2) == 0)
	{
		ExecuteHelpCommand(inCommand + 2);
	} else if (::sscanf(inCommand, "wpr %X", &theArgInt) == 1)
	{
		if (mMemory->AddWatchpoint(theArgInt, 1))
		{
			::sprintf(theLine, "Setting watchpoint at %.8X failed", theArgInt);
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			::sprintf(theLine, "Watching read access at %.8X", theArgInt);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "wpw %X", &theArgInt) == 1)
	{
		if (mMemory->AddWatchpoint(theArgInt, 2))
		{
			::sprintf(theLine, "Setting watchpoint at %.8X failed", theArgInt);
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			::sprintf(theLine, "Watching write access at %.8X", theArgInt);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "wpc %X", &theArgInt) == 1)
	{
		if (mMemory->ClearWatchpoint(theArgInt))
		{
			::sprintf(theLine, "Clearing watchpoint at %.8X failed", theArgInt);
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			::sprintf(theLine, "Watchpoint at %.8X cleared", theArgInt);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::sscanf(inCommand, "wp %X", &theArgInt) == 1)
	{
		if (mMemory->AddWatchpoint(theArgInt, 3))
		{
			::sprintf(theLine, "Setting watchpoint at %.8X failed", theArgInt);
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			::sprintf(theLine, "Watching read and write access at %.8X", theArgInt);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (strcmp(inCommand, "wpl") == 0)
	{
		int i = 0;
		const char* lut[] = { "--", "read", "write", "read/write" };
		for (;; i++)
		{
			KUInt32 addr;
			KUInt8 type;
			if (mMemory->GetWatchpoint(i, addr, type))
				break;
			::sprintf(theLine, "WP %2d at %.8X, %s", i, (unsigned int) addr, lut[type & 3]);
			PrintLine(theLine, MONITOR_LOG_INFO);
		}
	} else if (::strcmp(inCommand, "p tasks") == 0)
	{
		PrintLine("List of Tasks", MONITOR_LOG_INFO);
		KUInt32 kernelScheduler;
		mMemory->Read(0x0C100FD0, kernelScheduler);
		int i;
		for (i = 0; i < 32; i++)
		{
			KUInt32 taskQ;
			mMemory->Read(kernelScheduler + 0x1C + 4 * i, taskQ); // TTaskQueue
			if (taskQ)
			{
				KUInt32 task;
				mMemory->Read(taskQ, task); // TTask
				::sprintf(theLine, " Pri %d: first Task at 0x%08X", i, (unsigned int) task);
				PrintLine(theLine, MONITOR_LOG_INFO);
			}
		}
	} else if (::sscanf(inCommand, "po **%X", &theArgInt) == 1)
	{
		KUInt32 theData;
		if (mMemory->Read(
				(TMemory::VAddr) theArgInt, theData))
		{
			(void) ::sprintf(
				theLine, "Memory error when accessing %.8X [%.8X]",
				(unsigned int) theArgInt,
				(unsigned int) mMemory->GetFaultStatusRegister());
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			if (mMemory->Read(
					(TMemory::VAddr) theData, theData))
			{
				(void) ::sprintf(
					theLine, "Memory error when accessing %.8X [%.8X]",
					(unsigned int) theData,
					(unsigned int) mMemory->GetFaultStatusRegister());
				PrintLine(theLine, MONITOR_LOG_ERROR);
			} else
			{
				PrintNSRef(theData);
			}
		}
	} else if (::sscanf(inCommand, "po *%X", &theArgInt) == 1)
	{
		KUInt32 theData;
		if (mMemory->Read(
				(TMemory::VAddr) theArgInt, theData))
		{
			(void) ::sprintf(
				theLine, "Memory error when accessing %.8X [%.8X]",
				(unsigned int) theArgInt,
				(unsigned int) mMemory->GetFaultStatusRegister());
			PrintLine(theLine, MONITOR_LOG_ERROR);
		} else
		{
			PrintNSRef(theData);
		}
	} else if (::sscanf(inCommand, "po %X", &theArgInt) == 1)
	{
		PrintNSRef(theArgInt);
	} else if (::strcmp(inCommand, "cdr") == 0)
	{
		::chdir(mROMPath);
	} else if (inCommand[0] == '!')
	{
		theResult = ExecuteScript(inCommand + 1);
	} else
	{
		theResult = TMonitorCore::ExecuteCommand(inCommand);
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
// ExecuteHelpCommand( const char* inCommand )
// -------------------------------------------------------------------------- //
Boolean
TMonitor::ExecuteHelpCommand(const char* inCommand)
{
	Boolean theResult = true;
	if (::strcmp(inCommand, "log") == 0)
	{
		PrintLoggingHelp();
	} else if (::strcmp(inCommand, "script") == 0)
	{
		PrintScriptingHelp();
	} else if (::strcmp(inCommand, "wp") == 0)
	{
		PrintWatchpointHelp();
	} else
	{
		theResult = false;
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
// PrintHelp( void )
// -------------------------------------------------------------------------- //
void
TMonitor::PrintHelp()
{
	PrintLine("Monitor commands available when the machine is halted:", MONITOR_LOG_INFO);
	PrintLine(" <return>|step      step once", MONITOR_LOG_INFO);
	//	PrintLine(" step <count>       step for count steps");
	PrintLine(" t|trace            step over", MONITOR_LOG_INFO);
	PrintLine(" g|run              run", MONITOR_LOG_INFO);
	PrintLine(" mmu                display mmu registers", MONITOR_LOG_INFO);
	PrintLine(" mmu <address>      mmu lookup address", MONITOR_LOG_INFO);
	PrintLine(" mr                 magic return (relies on lr)", MONITOR_LOG_INFO);
	PrintLine(" r<i>=<val>         set ith register", MONITOR_LOG_INFO);
	PrintLine(" pc=<val>           set pc", MONITOR_LOG_INFO);
	PrintLine(" bt [n]             backtrace the stack", MONITOR_LOG_INFO);
	PrintLine("Monitor commands available when the machine is running:", MONITOR_LOG_INFO);
	PrintLine(" stop               interrupt the machine", MONITOR_LOG_INFO);
	PrintLine("Monitor commands always available:", MONITOR_LOG_INFO);
	PrintLine(" break|bp <address> set breakpoint at address", MONITOR_LOG_INFO);
	PrintLine(" watch <x> <addr>   set watchpoint with x params at address", MONITOR_LOG_INFO);
	PrintLine(" bc <address>       clear breakpoint at address", MONITOR_LOG_INFO);
	PrintLine(" dis <address>      disassemble code at address", MONITOR_LOG_INFO);
	PrintLine(" dl <address>       display long at address", MONITOR_LOG_INFO);
	PrintLine(" dl P<address>      display long at physical address", MONITOR_LOG_INFO);
	PrintLine(" dm <addr>[-<addr>] display memory at address/between addresses", MONITOR_LOG_INFO);
	PrintLine(" dm P<addr>[-<add>] display memory at physical address(es)", MONITOR_LOG_INFO);
	PrintLine(" sl [P]<addr> <val> set long at [physical] address", MONITOR_LOG_INFO);
	PrintLine(" po <ref>           print NS reference", MONITOR_LOG_INFO);
	PrintLine(" po *<addr>         print NS object at address (Ref)", MONITOR_LOG_INFO);
	PrintLine(" po **<addr>        print NS object handle at address (RefVar)", MONITOR_LOG_INFO);
	PrintLine(" raise <val>        raise the interrupts", MONITOR_LOG_INFO);
	PrintLine(" gpio <val>         raise the gpio interrupts", MONITOR_LOG_INFO);
	PrintLine(" load|save path     load or save the emulator state", MONITOR_LOG_INFO);
	PrintLine(" snap|revert        (re)store machine state while running", MONITOR_LOG_INFO);
	PrintLine(" help log           help with logging", MONITOR_LOG_INFO);
	PrintLine(" help script        help with scripting", MONITOR_LOG_INFO);
	PrintLine(" help wp            help with watchpoint commands", MONITOR_LOG_INFO);
}

// -------------------------------------------------------------------------- //
// PrintLoggingHelp( void )
// -------------------------------------------------------------------------- //
void
TMonitor::PrintLoggingHelp()
{
	PrintLine("Logging commands start or stop a log file for all", MONITOR_LOG_INFO);
	PrintLine("text output in the Monitor window.", MONITOR_LOG_INFO);
	PrintLine("", MONITOR_LOG_INFO);
	PrintLine(" log path           start logging to file", MONITOR_LOG_INFO);
	PrintLine(" log                stop logging to file", MONITOR_LOG_INFO);
	PrintLine(" disable log        disable the log", MONITOR_LOG_INFO);
	PrintLine(" enable log         enable the log", MONITOR_LOG_INFO);
}

// -------------------------------------------------------------------------- //
// PrintScriptingHelp( void )
// -------------------------------------------------------------------------- //
void
TMonitor::PrintScriptingHelp()
{
	PrintLine("Scripting can run many Monitor commands in a text file.", MONITOR_LOG_INFO);
	PrintLine("When Einstein starts, the script /ROMpath/monitorrc is", MONITOR_LOG_INFO);
	PrintLine("executed first.", MONITOR_LOG_INFO);
	PrintLine("", MONITOR_LOG_INFO);
	PrintLine(" !filename          run a script file", MONITOR_LOG_INFO);
	PrintLine(" cd                 change to the user HOME directory", MONITOR_LOG_INFO);
	PrintLine(" cd path            set a new current directory", MONITOR_LOG_INFO);
	PrintLine(" cdr                cd to directory where the ROM is located", MONITOR_LOG_INFO);
	PrintLine(" cwd                print current working directory", MONITOR_LOG_INFO);
	//	PrintLine("*mon show           show the monitor window");
	//	PrintLine("*mon hide           hide the monitor window");
	PrintLine(" # comment          marks commentary lines in the script file", MONITOR_LOG_INFO);
	PrintLine(" 'text              print text", MONITOR_LOG_INFO);
}

// -------------------------------------------------------------------------- //
// PrintWatchpointHelp( void )
// -------------------------------------------------------------------------- //
void
TMonitor::PrintWatchpointHelp()
{
	PrintLine("Watchpoints stop execution whenever the value of", MONITOR_LOG_INFO);
	PrintLine("a virtual memory address changes.", MONITOR_LOG_INFO);
	PrintLine("", MONITOR_LOG_INFO);
	PrintLine("Watchpoint commands available when the machine is halted:", MONITOR_LOG_INFO);
	PrintLine(" wp <addr>          set a watchpoint for reading and writing", MONITOR_LOG_INFO);
	PrintLine(" wpr <addr>         set a watchpoint for reading only", MONITOR_LOG_INFO);
	PrintLine(" wpw <addr>         set a watchpoint for writing only", MONITOR_LOG_INFO);
	PrintLine(" wpc <addr>         clear a watchpoint", MONITOR_LOG_INFO);
	PrintLine(" wpl                list all watchpoints", MONITOR_LOG_INFO);
}

// -------------------------------------------------------------------------- //
// DrawScreen( void )
// -------------------------------------------------------------------------- //
Boolean
TMonitor::DrawScreen()
{
	Boolean theResult = false;
	if (mHalted)
	{
		if (!mLastScreenHalted)
		{
			// Clear the terminal.
#if !TARGET_UI_FLTK
			(void) ::printf("\033[2J");
#endif
			theResult = true;
		}
		mLastScreenHalted = true;
		DrawScreenHalted();
	} else
	{
		if (mLastScreenHalted)
		{
			// Clear the terminal.
#if !TARGET_UI_FLTK
			(void) ::printf("\033[2J");
#endif
			theResult = true;
		}
		mLastScreenHalted = false;
		DrawScreenRunning();
	}

	return theResult;
}

// -------------------------------------------------------------------------- //
// DrawScreenHalted( void )
// -------------------------------------------------------------------------- //
void
TMonitor::DrawScreenHalted(void)
{
	KUInt32 realPC = mProcessor->GetRegister(15) - 4;

	// Go to the uppermost position.
#if !TARGET_UI_FLTK
	(void) ::printf("\033[1;1H");
#endif
	int indexRegisters;
	for (indexRegisters = 0; indexRegisters < 16; indexRegisters++)
	{
		(void) ::printf("%s%s= %.8X | %s\n",
			kEraseLine,
			kRegisterNames[indexRegisters],
			(unsigned int) mProcessor->GetRegister(indexRegisters),
			mLog->GetLine(indexRegisters));
	}
	KUInt32 theCPSR = mProcessor->GetCPSR();
	KUInt32 theMode = theCPSR & TARMProcessor::kPSR_ModeMask;
	(void) ::printf("%s%c%c%c%c %c%c%c %s  | %s\n",
		kEraseLine,
		theCPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
		theCPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
		theCPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
		theCPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
		theCPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
		theCPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
		theCPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
		kModesNames[theMode],
		mLog->GetLine(16));
	KUInt32 theSPSR = 0;
	if ((theMode != TARMProcessor::kSystemMode)
		&& (theMode != TARMProcessor::kUserMode))
	{
		theSPSR = mProcessor->GetSPSR();
	}
	if (theSPSR == 0)
	{
		(void) ::printf("%s---- --- ---  | %s\n",
			kEraseLine,
			mLog->GetLine(17));
	} else
	{
		(void) ::printf("%s%c%c%c%c %c%c%c %s  | %s\n",
			kEraseLine,
			theSPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
			theSPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
			theSPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
			theSPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
			theSPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
			theSPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
			theSPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
			kModesNames[theSPSR & TARMProcessor::kPSR_ModeMask],
			mLog->GetLine(17));
	}

	(void) ::printf("%s==============| %s\n",
		kEraseLine,
		mLog->GetLine(18));

	(void) ::printf("%sTmr= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetFrozenTimer(),
		mLog->GetLine(19));

	(void) ::printf("%sTM0= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetTimerMatchRegister(0),
		mLog->GetLine(20));

	(void) ::printf("%sTM1= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetTimerMatchRegister(1),
		mLog->GetLine(21));

	(void) ::printf("%sTM2= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetTimerMatchRegister(2),
		mLog->GetLine(22));

	(void) ::printf("%sTM3= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetTimerMatchRegister(3),
		mLog->GetLine(23));

	(void) ::printf("%sRTC= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetRealTimeClock(),
		mLog->GetLine(24));

	(void) ::printf("%sAlm= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetAlarm(),
		mLog->GetLine(25));

	(void) ::printf("%sIR = %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetIntRaised(),
		mLog->GetLine(26));

	(void) ::printf("%sICR= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetIntCtrlReg(),
		mLog->GetLine(27));

	(void) ::printf("%sFM = %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetFIQMask(),
		mLog->GetLine(28));

	(void) ::printf("%sIC1= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetIntEDReg1(),
		mLog->GetLine(29));

	(void) ::printf("%sIC2= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetIntEDReg2(),
		mLog->GetLine(30));

	(void) ::printf("%sIC3= %.8X | %s\n",
		kEraseLine,
		(unsigned int) mInterruptManager->GetIntEDReg3(),
		mLog->GetLine(31));

	(void) ::printf("%s-------------------------------------------------------------------------------\n", kEraseLine);

	char theInstr[512];
	theInstr[0] = 0;

	char theSymbol[512];
	char theComment[512];
	int theOffset;
	char theLine[768];

	if (mSymbolList)
	{
		// Get the symbol.
		mSymbolList->GetNearestSymbolByAddress(
			realPC,
			theSymbol,
			theComment,
			&theOffset);
	} else
	{
		theSymbol[0] = '\0';
		theComment[0] = '\0';
		theLine[0] = '\0';
		theOffset = 0;
	}

	(void) ::printf(
		"%s%s+%X\n",
		kEraseLine, theSymbol, theOffset);

	KUInt32 instruction;

	// Write 5 lines.
	int indexLines;
	for (indexLines = 0; indexLines < 20; indexLines += 4)
	{
		if (mMemory->Read(
				(TMemory::VAddr) realPC + indexLines, instruction))
		{
			(void) ::printf(
				"%s%.8X Memory Error [%.8X]\n",
				kEraseLine,
				(unsigned int) realPC + indexLines,
				(unsigned int) mMemory->GetFaultStatusRegister());
		} else
		{
			Boolean instIsBP = false;
			if ((instruction & 0xFFF000F0) == 0xE1200070)
			{
				if (!mMemory->ReadBreakpoint(
						(TMemory::VAddr) realPC + indexLines, instruction))
				{
					instIsBP = true;
				}
			}
			UDisasm::Disasm(
				theInstr,
				sizeof(theInstr),
				realPC + indexLines,
				instruction,
				mSymbolList);

			char status[32];
			status[0] = '\0';
			if (indexLines == 0)
			{
				if (instruction >> 28 != 0xE)
				{
					KUInt32 skip = 0;
					switch (instruction >> 28)
					{
							// 0000 = EQ - Z set (equal)
						case 0x0:
							skip = !(theCPSR & TARMProcessor::kPSR_ZBit);
							break;

							// 0001 = NE - Z clear (not equal)
						case 0x1:
							skip = theCPSR & TARMProcessor::kPSR_ZBit;
							break;
							// 0010 = CS - C set (unsigned higher or same)
						case 0x2:
							skip = !(theCPSR & TARMProcessor::kPSR_CBit);
							break;

							// 0011 = CC - C clear (unsigned lower)
						case 0x3:
							skip = theCPSR & TARMProcessor::kPSR_CBit;
							break;

							// 0100 = MI - N set (negative)
						case 0x4:
							skip = !(theCPSR & TARMProcessor::kPSR_NBit);
							break;

							// 0101 = PL - N clear (positive or zero)
						case 0x5:
							skip = theCPSR & TARMProcessor::kPSR_NBit;
							break;

							// 0110 = VS - V set (overflow)
						case 0x6:
							skip = !(theCPSR & TARMProcessor::kPSR_VBit);
							break;

							// 0111 = VC - V clear (no overflow)
						case 0x7:
							skip = theCPSR & TARMProcessor::kPSR_VBit;
							break;

							// 1000 = HI - C set and Z clear (unsigned higher)
						case 0x8:
							skip = !(theCPSR & TARMProcessor::kPSR_CBit)
								|| (theCPSR & TARMProcessor::kPSR_ZBit);
							break;

							// 1001 = LS - C clear or Z set (unsigned lower or same)
						case 0x9:
							skip = (theCPSR & TARMProcessor::kPSR_CBit)
								&& !(theCPSR & TARMProcessor::kPSR_ZBit);
							break;

							// 1010 = GE - N set and V set, or N clear and V clear (greater or equal)
						case 0xA:
							skip = ((theCPSR & TARMProcessor::kPSR_NBit) != 0)
								!= ((theCPSR & TARMProcessor::kPSR_VBit) != 0);
							break;

							// 1011 = LT - N set and V clear, or N clear and V set (less than)
						case 0xB:
							skip = ((theCPSR & TARMProcessor::kPSR_NBit) != 0)
								== ((theCPSR & TARMProcessor::kPSR_VBit) != 0);
							break;

							// 1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
						case 0xC:
							skip = (theCPSR & TARMProcessor::kPSR_ZBit) || (((theCPSR & TARMProcessor::kPSR_NBit) != 0) != ((theCPSR & TARMProcessor::kPSR_VBit) != 0));
							break;

							// 1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
						case 0xD:
							skip = (!(theCPSR & TARMProcessor::kPSR_ZBit))
								&& (((theCPSR & TARMProcessor::kPSR_NBit) != 0)
									== ((theCPSR & TARMProcessor::kPSR_VBit) != 0));
							break;

							// 1111 = NV - never
						case 0xF:
						default:
							skip = 1;
					}

					if (skip)
					{
						(void) ::sprintf(status, " (will skip)");
					} else
					{
						(void) ::sprintf(status, " (will do it)");
					}
				}
			}
			if (instIsBP)
			{
				(void) ::sprintf(theLine, "%.8X * %s",
					(unsigned int) realPC + indexLines,
					theInstr);
			} else
			{
				(void) ::sprintf(theLine, "%.8X   %s",
					(unsigned int) realPC + indexLines,
					theInstr);
			} // if (indexLines == 0)
			(void) ::printf("%s%s%s\n",
				kEraseLine,
				theLine,
				status);
		}
	}

	// Footer.
	(void) ::printf("%s-------------------------------------------------------------------------------\n", kEraseLine);
}

// -------------------------------------------------------------------------- //
// DrawScreenRunning( void )
// -------------------------------------------------------------------------- //
void
TMonitor::DrawScreenRunning(void)
{
	// Go to the uppermost position.
	(void) ::printf("\033[1;1H");

	(void) ::printf("%sMachine is running. Use stop to halt it.\n", kEraseLine);
	(void) ::printf("%s-------------------------------------------------------------------------------\n", kEraseLine);
	int indexLog;
	for (indexLog = 0; indexLog < 32; indexLog++)
	{
		(void) ::printf("%s%s\n", kEraseLine, mLog->GetLine(indexLog));
	}
	(void) ::printf("%s-------------------------------------------------------------------------------\n", kEraseLine);
}

// -------------------------------------------------------------------------- //
// PrintCurrentInstruction( void )
// -------------------------------------------------------------------------- //
void
TMonitor::PrintCurrentInstruction()
{
	KUInt32 realPC = mProcessor->GetRegister(15) - 4;
	PrintInstruction(realPC);
}

// -------------------------------------------------------------------------- //
// PrintInstruction( KUInt32 )
// -------------------------------------------------------------------------- //
void
TMonitor::PrintInstruction(KUInt32 inAddress)
{
	char theSymbol[128];
	char theComment[512];
	int theOffset;
	char theLine[768];
	// If instruction is at the top of a function, print its name.
	mSymbolList->GetNearestSymbolByAddress(inAddress, theSymbol, theComment, &theOffset);
	if (theOffset == 0)
	{
		(void) ::sprintf(
			theLine,
			"%s\t%c %s",
			theSymbol,
			(theComment[0] == '\0') ? ' ' : ';',
			theComment);

		PrintLine(theLine, MONITOR_LOG_INFO);
	}

	KUInt32 instruction;

	if (mMemory->Read((TMemory::VAddr) inAddress, instruction))
	{
		(void) ::sprintf(
			theLine,
			"Memory error while reading %.8X\n",
			(unsigned int) inAddress);
		PrintLine(theLine, MONITOR_LOG_ERROR);
	} else
	{
		theLine[0] = 0;
		(void) ::sprintf(theLine, "  %.8X   ", (int) inAddress);
		if ((instruction & 0xFFF000F0) == 0xE1200070)
		{
			(void) mMemory->ReadBreakpoint(
				(TMemory::VAddr) inAddress, instruction);
		}
		UDisasm::Disasm(
			&theLine[13],
			sizeof(theLine) - 13,
			inAddress,
			instruction,
			mSymbolList);
		PrintLine(theLine, MONITOR_LOG_CODE);
	}
}

// -------------------------------------------------------------------------- //
//  * PrintBacktrace(KSInt32 inNWords)
// -------------------------------------------------------------------------- //
void
TMonitor::PrintBacktrace(KSInt32 inNWords)
{
	// backtrack stack content
	KUInt32 sp = mProcessor->GetRegister(13), lr = mProcessor->GetRegister(14);
	KUInt32 theData;
	char theSymbol[512];
	char theComment[512];
	char theLine[768];
	int theOffset;
	if (inNWords <= 0)
		inNWords = 28; // approximatly one full screen
	for (int i = inNWords; i >= 0; i--)
	{
		mMemory->Read((TMemory::VAddr) sp + 4 * i, theData);
		mSymbolList->GetNearestSymbolByAddress(theData, theSymbol, theComment, &theOffset);
		sprintf(theLine, "sp+%3d: 0x%08X = %s+%d", (int) 4 * i, (unsigned int) theData, theSymbol, theOffset);
		theLine[62] = 0;
		PrintLine(theLine, MONITOR_LOG_INFO);
	}
	mSymbolList->GetNearestSymbolByAddress(lr, theSymbol, theComment, &theOffset);
	sprintf(theLine, "    lr: 0x%08X = %s+%d", (unsigned int) lr, theSymbol, theOffset);
	theLine[62] = 0;
	PrintLine(theLine, MONITOR_LOG_INFO);
}

// -------------------------------------------------------------------------- //
//  * PrintNSRef(KUInt32 inRef)
// -------------------------------------------------------------------------- //
void
TMonitor::PrintNSRef(KUInt32 inRef)
{
	char* objectDesc = (char*) ::malloc(4096);
	int n = FormatNSRef(objectDesc, 4096, inRef, 0, 50);
	if (n > 0)
	{
		char* str = objectDesc;
		for (int index = 0; index < n; index++)
		{
			if (objectDesc[index] == '\n')
			{
				objectDesc[index] = 0;
				PrintLine(str, MONITOR_LOG_ERROR);
				str = &objectDesc[index + 1];
			}
		}
		if (*str != 0)
		{
			PrintLine(str, MONITOR_LOG_ERROR);
		}
	}
	::free(objectDesc);
}

int
TMonitor::FormatNSRef(char* buffer, size_t bufferSize, KUInt32 inRef, int indent, int maxDepth)
{
	switch (inRef & 0x3)
	{
		case kTagInteger: {
			KSInt32 value = ((KSInt32) inRef) >> 2;
			return snprintf(buffer, bufferSize, "%ld", (long) value);
		}

		case kTagPointer: {
			TMemory::VAddr addr = (TMemory::VAddr)(inRef - kTagPointer);
			KUInt32 objectHeader;
			if (mMemory->Read(addr, objectHeader))
			{
				char theLine[512];
				(void) ::snprintf(theLine, sizeof(theLine), "Memory error while reading %.8X\n", (unsigned int) addr);
				PrintLine(theLine, MONITOR_LOG_ERROR);
				return -1;
			}
			bool slotted = objectHeader & 1;
			bool frame = objectHeader & 2;
			//			bool locked = objectHeader & 16;
			//			bool readOnly = objectHeader & 64;
			//			bool dirty = objectHeader & 128;
			unsigned int size = objectHeader >> 8;
			// Binary
			KUInt32 nextRef;
			if (mMemory->Read(addr + 8, nextRef))
			{
				char theLine[512];
				(void) ::snprintf(theLine, sizeof(theLine), "Memory error while reading %.8X\n", (unsigned int) addr + 8);
				PrintLine(theLine, MONITOR_LOG_ERROR);
				return -1;
			}
			if (!slotted)
			{
				unsigned int length = size - 8;
				return FormatNSBinary(buffer, bufferSize, addr, length, nextRef, indent, maxDepth);
			} else if (!frame)
			{
				int r = ::snprintf(buffer, bufferSize, "[array, class ");
				r += FormatNSRef(buffer + r, bufferSize - r, nextRef, indent, maxDepth);
				r += ::snprintf(buffer + r, bufferSize - r, ", length %d]", size);
				return r;
			} else
			{
				return FormatNSFrame(buffer, bufferSize, addr, (size / sizeof(KUInt32)) - 3, nextRef, indent, maxDepth);
			}
		}

		case kTagImmed: {
			if (inRef == 0x2)
			{
				return snprintf(buffer, bufferSize, "NIL");
			}
			if (inRef == 0x1A)
			{
				return snprintf(buffer, bufferSize, "TRUE");
			}
			if (inRef == 0x55552)
			{
				return snprintf(buffer, bufferSize, "kSymbolClass");
			}
			if ((inRef & 0xFF) == 0x32)
			{
				KUInt32 funcKind = inRef >> 8;
				return snprintf(buffer, bufferSize, "kFuncClass<%d>", funcKind);
			}
			if ((inRef & 0xF) == 0x6)
			{
				KUInt32 immedValue = inRef >> 4;
				if (immedValue > 32 && immedValue < 127)
				{
					return snprintf(buffer, bufferSize, "$%c", (char) immedValue);
				} else if (immedValue < 0x100)
				{
					return snprintf(buffer, bufferSize, "$\\%.2X", (unsigned int) immedValue);
				} else
				{
					return snprintf(buffer, bufferSize, "$\\u%.4lX", (unsigned long) immedValue);
				}
			}
			return snprintf(buffer, bufferSize, "Immed<%lX>", (unsigned long) (inRef >> 2));
		}

		case kTagMagicPtr: {
			KSInt32 index = inRef >> 2;
			return snprintf(buffer, bufferSize, "@%ld", (long) index);
		}
	}
	return 0;
}

int
TMonitor::FormatNSFrame(char* buffer, size_t bufferSize, KUInt32 inAddr, unsigned int length, KUInt32 mapRef, int indent, int maxDepth)
{

	KUInt32* flattenMap = (KUInt32*) ::malloc(length * sizeof(KUInt32));
	int mapIndex = length;
	while (true)
	{
		if ((mapRef & 0x3) != kTagPointer)
		{
			::free(flattenMap);
			return snprintf(buffer, bufferSize, "{frame with invalid map ref, length = %d}", length);
		}
		KUInt32 mapObjectHeader;
		KUInt32 mapAddr = mapRef - kTagPointer;
		if (mMemory->Read((TMemory::VAddr) mapAddr, mapObjectHeader))
		{
			char theLine[512];
			(void) ::snprintf(theLine, sizeof(theLine), "Memory error while reading %.8X\n", (unsigned int) mapAddr);
			PrintLine(theLine, MONITOR_LOG_ERROR);
			::free(flattenMap);
			return -1;
		}
		if ((mapObjectHeader & 0x3) != 1)
		{
			::free(flattenMap);
			return snprintf(buffer, bufferSize, "{frame with invalid map ref, length = %d}", length);
		}
		unsigned int mapObjectSize = mapObjectHeader >> 8;
		unsigned int mapObjectLength = (mapObjectSize / sizeof(KUInt32)) - 3;
		for (unsigned int i = mapObjectLength - 1; i > 0; i--)
		{
			KUInt32 refAddr = mapAddr + (3 + i) * sizeof(KUInt32);
			KUInt32 refValue;
			if (mMemory->Read((TMemory::VAddr) refAddr, refValue))
			{
				char theLine[512];
				(void) ::snprintf(theLine, sizeof(theLine), "Memory error while reading %.8X\n", (unsigned int) refAddr);
				PrintLine(theLine, MONITOR_LOG_ERROR);
				::free(flattenMap);
				return -1;
			}
			if (mapIndex <= 0)
			{
				::free(flattenMap);
				return snprintf(buffer, bufferSize, "{frame with invalid map, length = %d}", length);
			}
			flattenMap[--mapIndex] = refValue;
		}
		KUInt32 supermapRef;
		KUInt32 supermapAddr = mapAddr + (3 * sizeof(KUInt32));
		if (mMemory->Read((TMemory::VAddr) supermapAddr, supermapRef))
		{
			char theLine[512];
			(void) ::snprintf(theLine, sizeof(theLine), "Memory error while reading %.8X\n", (unsigned int) supermapAddr);
			PrintLine(theLine, MONITOR_LOG_ERROR);
			::free(flattenMap);
			return -1;
		}
		if (supermapRef == 0x2)
		{
			break;
		}
		mapRef = supermapRef;
	}
	if (mapIndex > 0)
	{
		::free(flattenMap);
		return snprintf(buffer, bufferSize, "{frame with invalid map, length = %d}", length);
	}

	int r = ::snprintf(buffer, bufferSize, "{");
	for (unsigned int i = 0; i < length; i++)
	{
		int symbolStart = r;
		r += FormatNSRef(buffer + r, std::max((ssize_t) bufferSize - r, 0L), flattenMap[i], indent + 1, maxDepth);
		r += ::snprintf(buffer + r, std::max((ssize_t) bufferSize - r, 0L), ": ");
		KUInt32 valueRef;
		KUInt32 valueAddr = inAddr + ((3 + i) * sizeof(KUInt32));
		if (mMemory->Read((TMemory::VAddr) valueAddr, valueRef))
		{
			char theLine[512];
			(void) ::snprintf(theLine, sizeof(theLine), "Memory error while reading %.8X\n", (unsigned int) valueAddr);
			PrintLine(theLine, MONITOR_LOG_ERROR);
			::free(flattenMap);
			return -1;
		}
		int symbolLen = r - symbolStart;
		r += FormatNSRef(buffer + r, std::max((ssize_t) bufferSize - r, 0L), valueRef, indent + symbolLen, maxDepth - 1);
		if (i < length - 1)
		{
			r += ::snprintf(buffer + r, std::max((ssize_t) bufferSize - r, 0L), ",\n");
			for (int j = 0; j <= indent; j++)
			{
				r += ::snprintf(buffer + r, std::max((ssize_t) bufferSize - r, 0L), " ");
			}
		} else
		{
			r += ::snprintf(buffer + r, std::max((ssize_t) bufferSize - r, 0L), "}");
		}
	}
	::free(flattenMap);
	return r;
}

int
TMonitor::FormatNSBinary(char* buffer, size_t bufferSize, KUInt32 inAddr, unsigned int length, KUInt32 classRef, int indent, int maxDepth)
{
	if (classRef == 0x55552)
	{
		// Symbol, skip hash value.
		bool needEscape = false;
		char* symbolStr = (char*) ::malloc(length + 1);
		for (unsigned int i = 0; i < length; i++)
		{
			KUInt8 byte;
			KUInt32 byteAddr = inAddr + 16 + i;
			if (mMemory->ReadB((TMemory::VAddr) byteAddr, byte))
			{
				char theLine[512];
				(void) ::snprintf(theLine, sizeof(theLine), "Memory error while reading %.8X\n", (unsigned int) byteAddr);
				PrintLine(theLine, MONITOR_LOG_ERROR);
				::free(symbolStr);
				return -1;
			}
			if (i == (length - 1) && byte != 0)
			{
				::free(symbolStr);
				return ::snprintf(buffer, bufferSize, "<invalid symbol not null terminated, length %d>", length);
			}
			if (i == 0)
			{
				needEscape = (byte < 'a' || byte > 'z') && (byte < 'A' || byte > 'Z') && (byte != '_');
			} else if (i < length - 1)
			{
				needEscape = needEscape || ((byte < 'a' || byte > 'z') && (byte < 'A' || byte > 'Z') && (byte != '_') && (byte < '0' || byte > '9'));
			}
			symbolStr[i] = byte;
		}
		int ret = 0;
		if (needEscape)
		{
			ret = ::snprintf(buffer, bufferSize, "'|%s|", symbolStr);
		} else
		{
			ret = ::snprintf(buffer, bufferSize, "'%s", symbolStr);
		}
		::free(symbolStr);
		return ret;
	}
	int r = ::snprintf(buffer, bufferSize, "<binary, class ");
	r += FormatNSRef(buffer + r, bufferSize - r, classRef, indent, maxDepth);
	r += ::snprintf(buffer + r, bufferSize - r, ", length %d, ptr %.8X>", length, (unsigned int) (inAddr + 12));
	return r;
}

// -------------------------------------------------------------------------- //
//  * CreateCondVarAndMutex( void )
// -------------------------------------------------------------------------- //
inline void
TMonitor::CreateCondVarAndMutex()
{
	mCondVar = new TCondVar();
	mMutex = new TMutex();
}

// -------------------------------------------------------------------------- //
//  * DeleteCondVarAndMutex( void )
// -------------------------------------------------------------------------- //
inline void
TMonitor::DeleteCondVarAndMutex()
{
	mMutex->Unlock();
	delete mMutex;
	delete mCondVar;
}

// -------------------------------------------------------------------------- //
//  * SignalCondVar( void )
// -------------------------------------------------------------------------- //
inline void
TMonitor::SignalCondVar()
{
	mCondVar->Signal();
}

// -------------------------------------------------------------------------- //
//  * WaitOnCondVar( void )
// -------------------------------------------------------------------------- //
inline void
TMonitor::WaitOnCondVar()
{
	mCondVar->Wait(mMutex);
}

// -------------------------------------------------------------------------- //
//  * AcquireMutex( void )
// -------------------------------------------------------------------------- //
inline void
TMonitor::AcquireMutex()
{
	mMutex->Lock();
}

// -------------------------------------------------------------------------- //
//  * ReleaseMutex( void )
// -------------------------------------------------------------------------- //
inline void
TMonitor::ReleaseMutex()
{
	mMutex->Unlock();
}

// ==================================================================== //
// I am not now, nor have I ever been, a member of the demigodic party. //
//                 -- Dennis Ritchie                                    //
// ==================================================================== //
