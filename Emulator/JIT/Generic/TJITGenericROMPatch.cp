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
#include "JIT.h"

#ifdef JITTARGET_GENERIC

#include "TJITGenericROMPatch.h"

#include "Emulator/TEmulator.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/TDMAManager.h"
#include "Emulator/Screen/TScreenManager.h"
#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"


// MARK: -
// ========================================================================== //
//  * Various patches that are always useful


/*
 * This ROM injection creates a boot progress indicator.
 * Furter updates are displayed whenever NewtonOS jumps into native driver
 * functions.
 *
 * \see TNativePrimitives::ExecuteNative(KUInt32 inInstruction)
 */
T_ROM_INJECTION(0x00018688, "Progress_ROMBoot") {
	TScreenManager *screen = ioCPU->GetEmulator()->GetScreenManager();
	if (screen->OverlayIsOn()) {
		screen->OverlayPrintProgress(1, 2);
		screen->OverlayPrintAt(0, 3, "ROMBoot", 1);
		screen->OverlayFlush();
	}
	return ioUnit;
}


/*
 * Avoid calibration screen early in the game.
 * This patch changes a branch instruction in CheckTabletCalibration(void).
 */
TJITGenericPatch p001412f8(0x001412f8, 0xea000009, "Avoid screen calibration");


/*
 * Disable "TGeoPortDebugLink::BeaconDetect(long)"
 * Replace the function with a `return 0;` equivalent.
 */
TJITGenericPatch p000db0d8(0x000db0d8, 0xe3a00000, "BeaconDetect (1/2)"); // #  mov r0, 0x00000000
TJITGenericPatch p000db0dc(0x000db0dc, 0xe1a0f00e, "BeaconDetect (2/2)"); // #  mov pc, lr


/*
 * This patch seems to disable runtime debugging statistics.
 */
TJITGenericPatch gDebuggerPatch(0x000013f4, 1, "gDebugger patch");


/*
 * This patch seems to enable stdin and stdout for NewtonOS.
 */
TJITGenericPatch gNewtConfigPatch(0x000013fc,
								    0x00000002 /*kEnableListener*/
								  | 0x00000200 /*kDefaultStdioOn*/
								  | 0x00008000 /*kEnableStdout*/,
								  "gNewtConfig patch");


// ========================================================================== //
// MARK: -
// TJITGenericPatchManager


/**
 Initialize static TJITGenericPatchObject member variables
 */
TJITGenericPatchObject **TJITGenericPatchManager::mPatchList = 0L;
KUInt32     TJITGenericPatchManager::mPatchListTop = 0;
KUInt32     TJITGenericPatchManager::mPatchListSize = 0;


/**
 Get patch at a given index.
 \param ix index into the patch table
 \return pointer to the patch
 */
TJITGenericPatchObject *TJITGenericPatchManager::GetPatchAt(KUInt32 ix)
{
	if (ix<mPatchListTop) {
		return mPatchList[ix];
	} else {
		fprintf(stderr, "ERROR in %s %d: accessing invalid patch index %d of %d\n",
				__FILE__, __LINE__, ix, mPatchListTop);
		return 0;
	}
}


/**
 Add another patch to the patch list and return the index
 \param p newly generated patch
 \return index into the patch table
 */
KUInt32 TJITGenericPatchManager::Add(TJITGenericPatchObject *p)
{
	if (p==0)
		return 0;
	if (mPatchListTop==mPatchListSize) {
		if (mPatchListTop) {
			mPatchListTop *= 2;
		} else {
			mPatchListSize = 64;
		}
		mPatchList = (TJITGenericPatchObject**)
			realloc(mPatchList, mPatchListSize*sizeof(TJITGenericPatchObject*));
	}
	mPatchList[mPatchListTop++] = p;
	return (mPatchListTop-1);
}


/**
 Apply all patches to the ROM.
 \param inROMPtr pointer to an array of ROM words
 \param inMachineName this is actually the version number of the ROM. As of
 		April 2018, we only support patches for 717006 which is the ROM file
 		that comes with the Native Developer Kit.
 */
void TJITGenericPatchManager::DoPatchROM(KUInt32* inROMPtr, const std::string& inMachineName)
{
	if (inMachineName == "717006") {
		for (KUInt32 i=0; i<mPatchListTop; i++) {
			mPatchList[i]->Apply(inROMPtr);
		}
		fprintf(stderr, "%u patches applied\n", GetNumPatches());
	}
}


// ========================================================================== //
// MARK: -
// TJITGenericPatchObject


/**
 TJITGenericPatchObject constructor
 */
