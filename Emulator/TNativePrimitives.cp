// ==============================
// File:			TNativePrimitives.cp
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
#include "TNativePrimitives.h"

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

#if !TARGET_OS_WIN32
	#include <unistd.h>
#endif

// K
#include <K/Streams/TStream.h>

// Einstein
#include "Log/TLog.h"
#include "TEmulator.h"
#include "TMemory.h"
#include "TARMProcessor.h"
#include "Screen/TScreenManager.h"
#include "Network/TNetworkManager.h"
#include "Sound/TSoundManager.h"
#if !TARGET_OS_MAC
#include "NativeCalls/TNativeCalls.h"
#endif
#include "NativeCalls/TVirtualizedCalls.h"
#include "Platform/TPlatformManager.h"
#include "Platform/PlatformGestalt.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"
#if TARGET_OS_MAC
#include "NativeCalls/TObjCBridgeCalls.h"
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

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TNativePrimitives( TLog*, TMemory* )
// -------------------------------------------------------------------------- //
TNativePrimitives::TNativePrimitives(
			TLog* inLog,
			TMemory* inMemory )
	:
		mProcessor( nil ),
		mLog( inLog ),
		mMemory( inMemory ),
		mEmulator( nil ),
		mNetworkManager( nil ),
		mSoundManager( nil ),
		mScreenManager( nil ),
		mPlatformManager( nil ),
#if !TARGET_OS_MAC
		mNativeCalls( new TNativeCalls(inMemory) ),
#endif
#if TARGET_OS_MAC
		mObjCBridgeCalls( new TObjCBridgeCalls(inMemory)),
#endif
		mVirtualizedCalls( nil ),
		mInputVolume( 0 ),
		mQuit( false )
{
}

