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

/*
//T_ROM_INJECTION(0x000E5CA4, "DispatchFIQ") {
T_ROM_INJECTION(0x000E5BF4, "DispatchIRQ") {
//T_ROM_INJECTION(0x0038D7A0, "DispatchFIQ") {
	// pc = r1+16
	static KUInt32 prev[32] = { 0 };
	KUInt32 addr = ioCPU->mCurrentRegisters[TARMProcessor::kR1] + 16;
	KUInt32 newPC;
	ioCPU->GetMemory()->Read(addr, newPC);
	int i;
	for (i=0; i<32; i++) {
		if (prev[i]==newPC) break;
		if (prev[i]==0) {
			prev[i] = newPC;
			fprintf(stderr, "FIQ Handler found at 0x%08X\n", (unsigned int)newPC);
			break;
		}
	}
    return ioUnit;
}
*/

/* Example ROM injections. The return value is ignored. */
/*
T_ROM_INJECTION(0x00000000, "RESET") {
    fprintf(stderr, "RESET called with r0=0x%08x\n", ioCPU->mCurrentRegisters[TARMProcessor::kR0]);
    return ioUnit;
}
T_ROM_INJECTION(0x0000000c, "Prefetch Abort") {
    fprintf(stderr, "PREFETCH ABORT: at 0x%08x\n", 
            ioCPU->GetMemory()->GetFaultAddressRegister());
    return ioUnit;
}
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
TJITGenericROMPatch  *TJITGenericROMPatch::first_ = 0L;
TJITGenericROMPatch **TJITGenericROMPatch::patch_ = 0L;
KUInt32     TJITGenericROMPatch::nPatch = 0;
KUInt32     TJITGenericROMPatch::NPatch = 0;

// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, KUInt32 val)
:	next_(first_),
	address_(addr>>2),
	value_(val),
	stub_(0L),
	function_(0L),
	method_(0L)
{
    first_ = this;
}

// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, KUInt32 val, const char *name)
:   next_(first_),
    address_(addr>>2),
    value_(val),
    stub_(0L),
    function_(0L),
    method_(0L)
{
    first_ = this;
	name_ = name;
    fprintf(stderr, "Adding ROM patch: %s\n", name);
}

// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor for Simulator function calls
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name, AnyFunctionPtr function)
:   next_(first_),
    address_(addr>>2),
    value_(0xef800000),
    stub_(stub),
    function_(function),
    method_(0L)
{
    first_ = this;
	name_ = name;
//    fprintf(stderr, "Adding ROM patch to Simulator function: %3d = %s\n", (int)nPatch, name);
    value_ |= addPatch(this);
}

// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor for JIT instructions
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name)
:   next_(first_),
    address_(addr>>2),
    value_(0xef800000),
    stub_(stub),
    function_(0L),
    method_(0L)
{
    first_ = this;
	name_ = name;
//    fprintf(stderr, "Adding ROM patch to JIT function: %3d = %s\n", (int)nPatch, name);
    value_ |= addPatch(this);
}

// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor for Simulator method calls
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name, AnyMethodPtr method)
:   next_(first_),
address_(addr>>2),
value_(0xef800000),
stub_(stub),
function_(0L),
method_(method)
{
	first_ = this;
	name_ = name;
//	fprintf(stderr, "Adding ROM patch to Simulator method:   %3d = %s\n", (int)nPatch, name);
	value_ |= addPatch(this);
}

// -------------------------------------------------------------------------- //
//  * TJITGenericROMPatch constructor for Simulator method calls
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::TJITGenericROMPatch(KUInt32 addr, JITFuncPtr stub, const char *name, JITSimPtr function)
:   next_(first_),
	address_(addr>>2),
	value_(0xef800000),
	stub_(stub),
	function_((AnyFunctionPtr)function),
	method_(0L)
{
	first_ = this;
	name_ = name;
//	fprintf(stderr, "Adding ROM patch to Simulator function:   %3d = %s\n", (int)nPatch, name);
	value_ |= addPatch(this);
}

// -------------------------------------------------------------------------- //
//  * Destructor
// -------------------------------------------------------------------------- //
TJITGenericROMPatch::~TJITGenericROMPatch() { 
}


// -------------------------------------------------------------------------- //
//  * Return the native stub in Simulator
// -------------------------------------------------------------------------- //
JITFuncPtr TJITGenericROMPatch::GetSimulatorStubAt(KUInt32 index) 
{
    index = index & 0x003fffff;
    if (index>=nPatch)
        return 0L;
    return patch_[index]->stub_;
}

// -------------------------------------------------------------------------- //
//  * Return the address of the Simulator function
// -------------------------------------------------------------------------- //
AnyFunctionPtr TJITGenericROMPatch::GetSimulatorFunctionAt(KUInt32 index)
{
    index = index & 0x003fffff;
    if (index>=nPatch)
        return 0L;
    return patch_[index]->function_;
}

// -------------------------------------------------------------------------- //
//  * Return the address of the Simulator function
// -------------------------------------------------------------------------- //
AnyMethodPtr TJITGenericROMPatch::GetSimulatorMethodAt(KUInt32 index) 
{
    index = index & 0x003fffff;
    if (index>=nPatch)
        return 0L;
    return patch_[index]->method_;
}

// -------------------------------------------------------------------------- //
//  * Return the original instruction for this patch
// -------------------------------------------------------------------------- //
KUInt32 TJITGenericROMPatch::GetOriginalInstructionAt(KUInt32 command, KUInt32 address)
{
	// extract the patch index from the command (this assumes that we already
	// know that this is a high-digit SWI)
    KUInt32 index = command & 0x001fffff;
	
	// check if the index is in range
    if (index>=nPatch)
        return command;
	
	// if the address of the pacth is known, verify that we have the correct patch
	if (address!=0xFFFFFFFF) {
		if (patch_[index]->address_ != (address>>2))
			return command;
	}
	
	// return the word that we patched out
    return patch_[index]->originalInstruction_;
}

// -------------------------------------------------------------------------- //
//  * Return the name of the Simulator function
// -------------------------------------------------------------------------- //
const char* TJITGenericROMPatch::GetNameAt(KUInt32 index)
{
    index = index & 0x003fffff;
    if (index>=nPatch)
        return 0L;
    return patch_[index]->name_;
}

// -------------------------------------------------------------------------- //
//  * Add another stub to the array of stubs and return the index
// -------------------------------------------------------------------------- //
KUInt32 TJITGenericROMPatch::addPatch(TJITGenericROMPatch *p)
{
	if (p==0)
		return 0;
    if (nPatch==NPatch) {
		if (NPatch) {
			NPatch *= 2;
			patch_ = (TJITGenericROMPatch**)realloc(patch_, NPatch*sizeof(TJITGenericROMPatch*));
		} else {
			NPatch = 64;
			patch_ = (TJITGenericROMPatch**)malloc(NPatch*sizeof(TJITGenericROMPatch*));
		}
    }
    patch_[nPatch++] = p;
    return (nPatch-1);
}

// -------------------------------------------------------------------------- //
//  * Apply the patch to the ROM words
// -------------------------------------------------------------------------- //
void TJITGenericROMPatch::apply(KUInt32 *ROM)
{
    ROM[address()] = value();
}

// -------------------------------------------------------------------------- //
//  * DoPatchROM(KUInt32*, const std::string&)
// -------------------------------------------------------------------------- //
void
TJITGenericROMPatch::DoPatchROM(KUInt32* inROMPtr, const std::string& inMachineName) {
	if (inMachineName == "717006") {
		// Iterate on patches.
		TJITGenericROMPatch *p;
		for (p=TJITGenericROMPatch::first(); p; p=p->next())
		{
			p->apply(inROMPtr);
		}
		
		fprintf(stderr, "%u/19300 patches added %.2f%%.\n",
				(unsigned int)GetNumPatches(), GetNumPatches()/193.00);
	}
}

// -------------------------------------------------------------------------- //
//  * Apply the injection patch to the ROM words
// -------------------------------------------------------------------------- //
void TJITGenericROMInjection::apply(KUInt32 *ROM)
{
    originalInstruction_ = ROM[address()];
    ROM[address()] = value() | 0xefc00000;
}

// -------------------------------------------------------------------------- //
//  * Apply the simulator injection patch to the ROM words
// -------------------------------------------------------------------------- //
void TJITGenericROMSimulatorInjection::apply(KUInt32 *ROM)
{
    originalInstruction_ = ROM[address()];
    ROM[address()] = value() | 0xefa00000;
}


#endif
