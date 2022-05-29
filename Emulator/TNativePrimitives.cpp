// ==============================
// File:			TNativePrimitives.cp
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

#include "TNativePrimitives.h"

// POSIX
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

#if !TARGET_OS_WIN32
#include <unistd.h>
#endif

// K
#include <K/Streams/TStream.h>

// Einstein
#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Printer/TPrinterManager.h"
#include "Emulator/Screen/TScreenManager.h"
#include "Emulator/Serial/TSerialHostPortDirect.h"
#include "Emulator/Sound/TSoundManager.h"
#if !TARGET_OS_MAC
#include "Emulator/NativeCalls/TNativeCalls.h"
#endif
#include "Emulator/NativeCalls/TVirtualizedCalls.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"
#include "Emulator/Platform/PlatformGestalt.h"
#include "Emulator/Platform/TPlatformManager.h"
#if TARGET_OS_MAC
#include "Emulator/NativeCalls/TObjCBridgeCalls.h"
#endif

#ifdef USE_TOOLKIT
#include "Toolkit/TToolkit.h"
#include "app/FLTK/TFLApp.h"
#include <FL/fl_utf8.h>
#endif

// Native primitives implement stores to coprocessor #10

struct NewtonPixmap {
	KUInt32 addy;
	KUInt32 rowBytes;
	TScreenManager::SRect bounds;
	KUInt32 flags;
	KUInt32 table;
};

#define debugFlash 0

#define ENABLE_LOG_PLATFORM (1 << 0x1)
#define ENABLE_LOG_SOUND (1 << 0x2)
#define ENABLE_LOG_BATTERY (1 << 0x3)
#define ENABLE_LOG_SCREEN (1 << 0x4)
#define ENABLE_LOG_TABLET (1 << 0x5)
#define ENABLE_LOG_SERIAL (1 << 0x6)
#define ENABLE_LOG_IN_TRANSLATOR (1 << 0x7)
#define ENABLE_LOG_OUT_TRANSLATOR (1 << 0x8)
#define ENABLE_LOG_HOSTCALL (1 << 0x9)
#define ENABLE_LOG_NETWORKMANAGER (1 << 0xa)
#define ENABLE_LOG_HOSTIOS_NATIVEIOS (1 << 0xb)
#define ENABLE_LOG_PRINTER (1 << 0xc)

#define LOG_PLATFORM (mLog && (mLogMask & ENABLE_LOG_PLATFORM))
#define LOG_SOUND (mLog && (mLogMask & ENABLE_LOG_SOUND))
#define LOG_BATTERY (mLog && (mLogMask & ENABLE_LOG_BATTERY))
#define LOG_SCREEN (mLog && (mLogMask & ENABLE_LOG_SCREEN))
#define LOG_TABLET (mLog && (mLogMask & ENABLE_LOG_TABLET))
#define LOG_SERIAL (mLog && (mLogMask & ENABLE_LOG_SERIAL))
#define LOG_IN_TRANSLATOR (mLog && (mLogMask & ENABLE_LOG_IN_TRANSLATOR))
#define LOG_OUT_TRANSLATOR (mLog && (mLogMask & ENABLE_LOG_OUT_TRANSLATOR))
#define LOG_HOSTCALL (mLog && (mLogMask & ENABLE_LOG_HOSTCALL))
#define LOG_NETWORKMANAGER (mLog && (mLogMask & ENABLE_LOG_NETWORKMANAGER))
#define LOG_HOSTIOS_NATIVEIOS (mLog && (mLogMask & ENABLE_LOG_HOSTIOS_NATIVEIOS))
#define LOG_PRINTER (mLog && (mLogMask & ENABLE_LOG_PRINTER))

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TNativePrimitives( TLog*, TMemory* )
// -------------------------------------------------------------------------- //
TNativePrimitives::TNativePrimitives(
	TLog* inLog,
	TMemory* inMemory) :
		mProcessor(nil),
		mLog(inLog),
		mLogMask(0b000000000000),
		mMemory(inMemory),
		mEmulator(nil),
		mNetworkManager(nil),
		mSoundManager(nil),
		mScreenManager(nil),
		mPlatformManager(nil),
#if !TARGET_OS_MAC
		mNativeCalls(new TNativeCalls(inMemory)),
#endif
		mVirtualizedCalls(nil),
#if TARGET_OS_MAC
		mObjCBridgeCalls(new TObjCBridgeCalls(inMemory)),
#endif
		mInputVolume(0),
		mQuit(false)
{
}

// -------------------------------------------------------------------------- //
//  * ~TNativePrimitives( void )
// -------------------------------------------------------------------------- //
TNativePrimitives::~TNativePrimitives(void)
{
#if !TARGET_OS_MAC
	delete mNativeCalls;
#endif

	if (mVirtualizedCalls)
	{
		delete mVirtualizedCalls;
		mVirtualizedCalls = nil;
	}
}

// -------------------------------------------------------------------------- //
//  * SetEmulator( TEmulator* )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::SetEmulator(TEmulator* inEmulator)
{
	if (inEmulator)
	{
		mNetworkManager = inEmulator->GetNetworkManager();
		mPrinterManager = inEmulator->GetPrinterManager();
		mSoundManager = inEmulator->GetSoundManager();
		mScreenManager = inEmulator->GetScreenManager();
		mPlatformManager = inEmulator->GetPlatformManager();
		mEmulator = inEmulator;
		mVirtualizedCalls = new TVirtualizedCalls(inEmulator, mMemory, inEmulator->GetProcessor());
	} else
	{
		mNetworkManager = nil;
		mPrinterManager = nil;
		mSoundManager = nil;
		mScreenManager = nil;
		mPlatformManager = nil;
		mEmulator = nil;
		if (mVirtualizedCalls)
		{
			delete mVirtualizedCalls;
			mVirtualizedCalls = nil;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteNative(KUInt32 inInstruction)
{
	if (inInstruction & 0x80000000)
	{
		// If the high bit is set, this instruction is actually a patch, not
		// a real ARM instruction.  The lower 31 bits are an enum value
		// identifying a virtualized call. These enums are defined in
		// TVirtualizedCallsPatches.h

		mVirtualizedCalls->Execute(inInstruction & ~0x80000000);
	} else
	{
		// Now execute the native implementation of the coprocessor

		switch (inInstruction >> 8)
		{
			case 0x000000:
				ExecuteFlashDriverNative(inInstruction);
				break;

			case 0x000001:
				ExecutePlatformDriverNative(inInstruction);
				break;

			case 0x000002:
				ExecuteSoundDriverNative(inInstruction);
				break;

			case 0x000003:
				ExecuteBatteryDriverNative(inInstruction);
				break;

			case 0x000004:
				ExecuteScreenDriverNative(inInstruction);
				break;

			case 0x000005:
				ExecuteTabletDriverNative(inInstruction);
				break;

			case 0x000006:
				ExecuteSerialDriverNative(inInstruction);
				break;

			case 0x000007:
				ExecuteInTranslatorNative(inInstruction);
				break;

			case 0x000008:
				ExecuteOutTranslatorNative(inInstruction);
				break;

			case 0x000009:
				ExecuteHostCallNative(inInstruction);
				break;

			case 0x00000A:
				ExecuteNetworkManagerNative(inInstruction);
				break;

#if TARGET_OS_MAC
			case 0x00000B:
				ExecuteHostiOSNativeiOS(inInstruction);
				break;
#endif

			case 0x00000C:
				ExecutePrinterDriverNative(inInstruction);
				break;

			default:
				if (mLog)
				{
					mLog->FLogLine(
						"Unimplemented native primitive %.8X (pc=%.8X)",
						(unsigned int) inInstruction,
						(unsigned int) mProcessor->GetRegister(15));
				}
		}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteFlashDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteFlashDriverNative(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
		case 0x01: {
#if debugFlash
			KUInt32 chipAddr = mProcessor->GetRegister(1);
#endif
			// 34000000 & 34400000
			KUInt32 mask = mProcessor->GetRegister(2);
			KUInt32 theIDStructAddr = mProcessor->GetRegister(3);
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::Identify(%.8X, %.8X, %.8X)",
					(unsigned int) chipAddr,
					(unsigned int) mask,
					(unsigned int) theIDStructAddr);
			}
#endif
			// For 4MB, just return 1 for 0x34000000 only.
			//			if ((chipAddr == 0x34000000)
			//				&&
			if ((mask == 0xFF000000)
				|| (mask == 0x00FF0000)
				|| (mask == 0x0000FF00)
				|| (mask == 0x000000FF))
			//				if ((mask == 0xFFFF0000)
			//					|| (mask == 0x0000FFFF))
			//				if (mask == 0x0000FF00) // 1 lane.
			{
				mProcessor->SetRegister(0, 1);
				(void) mMemory->Write(theIDStructAddr + 0x00, 0x00000089);
				(void) mMemory->Write(theIDStructAddr + 0x04, 0x00000000);
				(void) mMemory->Write(theIDStructAddr + 0x08, 0x00000002);
				(void) mMemory->Write(theIDStructAddr + 0x0C, 0x00000002);
				(void) mMemory->Write(theIDStructAddr + 0x10, 0x00200000);
				(void) mMemory->Write(theIDStructAddr + 0x14, 0x00010000);
			} else
			{
				mProcessor->SetRegister(0, 0);
			}
		}
		break;

		case 0x02:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::CleanUp");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x03:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::Init");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x04:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::InitializeDriverData");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::CleanUpDriverData");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x06:
#if debugFlash > 1
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::StartReadingArray");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07:
#if debugFlash > 1
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::DoneReadingArray");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x08: {
			KUInt32 flashRange;
			(void) mMemory->Read(mProcessor->GetRegister(13) + 4, flashRange);
			KUInt32 virtualTable;
			(void) mMemory->Read(flashRange, virtualTable);
			// Very ugly way to determine that we do 32 bits.
			// (btw, we only have 16 bits accesses from the OS, but
			// 8 bits may be possible as well).

			// PLATFORM SPECIFIC HACK
			//					16 bits		32 bits
			// MP2100D			0001E3C8	0001E3E0
			// MP2x00US			0001E3BC	0001E3D4
			// EM300			0001E168	0001E180
			// Watson			0001E3BC	0001E3D4
			Boolean is32bits = (virtualTable == 0x0001E3D4)
				|| (virtualTable == 0x0001E3E0)
				|| (virtualTable == 0x0001E180);
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::Write(data=%.8X, mask=%.8X, addr=%.8X, VT=%.8X, %s)",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3),
					(unsigned int) virtualTable,
					is32bits ? "32bits" : "16bits");
			}
