// ==============================
// File:			TJITGenericROMPatch.cp
// Project:			Einstein
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

#include "Emulator/JIT/JIT.h"

#ifdef JITTARGET_GENERIC

#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"

#include "Emulator/TARMProcessor.h"
#include "Emulator/TDMAManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/Screen/TScreenManager.h"
#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"

#include <cassert>

// MARK: -
// ========================================================================== //
//  * Various patches that are always useful

/*
 * Avoid calibration screen early in the game.
 * This patch changes a branch instruction in CheckTabletCalibration(void),
 * so it always returns without asking (b 0x00141324 in the US ROM, the same
 * offset in the German, eMate and Watson ROMs).
 */
TJITGenericPatch p001412f8(0x001412f8, 0x00142E4C, 0x00133760, 0x00141D1C,
	0xea000009, "Avoid screen calibration");

/*
 * Disable "TGeoPortDebugLink::BeaconDetect(long)"
 * Replace the function with a `return 0;` equivalent.
 *
 * The stock ROM has gDebuggerBits (0x000013F4) set to 0x40, so
 * InitializeCommHardware calls InitSerialDebugging(1, 1, 'gpdl', 57600) at
 * every boot. That installs the GeoPort debug link on the 'extr' serial port
 * and waits about half a second for a host beacon (a pulse on CTS). Without a
 * beacon, the link is not installed.
 */
TJITGenericPatch p000db0d8(0x000db0d8, kROMPatchVoid, kROMPatchVoid, kROMPatchVoid,
	0xe3a00000, "BeaconDetect (1/2)"); // #  mov r0, 0x00000000
TJITGenericPatch p000db0dc(0x000db0dc, kROMPatchVoid, kROMPatchVoid, kROMPatchVoid,
	0xe1a0f00e, "BeaconDetect (2/2)"); // #  mov pc, lr

/*
 * Set gDebuggerBits to 1 (stock value 0x40, see BeaconDetect above).
 * Bit 0x01 tells the OS that a hardware debugger is attached: IsDebuggerPresent()
 * returns true, packages and loaded code are registered with the debugger, and
 * FIQHandler passes the reset switch interrupt to DebuggerIntHandler instead of
 * resetting. Clearing bit 0x40 also skips the GeoPort debug link at boot.
 */
TJITGenericPatch gDebuggerPatch(0x000013f4, kROMPatchVoid, kROMPatchVoid, kROMPatchVoid,
	1, "gDebugger patch");

/*
 * This patch seems to enable stdin and stdout for NewtonOS.
 */
TJITGenericPatch gNewtConfigPatch(0x000013fc, kROMPatchVoid, kROMPatchVoid, kROMPatchVoid,
	0x00000002 /*kEnableListener*/
		| 0x00000200 /*kDefaultStdioOn*/
		| 0x00008000 /*kEnableStdout*/,
	"gNewtConfig patch");

/*
 * Intercept DebugStr calls and prints them via the Emulator logger.
 */

T_ROM_PATCH(0x0038CE6C, kROMPatchVoid, kROMPatchVoid, kROMPatchVoid, "DebugStr")
{
	(void) ioUnit;
	TLog* log = ioCPU->GetEmulator()->GetLog();
	if (log != nullptr)
	{
		KUInt32 theAddress = ioCPU->GetRegister(0);
		char theLine[512];
		KUInt32 amount = sizeof(theLine);
		ioCPU->GetMemory()->FastReadString(theAddress, &amount, theLine);
		log->LogLine(theLine);
	}
	ioCPU->SetRegister(15, ioCPU->GetRegister(14) + 4);
	return 0L;
}

/*
 * Break via the monitor. Adjust PC to LR + 8 to undo PC adjustment by the monitor
 */

T_ROM_PATCH(0x0038CE70, kROMPatchVoid, kROMPatchVoid, kROMPatchVoid, "Debugger")
{
	(void) ioUnit;
	ioCPU->GetEmulator()->BreakInMonitor();
	ioCPU->SetRegister(15, ioCPU->GetRegister(14) + 8);
	return 0L;
}