TJITGenericPatchObject::TJITGenericPatchObject(KUInt32 addr, const char *name)
:	mIndex(AddToManager()),
	mOriginalInstruction(0xFFFFFFFF),
	mName(name),
    mAddress(addr)
{
//    fprintf(stderr, "Adding ROM patch: %s\n", name);
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
KUInt32 TJITGenericPatchObject::GetOffsetInROM()
{
	if (mAddress>=TMemoryConsts::kHighROMEnd) {
		fprintf(stderr, "ERROR in %s %d: patch address not in ROM at 0x%08X in patch '%s'\n",
				__FILE__, __LINE__, mAddress, mName?mName:"(unnamed)");
		return 0;
	}
	if (mAddress&0x00000003) {
		fprintf(stderr, "ERROR in %s %d: patch address not word-aligned at 0x%08X in patch '%s'\n",
				__FILE__, __LINE__, mAddress, mName?mName:"(unnamed)");
		return 0;
	}
	return mAddress>>2;
}


// ========================================================================== //
// MARK: -
// TJITGenericPatch


/**
 \brief Create and add a new patch
 \param address address in ROM, must be word aligned
 \param value a new value for that address, can be data or instructions
 \param name a name for this patch
 */
TJITGenericPatch::TJITGenericPatch(KUInt32 address, KUInt32 value, const char *name)
: 	TJITGenericPatchObject(address, name),
	mNewInstruction(value)
{
}


/**
 \brief Apply the patch to the ROM words.
 */
void TJITGenericPatch::Apply(KUInt32 *ROM)
{
	SetOrigialInstruction(ROM[GetOffsetInROM()]);
	ROM[GetOffsetInROM()] = GetNewInstruction();
}


// ========================================================================== //
// MARK: -
// TJITGenericPatchFindAndReplace


/**
 \brief Find and replace word in an area of the ROM.

 Find a sequence of words in ROM and replace them with a new sequence.

 \param startAddress start searching here
 \param endAddress stop searching before reaching this address
 \param key an array of `KUInt32`, where the first entry is the size of the
 remaining array. This is what we are looking for.
 \param replacement an array of `KUInt32`, where the first entry is the size
 of the remaining array. This is the replacement data.
 \param name an optional name for this patch
 */
TJITGenericPatchFindAndReplace::TJITGenericPatchFindAndReplace(
								KUInt32 startAddress, KUInt32 endAddress,
								KUInt32 *key, KUInt32 *replacement,
								const char *name)
: 	TJITGenericPatch(startAddress, 0, name),
	mStart(startAddress),
	mEnd(endAddress),
	mKey(key),
	mReplacement(replacement)
{
}


/**
 Patch the ROM
 */
void TJITGenericPatchFindAndReplace::Apply(KUInt32 *ROM)
{
	KUInt32 keyLen = mKey[0];
	KUInt32 rplLen = mReplacement[0];
	KUInt32 len = keyLen>rplLen ? keyLen : rplLen;
	KUInt32 start = mStart/4, end = (mEnd - len)/4;
	KUInt32 addr;

	for (addr = start; addr<end; ++addr) {
		KUInt32 i;
		for (i=0; i<keyLen; ++i) {
			if (ROM[addr+i] != mKey[i+1])
				break;
		}
		if (i==keyLen) {
			for (KUInt32 j=0; j<rplLen; ++j) {
				ROM[addr+j] = mReplacement[j+1];
			}
			break;
		}
	}
	if (addr==end && GetName()) {
		fprintf(stderr, "WARNING in %s %d: Pattern not found, patch \"%s\" not applied.\n",
			__FILE__, __LINE__, GetName());
	}
}


// ========================================================================== //
// MARK: -
// TJITGenericPatchNativeCall


/**
 \brief Apply the patch to the ROM words.

 This generates an SWI instruction with an index and special flags.
 */
void TJITGenericPatchNativeCall::Apply(KUInt32 *ROM)
{
	SetOrigialInstruction(ROM[GetOffsetInROM()]);
    ROM[GetOffsetInROM()] = kSWINativeCall|GetIndex();
}


/**
 \brief Call the patch code.
 */
JITUnit *TJITGenericPatchNativeCall::Call(JITUnit *ioUnit, TARMProcessor *ioCPU)
{
	JITFuncPtr callJIT = GetStub();
	if (callJIT) {
		return callJIT(ioUnit, ioCPU);
	} else {
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
void TJITGenericPatchNativeInjection::Apply(KUInt32 *ROM)
{
    SetOrigialInstruction(ROM[GetOffsetInROM()]);
	ROM[GetOffsetInROM()] = kSWINativeInjection | GetIndex();
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
					fprintf(stderr, "SIM_INFO[%u]: %s caught at 0x%08X, lr=0x%08X (pcAbort=0x%08X)\n",
							(unsigned int)errCnt++, err, (unsigned int)pc, (unsigned int)ioCPU->GetRegister(14)-4, (unsigned int)ioCPU->mR14abt_Bkup);
					if (symbol) {
						if (offset) {
							fprintf(stderr, "SIM_INFO: ... at %s%+d\n", symbol, offset);
						} else {
							fprintf(stderr, "SIM_INFO: ... at %s\n", symbol);
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