#endif
			Boolean theResult;
			if (is32bits)
			{
				theResult = mMemory->WriteToFlash32Bits(
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3));
			} else
			{
				theResult = mMemory->WriteToFlash16Bits(
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3));
			}
			if (theResult)
			{
				mProcessor->SetRegister(0,
					(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */);
			} else
			{
				mProcessor->SetRegister(0, 0);
			}
		}
		break;

		case 0x09: {
			KUInt32 flashRange = mProcessor->GetRegister(1);
			KUInt32 virtualTable;
			(void) mMemory->Read(flashRange, virtualTable);
			// Very ugly way to determine that we do 32 bits.
			// (btw, we only have 16 bits accesses from the OS, but
			// 8 bits may be possible as well).

			// PLATFORM SPECIFIC HACK
			//					16 bits		32 bits
			// MP2100D			0001E3C8	0001E3E0
			// MP2x00US			0001E3BC	0001E3D4
			// EM300			0001E168	0001E180
			// Watson			0001E3BC	0001E3D4
			Boolean is32bits = (virtualTable == 0x0001E3D4)
				|| (virtualTable == 0x0001E3E0)
				|| (virtualTable == 0x0001E180);

#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::StartErase(FR, %.8X, %s)",
					(unsigned int) mProcessor->GetRegister(2),
					is32bits ? "32bits" : "16bits");
			}
#endif
			Boolean theResult;
			if (is32bits)
			{
				theResult = mMemory->EraseFlash(
					(unsigned int) mProcessor->GetRegister(2),
					0x20000);
			} else
			{
				theResult = mMemory->EraseFlash(
					(unsigned int) mProcessor->GetRegister(2),
					0x10000);
			}

			if (theResult)
			{
				mProcessor->SetRegister(0,
					(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */);
			} else
			{
				mProcessor->SetRegister(0, 0);
			}
		}
		break;

		case 0x0A:
#if debugFlash > 1
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::ResetBlockStatus");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0B:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::IsEraseComplete");
			}
#endif
			// Erase is always complete with no error :D
			mProcessor->SetRegister(0, 1);
			(void) mMemory->Write(
				(unsigned int) mProcessor->GetRegister(3),
				(unsigned int) 0x00000000);
			break;

		case 0x0C:
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::LockBlock(FR, %.8X)",
					(unsigned int) mProcessor->GetRegister(2));
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0D:
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::BeginWrite(%.8X, %.8X, %.8X)",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3));
			}
#endif
			if (mMemory->TranslateAndCheckFlashAddress(mProcessor->GetRegister(2), nil))
			{
				mProcessor->SetRegister(0,
					(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */);
			} else
			{
				mProcessor->SetRegister(0, 0);
			}
			break;

		case 0x0E:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine("TEinsteinFlashDriver::ReportWriteResult");
			}
#endif
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0F: {
			KUInt32 startOfBlock;
			(void) mMemory->Read(mProcessor->GetRegister(13) + 4, startOfBlock);
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::DoWrite(data=%.8X, mask=%.8X, addr=%.8X, Start=%.8X)",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3),
					(unsigned int) startOfBlock);
			}
#endif
			mProcessor->SetRegister(0, 0);
		}
		break;

		case 0x10: {
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::DoErase(start=%.8X, size=%.8X)",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2));
			}
