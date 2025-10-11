// ==============================
// File:			TNativePrimitives.h
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

#ifndef _TNATIVEPRIMITIVES_H
#define _TNATIVEPRIMITIVES_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#include <stdio.h>

class TLog;
class TEmulator;
class TMemory;
class TARMProcessor;
class TNetworkManager;
class TPrinterManager;
class TSoundManager;
class TScreenManager;
class TPlatformManager;
#if !TARGET_OS_MAC
class TNativeCalls;
#endif
class TVirtualizedCalls;
class TStream;
#if TARGET_OS_MAC
class TObjCBridgeCalls;
#endif

struct TabletCalibration {
	KUInt32 fUnknown_00;
	KUInt32 fUnknown_04;
	KUInt32 fUnknown_08;
	KUInt32 fUnknown_0C;
	KUInt32 fUnknown_10;
};

struct s64bits {
	KUInt32 hi;
	KUInt32 lo;
};

///
/// Class for the native primitives (executed as a coprocessor).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TNativePrimitives
{
public:
	///
	/// Constructor from the log and the emulator.
	///
	/// \param inLog		interface for logging.
	/// \param inMemory		interface to the memory.
	/// \param inEmulator	interface with the emulator.
	///
	TNativePrimitives(
		TLog* inLog,
		TMemory* inMemory);

	///
	/// Destructor.
	///
	~TNativePrimitives(void);

	///
	/// Set the processor.
	///
	/// \param inProcessor  processor using this interface.
	///
	void
	SetProcessor(TARMProcessor* inProcessor)
	{
		mProcessor = inProcessor;
	}

	///
	/// Set the emulator.
	///
	/// \param inEmulator emulator (access to hardware managers).
	///
	void SetEmulator(TEmulator* inEmulator);

	///
	/// Execute a native instruction.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteNative(KUInt32 inInstruction);

	///
	/// Load or save the state to a stream.
	///
	void TransferState(TStream* inStream);

private:
	/// Constants
	enum {
		kScreenOrientation_AppleTop = 0x00000000,
		kScreenOrientation_AppleRight = 0x00000001,
		kScreenOrientation_AppleBottom = 0x00000002,
		kScreenOrientation_AppleLeft = 0x00000003,
		kScreenOrientation_LandscapeBit = 0x00000001
	};

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TNativePrimitives(const TNativePrimitives& inCopy) = delete;

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TNativePrimitives& operator=(const TNativePrimitives& inCopy) = delete;

	///
	/// Execute a native instruction related to the flash driver.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteFlashDriverNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to the platform driver.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecutePlatformDriverNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to the sound driver.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteSoundDriverNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to the battery driver.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteBatteryDriverNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to the screen driver.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteScreenDriverNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to the tablet driver.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteTabletDriverNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to the serial driver.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteSerialDriverNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to in-translator.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteInTranslatorNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to out-translator.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteOutTranslatorNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to host interface.
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteHostCallNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction related to network access
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteNetworkManagerNative(KUInt32 inInstruction);

	///
	/// Execute a native instruction supporting iOS integration
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecuteHostiOSNativeiOS(KUInt32 inInstruction);

	///
	/// Execute a native instruction for printer drivers
	///
	/// \param inInstruction	opcode of the instruction to execute.
	///
	void ExecutePrinterDriverNative(KUInt32 inInstruction);

	/// \name Variables

	/// Reference to the CPU.
	TARMProcessor* mProcessor { nullptr };

	/// Interface for logging.
	TLog* mLog { nullptr };

	/// Bitmask for enabled logs
	KUInt32 mLogMask { 0b000000000000 };

	/// Interface to the memory.
	TMemory* mMemory { nullptr };

	/// Emulator (interface to hardware).
	TEmulator* mEmulator { nullptr };

	/// Network manager.
	TNetworkManager* mNetworkManager { nullptr };

	/// Printer manager
	TPrinterManager* mPrinterManager { nullptr };

	/// Sound manager
	TSoundManager* mSoundManager { nullptr };

	/// Screen manager.
	TScreenManager* mScreenManager { nullptr };

	/// Platform manager.
	TPlatformManager* mPlatformManager { nullptr };

#if !TARGET_OS_MAC
	/// Interface for native calls.
	TNativeCalls* mNativeCalls { nullptr };
#endif

	/// Interface for virtualized calls.
	TVirtualizedCalls* mVirtualizedCalls { nullptr };

#if TARGET_OS_MAC
	/// Interface for iOS calls.
	TObjCBridgeCalls* mObjCBridgeCalls { nullptr };
#endif

	// (unused) Feature 0x0 (probably the contrast).
	//KUInt32 mScreenContrast { 0 };

	// (unused) Feature 0x2.
	//KUInt32 mScreenBacklight { 0 };

	// (unused) Feature 0x4.
	// \note There is also TScreenManager::mScreenOrientation!
	//KUInt32 mScreenOrientation { 0 };

	/// Tablet calibration.
	TabletCalibration mTabletCalibration { 0, 0, 0, 0, 0 };

	/// (unused) Feature 0x2.
	KUInt32 mTabletSampleRate { 0 };

 	/// Input volume (0-255)
	KUInt8 mInputVolume { 0 };

	/// Whether to quit.
	Boolean mQuit { false };

	/// Output buffer #1: the addr.
	KUInt32 mSoundOutputBuffer1Addr { 0 };

	/// Output buffer #2: the addr.
	KUInt32 mSoundOutputBuffer2Addr { 0 };
};

#endif
// _TNATIVEPRIMITIVES_H

// ============================================================== //
// X windows:                                                     //
//         The ultimate bottleneck.                               //
//         Flawed beyond belief.                                  //
//         The only thing you have to fear.                       //
//         Somewhere between chaos and insanity.                  //
//         On autopilot to oblivion.                              //
//         The joke that kills.                                   //
//         A disgrace you can be proud of.                        //
//         A mistake carried out to perfection.                   //
//         Belongs more to the problem set than the solution set. //
//         To err is X windows.                                   //
//         Ignorance is our most important resource.              //
//         Complex nonsolutions to simple nonproblems.            //
//         Built to fall apart.                                   //
//         Nullifying centuries of progress.                      //
//         Falling to new depths of inefficiency.                 //
//         The last thing you need.                               //
//         The defacto substandard.                               //
//                                                                //
// Elevating brain damage to an art form.                         //
//         X windows.                                             //
// ============================================================== //