/*
 NewtonOS keeps the checksums of the ROM and of every REx in the flash, and
 erases the internal store when they change ("The internal store was erased
 because a different ROM has been installed"). Einstein writes the checksums
 of the patched ROM image into the flash at every launch (TFlash), so adding
 or removing a patch is safe. But words that change while Einstein runs, for
 example Monitor breakpoints in ROM, would change what NewtonOS computes, and
 the next boot (Hardware Reset, NewtonScript Reboot) would erase the store.

 static OSCalibrationParameters::CalculateROMREXCheckSums(
	unsigned long inStart, unsigned long inSize, TOSCheckSum& outSum)
 sums the high and the low 16 bits of every word of a segment. For the ROM
 and the RExes, this returns the checksums of the image as it was created.
 Any other range is summed as the ROM does it.
 */
T_ROM_PATCH(0x001A71B8, 0x001A973C, 0x0019501C, 0x001A77FC, "CalculateROMREXCheckSums")
{
	(void) ioUnit;
	TMemory* theMemory = ioCPU->GetMemory();
	KUInt32 theStart = ioCPU->GetRegister(0);
	KUInt32 theSize = ioCPU->GetRegister(1);
	KUInt32 theSum = ioCPU->GetRegister(2);
	KUInt32 theChecksums[2];
	if (!theMemory->GetLaunchSegmentChecksums(theStart, theSize, theChecksums))
	{
		KUInt32 highBits = 0;
		KUInt32 lowBits = 0;
		for (KUInt32 offset = 0; offset < (theSize & ~3U); offset += 4)
		{
			Boolean fault = false;
			KUInt32 theWord = theMemory->ReadPAligned(theStart + offset, fault);
			if (fault)
				break;
			lowBits += theWord & 0x0000FFFF;
			highBits += theWord >> 16;
		}
		theChecksums[0] = highBits;
		theChecksums[1] = lowBits;
	}
	// TOSCheckSum: the sum of the high bits, then the sum of the low bits
	theMemory->Write(theSum, theChecksums[0]);
	theMemory->Write(theSum + 4, theChecksums[1]);
	ioCPU->SetRegister(15, ioCPU->GetRegister(14) + 4);
	return 0L;
}

/*
 * Set the time to the host time. The RTC counts UTC, as on a real Newton.
 * NewtonOS converts between the RTC and local time with GMTOffset() and
 * DaylightSavingsOffset() (RealClockSeconds, SetRealClockSeconds and
 * SetSysAlarm are their only callers). Those read the Newton's home city
 * and daylight saving preferences; they return the host's time zone
 * instead, so alarms match the RTC whatever city the Newton is set to.
 */

T_ROM_PATCH(0x002554C0, 0x00253578, 0x0023575C, 0x002520B0, "GMTOffset")
{
	(void) ioUnit;
	KSInt32 theGMTOffset, theDSTOffset;
	TInterruptManager::GetHostTimeZone(theGMTOffset, theDSTOffset);
	ioCPU->SetRegister(0, (KUInt32) theGMTOffset);
	ioCPU->SetRegister(15, ioCPU->GetRegister(14) + 4);
	return 0L;
}

/*
 * SetRealClockSeconds(local seconds) writes the RTC when the user sets the
 * time. The time comes from the host, and a changed RTC would put alarms
 * off, so it does nothing (as SetTimeInSeconds, "Ignore setting time").
 */
TJITGenericPatch ignoreSetRealClock(0x002555B8, 0x00253670, 0x00235854, 0x002521A8,
	0xe1a0f00e, "Ignore SetRealClockSeconds"); // mov pc, lr

T_ROM_PATCH(0x0025551C, 0x002535D4, 0x002357B8, 0x0025210C, "DaylightSavingsOffset")
{
	(void) ioUnit;
	KSInt32 theGMTOffset, theDSTOffset;
	TInterruptManager::GetHostTimeZone(theGMTOffset, theDSTOffset);
	ioCPU->SetRegister(0, (KUInt32) theDSTOffset);
	ioCPU->SetRegister(15, ioCPU->GetRegister(14) + 4);
	return 0L;
}