#endif
			Boolean theResult = mMemory->EraseFlash(
				(unsigned int) mProcessor->GetRegister(1),
				(unsigned int) mProcessor->GetRegister(2));

			if (theResult)
			{
				mProcessor->SetRegister(0,
					(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */);
			} else
			{
				mProcessor->SetRegister(0, 0);
			}
		}
		break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown flash driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecutePlatformDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecutePlatformDriverNative(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::New");
			}
			break;

		case 0x02:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::Delete");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x03:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::Init");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x04:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::BacklightTrigger");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::RegisterPowerSwitchInterrupt");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x06:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::EnableSysPowerInterrupt");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::InterruptHandler");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x08:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::TimerInterruptHandler");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x09:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::ResetZAPStoreCheck");
			}
			if (mEmulator->ZAPMemory())
			{
				mProcessor->SetRegister(0, 1);
				mEmulator->ZAPMemory(false);
			} else
			{
				mProcessor->SetRegister(0, 0);
			}
			break;

		case 0x0A: {
			// Boot process powers system up in this order: 0x1d, 0x01, 0x23, 0x22
			// Power swicth on events create the same event order
			// Inserting a PCMCIA card powers up 0x1d
			// Starting "Dock" creates 0x1d, starting serial com generates 0x01, 0x23, 0x22

			// Physical supply bitmap
			// 01: TGeoPortDebugLink::PowerCycle(unsigned char)
			// 02: PowerOnSrc12v
			// 08: PowerOnIC5v
			// 10: PowerOnDMA

			// Calls to IOPowerOn, calling TPlatformDriver::PowerOnSubsystem(unsigned long):
			// 01: TSerialChipVoyager
			// 01, 02, 03, 04: TSerialChipVoyager::PowerOn(void), PowerOnSrc5v
			// 10 (Vcc), 14 (Vpp): PCMCIA Port 0
			// 11 (Vcc), 15 (Vpp): PCMCIA Port 1
			// 1c: TBIOInterface::Init(void)
			// 1d: PCMCIA Cards power: RestoreCardPower(unsigned long)
			// 18, 1a: sound: PCirrusSoundDriver::PowerInputOn(long)
			// 1b: PCirrusSoundDriver::PowerOutputOn(long)
			// 1f, 20, 1c: TVoyagerPlatform::PowerOnSystem(void)
			// 22: TDMAManager::PowerOnAssignment(unsigned long)
			// 23: TAsyncSerToolF, DMAManager

			// Subsystems may be Serial, Screen :: 0x0014824C
			KUInt32 theSubsystem = mProcessor->GetRegister(1);
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::PowerOnSubsystem( %.8X )",
					(unsigned int) theSubsystem);
			}
			if (theSubsystem == 0x1D)
			{
				mMemory->PowerOnFlash();
				//					mEmulator->BreakInMonitor();
			}
			if (theSubsystem == 0x23)
			{
				// the system is up and running, check if we missed any Einstein events
				// and they are still pending in the queue
				mPlatformManager->UnlockQueueBootLock();
				mEmulator->DoPowerRestored();
			}
			mProcessor->SetRegister(0, 0);
		}
		break;

		case 0x0B: {
			KUInt32 theSubsystem = mProcessor->GetRegister(1);
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::PowerOffSubsystem( %.8X )",
					(unsigned int) theSubsystem);
			}
			if (theSubsystem == 0x1D)
			{
				mMemory->PowerOffFlash();
			}
			mProcessor->SetRegister(0, 0);
		}
		break;

		case 0x0C:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::PowerOffAllSubsystems");
			}
			mMemory->PowerOffFlash();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0D:
			if (LOG_PLATFORM)
			{
				//				mLog->LogLine( "TMainPlatformDriver::PauseSystem" );
			}
			mEmulator->PauseSystem();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0E:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::PowerOffSystem");
			}
			mMemory->PowerOffFlash();
			mPlatformManager->PowerOff();
			if (mQuit)
			{
				mEmulator->Quit();
			} else
			{
				mEmulator->PauseSystem();
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0F:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::PowerOnSystem");
			}
			mMemory->PowerOnFlash();
			mPlatformManager->PowerOn();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x10:
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::TranslatePowerEvent( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x11:
			// kPCMCIA5VAvailable   =    0x0001,    // 5.0 V available
			// kPCMCIA12VAvailable  =    0x0002,    // 12.0 V available
			// kPCMCIA3p3VAvailable =    0x0004,    // 3.3 V available
			// kPCMCIAXpXVAvailable =    0x0008,    // X.X V available
			// kPCMCIAYpYVAvailable =    0x0010,    // Y.Y V available
			// kPCMCIA0VAvailable   =    0x0020     // 0 V available (for example: 0 V Vpp)
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::GetPCMCIAPowerSpec( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
			}

			if (mProcessor->GetRegister(1) == 0)
			{
				(void) mMemory->Write(
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) 5); // 3.3V, 5V
				mProcessor->SetRegister(0, 0);
			} else if (mProcessor->GetRegister(1) == 1)
			{
				(void) mMemory->Write(
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) 7); // 3.3V, 5V, 12V
				mProcessor->SetRegister(0, 0);
			} else
			{
				mProcessor->SetRegister(0, (unsigned int) -10005); // "Call not implemented"
			}
			break;

		case 0x12:
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::PowerOnDeviceCheck( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
			}
			{
				static int firstPause = 1;
				if (firstPause)
				{
					firstPause--;
					// This will remove the boot-progress display
					if (mScreenManager->OverlayIsOn())
					{
						mScreenManager->OverlayOff();
					}
					// this is a hack that will install packages that were added to a
					// directory on the host. This is used by iOS/iPhone/Android.
					if (firstPause == 0)
					{
						mPlatformManager->InstallNewPackages();
					}
				}
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x13:
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::SetSubsystemPower( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2));
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x14:
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::GetSubsystemPower( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
			}
			(void) mMemory->Write(
				(unsigned int) mProcessor->GetRegister(2),
				(unsigned int) 0);
			mProcessor->SetRegister(0, 0);
			break;

		case 0x15:
			// GetNextEvent.
			{
				KUInt32 theWiredAddress;
				(void) mMemory->TranslateW(mProcessor->GetRegister(1), theWiredAddress);
				Boolean gotSomeEvent = mPlatformManager->GetNextEvent(theWiredAddress);
				mProcessor->SetRegister(0, gotSomeEvent);
			}
			break;

		case 0x16:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::Hop!");
			}
			mProcessor->SetRegister(15, mProcessor->GetRegister(15) + 4);
			mEmulator->BreakInMonitor("TMainPlatformDriver::Hop!");
			break;

		case 0x17:
			if (LOG_PLATFORM)
			{
				mLog->LogLine("TMainPlatformDriver::FillGestaltEmulatorInfo");
			}
			(void) mMemory->Write(
				(unsigned int) mProcessor->GetRegister(1),
				(unsigned int) kUP2Version);
			mProcessor->SetRegister(0, 0);
			break;

		case 0x18:
			mPlatformManager->LockEventQueue();
			break;

		case 0x19:
			mPlatformManager->UnlockEventQueue();
			break;

		case 0x1A:
			// Log
			{
				KUInt32 theAddress = mProcessor->GetRegister(1);
				char theLine[512];
				KUInt32 amount = sizeof(theLine);
				(void) mMemory->FastReadString(theAddress, &amount, theLine);
#ifdef USE_TOOLKIT
				// theLine is encoded in ISO format
				// if the Toolkit is available, send the text to the monitor
				if (gToolkit)
				{
					unsigned srcLen = strlen(theLine);
					unsigned dstLen = fl_utf8froma(nullptr, 0, theLine, srcLen);

					char* dstText = (char*) ::malloc(dstLen + 1);
					fl_utf8froma(dstText, dstLen + 1, theLine, srcLen);
					for (char* t = dstText; *t; t++)
						if (*t == '\r')
							*t = '\n';
					gToolkit->PrintStd(dstText);
					free(dstText);
					// gToolkit->PrintStd("\n");
				}
#endif
				// TODO should we convert the output text into another encoding?
				if (mLog)
				{
					mLog->LogLine(theLine);
				} else
				{
					KPrintf("Log: %s\n", theLine);
				}
			}
			break;

		case 0x1B:
			mProcessor->SetRegister(0,
				mPlatformManager->GetUserInfo(
					(EUserInfoSel) mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3)));
			break;

		case 0x1C:
			mProcessor->SetRegister(0,
				mPlatformManager->GetHostTimeZone());
			break;

		case 0x1D:
// CalibrateTablet.
#if TARGET_OS_WIN32
			// FIXME call the Win32 tablet calibration app
#elif TARGET_IOS
#elif TARGET_OS_MAC
#elif TARGET_ANDROID
#else
		{
			// Try xtscal
			FILE* theFile = popen("xtscal", "r+");
			if (theFile)
			{
				(void) pclose(theFile);
			} // Otherwise, don't do anything.
		}
