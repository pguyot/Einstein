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

T_ROM_INJECTION(0x00018688, "Progress_ROMBoot") {
	TScreenManager *screen = ioCPU->GetEmulator()->GetScreenManager();
	if (screen->OverlayIsOn()) {
		screen->OverlayPrintProgress(1, 2);
		screen->OverlayPrintAt(0, 3, "ROMBoot", 1);
		screen->OverlayFlush();
	}
	return ioUnit;
}


// avoid calibration screen early in the game
TJITGenericROMPatch p001412f8(0x001412f8, 0xea000009, "Avoid screen calibration");

// disable "FlushDCache" since we do not emulate a DCache 
//TJITGenericROMPatch p00018994(0x00018994, 0xe1a0f00e, "Obsolete FlushDCache"); // #  mov pc, lr

// disable "CleanPageInDCache" since we do not emulate a DCache 
//TJITGenericROMPatch p0001894c(0x0001894c, 0xe1a0f00e, "Obsolete CleanPageInDCache"); // #  mov pc, lr

// disable "TGeoPortDebugLink::BeaconDetect(long)"
TJITGenericROMPatch p000db0d8(0x000db0d8, 0xe3a00000, "BeaconDetect (1/2)"); // #  mov r0, 0x00000000
TJITGenericROMPatch p000db0dc(0x000db0dc, 0xe1a0f00e, "BeaconDetect (2/2)"); // #  mov pc, lr

TJITGenericROMPatch gDebuggerPatch(0x000013f4, 1, "gDebugger patch");

TJITGenericROMPatch gNewtConfigPatch(0x000013fc, 0x00000002 /*kEnableListener*/ | 0x00000200 /*kDefaultStdioOn*/ | 0x00008000 /*kEnableStdout*/, "gNewtConfig patch");

/* Example ROM injections. The return value is ignored. */
/*
T_ROM_INJECTION(0x00000010, "Data Abort") {
    const char *what = "unknown";
    unsigned int addr = ioCPU->GetMemory()->GetFaultAddressRegister();
    
    if (addr < 0x00800000) 
        what = "ROM";
    else if (addr<0x00f00000)
        what = "ROM Extension";
    else if (addr>=0x01a00000 && addr<0x01c10000)
        what = "C++ Jump Tables";
    else if (addr>=0x01d80000 && addr<0x01e00000)
        what = "REx Jump Table";
    else if (addr>=0x03500000 && addr<0x03d00000)
        what = "ROM Mirror";
    else if (addr>=0x04000000 && addr<0x04100000)
        what = "First ROM Page Mirror";
    else if (addr>=0x05000000 && addr<0x05200000)
        what = "Flash Bank 1";
    else if (addr>=0x0c000000 && addr<0x0c002000)
        what = "Kernel Stack";
    else if (addr>=0x0c002c00 && addr<0x0c003400)
        what = "IRQ Stack";
    else if (addr>=0x0c003400 && addr<0x0c004000)
        what = "FIQ Stack";
    else if (addr>=0x0c004000 && addr<0x0c004c00)
        what = "Svc Stack";
    else if (addr>=0x0c004c00 && addr<0x0c006000)
        what = "Undef Stack";
    else if (addr>=0x0c007400 && addr<0x0c010000)
        what = "User Stack";
    else if (addr>=0x0c100000 && addr<0x0c100800)
        what = "Kernel Domain Heap Base";
    else if (addr>=0x0c100800 && addr<0x0c126000)
        what = "Kernel Globals";
    
    fprintf(stderr, "DATA ABORT: accessing 0x%08X from 0x%08X (%s)\n", 
            addr, ioCPU->mR14abt_Bkup-8, what);
    return ioUnit;
}
*/

/*
T_ROM_INJECTION(0x0038D8DC, "Floating Point Exception") {
    fprintf(stderr, "Floating Point Exception\n");
    return ioUnit;
}
*/

/*
T_ROM_INJECTION(0x001A7248, "Long loop") {
    fprintf(stderr, "ROM REX cheksum time eater (r2=0x%08x)\n", 
			ioCPU->GetRegister(2));
	// r0 = 0x40000000
	// r1 = 0x001c7f12
	// r2 = 0x001c7f12
	// r3 = 0x000011b5
	// r7 = 0
	// r9 = 0
	
	// r0 = 0x4001fc4c
	// r1 = 0x000338fe
	// r2 = 0x000338ff
	// r3 = 0x000011b5
	// r7 = 0
	// r9 = 0
	return ioUnit;
}
T_ROM_INJECTION(0x001A726C, "Long loop end") {
    fprintf(stderr, "ROM REX cheksum time eater done\n");
	// r0 = 0x4071fc4c
	// r1 = 0xffffffff
	// r2 = 0
	// r3 = 0
	// r7 = 0x18741e4d
	// r9 = 0x4aff4c37
	
	// r0 = 0x400ee048
	// r1 = 0xffffffff
	// r2 = 0
	// r3 = 0x968d0000
	// r7 = 0xbbb55d42
	// r9 = 0x68481a07
	return ioUnit;
}
*/

// Swap: 0x003AE204


// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch first member of database
// -------------------------------------------------------------------------- //
TJITGenericROMPatch **TJITGenericPatchManager::mPatchList = 0L;
KUInt32     TJITGenericPatchManager::mPatchListTop = 0;
KUInt32     TJITGenericPatchManager::mPatchListSize = 0;


// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, KUInt32 val)
:	mIndex(addToManager()),
	mOriginalInstruction(0xFFFFFFFF),
	mName(0L),
	mStub(0L),
	mFunction(0L),
	mAddress(addr),
	mNewInstruction(val)
{
}


// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, KUInt32 val, const char *name)
:	mIndex(addToManager()),
	mOriginalInstruction(0xFFFFFFFF),
	mName(name),
	mStub(0L),
	mFunction(0L),
    mAddress(addr),
    mNewInstruction(val)
{
    fprintf(stderr, "Adding ROM patch: %s\n", name);
}


// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor for Simulator function calls
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name, AnyFunctionPtr function)
:	mIndex(addToManager()),
	mOriginalInstruction(0xFFFFFFFF),
	mName(name),
	mStub(stub),
	mFunction(0L),
	mAddress(addr),
    mNewInstruction(0xef800000|mIndex)
{
//    fprintf(stderr, "Adding ROM patch to Simulator function: %3d = %s\n", (int)nPatch, name);
}


// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor for JIT instructions
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name)
:	mIndex(addToManager()),
	mOriginalInstruction(0xFFFFFFFF),
	mName(name),
	mStub(stub),
	mFunction(0L),
    mAddress(addr),
    mNewInstruction(0xef800000|mIndex)
{
//    fprintf(stderr, "Adding ROM patch to JIT function: %3d = %s\n", (int)nPatch, name);
}


// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor for Simulator method calls
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name, JITSimPtr function)
:	mIndex(addToManager()),
	mOriginalInstruction(0xFFFFFFFF),
	mName(name),
	mStub(stub),
	mFunction(0L),
	mAddress(addr),
	mNewInstruction(0xef800000|mIndex)
{
//	fprintf(stderr, "Adding ROM patch to Simulator function:   %3d = %s\n", (int)nPatch, name);
}


// -------------------------------------------------------------------------- //
//  * Destructor
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::~TJITGenericROMPatch() {
	// This should never be called except when quitting the app.
	// We don't worry about reversing the patch or unmanaging the patch.
}


// -------------------------------------------------------------------------- //
//  * Get patch at a given index.
// -------------------------------------------------------------------------- //
TJITGenericROMPatch *TJITGenericPatchManager::GetPatchAt(KUInt32 ix)
{
	if (ix<mPatchListTop) {
		return mPatchList[ix];
	} else {
		fprintf(stderr, "ERROR in %s %d: accessing invalid patch index %d of %d\n", __FILE__, __LINE__, ix, mPatchListTop);
		return 0;
	}
}


// -------------------------------------------------------------------------- //
//  * Add another stub to the array of stubs and return the index
// -------------------------------------------------------------------------- //
KUInt32 TJITGenericPatchManager::add(TJITGenericROMPatch *p)
{
	if (p==0)
		return 0;
    if (mPatchListTop==mPatchListSize) {
		if (mPatchListTop) {
			mPatchListTop *= 2;
		} else {
			mPatchListSize = 64;
		}
		mPatchList = (TJITGenericROMPatch**)realloc(mPatchList, mPatchListSize*sizeof(TJITGenericROMPatch*));
    }
    mPatchList[mPatchListTop++] = p;
    return (mPatchListTop-1);
}


// -------------------------------------------------------------------------- //
//  * Apply the patch to the ROM words
// -------------------------------------------------------------------------- //
void TJITGenericROMPatch::apply(KUInt32 *ROM)
{
	SetOrigialInstruction(ROM[mAddress]);
    ROM[mAddress] = GetNewInstruction();
}


// -------------------------------------------------------------------------- //
//  * DoPatchROM(KUInt32*, const std::string&)
// -------------------------------------------------------------------------- //
void
TJITGenericPatchManager::DoPatchROM(KUInt32* inROMPtr, const std::string& inMachineName) {
	if (inMachineName == "717006") {
		for (KUInt32 i=0; i<mPatchListTop; i++) {
			mPatchList[i]->apply(inROMPtr);
		}
		fprintf(stderr, "%u patches applied\n", GetNumPatches());
	}
#if TARGET_OS_MAC
	// Matt: this is an ugly hack that removes a reference from the 'extr'
	//		serial port driver from the REx. This is required to make low level
	//		comm emulation possible. Removing this patch is not harmful beyond
	//		disabling comm emulation. Keeping the patch should not be harmful
	//		either.
	KUInt32 *ROM = inROMPtr;
	// Find the pattern 'extr\0\0\0\0' in the first 1MB of the REx.
	for (KUInt32 addr = 0x00800634/4; addr<0x00900000/4; ++addr) {
		if (ROM[addr]=='extr' && ROM[addr+1]==0) {
			ROM[addr]='~xtr'; // just change this into an unlikely FourCC
			fprintf(stderr, "Removing 'extr' serial port driver from REx.\n");
			break;
		}
	}
#endif
}


// -------------------------------------------------------------------------- //
//  * Apply the injection patch to the ROM words
// -------------------------------------------------------------------------- //
void TJITGenericROMInjection::apply(KUInt32 *ROM)
{
    SetOrigialInstruction(ROM[GetOffsetInROM()]);
    ROM[GetOffsetInROM()] = GetNewInstruction() | 0xefc00000;
}


// -------------------------------------------------------------------------- //
//  * Apply the simulator injection patch to the ROM words
// -------------------------------------------------------------------------- //
void TJITGenericROMSimulatorInjection::apply(KUInt32 *ROM)
{
	SetOrigialInstruction(ROM[GetOffsetInROM()]);
    ROM[GetOffsetInROM()] = GetNewInstruction() | 0xefa00000;
}


#endif