T_ROM_PATCH(0x00255578, 0x00253630, 0x00235814, 0x00252168, "RealClockSeconds")
{
	(void) ioUnit;
	ioCPU->SetRegister(0, TInterruptManager::GetHostLocalTime());
	ioCPU->SetRegister(15, ioCPU->GetRegister(14) + 4);
	return 0L;
}

/* Patches for the 2010 problem:
 *
 * The addresses for the German, eMate and Watson ROMs were found by matching
 * the instruction words of the US ROM (branch offsets ignored), and the data
 * words by value; each has exactly one match per ROM.
 *
 * The low level NewtonOS clock will continue to run with seconds since
 * 1904-01-01T00:00:00. The NewtonScript functions which have been using a time
 * base of 1993-01-01T00:00:00 will however be patched so that they will use a
 * new time base of 2024-01-01T00:00:00. The patches will add or substract the
 * delta between the time bases.
 *
 * NewtonScript integers have 30 bits, so seconds from the time base wrap
 * after 2^29 seconds (17 years). The time base was 2008-01-01 until 2026; in
 * 2025, alarms set in seconds wrapped to 1993, fired at once, and kept the
 * Newton from going to sleep. The base year will need to be updated in 2040.
 */

/* Delta from 2024-01-01T00:00:00 to 1993-01-01T00:00:00 (11322 days) */

static const KUInt32 safeIntervalDeltaSeconds = 978220800;

/*
 * Adjust seconds values returned to NS layer into safe interval
 */

T_ROM_PATCH(0x00089B80, 0x0008ADCC, 0x000863B8, 0x0008A0B0, "FTimeInSeconds")
{
	ioCPU->SetRegister(0, (ioCPU->GetRegister(0) - safeIntervalDeltaSeconds) << 2);
	return ioUnit;
}

/*
 * Fix date from seconds returned to NS layer
 */

T_ROM_PATCH(0x0008A8A8, 0x0008BAF4, 0x000870E0, 0x0008ADD8, "FDateFromSeconds")
{
	ioCPU->SetRegister(1, ioCPU->GetRegister(1) + safeIntervalDeltaSeconds);
	ioCPU->SetRegister(0, ioCPU->GetRegister(13));
	return ioUnit;
}

static const KUInt32 newTimeBaseMinutes = 252460800; /* 2024/1/1 in minutes from 1904/1/1 as NS integer (43830 days) */
static const KUInt32 newTimeBaseSeconds = 3786912000; /* 2024/1/1 in seconds from 1904/1/1 as NS integer */

TJITGenericPatch timeBase1(0x00420750, 0x0043CE08, 0x003B5FD8, 0x00443384,
	newTimeBaseMinutes, "Time base (1/4)");
TJITGenericPatch timeBase2(0x00420798, 0x0043CE50, 0x003B6020, 0x004433CC,
	newTimeBaseMinutes, "Time base (2/4)");
TJITGenericPatch timeBase3(0x004DCA14, 0x004F7E68, 0x0047FE3C, 0x004F3C08,
	newTimeBaseMinutes, "Time base (3/4)");
TJITGenericPatch timeBase4(0x0030F088, 0x002E9D74, 0x002A2838, 0x0030E608,
	newTimeBaseSeconds, "Time base (4/4)");
TJITGenericPatch ignoreSettingTime(0x0008A20C, 0x0008B458, 0x00086A44, 0x0008A73C,
	0xe1a0f00e, "Ignore setting time"); // mov pc, lr

/*
 * AdjustRealTimeClock runs once per boot. It resets the RTC to 1996-07-08 if
 * the RTC is before that date or after 2010-01-01. Einstein's RTC runs on
 * host time, so it was reset to 1996 at every boot, and alarms set for
 * today were 30 years in the future. Keep the lower bound, drop the upper
 * one: the instruction after the "too early" branch returns. The RTC then
 * runs on host time (UTC) until its 32-bit seconds overflow in February 2040.
 */