#endif
			break;

		case 0x1E:
			// Quit.
			if (mQuit)
			{
				// Force.
				mEmulator->Quit();
			} else
			{
				mQuit = true;
				mPlatformManager->SendPowerSwitchEvent();
			}
			break;

		case 0x1F:
			// DisposeBuffer.
			mProcessor->SetRegister(0,
				mPlatformManager->DisposeBuffer(mProcessor->GetRegister(1)));
			break;

		case 0x20:
			// CopyBufferData.
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				mProcessor->SetRegister(0,
					mPlatformManager->CopyBufferData(
						mProcessor->GetRegister(1),
						mProcessor->GetRegister(2),
						mProcessor->GetRegister(3),
						fourthParam));
			}
			break;

		case 0x21:
			if (LOG_PLATFORM)
			{
				mLog->FLogLine("TMainPlatformDriver::OpenEinsteinMenu()");
			}
			mPlatformManager->OpenEinsteinMenu();
			break;

		case 0x22:
			if (LOG_PLATFORM)
			{
				mLog->FLogLine("TMainPlatformDriver::NewtonScriptCall()");
			}
			{
				using namespace TNewt;
				NewtRef ret = mPlatformManager->NewtonScriptCall(
					RefVar::FromPtr(mProcessor->GetRegister(0)),
					RefVar::FromPtr(mProcessor->GetRegister(1)),
					RefVar::FromPtr(mProcessor->GetRegister(2)));
				mProcessor->SetRegister(0, ret);
			}
			break;

		default:
			if (LOG_PLATFORM)
			{
				mLog->FLogLine(
					"Unknown platform driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteSoundDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteSoundDriverNative(KUInt32 inInstruction)
{
	//	mEmulator->BreakInMonitor();
	switch (inInstruction & 0xFF)
	{
			// No longer native.
			//		case 0x01:
			//			if (LOG_SOUND)
			//			{
			//				mLog->LogLine( "PMainSoundDriver::New" );
			//			}
			//			break;

			// No longer native.
			//		case 0x02:
			//			if (LOG_SOUND)
			//			{
			//				mLog->LogLine( "PMainSoundDriver::Delete" );
			//			}
			//			mProcessor->SetRegister( 0, 0 );
			//			break;

		case 0x03:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::SetSoundHardwareInfo");
			}
			mProcessor->SetRegister(0, (KUInt32) -30009);
			break;

		case 0x04:
			//			mEmulator->BreakInMonitor();
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::GetSoundHardwareInfo");
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister(1);
				(void) mMemory->Write(theInfoStructAddr + 0x00, 0x00000001); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x04, 0x00000001); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x08, 0x00000001); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x54600000); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00000006); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x00000010); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000001); // unknown
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05: {
			// mEmulator->BreakInMonitor();
			KUInt32 fourthParam;
			(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::SetOutputBuffers(");
				mLog->FLogLine(
					"  %.8X, %.8X, %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3),
					(unsigned int) fourthParam);
			}
			mSoundOutputBuffer1Addr = mProcessor->GetRegister(1);
			//				mSoundOutputBuffer1Size = mProcessor->GetRegister(2);
			mSoundOutputBuffer2Addr = mProcessor->GetRegister(3);
			//				mSoundOutputBuffer2Size = fourthParam;
		}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x06:
			if (LOG_SOUND)
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);

				mLog->LogLine("PMainSoundDriver::SetInputBuffers(");
				mLog->FLogLine(
					"  %.8X, %.8X, %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3),
					(unsigned int) fourthParam);
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07:
			if (LOG_SOUND)
			{
				mLog->FLogLine(
					"PMainSoundDriver::ScheduleOutputBuffer( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2));
			}
			{
				KUInt32 buffer;
				if (mProcessor->GetRegister(1))
				{
					buffer = mSoundOutputBuffer2Addr;
				} else
				{
					buffer = mSoundOutputBuffer1Addr;
				}
				KUInt32 amount = mProcessor->GetRegister(2);
				mSoundManager->ScheduleOutputBuffer(buffer, amount);
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x08:
			if (LOG_SOUND)
			{
				mLog->FLogLine(
					"PMainSoundDriver::ScheduleInputBuffer( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2));
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x09:
			if (LOG_SOUND)
			{
				mLog->FLogLine(
					"PMainSoundDriver::PowerOutputOn( %i )",
					(unsigned int) mProcessor->GetRegister(1));
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0A:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::PowerOutputOff");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0B:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::PowerInputOn");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0C:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::PowerInputOff");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0D:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::StartOutput");
			}
			mSoundManager->StartOutput();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0E:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::StartInput");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0F:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::StopOutput");
			}
			mSoundManager->StopOutput();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x10:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::StopInput");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x11:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::OutputIsEnabled");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x12:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::InputIsEnabled");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x13:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::OutputIsRunning");
			}
			mProcessor->SetRegister(0, mSoundManager->OutputIsRunning());
			break;

		case 0x14:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::InputIsRunning");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x15:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::CurrentOutputPtr");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x16:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::CurrentInputPtr");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x17:
			if (LOG_SOUND)
			{
				mLog->FLogLine(
					"PMainSoundDriver::OutputVolume( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
			}
			mSoundManager->OutputVolume(mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x18:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::OutputVolume");
			}
			mProcessor->SetRegister(0, mSoundManager->OutputVolume());
			break;

		case 0x19:
			if (LOG_SOUND)
			{
				mLog->FLogLine(
					"PMainSoundDriver::InputVolume( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
			}
			{
				KUInt32 param = mProcessor->GetRegister(1);
				if (param > 0xFF)
				{
					param = 0xFF;
				}
				mInputVolume = (KUInt8) param;
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1A:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::InputVolume");
			}
			mProcessor->SetRegister(0, mInputVolume);
			break;

		case 0x1B:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::EnableExtSoundSource");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1C:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::DisableExtSoundSource");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1D:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::OutputIntHandler");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1E:
			if (LOG_SOUND)
			{
				mLog->LogLine("PMainSoundDriver::InputIntHandler");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1F:
			if (LOG_SOUND)
			{
				mLog->FLogLine(
					"PMainSoundDriver::NativeSetInterruptMask( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2));
			}
			mSoundManager->SetInterruptMask(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2));
			break;

		default:
			if (LOG_SOUND)
			{
				mLog->FLogLine(
					"Unknown sound driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteBatteryDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteBatteryDriverNative(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::New");
			}
			break;

		case 0x02:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::Delete");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x03:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::Init");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x04:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::WakeUp");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::ShutDown");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x06:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::Count");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07:
			/*
			 #import <IOKit/ps/IOPowerSources.h>
			 #import <IOKit/ps/IOPSKeys.h>
			 CFTypeRef               info;
			 CFArrayRef              list;
			 CFDictionaryRef         battery;
			 info = IOPSCopyPowerSourcesInfo();
			 list = IOPSCopyPowerSourcesList(info);	// ->CFRelease(info);
			 if(CFArrayGetCount(list) && (battery = IOPSGetPowerSourceDescription(info, CFArrayGetValueAtIndex(list, 0)))) {
				outputCapacity = [[(NSDictionary*)battery objectForKey:@kIOPSCurrentCapacityKey] doubleValue];
			 }
			 */
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::Status");
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister(2);
				(void) mMemory->Write(theInfoStructAddr + 0x00, 0x00000003); // mBatteryType
				(void) mMemory->Write(theInfoStructAddr + 0x04, 0x000587C0); // mVoltage1
				(void) mMemory->Write(theInfoStructAddr + 0x08, 0x00000064); // mBatteryLevel
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x00000014); // mBatteryAlert
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00000000); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x006CF999); // mVoltage6
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000000); // mAdapterPlugged
				(void) mMemory->Write(theInfoStructAddr + 0x1C, 0x00003F36); // mVoltage7
				(void) mMemory->Write(theInfoStructAddr + 0x20, 0x00000000); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x24, 0xFFFFFFFF); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x28, 0xFFFFFFFF); // mUnknownDIOPins33Related
				(void) mMemory->Write(theInfoStructAddr + 0x2C, 0x001A2F28); // mVoltage4
				(void) mMemory->Write(theInfoStructAddr + 0x30, 0x001A8D79); // mVoltage5
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x08:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::RawStatus");
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister(2);
				(void) mMemory->Write(theInfoStructAddr + 0x00, 0x00000003); // mBatteryType
				(void) mMemory->Write(theInfoStructAddr + 0x04, 0x0C97D000); // mVoltage1
				(void) mMemory->Write(theInfoStructAddr + 0x08, 0x00000064); // mBatteryLevel
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x00000014); // mBatteryAlert
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00000000); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x00E19000); // mVoltage6
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000000); // mAdapterPlugged
				(void) mMemory->Write(theInfoStructAddr + 0x1C, 0x005C0000); // mVoltage7
				(void) mMemory->Write(theInfoStructAddr + 0x20, 0x00000000); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x24, 0xFFFFFFFF); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x28, 0xFFFFFFFF); // mUnknownDIOPins33Related
				(void) mMemory->Write(theInfoStructAddr + 0x2C, 0x086E2000); // mVoltage4
				(void) mMemory->Write(theInfoStructAddr + 0x30, 0x07D3B000); // mVoltage5
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x09:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::StartSleepCharge");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0A:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::SetType");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0B:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::ReadADCVoltage");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0C:
			if (LOG_BATTERY)
			{
				mLog->LogLine("PMainBatteryDriver::ConvertVoltage");
			}
			mProcessor->SetRegister(0, 0);
			break;

		default:
			if (LOG_BATTERY)
			{
				mLog->FLogLine(
					"Unknown battery driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteScreenDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteScreenDriverNative(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::Delete");
			}
			mProcessor->SetRegister(0, 0);
			break;

			//		case 0x02:
			//			if (LOG_SCREEN)
			//			{
			//				mLog->LogLine( "TMainDisplayDriver::ScreenSetup" );
			//			}
			//			mProcessor->SetRegister( 0, 0 );
			//			break;

		case 0x03:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::GetScreenInfo");
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister(1);
				(void) mMemory->Write(theInfoStructAddr + 0x00, mScreenManager->GetScreenHeight());
				(void) mMemory->Write(theInfoStructAddr + 0x04, mScreenManager->GetScreenWidth());
				(void) mMemory->Write(theInfoStructAddr + 0x08, TScreenManager::kBitsPerPixel);
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x00000037); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00640064); // resolution
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x00000020); // unknown
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000020); // unknown
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x04:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::PowerInit");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::PowerOn");
			}
			mScreenManager->PowerOnScreen();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x06:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::PowerOff");
			}
			mScreenManager->PowerOffScreen();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07: {
			KUInt32 theMode;
			(void) mMemory->Read(mProcessor->GetRegister(13) + 4, theMode);

			//	ULong	Blit(PixelMap*, Rect*, Rect*, long);
			TScreenManager::SRect srcRect;

			KUInt32 tmp = 0;

			(void) mMemory->Read(mProcessor->GetRegister(2), tmp);
			srcRect.fTop = (KUInt16) (tmp >> 16);
			srcRect.fLeft = (KUInt16) (tmp & 0x0000FFFF);
			(void) mMemory->Read(mProcessor->GetRegister(2) + 4, tmp);
			srcRect.fBottom = (KUInt16) (tmp >> 16);
			srcRect.fRight = (KUInt16) (tmp & 0x0000FFFF);

			TScreenManager::SRect dstRect;

			(void) mMemory->Read(mProcessor->GetRegister(3), tmp);
			dstRect.fTop = (KUInt16) (tmp >> 16);
			dstRect.fLeft = (KUInt16) (tmp & 0x0000FFFF);
			(void) mMemory->Read(mProcessor->GetRegister(3) + 4, tmp);
			dstRect.fBottom = (KUInt16) (tmp >> 16);
			dstRect.fRight = (KUInt16) (tmp & 0x0000FFFF);

			mScreenManager->Blit(
				mProcessor->GetRegister(1),
				&srcRect,
				&dstRect,
				theMode);
			mProcessor->SetRegister(0, 0);
		}
		break;

		case 0x08:
			if (LOG_SCREEN)
			{
				mLog->FLogLine(
					"TMainDisplayDriver::GetFeature( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
			}
			{
				KUInt32 theFeature = (unsigned int) mProcessor->GetRegister(1);
				if (theFeature == 0x00000000)
				{
					// Contrast?
					mProcessor->SetRegister(0, mScreenManager->GetContrast());
				} else if (theFeature == 0x00000001)
				{
					mProcessor->SetRegister(0, 0x00000001);
				} else if (theFeature == 0x00000002)
				{
					mProcessor->SetRegister(0, mScreenManager->GetBacklight());
				} else if (theFeature == 0x00000003)
				{
					mProcessor->SetRegister(0, 0x00000000);
				} else if (theFeature == 0x00000004)
				{
					mProcessor->SetRegister(0, mScreenManager->GetScreenOrientation());
				} else if (theFeature == 0x00000005)
				{
					mProcessor->SetRegister(0, 0x0000000A);
				} else
				{
					mProcessor->SetRegister(0, 0xFFFFFFFF);
				}
			}
			break;

		case 0x09:
			if (LOG_SCREEN)
			{
				mLog->FLogLine(
					"TMainDisplayDriver::SetFeature( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2));
			}
			{
				KUInt32 theFeature = (unsigned int) mProcessor->GetRegister(1);
				KUInt32 theValue = (unsigned int) mProcessor->GetRegister(2);
				if (theFeature == 0x00000000)
				{
					mScreenManager->SetContrast(theValue != 0);
				} else if (theFeature == 0x00000002)
				{
					mScreenManager->SetBacklight(theValue != 0);
				} else if (theFeature == 0x00000004)
				{
					mScreenManager->SetScreenOrientation(
						(TScreenManager::EOrientation) theValue);
				}
			}
			break;

		case 0x0A:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::AutoAdjustFeatures");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0B:
			if (LOG_SCREEN)
			{
				mLog->FLogLine(
					"TMainDisplayDriver::DoubleBlit( PM=%.8X, PM=%.8X, R=%.8X, R, long )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3));
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0C:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::EnterIdleMode");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0D:
			if (LOG_SCREEN)
			{
				mLog->LogLine("TMainDisplayDriver::ExitIdleMode");
			}
			mProcessor->SetRegister(0, 0);
			break;

		default:
			if (LOG_SCREEN)
			{
				mLog->FLogLine(
					"Unknown screen driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteTabletDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteTabletDriverNative(KUInt32 inInstruction)
{
	//	mEmulator->BreakInMonitor();
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::New");
			}
			break;

		case 0x02:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::Delete");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x03:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::Init");
			}
			mTabletCalibration.fUnknown_00 = 0xFFFFDFA5;
			mTabletCalibration.fUnknown_04 = 0x000015EC;
			mTabletCalibration.fUnknown_08 = 0x01F5F6B0;
			mTabletCalibration.fUnknown_0C = 0xFFEE8314;
			mTabletCalibration.fUnknown_10 = 0xC8E60000;
			mTabletSampleRate = 0x0000B400;
			break;

		case 0x04:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::WakeUp");
			}
			mScreenManager->WakeUpTablet();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::ShutDown");
			}
			mScreenManager->ShutDownTablet();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x06:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::TabletIdle");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::GetSampleRate");
			}
			mProcessor->SetRegister(0, mScreenManager->GetTabletSampleRate());
			break;

		case 0x08:
			if (LOG_TABLET)
			{
				mLog->FLogLine(
					"TMainTabletDriver::SetSampleRate( %.8X )",
					(unsigned int) mProcessor->GetRegister(1));
				// mEmulator->BreakInMonitor();
			}
			mScreenManager->SetTabletSampleRate(mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x09:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::GetTabletCalibration");
			}
			{
				KUInt32 tabletCalibrationAddy = mProcessor->GetRegister(1);
				(void) mMemory->Write(
					tabletCalibrationAddy,
					mTabletCalibration.fUnknown_00);
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x04,
					mTabletCalibration.fUnknown_04);
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x08,
					mTabletCalibration.fUnknown_08);
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C);
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C);
			}
			break;

		case 0x0A:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::SetTabletCalibration");
			}
			{
				KUInt32 tabletCalibrationAddy = mProcessor->GetRegister(1);
				(void) mMemory->Read(
					tabletCalibrationAddy,
					mTabletCalibration.fUnknown_00);
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x04,
					mTabletCalibration.fUnknown_04);
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x08,
					mTabletCalibration.fUnknown_08);
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C);
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C);
			}
			break;

		case 0x0B:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::SetDoingCalibration");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0C:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::GetTabletResolution");
				// mEmulator->BreakInMonitor();
			}
			(void) mMemory->Write(
				mProcessor->GetRegister(1),
				0x3200000);
			(void) mMemory->Write(
				mProcessor->GetRegister(2),
				0x3200000);
			break;

		case 0x0D:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::TabSetOrientation");
			}
			mScreenManager->SetTabletOrientation(
				(TScreenManager::EOrientation) mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x0E:
			if (LOG_TABLET)
			{
				//				mLog->LogLine( "TMainTabletDriver::GetTabletState" );
			}
			mProcessor->SetRegister(0, (KUInt32) mScreenManager->GetTabletState());
			break;

		case 0x0F:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::GetFingerInputState");
			}
			mProcessor->SetRegister(0, (KUInt32) -56008);
			break;

		case 0x10:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::SetFingerInputState");
			}
			mProcessor->SetRegister(0, (KUInt32) -56008);
			break;

		case 0x11:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::RecalibrateTabletAfterRotate");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x12:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::TabletNeedsRecalibration");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x13:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::StartBypassTablet");
			}
			mScreenManager->StartBypassTablet();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x14:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::StopBypassTablet");
			}
			mScreenManager->StopBypassTablet();
			mProcessor->SetRegister(0, 0);
			break;

		case 0x15:
			if (LOG_TABLET)
			{
				mLog->LogLine("TMainTabletDriver::ReturnTabletToConsciousness");
			}
			mProcessor->SetRegister(0, 0);
			break;

		case 0x16:
			if (LOG_TABLET)
			{
				//				mLog->LogLine( "TMainTabletDriver::NativeGetSample" );
			}
			{
				//				mEmulator->BreakInMonitor();
				KUInt32 theSampleRecord = 0;
				KUInt32 theSampleTime = 0;
				Boolean gotSomeSample = mScreenManager->GetSample(&theSampleRecord, &theSampleTime);
				if (gotSomeSample)
				{
					(void) mMemory->Write(
						mProcessor->GetRegister(1), theSampleRecord);
					(void) mMemory->Write(
						mProcessor->GetRegister(2), theSampleTime);
				}
				mProcessor->SetRegister(0, gotSomeSample);
			}
			break;

		default:
			if (LOG_TABLET)
			{
				mLog->FLogLine(
					"Unknown tablet driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteSerialDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteSerialDriverNative(KUInt32 inInstruction)
{
	KUInt32 r = 0;
	KUInt32 chip = mProcessor->GetRegister(0);

	// Ignore calls for the Voyager chipset as it needs to be handled on
	// the lower levels of the emulator.
	if ((inInstruction & 0xFF) < 0x30)
	{
		mProcessor->SetRegister(0, -10000);
		return;
	}
	if ((inInstruction & 0xFF) == 0x4C || (inInstruction & 0xFF) == 0x4D)
	{
		// Intercept the option calls to set the correct host driver
		KUInt32 optionAddr = mProcessor->GetRegister(1);
		KUInt32 label;
		KUInt32 flags;
		mMemory->ReadAligned(optionAddr, label);
		mMemory->ReadAligned(optionAddr + 8, flags);
		if (LOG_SERIAL)
		{
			mLog->FLogLine("TSerialChipEinstein::Process/InitByOption %08x %08x %08x", chip, label, flags);
		}

		switch (label)
		{
			// The 'eloc' (Einstein location) option determines which host driver to use
			case 'eloc': {
				KUInt32 location;
				KUInt32 type;
				mMemory->ReadAligned(optionAddr + 12, location);
				mMemory->ReadAligned(optionAddr + 16, type);
				if (LOG_SERIAL)
				{
					mLog->FLogLine("  'eloc' %08x %d", location, (KSInt32) type);
				}
				char c[61];
				KUInt32 configDataSize = 0;
				memset(c, 0, sizeof(c));

				// Update the driver on the Newton side with the requested location
				mMemory->WriteAligned(chip + 0x10, location);

				// If configuration data is set, use it during initialization
				mMemory->ReadAligned(optionAddr + 20, configDataSize);
				if (configDataSize > 0)
				{
					mMemory->FastReadString(optionAddr + 24, &configDataSize, c);
				}
				std::string configData(c);

				// Enable the driver on the Einstein side with the available configuration data
				TSerialHostPort* port = mEmulator->SerialPorts.SetDriver(location,
					static_cast<TSerialPorts::EDriverID>(type), configData);
				if (port == NULL)
				{
					mProcessor->SetRegister(0, -16022);
					return;
				}

				// Update the driver on the Newton side with the interrupt ID
				mMemory->WriteAligned(chip + 0x14, port->GetInterruptID());
			}
			break;
			case 'sers':
				KUInt32 features, location, supported, type;
				mMemory->ReadAligned(optionAddr + 12, location);
				if (LOG_SERIAL)
				{
					mMemory->ReadAligned(optionAddr + 16, features);
					mMemory->ReadAligned(optionAddr + 20, supported);
					mMemory->ReadAligned(optionAddr + 24, type);
					mLog->FLogLine("  'sers' %08x %08x %08x %08x", location, features, supported, type);
				}

				// Update the serial chip spec option with some reasonable defaults, using the
				// location as stored in the driver on the Newton side
				mMemory->ReadAligned(chip + 0x10, location);
				mMemory->WriteAligned(optionAddr + 8, 0x0C000100); // fFlags: kOptionType | opSetNegotiate
				mMemory->WriteAligned(optionAddr + 12, location);
				mMemory->WriteAligned(optionAddr + 16, 0x80000003); // fSerFeatures: Einstein | kSerFeatureVersion2 | kSerFeatureDefault
				mMemory->WriteAligned(optionAddr + 20, 0x02200018); // f...Support: kSerOutRTS | kSerInCTS | kSerCap_DataBits_8 | kSerCap_StopBits_1
				mMemory->WriteAligned(optionAddr + 24, 0x02010000); // fUARTType: kSerialChip16550, fChipNotInUse
				break;
			default:
				break;
		}
		mProcessor->SetRegister(0, r);
		return;
	}
	// Forward calls to the host driver
	KUInt32 location;
	mMemory->ReadAligned(chip + 0x10, location);
	TSerialHostPort* port = mEmulator->SerialPorts.GetDriverFor(location);
	if (port == NULL)
	{
		if (LOG_SERIAL)
		{
			mLog->FLogLine("TSerialChipEinstein: %08x %02x %08x", chip, inInstruction & 0xff, location);
		}
		mProcessor->SetRegister(0, r);
		return;
	}
	switch (inInstruction & 0xFF)
	{
		// TSerialChipEinstein primitives
		case 0x33:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::InstallChipHandler %08x", chip);
			}
			break;

		case 0x34:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::RemoveChipHandler %08x", chip);
			}
			break;

		case 0x35:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::PutByte %08x [%02x]", chip, mProcessor->GetRegister(1));
			}
			port->PutByte(mProcessor->GetRegister(1));
			break;

		case 0x36:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::ResetTxBEmpty %08x", chip);
			}
			port->ResetTxBEmpty();
			break;

		case 0x37:
			r = port->GetByte();
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::GetByte %08x [%02x]", chip, r);
			}
			break;

		case 0x38:
			r = port->TxBufEmpty();
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::TxBufEmpty %08x %d", chip, r);
			}
			break;

		case 0x39:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::RxBufFull %08x", chip);
			}
			r = port->RxBufFull();
			break;

		case 0x3A:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::GetRxErrorStatus %08x", chip);
			}
			r = -10000;
			break;

		case 0x3B:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::GetSerialStatus %08x", chip);
			}
			r = port->GetSerialStatus();
			break;

		case 0x3C:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::ResetSerialStatus %08x", chip);
			}
			port->ResetSerialStatus();
			break;

		case 0x3D:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetSerialOutputs %08x", chip);
			}
			port->SetSerialOutputs(mProcessor->GetRegister(1));
			break;

		case 0x3E:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::ClearSerialOutputs %08x", chip);
			}
			port->ClearSerialOutputs(mProcessor->GetRegister(1));
			break;

		case 0x3F:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::GetSerialOutputs %08x", chip);
			}
			r = port->GetSerialOutputs();
			break;

		case 0x40:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::PowerOff %08x", chip);
			}
			break;

		case 0x41:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::PowerOn %08x", chip);
			}
			break;

		case 0x42:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::PowerIsOn %08x", chip);
			}
			r = 1;
			break;

		case 0x43:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetInterruptEnable %08x", chip);
			}
			break;

		case 0x44:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::Reset %08x", chip);
			}
			port->Reset();
			break;

		case 0x45:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetBreak %08x", chip);
			}
			port->SetBreak(mProcessor->GetRegister(1));
			break;

		case 0x46:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetSpeed %08x", chip);
			}
			port->SetSpeed(mProcessor->GetRegister(1));
			break;

		case 0x47:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetIOParms %08x", chip);
			}
			break;

		case 0x48:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::Reconfigure %08x", chip);
			}
			port->Reconfigure();
			break;

		case 0x49:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::Init %08x", chip);
			}
			break;

		case 0x4A:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::CardRemoved %08x", chip);
			}
			break;

		case 0x4B:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::GetFeatures %08x", chip);
			}
			r = port->GetFeatures();
			break;

		case 0x4E:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetSerialMode %08x", chip);
			}
			port->SetSerialMode(mProcessor->GetRegister(1));
			break;

		case 0x4F:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SysEventNotify %08x", chip);
			}
			break;

		case 0x50:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetTxDTransceiverEnable %08x", chip);
			}
			break;

		case 0x51:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::GetByteAndStatus %08x", chip);
			}
			break;

		case 0x52:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetIntSourceEnable %08x", chip);
			}
			break;

		case 0x53:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::AllSent %08x", chip);
			}
			r = port->AllSent();
			break;

		case 0x54:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::ConfigureForOutput %08x", chip);
			}
			break;

		case 0x55:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::InitTxDMA %08x", chip);
			}
			break;

		case 0x56:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::InitRxDMA %08x", chip);
			}
			break;

		case 0x57:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::TxDMAControl %08x", chip);
			}
			break;

		case 0x58:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::RxDMAControl %08x", chip);
			}
			break;

		case 0x59:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SetSDLCAddress %08x", chip);
			}
			break;

		case 0x5A:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::ReEnableReceiver %08x", chip);
			}
			break;

		case 0x5B:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::LinkIsFree %08x", chip);
			}
			break;

		case 0x5C:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::SendControlPacket %08x", chip);
			}
			break;

		case 0x5D:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::WaitForPacket %08x", chip);
			}
			break;

		case 0x5E:
			if (LOG_SERIAL)
			{
				mLog->FLogLine("TSerialChipEinstein::WaitForAllSent %08x", chip);
			}
			break;

		default:
			if (LOG_SERIAL)
			{
				mLog->FLogLine(
					"Unknown serial driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
	mProcessor->SetRegister(0, r);
}

// -------------------------------------------------------------------------- //
//  * ExecuteInTranslatorNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteInTranslatorNative(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
		default:
			if (LOG_IN_TRANSLATOR)
			{
				mLog->FLogLine(
					"Unknown in-translator native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
			mProcessor->SetRegister(0, 0);
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteOutTranslatorNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteOutTranslatorNative(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
		default:
			if (LOG_OUT_TRANSLATOR)
			{
				mLog->FLogLine(
					"Unknown out-translator native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
			mProcessor->SetRegister(0, 0);
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteHostCallNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteHostCallNative(KUInt32 inInstruction)
{
	(void) inInstruction;
#if !TARGET_OS_ANDROID && !TARGET_OS_MAC && !__LP64__ && !TARGET_OS_WIN32
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::New");
			}
			break;

		case 0x02:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::Delete");
			}
			break;

		case 0x03:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::OpenLib");
			}
			mProcessor->SetRegister(0,
				mNativeCalls->OpenLib(mProcessor->GetRegister(1)));
			break;

		case 0x04:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::CloseLib");
			}
			mNativeCalls->CloseLib(mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::PrepareFFIStructure");
			}
			mProcessor->SetRegister(0,
				mNativeCalls->PrepareFFIStructure(
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3)));
			break;

		case 0x06:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::DisposeFFIStructure");
			}
			mNativeCalls->DisposeFFIStructure(mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::GetErrorMessage");
			}
			mNativeCalls->GetErrorMessage(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x10:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_uint8");
			}
			mNativeCalls->SetArgValue_uint8(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x11:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_sint8");
			}
			mNativeCalls->SetArgValue_sint8(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x12:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_uint16");
			}
			mNativeCalls->SetArgValue_uint16(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x13:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_sint16");
			}
			mNativeCalls->SetArgValue_sint16(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x14:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_uint32");
			}
			mNativeCalls->SetArgValue_uint32(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x15:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_sint32");
			}
			mNativeCalls->SetArgValue_sint32(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x16:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_uint64");
			}
			//			mNativeCalls->SetArgValue_uint64(
			//							mProcessor->GetRegister(1),
			//							mProcessor->GetRegister(2),
			//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x17:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_sint64");
			}
			//			mNativeCalls->SetArgValue_sint64(
			//							mProcessor->GetRegister(1),
			//							mProcessor->GetRegister(2),
			//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x18:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_float");
			}
			//			mNativeCalls->SetArgValue_float(
			//							mProcessor->GetRegister(1),
			//							mProcessor->GetRegister(2),
			//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x19:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_double");
			}
			//			mNativeCalls->SetArgValue_double(
			//							mProcessor->GetRegister(1),
			//							mProcessor->GetRegister(2),
			//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1A:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_longdouble");
			}
			//			mNativeCalls->SetArgValue_longdouble(
			//							mProcessor->GetRegister(1),
			//							mProcessor->GetRegister(2),
			//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1B:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_string");
			}
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				mNativeCalls->SetArgValue_string(
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3),
					fourthParam);
				mProcessor->SetRegister(0, 0);
			}
			break;

		case 0x1C:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_binary");
			}
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				mNativeCalls->SetArgValue_binary(
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3),
					fourthParam);
				mProcessor->SetRegister(0, 0);
			}
			break;

		case 0x1D:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetArgValue_pointer");
			}
			mNativeCalls->SetArgValue_pointer(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x20:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::SetResultType");
			}
			mNativeCalls->SetResultType(
				mProcessor->GetRegister(1),
				(EFFI_Type) mProcessor->GetRegister(2));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x21:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::GetOutArgValue_string");
			}
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				mNativeCalls->GetOutArgValue_string(
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3),
					fourthParam);
				mProcessor->SetRegister(0, 0);
			}
			break;

		case 0x22:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::GetOutArgValue_binary");
			}
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				mNativeCalls->GetOutArgValue_binary(
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3),
					fourthParam);
				mProcessor->SetRegister(0, 0);
			}
			break;

		case 0x30:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::Call_void");
			}
			mNativeCalls->Call_void(
				mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x31:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::Call_int");
			}
			mProcessor->SetRegister(0,
				mNativeCalls->Call_int(
					mProcessor->GetRegister(1)));
			break;

		case 0x32:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::Call_real");
			}
			//			mProcessor->SetRegister(0,
			//				mNativeCalls->Call_real(
			//							mProcessor->GetRegister(1)));
			break;

		case 0x33:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::Call_string");
			}
			mNativeCalls->Call_string(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2),
				mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x34:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::Call_string");
			}
			mProcessor->SetRegister(0,
				/* (KUInt32) */ mNativeCalls->Call_pointer(mProcessor->GetRegister(1)));
			break;

		case 0x40:
			if (LOG_HOSTCALL)
			{
				mLog->LogLine("TEinsteinNativeCalls::GetErrno");
			}
			mProcessor->SetRegister(0, mNativeCalls->GetErrno());
			break;

		default:
			if (LOG_HOSTCALL)
			{
				mLog->FLogLine(
					"Unknown call native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
			mProcessor->SetRegister(0, 0);
	}
#else
	if (LOG_HOSTCALL)
	{
		mLog->FLogLine("Native primitives not supported on this platform");
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * ExecuteNetworkManagerNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteNetworkManagerNative(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
			// Debugging Helpers

		case 0x00:
			// Just log some unknown function without causing a fuss
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::Unknown");
			}
			break;
		case 0x01: {
			// Write some string to the log buffer (reg0 points to the string)
			KUInt32 addr = mProcessor->GetRegister(0);
			if (mLog && addr)
			{
				char buffer[1024], *dst = buffer + 22;
				strcpy(buffer, "TNetworkManager::Log: ");
				for (;;)
				{
					KUInt8 v;
					mMemory->ReadB(addr++, v);
					*dst++ = (char) v;
					if (v == 0)
						break;
				}
				mLog->LogLine(buffer);
			}
			break;
		}

			// Protocol Class Interface

		case 0x02:
			// a new driver is beeing created
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::New");
			}
			break;
		case 0x03:
			// the driver is beeing deleted
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::Delete");
			}
			break;

			// Task services

		case 0x04:
			// Initialize the card driver
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::Init");
			}
			break;
		case 0x05:
			// Enable the card (switch power on)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::Enable");
			}
			break;
		case 0x06:
			// Disable the card (power off to save battery)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::Disable");
			}
			break;
		case 0x07:
			// Handle any kind of interrupt by the hardware
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::InterruptHandler");
			}
			break;

			// Client Services (from event handlers of TLanternDriverAPI)

		case 0x08:
			// Newton transfers data to the world (calls SendPacket)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::SendBuffer");
			}
			break;
		case 0x09:
			// Newton transfers data to the world (calls SendPacket)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::SendCBufferList");
			}
			break;
		case 0x0a: {
			// Newton wants to send a raw packet into the world
			KUInt32 addr = mProcessor->GetRegister(1);
			KUInt32 size = mProcessor->GetRegister(2), i;
			if (LOG_NETWORKMANAGER)
			{
				mLog->FLogLine("TNetworkManager::SendPacket(0x%08x, %d)", (unsigned int) addr, (int) size);
			}
			if (mNetworkManager && size)
			{
				KUInt8 *data = (KUInt8*) malloc(size), v;
				for (i = 0; i < size; i++)
				{
					mMemory->ReadB(addr + i, v);
					data[i] = v;
				}
				mNetworkManager->SendPacket(data, size);
				free(data);
			}
			break;
		}
		case 0x0b: {
			// NewtonOS wants the hardware MAC address of the card
			KUInt32 dstBuffer = mProcessor->GetRegister(1);
			KUInt32 dstBufferSize = mProcessor->GetRegister(2);
			KUInt32 i, err = 0;
			if (LOG_NETWORKMANAGER)
			{
				mLog->FLogLine("TNetworkManager::GetDeviceAddress(0x%08x, %d)", (unsigned int) dstBuffer, (int) dstBufferSize);
			}
			if (mNetworkManager && dstBufferSize)
			{
				KUInt8 mac[6] = { 0 };
				err = (KUInt32) mNetworkManager->GetDeviceAddress(mac, dstBufferSize);
				for (i = 0; i < dstBufferSize; i++)
					mMemory->WriteB(dstBuffer + i, mac[i]);
			}
			mProcessor->SetRegister(0, err);
			break;
		}
		case 0x0c:
			// NewtonOS wants to add a multicast address (not implemented)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::AddMulticastAddress");
			}
			break;
		case 0x0d:
			// NewtonOS wants to remove a multicast address (not implemented)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::DelMulticastAddress");
			}
			break;
		case 0x0e:
			// NewtonOS wants to know if the links are OK  (not implemented)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::GetLinkIntegrity");
			}
			break;

			// Optional services

		case 0x0f:
			// receive every packet on the network, even those that are not meant for us (not implemented)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::SetPromiscuous");
			}
			break;
		case 0x10:
			// return some number that gives the actual speed of the connection (not implemented)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::GetThroughput");
			}
			break;

			// Private Template Services

		case 0x11:
			// a regular timer that can help us poll data and monitor integrity and throughput
			if (LOG_NETWORKMANAGER)
			{
				// mLog->LogLine( "TNetworkManager::TimerExpired" );
			}
			mNetworkManager->TimerExpired();
			break;

			// NE2000 Template driver specific

		case 0x12:
			// Initialize card (not needed?!)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::InitCard");
			}
			break;
		case 0x13:
			// Set the card info (not needed?!)
			if (LOG_NETWORKMANAGER)
			{
				mLog->LogLine("TNetworkManager::SetCardInfo");
			}
			break;
		case 0x14: {
			// Return number of bytes in the first packet available in R0
			KUInt32 size = 0;
			if (mNetworkManager)
			{
				size = mNetworkManager->DataAvailable();
			}
			mProcessor->SetRegister(0, size);
			if (LOG_NETWORKMANAGER)
			{
				if (size > 0)
					mLog->FLogLine("TNetworkManager::DataAvailable(Avail: %d)", (int) size);
			}
			break;
		}
		case 0x15: {
			// Copy the next available packet into the buffer pointed to by R1
			KUInt32 dst = mProcessor->GetRegister(1);
			KUInt32 i, n = mProcessor->GetRegister(2);
			if (LOG_NETWORKMANAGER)
			{
				mLog->FLogLine("TNetworkManager::ReceiveData (buffer=0x%08x, size=%d", (unsigned int) dst, (int) n);
			}
			if (mNetworkManager && n)
			{
				KUInt8* buffer = (KUInt8*) malloc(n);
				mNetworkManager->ReceiveData(buffer, n);
				for (i = 0; i < n; i++)
					mMemory->WriteB(dst + i, buffer[i]);
				free(buffer);
			}
			break;
		}
		case 0x16: {
			// Print some memory location
			KUInt32 addr = mProcessor->GetRegister(0);
			KUInt32 size = mProcessor->GetRegister(1), i;
			if (mLog && size)
			{
				KUInt8* buffer = (KUInt8*) malloc(size);
				for (i = 0; i < size; i++)
				{
					KUInt8 v;
					mMemory->ReadB(addr + i, v);
					buffer[i] = v;
				}
				mNetworkManager->LogBuffer(buffer, size);
			}
			break;
		}

		default:
			if (LOG_NETWORKMANAGER)
			{
				mLog->FLogLine(
					"TNetworkManager: Unknown native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15));
			}
	}
}