// -------------------------------------------------------------------------- //
//  * ~TNativePrimitives( void )
// -------------------------------------------------------------------------- //
TNativePrimitives::~TNativePrimitives( void )
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
TNativePrimitives::SetEmulator( TEmulator* inEmulator )
{
	if (inEmulator)
	{
		mNetworkManager = inEmulator->GetNetworkManager();
		mSoundManager = inEmulator->GetSoundManager();
		mScreenManager = inEmulator->GetScreenManager();
		mPlatformManager = inEmulator->GetPlatformManager();
		mEmulator = inEmulator;
		mVirtualizedCalls =
			new TVirtualizedCalls( inEmulator, mMemory, inEmulator->GetProcessor() );
	} else {
		mNetworkManager = nil;
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
TNativePrimitives::ExecuteNative( KUInt32 inInstruction )
{
	static KUInt16 n[0x1000] = { 0 };
	
	if (inInstruction & 0x80000000)
	{
		// If the high bit is set, this instruction is actually a patch, not
		// a real ARM instruction.  The lower 31 bits are an enum value
		// identifying a virtualized call. These enums are defined in
		// TVirtualizedCallsPatches.h
		
		mVirtualizedCalls->Execute(inInstruction &~ 0x80000000);
	} else {
		// This block updates the progress bar overlay as the
		// virtual Newton is going through its early boot phases.
		
		static Boolean traceProgress = true;
		if (traceProgress) {
			int nn = n[inInstruction&0xfff]++;
			static int prevProgress = 2;
			int progress = 0;
			const char *title = "";
			if (nn==0) {
				switch (inInstruction) {
					case 0x00000003:
						progress=1; title = "Init Flash"; break;
					case 0x00000001:
						progress=2; title = "Identify Flash"; break;
					case 0x00000004:
						progress=3; title = "Init Flash Driver"; break;
					case 0x0000000a:
						progress=4; title = "Reset Flash Block Status"; break;
					case 0x00000006:
						progress=5; title = "Read Flash Array"; break;
					case 0x00000007:
						progress=6; title = "Read Flash Array"; break;
					case 0x00000005:
						progress=7; title = "Cleanup Flash Driver Data"; break;
					case 0x00000002:
						progress=8; title = "Cleanup Flash"; break;
					case 0x00000117:
						progress=9; title = "Get Emulator Info"; break;
					case 0x00000103:
						progress=10; title = "Init Platform"; break;
					case 0x0000010b:
						progress=11; title = "Power Off Subsystem"; break;
					case 0x0000021f: progress=12; title = ""; break;
					case 0x00000205: progress=13; title = ""; break;
					case 0x00000206: progress=14; title = ""; break;
					case 0x0000020a: progress=15; title = ""; break;
					case 0x0000020c: progress=16; title = ""; break;
					case 0x0000010a: progress=17; title = "Power On Subsystem"; break;
					case 0x0000010d: progress=18; title = "Pause System"; break;
					case 0x00000109: progress=19; title = "Reset ZAP Store Check"; break;
					case 0x00000301: progress=20; title = ""; break;
					case 0x00000303: progress=21; title = "Battery Driver Init"; break;
					case 0x00000404: progress=22; title = "Display Power Init"; break;
					case 0x00000403: progress=23; title = "Get Screen Info"; break;
					case 0x00000408: progress=24; title = "Get Screen Features"; break;
					case 0x00000405: progress=25; title = "Screen Power On"; break;
					case 0x00000409: progress=26; title = "Set Screen Features"; break;
					case 0x00000204: progress=27; title = ""; break;
					case 0x00000217: progress=28; title = ""; break;
					case 0x00000218: progress=29; title = ""; break;
					case 0x00000503: progress=30; title = "Tablet Init"; break;
					case 0x0000050c: progress=31; title = "Get Tablet Resolution"; break;
					case 0x00000507: progress=32; title = "Get Tablet Sample Rate"; break;
					case 0x0000050d: progress=33; title = "Set Tablet Orientation"; break;
					case 0x00000407: progress=34; title = "Screen Output"; break;
					case 0x00000213: progress=35; title = ""; break;
					case 0x00000209: progress=36; title = ""; break;
					case 0x00000207: progress=37; title = ""; break;
					case 0x00000211: progress=38; title = ""; break;
					case 0x0000021d: progress=39; title = ""; break;
					case 0x0000020d: progress=40; title = ""; break;
					case 0x0000020f: progress=41; title = ""; break;
					case 0x00000307: progress=42; title = ""; break;
					case 0x0000000d: progress=43; title = "Write Flash Memory"; break;
					case 0x00000008: progress=44; title = "Write Flash Memory"; break;
					case 0x0000000e: progress=45; title = "Setup Flash Memory"; //break;
					case 0x00000112: progress=46; traceProgress = false;
						mScreenManager->OverlayOff();
						break;
					default:
						//printf("Unmanaged progress indicator: 0x%08x at %d\n", (unsigned int)inInstruction, progress);
						break;
				}
				if (progress>prevProgress) {
					prevProgress = progress;
					if (mScreenManager->OverlayIsOn()) {
						mScreenManager->OverlayPrintProgress(1, progress*100/46);
						if (*title) mScreenManager->OverlayPrintAt(0, 3, title, true);
						mScreenManager->OverlayFlush();
					}
				}
			}
		}
		
		// Now execute the native implementation of the coprocessor
		
		switch (inInstruction >> 8)
		{
			case 0x000000:
				ExecuteFlashDriverNative( inInstruction );
				break;
				
			case 0x000001:
				ExecutePlatformDriverNative( inInstruction );
				break;
				
			case 0x000002:
				ExecuteSoundDriverNative( inInstruction );
				break;
				
			case 0x000003:
				ExecuteBatteryDriverNative( inInstruction );
				break;
				
			case 0x000004:
				ExecuteScreenDriverNative( inInstruction );
				break;
	
			case 0x000005:
				ExecuteTabletDriverNative( inInstruction );
				break;
	
			case 0x000006:
				ExecuteSerialDriverNative( inInstruction );
				break;
	
			case 0x000007:
				ExecuteInTranslatorNative( inInstruction );
				break;
	
			case 0x000008:
				ExecuteOutTranslatorNative( inInstruction );
				break;
	
			case 0x000009:
				ExecuteHostCallNative( inInstruction );
				break;
				
			case 0x00000A:
				ExecuteNetworkManagerNative( inInstruction );
				break;
		
#if TARGET_OS_MAC	
			case 0x00000B:
				ExecuteHostiOSNativeiOS( inInstruction );
				break;
#endif
				
			default:
				if (mLog)
				{
					mLog->FLogLine(
						"Unimplemented native primitive %.8X (pc=%.8X)",
						(unsigned int) inInstruction,
						(unsigned int) mProcessor->GetRegister(15) );
				}					
		}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteFlashDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteFlashDriverNative( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
		{
#if debugFlash
			KUInt32 chipAddr = mProcessor->GetRegister( 1 );
#endif
				// 34000000 & 34400000
			KUInt32 mask = mProcessor->GetRegister( 2 );
			KUInt32 theIDStructAddr = mProcessor->GetRegister( 3 );
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::Identify(%.8X, %.8X, %.8X)",
					(unsigned int) chipAddr,
					(unsigned int) mask,
					(unsigned int) theIDStructAddr );
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
				mProcessor->SetRegister( 0, 1 );
				(void) mMemory->Write(theIDStructAddr + 0x00, 0x00000089);
				(void) mMemory->Write(theIDStructAddr + 0x04, 0x00000000);
				(void) mMemory->Write(theIDStructAddr + 0x08, 0x00000002);
				(void) mMemory->Write(theIDStructAddr + 0x0C, 0x00000002);
				(void) mMemory->Write(theIDStructAddr + 0x10, 0x00200000);
				(void) mMemory->Write(theIDStructAddr + 0x14, 0x00010000);
			} else {
				mProcessor->SetRegister( 0, 0 );
			}
		}
		break;

		case 0x02:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::CleanUp" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x03:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::Init" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x04:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::InitializeDriverData" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x05:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::CleanUpDriverData" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x06:
#if debugFlash > 1
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::StartReadingArray" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x07:
#if debugFlash > 1
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::DoneReadingArray" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x08:
			{
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
				Boolean is32bits =
					(virtualTable == 0x0001E3D4)
					|| (virtualTable == 0x0001E3E0)
					|| (virtualTable == 0x0001E180);
#if debugFlash
				if (mLog)
				{
					mLog->FLogLine(
						"TEinsteinFlashDriver::Write(data=%.8X, mask=%.8X, addr=%.8X, VT=%.8X, %s)",
						(unsigned int) mProcessor->GetRegister( 1 ),
						(unsigned int) mProcessor->GetRegister( 2 ),
						(unsigned int) mProcessor->GetRegister( 3 ),
						(unsigned int) virtualTable,
						is32bits ? "32bits" : "16bits" );
				}
#endif
				Boolean theResult;
				if (is32bits)
				{
					theResult = mMemory->WriteToFlash32Bits(
						(unsigned int) mProcessor->GetRegister( 1 ),
						(unsigned int) mProcessor->GetRegister( 2 ),
						(unsigned int) mProcessor->GetRegister( 3 ) );
				} else {
					theResult = mMemory->WriteToFlash16Bits(
						(unsigned int) mProcessor->GetRegister( 1 ),
						(unsigned int) mProcessor->GetRegister( 2 ),
						(unsigned int) mProcessor->GetRegister( 3 ) );
				}
				if (theResult)
				{
					mProcessor->SetRegister( 0,
						(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */ );
				} else {
					mProcessor->SetRegister( 0, 0 );
				}
			}
			break;

		case 0x09:
			{
				KUInt32 flashRange = mProcessor->GetRegister( 1 );
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
				Boolean is32bits =
					(virtualTable == 0x0001E3D4)
					|| (virtualTable == 0x0001E3E0)
					|| (virtualTable == 0x0001E180);

#if debugFlash
				if (mLog)
				{
					mLog->FLogLine(
						"TEinsteinFlashDriver::StartErase(FR, %.8X, %s)",
						(unsigned int) mProcessor->GetRegister( 2 ),
						is32bits ? "32bits" : "16bits" );
				}
#endif
				Boolean theResult;
				if (is32bits)
				{
					theResult = mMemory->EraseFlash(
						(unsigned int) mProcessor->GetRegister( 2 ),
						0x20000);
				} else {
					theResult = mMemory->EraseFlash(
						(unsigned int) mProcessor->GetRegister( 2 ),
						0x10000);
				}
				
				if (theResult)
				{
					mProcessor->SetRegister( 0,
						(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */ );
				} else {
					mProcessor->SetRegister( 0, 0 );
				}
			}
			break;

		case 0x0A:
#if debugFlash > 1
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::ResetBlockStatus" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x0B:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::IsEraseComplete" );
			}
#endif
			// Erase is always complete with no error :D
			mProcessor->SetRegister( 0, 1 );
			(void) mMemory->Write(
				(unsigned int) mProcessor->GetRegister( 3 ),
				(unsigned int) 0x00000000 );
			break;

		case 0x0C:
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::LockBlock(FR, %.8X)",
					(unsigned int) mProcessor->GetRegister( 2 ) );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x0D:
#if debugFlash
			if (mLog)
			{
				mLog->FLogLine(
					"TEinsteinFlashDriver::BeginWrite(%.8X, %.8X, %.8X)",
					(unsigned int) mProcessor->GetRegister( 1 ),
					(unsigned int) mProcessor->GetRegister( 2 ),
					(unsigned int) mProcessor->GetRegister( 3 ) );
			}
#endif
			if (mMemory->TranslateAndCheckFlashAddress( mProcessor->GetRegister( 2 ), nil ))
			{
				mProcessor->SetRegister( 0,
					(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */ );
			} else {
				mProcessor->SetRegister( 0, 0 );
			}
			break;

		case 0x0E:
#if debugFlash
			if (mLog)
			{
				mLog->LogLine( "TEinsteinFlashDriver::ReportWriteResult" );
			}
#endif
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x0F:
			{
				KUInt32 startOfBlock;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, startOfBlock);
#if debugFlash
				if (mLog)
				{
					mLog->FLogLine(
						"TEinsteinFlashDriver::DoWrite(data=%.8X, mask=%.8X, addr=%.8X, Start=%.8X)",
						(unsigned int) mProcessor->GetRegister( 1 ),
						(unsigned int) mProcessor->GetRegister( 2 ),
						(unsigned int) mProcessor->GetRegister( 3 ),
						(unsigned int) startOfBlock );
				}
#endif
				mProcessor->SetRegister( 0, 0 );
			}
			break;

		case 0x10:
			{
#if debugFlash
				if (mLog)
				{
					mLog->FLogLine(
						"TEinsteinFlashDriver::DoErase(start=%.8X, size=%.8X)",
						(unsigned int) mProcessor->GetRegister( 1 ),
						(unsigned int) mProcessor->GetRegister( 2 ) );
				}
#endif
				Boolean theResult = mMemory->EraseFlash(
						(unsigned int) mProcessor->GetRegister( 1 ),
						(unsigned int) mProcessor->GetRegister( 2 ));
				
				if (theResult)
				{
					mProcessor->SetRegister( 0,
						(unsigned int) -10562 /* kError_Flash_AddressOutOfRange */ );
				} else {
					mProcessor->SetRegister( 0, 0 );
				}
			}
			break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown flash driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}					
	}
}