TJITGenericPatch adjustRealTimeClock(0x00045314, 0x00045BE4, 0x00044BA8, 0x00045320,
	0xe1a0f00e, "AdjustRealTimeClock upper bound"); // mov pc, lr

// Disable the Schlumberger signature verification for Watson ROMs.
TJITGenericPatch watsonSig1(kROMPatchVoid, kROMPatchVoid, kROMPatchVoid, 0x007F3284,
	0xE3A00000, "Disable Watson Signature Check (1/2)"); // mov r0, #0
TJITGenericPatch watsonSig3(kROMPatchVoid, kROMPatchVoid, kROMPatchVoid, 0x007F3288,
	0xE1A0f00E, "Disable Watson Signature Check (2/2)"); // mov pc, lr

// ========================================================================== //
// MARK: -
// TJITGenericPatchManager

/**
 Initialize static TJITGenericPatchObject member variables
 */
TJITGenericPatchObject** TJITGenericPatchManager::mPatchList = nullptr;
KUInt32 TJITGenericPatchManager::mPatchListTop = 0;
KUInt32 TJITGenericPatchManager::mPatchListSize = 0;

/**
 Get patch at a given index.
 \param ix index into the patch table
 \return pointer to the patch
 */
TJITGenericPatchObject*
TJITGenericPatchManager::GetPatchAt(KUInt32 ix)
{
	if (ix < mPatchListTop)
	{
		return mPatchList[ix];
	} else
	{
		//		KPrintf("ERROR in %s %d: accessing invalid patch index %d of %d\n",
		//				__FILE__, __LINE__, (int)ix, (int)mPatchListTop);
		return NULL;
	}
}

/**
 Add another patch to the patch list and return the index
 \param p newly generated patch
 \return index into the patch table
 */
KUInt32
TJITGenericPatchManager::Add(TJITGenericPatchObject* p)
{
	if (p == 0)
		return 0;
	if (mPatchListTop == mPatchListSize)
	{
		if (mPatchListSize)
		{
			mPatchListSize *= 2;
		} else
		{
			mPatchListSize = 64;
		}
		mPatchList = (TJITGenericPatchObject**)
			realloc(mPatchList, mPatchListSize * sizeof(TJITGenericPatchObject*));
	}
	mPatchList[mPatchListTop++] = p;
	return (mPatchListTop - 1);
}

/**
 Apply all patches to the ROM.
 \param inROMPtr pointer to an array of ROM words
 \param inROMId is an ID of the ROM that the user selected. This can be used to apply the same patch to
		different locations in the  MP2100 US ROM, the German ROM, or the eMate ROM.
 */
void
TJITGenericPatchManager::DoPatchROM(KUInt32* inROMPtr, KSInt32 inROMId)
{
	if (inROMId != TROMImage::kUnknownROM)
	{
		for (KUInt32 i = 0; i < mPatchListTop; i++)
		{
			mPatchList[i]->Apply(inROMPtr, inROMId);
		}
		KPrintf("%u patches applied\n", (unsigned) GetNumPatches());
	}
}

// ========================================================================== //
// MARK: -
// TJITGenericPatchObject

/**
 TJITGenericPatchObject constructor
 */
TJITGenericPatchObject::TJITGenericPatchObject(KUInt32 inAddr0, KUInt32 inAddr1, KUInt32 inAddr2, KUInt32 inAddr3,
	const char* name) :
		mIndex(AddToManager()),
		mAddress { inAddr0, inAddr1, inAddr2, inAddr3 },
		mOriginalInstruction(0xFFFFFFFF),
		mName(name)
{
	static_assert(kROMPatchNumIDs == 4, "Fix this constructor if the number of supported ROMs changed");
	assert(inAddr0 == kROMPatchVoid || inAddr0 < 16 * 1024 * 1024);
	assert(inAddr1 == kROMPatchVoid || inAddr1 < 16 * 1024 * 1024);
	assert(inAddr2 == kROMPatchVoid || inAddr2 < 16 * 1024 * 1024);
	assert(inAddr3 == kROMPatchVoid || inAddr3 < 16 * 1024 * 1024);
	//    KPrintf("Adding ROM patch: %s\n", name);
}