#if TARGET_OS_MAC
void
TNativePrimitives::ExecuteHostiOSNativeiOS(KUInt32 inInstruction)
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostGetCPUArchitecture");
			}
			//
			mProcessor->SetRegister(0, mObjCBridgeCalls->HostGetCPUArchitecture());
			break;
		case 0x02:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostMakeNSInvocation");
			}
			mProcessor->SetRegister(0,
				mObjCBridgeCalls->HostMakeNSInvocation(mProcessor->GetRegister(0),
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2)));
			break;
		case 0x03:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostSetInvocationTarget");
			}
			mProcessor->SetRegister(0,
				mObjCBridgeCalls->HostSetInvocationTarget(mProcessor->GetRegister(0),
					mProcessor->GetRegister(1)));
			break;
		case 0x04:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostSetInvocationArgument_Object");
			}
			mProcessor->SetRegister(0,
				mObjCBridgeCalls->HostSetInvocationArgument_Object(mProcessor->GetRegister(0), mProcessor->GetRegister(1), mProcessor->GetRegister(2)));
			break;
		case 0x05:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostGetInvocationReturn_Object");
			}
			mProcessor->SetRegister(0,
				mObjCBridgeCalls->HostGetInvocationReturn_Object(mProcessor->GetRegister(0), mProcessor->GetRegister(1)));
			break;
		case 0x06:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostInvoke");
			}
			mProcessor->SetRegister(0,
				mObjCBridgeCalls->HostInvoke(mProcessor->GetRegister(0)));
			break;
		case 0x07:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostReleaseObject");
			}
			mProcessor->SetRegister(0,
				mObjCBridgeCalls->HostReleaseObject(mProcessor->GetRegister(0)));
			break;
		case 0x08:
			if (LOG_HOSTIOS_NATIVEIOS)
			{
				mLog->LogLine("TObjCBridgeCalls::HostMakeNSString");
			}
			mProcessor->SetRegister(0,
				mObjCBridgeCalls->HostMakeNSString(mProcessor->GetRegister(0),
					mProcessor->GetRegister(1)));
			break;
	}
}
#endif