// -------------------------------------------------------------------------- //
//  * ExecutePlatformDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecutePlatformDriverNative( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::New" );
			}
			break;
			
		case 0x02:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::Delete" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x03:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::Init" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x04:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::BacklightTrigger" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x05:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::RegisterPowerSwitchInterrupt" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x06:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::EnableSysPowerInterrupt" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x07:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::InterruptHandler" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x08:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::TimerInterruptHandler" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x09:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::ResetZAPStoreCheck" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0A:
			{
				KUInt32 theSubsystem = mProcessor->GetRegister(1);
				if (mLog)
				{
					mLog->FLogLine(
						"TMainPlatformDriver::PowerOnSubsystem( %.8X )",
						(unsigned int) theSubsystem );
				}
				if (theSubsystem == 0x1D)
				{
					mMemory->PowerOnFlash();
//					mEmulator->BreakInMonitor();
				}
				mProcessor->SetRegister( 0, 0 );
			}
			break;
			
		case 0x0B:
			{
				KUInt32 theSubsystem = mProcessor->GetRegister(1);
				if (mLog)
				{
					mLog->FLogLine(
						"TMainPlatformDriver::PowerOffSubsystem( %.8X )",
						(unsigned int) theSubsystem );
				}
				if (theSubsystem == 0x1D)
				{
					mMemory->PowerOffFlash();
				}
				mProcessor->SetRegister( 0, 0 );
			}
			break;
			
		case 0x0C:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::PowerOffAllSubsystems" );
			}
			mMemory->PowerOffFlash();
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0D:
			if (mLog)
			{
//				mLog->LogLine( "TMainPlatformDriver::PauseSystem" );
			}
			mEmulator->PauseSystem();
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0E:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::PowerOffSystem" );
			}
			mMemory->PowerOffFlash();
			mPlatformManager->PowerOff();
			if (mQuit)
			{
				mEmulator->Quit();
			} else {
				mEmulator->PauseSystem();
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0F:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::PowerOnSystem" );
			}
			mMemory->PowerOnFlash();
			mPlatformManager->PowerOn();
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x10:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::TranslatePowerEvent( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x11:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::GetPCMCIAPowerSpec( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			
			if (mProcessor->GetRegister(1) == 0)
			{
				(void) mMemory->Write(
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) 5 );
				mProcessor->SetRegister( 0, 0 );
			} else if (mProcessor->GetRegister(1) == 1) {
				(void) mMemory->Write(
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) 7 );
				mProcessor->SetRegister( 0, 0 );
			} else {
				mProcessor->SetRegister( 0, (unsigned int) -10005 );
			}
			break;
			
		case 0x12:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::PowerOnDeviceCheck( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			{
				static int firstPause = 1;
				if (firstPause) {
					firstPause--;
					// This will remove the boot-progress display
					if (mScreenManager->OverlayIsOn()) {
						mScreenManager->OverlayOff();
					}
					// this is a hack that will install packages that were added to a
					// directory on the host. This is used by iOS/iPhone/Android.
					if (firstPause==0) {
						mPlatformManager->InstallNewPackages();
					}
				}
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x13:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::SetSubsystemPower( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2) );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x14:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainPlatformDriver::GetSubsystemPower( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			(void) mMemory->Write(
				(unsigned int) mProcessor->GetRegister(2),
				(unsigned int) 0 );
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x15:
			// GetNextEvent.
			{
				KUInt32 theWiredAddress;
				(void) mMemory->TranslateW( mProcessor->GetRegister(1), theWiredAddress );
				Boolean gotSomeEvent =
						mPlatformManager->GetNextEvent( theWiredAddress );
				mProcessor->SetRegister( 0, gotSomeEvent );
			}
			break;

		case 0x16:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::Hop!" );
			}
			mEmulator->BreakInMonitor();
			break;
			
		case 0x17:
			if (mLog)
			{
				mLog->LogLine( "TMainPlatformDriver::FillGestaltEmulatorInfo" );
			}
			(void) mMemory->Write(
				(unsigned int) mProcessor->GetRegister(1),
				(unsigned int) kUP2Version );
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x18:
			mPlatformManager->LockEventQueue();
			break;

		case 0x19:
			mPlatformManager->UnlockEventQueue();
			break;

		case 0x1A:
			// Log
			if (mLog)
			{
				KUInt32 theAddress = mProcessor->GetRegister( 1 );
				char theLine[74];
				KUInt32 amount = sizeof(theLine);
				(void) mMemory->FastReadString(theAddress, &amount, theLine);
				mLog->LogLine( theLine );
			} else {
				KUInt32 theAddress = mProcessor->GetRegister( 1 );
				char theLine[512];
				KUInt32 amount = sizeof(theLine);
				(void) mMemory->FastReadString(theAddress, &amount, theLine);
				printf("Log: %s\n", theLine);
			}
			break;

		case 0x1B:
			mProcessor->SetRegister( 0,
				mPlatformManager->GetUserInfo(
					(EUserInfoSel) mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3)));
			break;

		case 0x1C:
			mProcessor->SetRegister( 0,
				mPlatformManager->GetHostTimeZone() );
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
				FILE* theFile = popen( "xtscal", "r+" );
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
			} else {
				mQuit = true;
				mPlatformManager->SendPowerSwitchEvent();
			}
			break;

		case 0x1F:
			// DisposeBuffer.
			mProcessor->SetRegister( 0,
				mPlatformManager->DisposeBuffer(mProcessor->GetRegister(1)));
			break;

		case 0x20:
			// CopyBufferData.
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				mProcessor->SetRegister( 0,
					mPlatformManager->CopyBufferData(
						mProcessor->GetRegister(1),
						mProcessor->GetRegister(2),
						mProcessor->GetRegister(3),
						fourthParam));
			}
			break;

		case 0x21:
			if (mLog)
			{
				mLog->FLogLine("TMainPlatformDriver::OpenEinsteinMenu()");
			}
			mPlatformManager->OpenEinsteinMenu();
			break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown platform driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}					
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteSoundDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteSoundDriverNative( KUInt32 inInstruction )
{
//	mEmulator->BreakInMonitor();
	switch (inInstruction & 0xFF)
	{
			// No longer native.
//		case 0x01:
//			if (mLog)
//			{
//				mLog->LogLine( "PMainSoundDriver::New" );
//			}
//			break;
			
			// No longer native.
//		case 0x02:
//			if (mLog)
//			{
//				mLog->LogLine( "PMainSoundDriver::Delete" );
//			}
//			mProcessor->SetRegister( 0, 0 );
//			break;
			
		case 0x03:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::SetSoundHardwareInfo" );
			}
			mProcessor->SetRegister( 0, (KUInt32) -30009 );
			break;
			
		case 0x04:
//			mEmulator->BreakInMonitor();
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::GetSoundHardwareInfo" );
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister( 1 );
				(void) mMemory->Write(theInfoStructAddr + 0x00, 0x00000001);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x04, 0x00000001);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x08, 0x00000001);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x54600000);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00000006);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x00000010);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000001);	// unknown
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x05:
			{
				// mEmulator->BreakInMonitor();
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				if (mLog)
				{
					mLog->LogLine( "PMainSoundDriver::SetOutputBuffers(" );
					mLog->FLogLine(
						"  %.8X, %.8X, %.8X, %.8X )",
						(unsigned int) mProcessor->GetRegister(1),
						(unsigned int) mProcessor->GetRegister(2),
						(unsigned int) mProcessor->GetRegister(3),
						(unsigned int) fourthParam );
				}
				mSoundOutputBuffer1Addr = mProcessor->GetRegister(1);
//				mSoundOutputBuffer1Size = mProcessor->GetRegister(2);
				mSoundOutputBuffer2Addr = mProcessor->GetRegister(3);
//				mSoundOutputBuffer2Size = fourthParam;
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x06:
			if (mLog)
			{
				KUInt32 fourthParam;
				(void) mMemory->Read(mProcessor->GetRegister(13) + 4, fourthParam);
				
				mLog->LogLine( "PMainSoundDriver::SetInputBuffers(" );
				mLog->FLogLine(
					"  %.8X, %.8X, %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3),
					(unsigned int) fourthParam );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x07:
			if (mLog)
			{
				mLog->FLogLine(
					"PMainSoundDriver::ScheduleOutputBuffer( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2) );
			}
			{
				KUInt32 buffer;
				if (mProcessor->GetRegister(1))
				{
					buffer = mSoundOutputBuffer2Addr;
				} else {
					buffer = mSoundOutputBuffer1Addr;
				}
				KUInt32 amount = mProcessor->GetRegister(2);
				mSoundManager->ScheduleOutputBuffer(buffer, amount);
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x08:
			if (mLog)
			{
				mLog->FLogLine(
					"PMainSoundDriver::ScheduleInputBuffer( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2) );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x09:
			if (mLog)
			{
				mLog->FLogLine(
					"PMainSoundDriver::PowerOutputOn( %i )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0A:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::PowerOutputOff" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0B:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::PowerInputOn" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0C:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::PowerInputOff" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0D:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::StartOutput" );
			}
			mSoundManager->StartOutput();
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0E:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::StartInput" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0F:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::StopOutput" );
			}
			mSoundManager->StopOutput();
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x10:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::StopInput" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x11:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::OutputIsEnabled" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x12:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::InputIsEnabled" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x13:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::OutputIsRunning" );
			}
			mProcessor->SetRegister( 0, mSoundManager->OutputIsRunning() );
			break;
			
		case 0x14:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::InputIsRunning" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x15:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::CurrentOutputPtr" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x16:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::CurrentInputPtr" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x17:
			if (mLog)
			{
				mLog->FLogLine(
					"PMainSoundDriver::OutputVolume( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			mSoundManager->OutputVolume( mProcessor->GetRegister(1) );
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x18:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::OutputVolume" );
			}
			mProcessor->SetRegister( 0, mSoundManager->OutputVolume() );
			break;
			
		case 0x19:
			if (mLog)
			{
				mLog->FLogLine(
					"PMainSoundDriver::InputVolume( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			{
				KUInt32 param = mProcessor->GetRegister(1);
				if (param > 0xFF)
				{
					param = 0xFF;
				}
				mInputVolume = (KUInt8) param;
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x1A:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::InputVolume" );
			}
			mProcessor->SetRegister( 0, mInputVolume );
			break;
			
		case 0x1B:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::EnableExtSoundSource" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x1C:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::DisableExtSoundSource" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x1D:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::OutputIntHandler" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x1E:
			if (mLog)
			{
				mLog->LogLine( "PMainSoundDriver::InputIntHandler" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
		
		case 0x1F:
			if (mLog)
			{
				mLog->FLogLine(
					"PMainSoundDriver::NativeSetInterruptMask( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2) );
			}
			mSoundManager->SetInterruptMask(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2));
			break;
		

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown sound driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}					
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteBatteryDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteBatteryDriverNative( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::New" );
			}
			break;
			
		case 0x02:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::Delete" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x03:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::Init" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x04:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::WakeUp" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x05:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::ShutDown" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x06:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::Count" );
			}
			mProcessor->SetRegister( 0, 0 );
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
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::Status" );
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister( 2 );
				(void) mMemory->Write(theInfoStructAddr + 0x00, 0x00000003);	// mBatteryType
				(void) mMemory->Write(theInfoStructAddr + 0x04, 0x000587C0);	// mVoltage1
				(void) mMemory->Write(theInfoStructAddr + 0x08, 0x00000064);	// mBatteryLevel
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x00000014);	// mBatteryAlert
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00000000);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x006CF999);	// mVoltage6
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000000);	// mAdapterPlugged
				(void) mMemory->Write(theInfoStructAddr + 0x1C, 0x00003F36);	// mVoltage7
				(void) mMemory->Write(theInfoStructAddr + 0x20, 0x00000000);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x24, 0xFFFFFFFF);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x28, 0xFFFFFFFF);	// mUnknownDIOPins33Related
				(void) mMemory->Write(theInfoStructAddr + 0x2C, 0x001A2F28);	// mVoltage4
				(void) mMemory->Write(theInfoStructAddr + 0x30, 0x001A8D79);	// mVoltage5
			}			
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x08:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::RawStatus" );
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister( 2 );
				(void) mMemory->Write(theInfoStructAddr + 0x00, 0x00000003);	// mBatteryType
				(void) mMemory->Write(theInfoStructAddr + 0x04, 0x0C97D000);	// mVoltage1
				(void) mMemory->Write(theInfoStructAddr + 0x08, 0x00000064);	// mBatteryLevel
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x00000014);	// mBatteryAlert
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00000000);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x00E19000);	// mVoltage6
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000000);	// mAdapterPlugged
				(void) mMemory->Write(theInfoStructAddr + 0x1C, 0x005C0000);	// mVoltage7
				(void) mMemory->Write(theInfoStructAddr + 0x20, 0x00000000);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x24, 0xFFFFFFFF);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x28, 0xFFFFFFFF);	// mUnknownDIOPins33Related
				(void) mMemory->Write(theInfoStructAddr + 0x2C, 0x086E2000);	// mVoltage4
				(void) mMemory->Write(theInfoStructAddr + 0x30, 0x07D3B000);	// mVoltage5
			}			
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x09:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::StartSleepCharge" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0A:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::SetType" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0B:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::ReadADCVoltage" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0C:
			if (mLog)
			{
				mLog->LogLine( "PMainBatteryDriver::ConvertVoltage" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown battery driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}					
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteScreenDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteScreenDriverNative( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::Delete" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

//		case 0x02:
//			if (mLog)
//			{
//				mLog->LogLine( "TMainDisplayDriver::ScreenSetup" );
//			}
//			mProcessor->SetRegister( 0, 0 );
//			break;

		case 0x03:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::GetScreenInfo" );
			}
			{
				KUInt32 theInfoStructAddr = mProcessor->GetRegister( 1 );
				(void) mMemory->Write(theInfoStructAddr + 0x00, mScreenManager->GetScreenHeight() );
				(void) mMemory->Write(theInfoStructAddr + 0x04, mScreenManager->GetScreenWidth() );
				(void) mMemory->Write(theInfoStructAddr + 0x08, TScreenManager::kBitsPerPixel);
				(void) mMemory->Write(theInfoStructAddr + 0x0C, 0x00000037);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x10, 0x00640064);	// resolution
				(void) mMemory->Write(theInfoStructAddr + 0x14, 0x00000020);	// unknown
				(void) mMemory->Write(theInfoStructAddr + 0x18, 0x00000020);	// unknown
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x04:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::PowerInit" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x05:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::PowerOn" );
			}
			mScreenManager->PowerOnScreen();
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x06:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::PowerOff" );
			}
			mScreenManager->PowerOffScreen();
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x07:
			{
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
					theMode );
				mProcessor->SetRegister( 0, 0 );
			}			
			break;
			
		case 0x08:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainDisplayDriver::GetFeature( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
			}
			{
				KUInt32 theFeature = (unsigned int) mProcessor->GetRegister(1);
				if (theFeature == 0x00000000) {
					// Contrast?
					mProcessor->SetRegister( 0, mScreenManager->GetContrast() );
				} else if (theFeature == 0x00000001) {
					mProcessor->SetRegister( 0, 0x00000001 );
				} else if (theFeature == 0x00000002) {
					mProcessor->SetRegister( 0, mScreenManager->GetBacklight() );
				} else if (theFeature == 0x00000003) {
					mProcessor->SetRegister( 0, 0x00000000 );
				} else if (theFeature == 0x00000004) {
					mProcessor->SetRegister( 0, mScreenManager->GetScreenOrientation() );
				} else if (theFeature == 0x00000005) {
					mProcessor->SetRegister( 0, 0x0000000A );
				} else {
					mProcessor->SetRegister( 0, 0xFFFFFFFF );
				}
			}
			break;
			
		case 0x09:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainDisplayDriver::SetFeature( %.8X, %.8X )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2) );
			}
			{
				KUInt32 theFeature = (unsigned int) mProcessor->GetRegister(1);
				KUInt32 theValue = (unsigned int) mProcessor->GetRegister(2);
				if (theFeature == 0x00000000) {
					mScreenManager->SetContrast( theValue != 0 );
				} else if (theFeature == 0x00000002) {
					mScreenManager->SetBacklight( theValue != 0 );
				} else if (theFeature == 0x00000004) {
					mScreenManager->SetScreenOrientation(
						(TScreenManager::EOrientation) theValue );
				}
			}
			break;

		case 0x0A:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::AutoAdjustFeatures" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x0B:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainDisplayDriver::DoubleBlit( PM=%.8X, PM=%.8X, R=%.8X, R, long )",
					(unsigned int) mProcessor->GetRegister(1),
					(unsigned int) mProcessor->GetRegister(2),
					(unsigned int) mProcessor->GetRegister(3) );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x0C:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::EnterIdleMode" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;
			
		case 0x0D:
			if (mLog)
			{
				mLog->LogLine( "TMainDisplayDriver::ExitIdleMode" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown screen driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}					
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteTabletDriverNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteTabletDriverNative( KUInt32 inInstruction )
{
//	mEmulator->BreakInMonitor();
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::New" );
			}
			break;

		case 0x02:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::Delete" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x03:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::Init" );
			}
			mTabletCalibration.fUnknown_00 = 0xFFFFDFA5;
			mTabletCalibration.fUnknown_04 = 0x000015EC;
			mTabletCalibration.fUnknown_08 = 0x01F5F6B0;
			mTabletCalibration.fUnknown_0C = 0xFFEE8314;
			mTabletCalibration.fUnknown_10 = 0xC8E60000;
			mTabletSampleRate = 0x0000B400;
			break;

		case 0x04:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::WakeUp" );
			}
			mScreenManager->WakeUpTablet();
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x05:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::ShutDown" );
			}
			mScreenManager->ShutDownTablet();
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x06:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::TabletIdle" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x07:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::GetSampleRate" );
			}
			mProcessor->SetRegister( 0, mScreenManager->GetTabletSampleRate() );
			break;

		case 0x08:
			if (mLog)
			{
				mLog->FLogLine(
					"TMainTabletDriver::SetSampleRate( %.8X )",
					(unsigned int) mProcessor->GetRegister(1) );
				// mEmulator->BreakInMonitor();
			}
			mScreenManager->SetTabletSampleRate( mProcessor->GetRegister(1) );
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x09:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::GetTabletCalibration" );
			}
			{
				KUInt32 tabletCalibrationAddy = mProcessor->GetRegister(1);
				(void) mMemory->Write(
					tabletCalibrationAddy,
					mTabletCalibration.fUnknown_00 );
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x04,
					mTabletCalibration.fUnknown_04 );
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x08,
					mTabletCalibration.fUnknown_08 );
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C );
				(void) mMemory->Write(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C );
			}
			break;

		case 0x0A:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::SetTabletCalibration" );
			}
			{
				KUInt32 tabletCalibrationAddy = mProcessor->GetRegister(1);
				(void) mMemory->Read(
					tabletCalibrationAddy,
					mTabletCalibration.fUnknown_00 );
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x04,
					mTabletCalibration.fUnknown_04 );
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x08,
					mTabletCalibration.fUnknown_08 );
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C );
				(void) mMemory->Read(
					tabletCalibrationAddy + 0x10,
					mTabletCalibration.fUnknown_0C );
			}
			break;

		case 0x0B:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::SetDoingCalibration" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x0C:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::GetTabletResolution" );
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
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::TabSetOrientation" );
			}
			mScreenManager->SetTabletOrientation(
				(TScreenManager::EOrientation) mProcessor->GetRegister(1) );
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x0E:
			if (mLog)
			{
//				mLog->LogLine( "TMainTabletDriver::GetTabletState" );
			}
			mProcessor->SetRegister( 0, (KUInt32) mScreenManager->GetTabletState() );
			break;

		case 0x0F:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::GetFingerInputState" );
			}
			mProcessor->SetRegister( 0, (KUInt32) -56008 );
			break;

		case 0x10:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::SetFingerInputState" );
			}
			mProcessor->SetRegister( 0, (KUInt32) -56008 );
			break;

		case 0x11:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::RecalibrateTabletAfterRotate" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x12:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::TabletNeedsRecalibration" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x13:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::StartBypassTablet" );
			}
			mScreenManager->StartBypassTablet();
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x14:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::StopBypassTablet" );
			}
			mScreenManager->StopBypassTablet();
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x15:
			if (mLog)
			{
				mLog->LogLine( "TMainTabletDriver::ReturnTabletToConsciousness" );
			}
			mProcessor->SetRegister( 0, 0 );
			break;

		case 0x16:
			if (mLog)
			{
//				mLog->LogLine( "TMainTabletDriver::NativeGetSample" );
			}
			{
//				mEmulator->BreakInMonitor();
				KUInt32 theSampleRecord = 0;
				KUInt32 theSampleTime = 0;
				Boolean gotSomeSample =
					mScreenManager->GetSample(&theSampleRecord, &theSampleTime);
				if (gotSomeSample)
				{
					(void) mMemory->Write(
								mProcessor->GetRegister(1), theSampleRecord);
					(void) mMemory->Write(
								mProcessor->GetRegister(2), theSampleTime);
				}
				mProcessor->SetRegister( 0, gotSomeSample );
			}
			break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown tablet driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteSerialDriverNative( KUInt32 )