/**
 Destructor.
 */
TJITGenericPatchObject::~TJITGenericPatchObject()
{
	// This should never be called except when quitting the app.
	// We don't worry about reversing the patch or unmanaging the patch.
}

/**
 \brief Return the offset of the instruction into the ROM word array.
 \return offset in words
 */
KUInt32
TJITGenericPatchObject::GetOffsetInROM(KSInt32 inROMId)
{
	if (inROMId == TROMImage::kUnknownROM)
		return kROMPatchVoid;

	KUInt32 address = mAddress[inROMId];
	if (address == kROMPatchVoid)
		return kROMPatchVoid;

	if (address >= TMemoryConsts::kHighROMEnd)
	{
		KPrintf("ERROR in %s %d: patch address not in ROM at 0x%08X in patch '%s'\n",
			__FILE__, __LINE__, (unsigned) address, mName ? mName : "(unnamed)");
		return kROMPatchVoid;
	}

	if (address & 0x00000003)
	{
		KPrintf("ERROR in %s %d: patch address not word-aligned at 0x%08X in patch '%s'\n",
			__FILE__, __LINE__, (unsigned) address, mName ? mName : "(unnamed)");
		return kROMPatchVoid;
	}

	return address >> 2;
}

// ========================================================================== //
// MARK: -
// TJITGenericPatch

/**
 \brief Create and add a new patch
 \param inAddr0, inAddr1, inAddr2, inAddr3 patch address for MP2100US, MP2100D, eMate ROMs, and Watson
 \param value a new value for that address, can be data or instructions
 \param name a name for this patch
 */
TJITGenericPatch::TJITGenericPatch(KUInt32 inAddr0, KUInt32 inAddr1, KUInt32 inAddr2, KUInt32 inAddr3,
	KUInt32 value, const char* name) :
		TJITGenericPatchObject(inAddr0, inAddr1, inAddr2, inAddr3, name),
		mNewInstruction(value)
{
}

/**
 \brief Apply the patch to the ROM words.
 */
void
TJITGenericPatch::Apply(KUInt32* ROM, KSInt32 inROMId)
{
	KUInt32 offset = GetOffsetInROM(inROMId);
	if (offset != kROMPatchVoid)
	{
		KPrintf("Applying patch at 0x%08X: %s\n", offset * 4, GetName());
		SetOrigialInstruction(ROM[offset]);
		ROM[offset] = GetNewInstruction();
	}
}

// ========================================================================== //
// MARK: -
// TJITGenericPatchFindAndReplace

/**
 \brief Verify and replace words in ROM.

 Verify a sequence of words in ROM and replace them with a new sequence.

 \param inAddr0, inAddr1, inAddr2 patch address for MP2100US, MP2100D, and eMate ROMs
 \param key an array of `KUInt32`, where the first entry is the size of the
 remaining array.
 \param replacement an array of `KUInt32`, where the first entry is the size
 of the remaining array. This is the replacement data.
 \param name an optional name for this patch
 */
TJITGenericPatchFindAndReplace::TJITGenericPatchFindAndReplace(
	KUInt32 inAddr0, KUInt32 inAddr1, KUInt32 inAddr2, KUInt32 inAddr3,
	KUInt32* key, KUInt32* replacement,
	const char* name) :
		TJITGenericPatch(inAddr0, inAddr1, inAddr2, inAddr3, 0, name),
		mKey(key),
		mReplacement(replacement)
{
}

/**
 Patch the ROM
 */
void
TJITGenericPatchFindAndReplace::Apply(KUInt32* ROM, KSInt32 inROMId)
{
	KUInt32 offset = GetOffsetInROM(inROMId);
	if (offset == kROMPatchVoid)
		return;

	KPrintf("Applying find-and-replace patch at 0x%08X: %s\n", offset * 4, GetName());

	KUInt32 keyLen = mKey[0];
	if (memcmp(ROM + offset, mKey + 1, keyLen * 4) != 0)
	{
		KPrintf("WARNING in %s %d: Key pattern does not match, patch \"%s\" not applied.\n",
			__FILE__, __LINE__, GetName());
		return;
	}

	KUInt32 rplLen = mReplacement[0];
	memcpy(ROM + offset, mReplacement + 1, rplLen * 4);
}