#ifdef TARGET_UI_FLTK
#include <FL/Fl_Printer.H>
// -------------------------------------------------------------------------- //
//  * ExecutePrinterDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecutePrinterDriverNative(KUInt32 inInstruction)
{
	KUInt32 ret = 0;

	switch (inInstruction & 0xFF)
	{
		case 0x01: // never called: void PDNew(DRIVERCLASSNAME *self);
			if (LOG_PRINTER)
			{
				mLog->LogLine("void PDNew(DRIVERCLASSNAME *self);");
			}
			break;

		case 0x02: // void PDDelete(DRIVERCLASSNAME *self);
			if (LOG_PRINTER)
			{
				mLog->LogLine("void PDDelete(DRIVERCLASSNAME *self);");
			}
			if (mPrinterManager)
				mPrinterManager->Delete(mProcessor->GetRegister(0));
			break;

		case 0x03: // NewtonErr PDOpen(DRIVERCLASSNAME *self);
			if (LOG_PRINTER)
			{
				mLog->LogLine("NewtonErr PDOpen(DRIVERCLASSNAME *self);");
			}
			if (mPrinterManager)
				ret = mPrinterManager->Open(mProcessor->GetRegister(0));
			mProcessor->SetRegister(0, ret);
			break;

		case 0x04: // NewtonErr PDClose(DRIVERCLASSNAME *self);
			if (LOG_PRINTER)
			{
				mLog->LogLine("NewtonErr PDClose(DRIVERCLASSNAME *self);");
			}
			if (mPrinterManager)
				ret = mPrinterManager->Close(mProcessor->GetRegister(0));
			mProcessor->SetRegister(0, ret);
			break;

		case 0x05: // NewtonErr PDOpenPage(DRIVERCLASSNAME *self);
			if (LOG_PRINTER)
			{
				mLog->LogLine("NewtonErr PDOpenPage(DRIVERCLASSNAME *self);");
			}
			if (mPrinterManager)
				ret = mPrinterManager->OpenPage(mProcessor->GetRegister(0));
			mProcessor->SetRegister(0, ret);
			break;

		case 0x06: // NewtonErr PDClosePage(DRIVERCLASSNAME *self);
			if (LOG_PRINTER)
			{
				mLog->LogLine("NewtonErr PDClosePage(DRIVERCLASSNAME *self);");
			}
			if (mPrinterManager)
				ret = mPrinterManager->ClosePage(mProcessor->GetRegister(0));
			mProcessor->SetRegister(0, ret);
			break;

		case 0x07: // NewtonErr PDImageBand(DRIVERCLASSNAME *self, PixelMap* theBand, const Rect* minRect);
			if (LOG_PRINTER)
			{
				mLog->LogLine("NewtonErr PDImageBand(DRIVERCLASSNAME *self, PixelMap* theBand, const Rect* minRect);");
			}
			if (mPrinterManager)
				ret = mPrinterManager->ImageBand(
					mProcessor->GetRegister(0),
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2));
			mProcessor->SetRegister(0, ret);
			break;

		case 0x08: // void PDCancelJob(DRIVERCLASSNAME *self, Boolean asyncCancel);
			if (LOG_PRINTER)
			{
				mLog->LogLine("void PDCancelJob(DRIVERCLASSNAME *self, Boolean asyncCancel);");
			}
			if (mPrinterManager)
				mPrinterManager->CancelJob(
					mProcessor->GetRegister(0),
					mProcessor->GetRegister(1));
			break;

		case 0x09: // PrProblemResolution PDIsProblemResolved(DRIVERCLASSNAME *self);
			if (LOG_PRINTER)
			{
				mLog->LogLine("PrProblemResolution PDIsProblemResolved(DRIVERCLASSNAME *self);");
			}
			if (mPrinterManager)
				ret = mPrinterManager->IsProblemResolved(mProcessor->GetRegister(0));
			mProcessor->SetRegister(0, ret);
			break;

		case 0x0A: // void PDGetPageInfo(DRIVERCLASSNAME *self, PrPageInfo* info);
			if (LOG_PRINTER)
			{
				mLog->LogLine("void PDGetPageInfo(DRIVERCLASSNAME *self, PrPageInfo* info);");
			}
			if (mPrinterManager)
				mPrinterManager->GetPageInfo(
					mProcessor->GetRegister(0),
					mProcessor->GetRegister(1));
			break;

		case 0x0B: // void PDGetBandPrefs(DRIVERCLASSNAME *self, DotPrinterPrefs* prefs);
			if (LOG_PRINTER)
			{
				mLog->LogLine("void PDGetBandPrefs(DRIVERCLASSNAME *self, DotPrinterPrefs* prefs);");
			}
			if (mPrinterManager)
				mPrinterManager->GetBandPrefs(
					mProcessor->GetRegister(0),
					mProcessor->GetRegister(1));
			break;

		case 0x0C: // NewtonErr PDFaxEndPage(DRIVERCLASSNAME *self, long pageCount);
			if (LOG_PRINTER)
			{
				mLog->LogLine("NewtonErr PDFaxEndPage(DRIVERCLASSNAME *self, long pageCount);");
			}
			if (mPrinterManager)
				ret = mPrinterManager->FaxEndPage(
					mProcessor->GetRegister(0),
					mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, ret);
			break;

		default:
			if (LOG_PRINTER)
			{
				mLog->FLogLine("Unknown Printer Driver Call: 0x%08x\n", inInstruction);
			}
			KPrintf("Unknown Printer Driver Call: 0x%08x\n", inInstruction);
			break;
	}
}
#else
void
TNativePrimitives::ExecutePrinterDriverNative(KUInt32 inInstruction)
{
  KPrintf("Printer Driver Calls not supported on this platform.\n");
}
#endif

// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::TransferState(TStream* inStream)
{
	// The various registers.
	inStream->TransferInt32BE(mTabletCalibration.fUnknown_00);
	inStream->TransferInt32BE(mTabletCalibration.fUnknown_04);
	inStream->TransferInt32BE(mTabletCalibration.fUnknown_08);
	inStream->TransferInt32BE(mTabletCalibration.fUnknown_0C);
	inStream->TransferInt32BE(mTabletCalibration.fUnknown_10);
	inStream->TransferInt32BE(mTabletSampleRate);
	inStream->TransferByte(mInputVolume);
}

// ============================================================================== //
// Dear Sir,                                                                      //
//         I am firmly opposed to the spread of microchips either to the home or  //
// to the office,  We have more than enough of them foisted upon us in public     //
// places.  They are a disgusting Americanism, and can only result in the farmers //
// being forced to grow smaller potatoes, which in turn will cause massive un-    //
// employment in the already severely depressed agricultural industry.            //
//         Yours faithfully,                                                      //
//         Capt. Quinton D'Arcy, J.P.                                             //
//         Sevenoaks                                                              //
//                 -- Letters To The Editor, The Times of London                  //
// ============================================================================== //