// Matt: to the best of my knowledge, the serial port drivers can not, or at
//		least not with some additional effort, be replaced by drivers from
//		a ROM extension. TBasicSerialPortManager was written to emulate
//		the serial port on the lowest hardware level.
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteSerialDriverNative( KUInt32 inInstruction )
{
	if (mLog)
	{
		KUInt32 theLocation = -1;
		(void) mMemory->Read(mProcessor->GetRegister(0) + 16, theLocation);
		mLog->FLogLine(
			"Hardware location (?) %.8X",
			theLocation);
	}
			
	switch (inInstruction & 0xFF)
	{
//		case 0x01:
//			// New__18TSerialChipVoyagerFv
//			break;

//		case 0x02:
//			// Delete__18TSerialChipVoyagerFv
//			break;

		case 0x03:
			// InstallChipHandler__18TSerialChipVoyagerFPvP14SCCChannelInts
			break;

		case 0x04:
			// RemoveChipHandler__18TSerialChipVoyagerFPv
			break;

		case 0x05:
			// PutByte__18TSerialChipVoyagerFUc
			break;

		case 0x06:
			// ResetTxBEmpty__18TSerialChipVoyagerFv
			break;

		case 0x07:
			// GetByte__18TSerialChipVoyagerFv
			break;

		case 0x08:
			// TxBufEmpty__18TSerialChipVoyagerFv
			break;

		case 0x09:
			// RxBufFull__18TSerialChipVoyagerFv
			break;

		case 0x0A:
			// GetRxErrorStatus__18TSerialChipVoyagerFv
			break;

		case 0x0B:
			// GetSerialStatus__18TSerialChipVoyagerFv
			break;

		case 0x0C:
			// ResetSerialStatus__18TSerialChipVoyagerFv
			break;

		case 0x0D:
			// SetSerialOutputs__18TSerialChipVoyagerFUl
			break;

		case 0x0E:
			// ClearSerialOutputs__18TSerialChipVoyagerFUl
			break;

		case 0x0F:
			// GetSerialOutputs__18TSerialChipVoyagerFv
			break;

		case 0x10:
			// PowerOff__18TSerialChipVoyagerFv
			break;

		case 0x11:
			// PowerOn__18TSerialChipVoyagerFv
			break;

		case 0x12:
			// PowerIsOn__18TSerialChipVoyagerFv
			break;

		case 0x13:
			// SetInterruptEnable__18TSerialChipVoyagerFUc
			break;

		case 0x14:
			// Reset__18TSerialChipVoyagerFv
			break;

		case 0x15:
			// SetBreak__18TSerialChipVoyagerFUc
			break;

		case 0x16:
			// SetSpeed__18TSerialChipVoyagerFUl
			break;

		case 0x17:
			// SetIOParms__18TSerialChipVoyagerFP17TCMOSerialIOParms
			break;

		case 0x18:
			// Reconfigure__18TSerialChipVoyagerFv
			break;

		case 0x19:
			// Init__18TSerialChipVoyagerFP11TCardSocketP12TCardHandlerPUc
			break;

		case 0x1A:
			// CardRemoved__18TSerialChipVoyagerFv
			break;

		case 0x1B:
			// GetFeatures__18TSerialChipVoyagerFv
			break;

		case 0x1C:
			// InitByOption__18TSerialChipVoyagerFP7TOption
			break;

		case 0x1D:
			// ProcessOption__18TSerialChipVoyagerFP7TOption
			break;

		case 0x1E:
			// SetSerialMode__18TSerialChipVoyagerFUl
			break;

		case 0x1F:
			// SysEventNotify__18TSerialChipVoyagerFUl
			break;

		case 0x20:
			// SetTxDTransceiverEnable__18TSerialChipVoyagerFUc
			break;

		case 0x21:
			// GetByteAndStatus__18TSerialChipVoyagerFPUc
			break;

		case 0x22:
			// SetIntSourceEnable__18TSerialChipVoyagerFUlUc
			break;

		case 0x23:
			// AllSent__18TSerialChipVoyagerFv
			break;

		case 0x24:
			// ConfigureForOutput__18TSerialChipVoyagerFUc
			break;

		case 0x25:
			// InitTxDMA__18TSerialChipVoyagerFP10TCircleBufPFPv_v
			break;

		case 0x26:
			// InitRxDMA__18TSerialChipVoyagerFP10TCircleBufUlPFPvUl_v
			break;

		case 0x27:
			// TxDMAControl__18TSerialChipVoyagerFUc
			break;

		case 0x28:
			// RxDMAControl__18TSerialChipVoyagerFUc
			break;

		case 0x29:
			// SetSDLCAddress__18TSerialChipVoyagerFUc
			break;

		case 0x2A:
			// ReEnableReceiver__18TSerialChipVoyagerFUc
			break;

		case 0x2B:
			// LinkIsFree__18TSerialChipVoyagerFUc
			break;

		case 0x2C:
			// SendControlPacket__18TSerialChipVoyagerFUcN21
			break;

		case 0x2D:
			// WaitForPacket__18TSerialChipVoyagerFUl
			break;

		case 0x2E:
			// WaitForAllSent__18TSerialChipVoyagerFv
			break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown serial driver native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}
			mProcessor->SetRegister( 0, 0 );
	}

	mProcessor->SetRegister( 0, 0 );
}