// ========================================================================== //
// MARK: -
// TJITGenericPatchNativeCall

/**
 \brief Apply the patch to the ROM words.

 This generates an SWI instruction with an index and special flags.
 */
void
TJITGenericPatchNativeCall::Apply(KUInt32* ROM, KSInt32 inROMId)
{
	KUInt32 offset = GetOffsetInROM(inROMId);
	if (offset != kROMPatchVoid)
	{
		KPrintf("Applying native call patch at 0x%08X: %s\n", offset * 4, GetName());
		SetOrigialInstruction(ROM[offset]);
		ROM[offset] = kSWINativeCall | GetIndex();
	}
}

/**
 \brief Call the patch code.
 */
JITUnit*
TJITGenericPatchNativeCall::Call(JITUnit* ioUnit, TARMProcessor* ioCPU)
{
	JITFuncPtr callJIT = GetStub();
	if (callJIT)
	{
		return callJIT(ioUnit, ioCPU);
	} else
	{
		CALLNEXTUNIT;
	}
}

// ========================================================================== //
// MARK: -
// TJITGenericPatchNativeInjection

/**
 \brief Apply the patch to the ROM words.

 This generates an SWI instruction with an index and special flags.
 */
void
TJITGenericPatchNativeInjection::Apply(KUInt32* ROM, KSInt32 inROMId)
{
	KUInt32 offset = GetOffsetInROM(inROMId);
	if (offset != kROMPatchVoid)
	{
		KPrintf("Applying native injection patch at 0x%08X: %s\n", offset * 4, GetName());
		SetOrigialInstruction(ROM[offset]);
		ROM[offset] = kSWINativeInjection | GetIndex();
	}
}

#if 0
// The following code is here only for reference. The Simulation is currently
// not working and waits to be reimplemented in a much nicer way.
// -------------------------------------------------------------------------- //
//  * CallHostInjection
//
//  An Injection allows for a quick native call without disturbing the original
//  instruction at the point of the injection.
// -------------------------------------------------------------------------- //
JITInstructionProto(CallHostInjection)
{
	static KUInt32 errCnt = 0;

	// Set the PC before jumping to the handler....
	KUInt32 callIndex;
	POPVALUE(callIndex);
	JITFuncPtr stub = TJITGenericPatchManager::GetPatchAt(callIndex)->GetStub();
	if (stub) {
		try {
			// The stub calls a native function.
			// If it returns 0L, the PC changed within the native code and we
			// need to find the next ioUnit.
			// Otherwise, we simply continue by running the already JIT'ed
			// instruction that was here before we injected code.
			JITUnit *ret = stub(ioUnit, ioCPU);
			if (ret==0L) {
				MMUCALLNEXT_AFTERSETPC; // returns from function
			}
		} catch (const char *err) {
			KUInt32 pc = ioCPU->GetRegister(15)-4;
			if (pc!=8 && (pc<0x00800000 || pc>0x008fffff)) { // don't chat about every SWI
				char *symbol = 0;
				int offset = 0;
				if (TSymbolList::List) {
					symbol = (char*)::malloc(1024);
					TSymbolList::List->GetNearestSymbolByAddress(pc, symbol, 0L, &offset);
					KPrintf("SIM_INFO[%u]: %s caught at 0x%08X, lr=0x%08X (pcAbort=0x%08X)\n",
							(unsigned int)errCnt++, err, (unsigned int)pc, (unsigned int)ioCPU->GetRegister(14)-4, (unsigned int)ioCPU->mR14abt_Bkup);
					if (symbol) {
						if (offset) {
							KPrintf("SIM_INFO: ... at %s%+d\n", symbol, offset);
						} else {
							KPrintf("SIM_INFO: ... at %s\n", symbol);
						}
						::free(symbol);
					}
				}
			}
			MMUCALLNEXT_AFTERSETPC;
		}
	}
	CALLNEXTUNIT;
}
#endif

#endif
