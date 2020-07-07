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

#include "Emulator/TEmulator.h"
#include "Emulator/ROM/TROMImage.h"
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
 * \note This patch is disabled because the boot process is now fast enough,
 *      and there is no visual indication needed anymore.
 *
 * \see TNativePrimitives::ExecuteNative(KUInt32 inInstruction)
 */
//T_ROM_INJECTION(0x00018688, kROMPatchVoid, kROMPatchVoid,
//                "Progress_ROMBoot")
//{
//	TScreenManager *screen = ioCPU->GetEmulator()->GetScreenManager();
//	if (screen->OverlayIsOn()) {
//		screen->OverlayPrintProgress(1, 2);
//		screen->OverlayPrintAt(0, 3, "ROMBoot", 1);
//		screen->OverlayFlush();
//	}
//	return ioUnit;
//}


/*
 * Avoid calibration screen early in the game.
 * This patch changes a branch instruction in CheckTabletCalibration(void).
 */
TJITGenericPatch p001412f8(0x001412f8, kROMPatchVoid, kROMPatchVoid,
                           0xea000009, "Avoid screen calibration");


/*
 * Disable "TGeoPortDebugLink::BeaconDetect(long)"
 * Replace the function with a `return 0;` equivalent.
 */
TJITGenericPatch p000db0d8(0x000db0d8, kROMPatchVoid, kROMPatchVoid,
                           0xe3a00000, "BeaconDetect (1/2)"); // #  mov r0, 0x00000000
TJITGenericPatch p000db0dc(0x000db0dc, kROMPatchVoid, kROMPatchVoid,
                           0xe1a0f00e, "BeaconDetect (2/2)"); // #  mov pc, lr


/*
 * This patch seems to disable runtime debugging statistics.
 */
TJITGenericPatch gDebuggerPatch(0x000013f4, kROMPatchVoid, kROMPatchVoid,
                                1, "gDebugger patch");


/*
 * This patch seems to enable stdin and stdout for NewtonOS.
 */
TJITGenericPatch gNewtConfigPatch(0x000013fc, kROMPatchVoid, kROMPatchVoid, 
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
TJITGenericPatchObject **TJITGenericPatchManager::mPatchList = nullptr;
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
//		fprintf(stderr, "ERROR in %s %d: accessing invalid patch index %d of %d\n",
//				__FILE__, __LINE__, (int)ix, (int)mPatchListTop);
		return NULL;
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
 \param inROMId is an ID of the ROM that the user selected. This can be used to apply the same patch to
        different locations in the  MP2100 US ROM, the German ROM, or the eMate ROM.
 */
void TJITGenericPatchManager::DoPatchROM(KUInt32* inROMPtr, KSInt32 inROMId)
{
    if (inROMId!=TROMImage::kUnknownROM) {
		for (KUInt32 i=0; i<mPatchListTop; i++) {
			mPatchList[i]->Apply(inROMPtr, inROMId);
		}
		fprintf(stderr, "%u patches applied\n", (unsigned)GetNumPatches());
	}
}


// ========================================================================== //
// MARK: -
// TJITGenericPatchObject


/**
 TJITGenericPatchObject constructor
 */
TJITGenericPatchObject::TJITGenericPatchObject(KUInt32 inAddr0, KUInt32 inAddr1, KUInt32 inAddr2,
                                               const char *name)
:	mIndex(AddToManager()),
    mAddress{inAddr0, inAddr1, inAddr2},
    mOriginalInstruction(0xFFFFFFFF),
	mName(name)
{
    static_assert(kROMPatchNumIDs==3, "Fix this constructor if the number of supported ROMs cahnged");
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
KUInt32 TJITGenericPatchObject::GetOffsetInROM(KSInt32 inROMId)
{
    if (inROMId==TROMImage::kUnknownROM)
        return kROMPatchVoid;

    KUInt32 address = mAddress[inROMId];
    if (address==kROMPatchVoid)
        return kROMPatchVoid;

    if (address>=TMemoryConsts::kHighROMEnd) {
        fprintf(stderr, "ERROR in %s %d: patch address not in ROM at 0x%08X in patch '%s'\n",
                __FILE__, __LINE__, (unsigned)address, mName?mName:"(unnamed)");
        return kROMPatchVoid;
    }

    if (address&0x00000003) {
        fprintf(stderr, "ERROR in %s %d: patch address not word-aligned at 0x%08X in patch '%s'\n",
                __FILE__, __LINE__, (unsigned)address, mName?mName:"(unnamed)");
        return kROMPatchVoid;
    }

    return address>>2;
}


// ========================================================================== //
// MARK: -
// TJITGenericPatch


/**
 \brief Create and add a new patch
 \param inAddr0, inAddr1, inAddr2 patch address for MP2100US, MP2100D, and eMate ROMs
 \param value a new value for that address, can be data or instructions
 \param name a name for this patch
 */
TJITGenericPatch::TJITGenericPatch(KUInt32 inAddr0, KUInt32 inAddr1, KUInt32 inAddr2,
                                   KUInt32 value, const char *name)
: 	TJITGenericPatchObject(inAddr0, inAddr1, inAddr2, name),
	mNewInstruction(value)
{
}


/**
 \brief Apply the patch to the ROM words.
 */
void TJITGenericPatch::Apply(KUInt32 *ROM, KSInt32 inROMId)
{
    KUInt32 offset = GetOffsetInROM(inROMId);
    if (offset!=kROMPatchVoid) {
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
								KUInt32 inAddr0, KUInt32 inAddr1, KUInt32 inAddr2,
								KUInt32 *key, KUInt32 *replacement,
								const char *name)
: 	TJITGenericPatch(inAddr0, inAddr1, inAddr2, 0, name),
	mKey(key),
	mReplacement(replacement)
{
}


/**
 Patch the ROM
 */
void TJITGenericPatchFindAndReplace::Apply(KUInt32 *ROM, KSInt32 inROMId)
{
    KUInt32 offset = GetOffsetInROM(inROMId);
    if (offset==kROMPatchVoid)
        return;

    KUInt32 keyLen = mKey[0];
    if (memcmp(ROM+offset, mKey+1, keyLen*4)!=0) {
        fprintf(stderr, "WARNING in %s %d: Key pattern does not match, patch \"%s\" not applied.\n",
                __FILE__, __LINE__, GetName());
        return;
    }

    KUInt32 rplLen = mReplacement[0];
    memcpy(ROM+offset, mReplacement+1, rplLen*4);
}


// ========================================================================== //
// MARK: -
// TJITGenericPatchNativeCall


/**
 \brief Apply the patch to the ROM words.

 This generates an SWI instruction with an index and special flags.
 */
void TJITGenericPatchNativeCall::Apply(KUInt32 *ROM, KSInt32 inROMId)
{
    KUInt32 offset = GetOffsetInROM(inROMId);
    if (offset!=kROMPatchVoid) {
        SetOrigialInstruction(ROM[offset]);
        ROM[offset] = kSWINativeCall|GetIndex();
    }
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
void TJITGenericPatchNativeInjection::Apply(KUInt32 *ROM, KSInt32 inROMId)
{
    KUInt32 offset = GetOffsetInROM(inROMId);
    if (offset!=kROMPatchVoid) {
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