// -------------------------------------------------------------------------- //
//  * ExecuteInTranslatorNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteInTranslatorNative( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown in-translator native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}
			mProcessor->SetRegister( 0, 0 );
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteOutTranslatorNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteOutTranslatorNative( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown out-translator native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}
			mProcessor->SetRegister( 0, 0 );
	}
}

// -------------------------------------------------------------------------- //
//  * ExecuteHostCallNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteHostCallNative( KUInt32 inInstruction )
{
#if !TARGET_OS_ANDROID && !TARGET_OS_MAC && !__LP64__
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::New" );
			}
			break;

		case 0x02:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::Delete" );
			}
			break;

		case 0x03:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::OpenLib" );
			}
			mProcessor->SetRegister(0,
				mNativeCalls->OpenLib(mProcessor->GetRegister(1)));
			break;

		case 0x04:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::CloseLib" );
			}
			mNativeCalls->CloseLib(mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x05:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::PrepareFFIStructure" );
			}
			mProcessor->SetRegister(0,
				mNativeCalls->PrepareFFIStructure(
					mProcessor->GetRegister(1),
					mProcessor->GetRegister(2),
					mProcessor->GetRegister(3)));
			break;

		case 0x06:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::DisposeFFIStructure" );
			}
			mNativeCalls->DisposeFFIStructure(mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x07:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::GetErrorMessage" );
			}
			mNativeCalls->GetErrorMessage(
				mProcessor->GetRegister(1),
				mProcessor->GetRegister(2));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x10:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_uint8" );
			}
			mNativeCalls->SetArgValue_uint8(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;


		case 0x11:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_sint8" );
			}
			mNativeCalls->SetArgValue_sint8(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x12:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_uint16" );
			}
			mNativeCalls->SetArgValue_uint16(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;


		case 0x13:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_sint16" );
			}
			mNativeCalls->SetArgValue_sint16(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x14:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_uint32" );
			}
			mNativeCalls->SetArgValue_uint32(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;


		case 0x15:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_sint32" );
			}
			mNativeCalls->SetArgValue_sint32(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x16:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_uint64" );
			}
//			mNativeCalls->SetArgValue_uint64(
//							mProcessor->GetRegister(1),
//							mProcessor->GetRegister(2),
//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;


		case 0x17:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_sint64" );
			}
//			mNativeCalls->SetArgValue_sint64(
//							mProcessor->GetRegister(1),
//							mProcessor->GetRegister(2),
//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x18:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_float" );
			}
//			mNativeCalls->SetArgValue_float(
//							mProcessor->GetRegister(1),
//							mProcessor->GetRegister(2),
//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x19:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_double" );
			}
//			mNativeCalls->SetArgValue_double(
//							mProcessor->GetRegister(1),
//							mProcessor->GetRegister(2),
//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1A:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_longdouble" );
			}
//			mNativeCalls->SetArgValue_longdouble(
//							mProcessor->GetRegister(1),
//							mProcessor->GetRegister(2),
//							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x1B:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_string" );
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
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_binary" );
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
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetArgValue_pointer" );
			}
			mNativeCalls->SetArgValue_pointer(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x20:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::SetResultType" );
			}
			mNativeCalls->SetResultType(
							mProcessor->GetRegister(1),
							(EFFI_Type) mProcessor->GetRegister(2));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x21:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::GetOutArgValue_string" );
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
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::GetOutArgValue_binary" );
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
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::Call_void" );
			}
			mNativeCalls->Call_void(
							mProcessor->GetRegister(1));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x31:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::Call_int" );
			}
			mProcessor->SetRegister(0, 
				mNativeCalls->Call_int(
							mProcessor->GetRegister(1)));
			break;

		case 0x32:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::Call_real" );
			}
//			mProcessor->SetRegister(0, 
//				mNativeCalls->Call_real(
//							mProcessor->GetRegister(1)));
			break;

		case 0x33:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::Call_string" );
			}
			mNativeCalls->Call_string(
							mProcessor->GetRegister(1),
							mProcessor->GetRegister(2),
							mProcessor->GetRegister(3));
			mProcessor->SetRegister(0, 0);
			break;

		case 0x34:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::Call_string" );
			}
			// TODO: We have a real 64bits problem here.
			mProcessor->SetRegister(0, 
				/* (KUInt32) */ mNativeCalls->Call_pointer(
							mProcessor->GetRegister(1)));
			break;

		case 0x40:
			if (mLog)
			{
				mLog->LogLine( "TEinsteinNativeCalls::GetErrno" );
			}
			mProcessor->SetRegister(0, mNativeCalls->GetErrno());
			break;

		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Unknown call native primitive %.8X (pc=%.8X)",
					(unsigned int) inInstruction,
					(unsigned int) mProcessor->GetRegister(15) );
			}
			mProcessor->SetRegister( 0, 0 );
	}
#else
	if (mLog)
	{
		mLog->FLogLine("Native primitives not supported on this platform");
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * ExecuteNetworkManagerNative( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::ExecuteNetworkManagerNative( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
			// Debugging Helpers
			
		case 0x00:
			// Just log some unknown function without causing a fuss
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::Unknown" );
			}
			break;
		case 0x01: {
			// Write some string to the log buffer (reg0 points to the string)
			KUInt32 addr = mProcessor->GetRegister(0);
			if (mLog && addr)
			{
				char buffer[1024], *dst = buffer+22;
				strcpy(buffer, "TNetworkManager::Log: ");
				for (;;) {
					KUInt8 v;
					mMemory->ReadB(addr++, v);
					*dst++ = (char)v;
					if (v==0) break;
				}
				mLog->LogLine(buffer);
			}
	        break; }
			
			// Protocol Class Interface
			
		case 0x02:
			// a new driver is beeing created
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::New" );
			}
			break;
		case 0x03:
			// the driver is beeing deleted
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::Delete" );
			}
			break;
			
			// Task services
			
		case 0x04:
			// Initialize the card driver
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::Init" );
			}
			break;
		case 0x05:
			// Enable the card (switch power on)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::Enable" );
			}
			break;
		case 0x06:
			// Disable the card (power off to save battery)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::Disable" );
			}
			break;
		case 0x07:
			// Handle any kind of interrupt by the hardware
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::InterruptHandler" );
			}
			break;
			
			// Client Services (from event handlers of TLanternDriverAPI)
			
		case 0x08:
			// Newton transfers data to the world (calls SendPacket)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::SendBuffer" );
			}
			break;
		case 0x09:
			// Newton transfers data to the world (calls SendPacket)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::SendCBufferList" );
			}
			break;
		case 0x0a: {
			// Newton wants to send a raw packet into the world
			KUInt32 addr = mProcessor->GetRegister(1);
			KUInt32 size = mProcessor->GetRegister(2), i;
			if (mLog)
			{
				mLog->FLogLine( "TNetworkManager::SendPacket(0x%08x, %d)", addr, size );
			}
			if (mNetworkManager && size) {
				KUInt8 *data = (KUInt8*)malloc(size), v;
				for (i=0; i<size; i++) {
					mMemory->ReadB(addr+i, v);
					data[i] = v;
				}
				mNetworkManager->SendPacket(data, size);
				free(data);
			}
			break; }
		case 0x0b: {
			// NewtonOS wants the hardware MAC address of the card
			KUInt32 dstBuffer = mProcessor->GetRegister(1);
			KUInt32 dstBufferSize = mProcessor->GetRegister(2);
			KUInt32 i, err = 0;
			if (mLog)
			{
				mLog->FLogLine( "TNetworkManager::GetDeviceAddress(0x%08x, %d)", dstBuffer, dstBufferSize );
			}
			if (mNetworkManager && dstBufferSize) {
				KUInt8 mac[6] = { 0 };
				err = (KUInt32)mNetworkManager->GetDeviceAddress(mac, dstBufferSize);
				for (i=0; i<dstBufferSize; i++)
					mMemory->WriteB(dstBuffer+i, mac[i]);
			}
			mProcessor->SetRegister(0, err);			
			break; }
		case 0x0c:
			// NewtonOS wants to add a multicast address (not implemented)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::AddMulticastAddress" );
			}
			break;
		case 0x0d:
			// NewtonOS wants to remove a multicast address (not implemented)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::DelMulticastAddress" );
			}
			break;
		case 0x0e:
			// NewtonOS wants to know if the links are OK  (not implemented)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::GetLinkIntegrity" );
			}
			break;
			
			// Optional services
			
		case 0x0f:
			// receive every packet on the network, even those that are not meant for us (not implemented)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::SetPromiscuous" );
			}
			break;
		case 0x10:
			// return some number that gives the actual speed of the connection (not implemented)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::GetThroughput" );
			}
			break;
			
			// Private Template Services
			
		case 0x11:
			// a regular timer that can help us poll data and monitor integrity and throughput
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::TimerExpired" );
			}
			mNetworkManager->TimerExpired();
			break;
			
			// NE2000 Template driver specific
			
		case 0x12:
			// Initialize card (not needed?!)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::InitCard" );
			}
			break;
		case 0x13:
			// Set the card info (not needed?!)
			if (mLog)
			{
				mLog->LogLine( "TNetworkManager::SetCardInfo" );
			}
			break;
		case 0x14: {
			// Return number of bytes in the first packet available in R0
			KUInt32 size = 0;
			if (mNetworkManager) {
				size = mNetworkManager->DataAvailable();
			}
			mProcessor->SetRegister(0, size);
			if (mLog)
			{
				mLog->FLogLine( "TNetworkManager::DataAvailable(Avail: %d)", size );
			}
			break; }
		case 0x15: { 
			// Copy the next available packet into the buffer pointed to by R1
			KUInt32 dst = mProcessor->GetRegister(1);
			KUInt32 i, n = mProcessor->GetRegister(2);
			if (mLog)
			{
				mLog->FLogLine( "TNetworkManager::ReceiveData (buffer=0x%08x, size=%d", dst, n );
			}
			if (mNetworkManager && n) {
				KUInt8 *buffer = (KUInt8*)malloc(n);
				mNetworkManager->ReceiveData(buffer, n);
				for (i=0; i<n; i++)
					mMemory->WriteB(dst+i, buffer[i]);
				free(buffer);
			}
			break; }
		case 0x16: {
			// Print some memory location
			KUInt32 addr = mProcessor->GetRegister(0);
			KUInt32 size = mProcessor->GetRegister(1), i;
			if (mLog && size)
			{
				KUInt8 *buffer = (KUInt8*)malloc(size);
				for (i=0;i<size;i++) {
					KUInt8 v;
					mMemory->ReadB(addr+i, v);
					buffer[i] = v;
				}
				mNetworkManager->LogBuffer(buffer, size);
			}
	        break; }
			
		default:
			if (mLog)
			{
				mLog->FLogLine(
							   "TNetworkManager: Unknown native primitive %.8X (pc=%.8X)",
							   (unsigned int) inInstruction,
							   (unsigned int) mProcessor->GetRegister(15) );
			}					
	}
}

#if TARGET_OS_MAC
void
TNativePrimitives::ExecuteHostiOSNativeiOS( KUInt32 inInstruction )
{
	switch (inInstruction & 0xFF)
	{
		case 0x01:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostGetCPUArchitecture" );
			}
			//
			mProcessor->SetRegister(0, mObjCBridgeCalls->HostGetCPUArchitecture());
			break;
		case 0x02:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostMakeNSInvocation" );
			}
			mProcessor->SetRegister(0,
									mObjCBridgeCalls->HostMakeNSInvocation(mProcessor->GetRegister(0),
																		   mProcessor->GetRegister(1),
																		   mProcessor->GetRegister(2)));
			break;
		case 0x03:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostSetInvocationTarget" );
			}
			mProcessor->SetRegister(0,
									mObjCBridgeCalls->HostSetInvocationTarget(mProcessor->GetRegister(0),
																			  mProcessor->GetRegister(1)));
			break;
		case 0x04:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostSetInvocationArgument_Object" );
			}
			mProcessor->SetRegister(0,
									mObjCBridgeCalls->HostSetInvocationArgument_Object(mProcessor->GetRegister(0), mProcessor->GetRegister(1), mProcessor->GetRegister(2)));
			break;
		case 0x05:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostGetInvocationReturn_Object" );
			}
			mProcessor->SetRegister(0,
									mObjCBridgeCalls->HostGetInvocationReturn_Object(mProcessor->GetRegister(0), mProcessor->GetRegister(1)));
			break;
		case 0x06:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostInvoke" );
			}
			mProcessor->SetRegister(0,
									mObjCBridgeCalls->HostInvoke(mProcessor->GetRegister(0)));
			break;
		case 0x07:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostReleaseObject" );
			}
			mProcessor->SetRegister(0,
									mObjCBridgeCalls->HostReleaseObject(mProcessor->GetRegister(0)));
			break;
		case 0x08:
			if (mLog)
			{
				mLog->LogLine( "TObjCBridgeCalls::HostMakeNSString" );
			}
			mProcessor->SetRegister(0,
									mObjCBridgeCalls->HostMakeNSString(mProcessor->GetRegister(0),
																	   mProcessor->GetRegister(1)));
			break;

			
			
	}
}
#endif

// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TNativePrimitives::TransferState( TStream* inStream )
{
	// The various registers.
	inStream->TransferInt32BE( mTabletCalibration.fUnknown_00 );
	inStream->TransferInt32BE( mTabletCalibration.fUnknown_04 );
	inStream->TransferInt32BE( mTabletCalibration.fUnknown_08 );
	inStream->TransferInt32BE( mTabletCalibration.fUnknown_0C );
	inStream->TransferInt32BE( mTabletCalibration.fUnknown_10 );
	inStream->TransferInt32BE( mTabletSampleRate );
	inStream->TransferByte( mInputVolume );
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
