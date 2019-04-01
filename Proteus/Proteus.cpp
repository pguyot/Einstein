// ==============================
// File:			Proteus.cpp
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
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


#include "Proteus.h"

// --- general includes

#include "TARMProcessor.h"
#include "TMemory.h"
#include "TJITGeneric_Macros.h"
#include "TJITGenericROMPatch.h"

// --- proteus variables

TARMProcessor *CPU = nullptr;
TMemory *MEM = nullptr;

// --- macros

/** Accessing the Registers */
#define R0 CPU->mCurrentRegisters[0]
#define R1 CPU->mCurrentRegisters[1]
#define R2 CPU->mCurrentRegisters[2]
#define R3 CPU->mCurrentRegisters[3]
#define R4 CPU->mCurrentRegisters[4]
#define R5 CPU->mCurrentRegisters[5]
#define R6 CPU->mCurrentRegisters[6]
#define R7 CPU->mCurrentRegisters[7]
#define R8 CPU->mCurrentRegisters[8]
#define R9 CPU->mCurrentRegisters[9]
#define R10 CPU->mCurrentRegisters[10]
#define R11 CPU->mCurrentRegisters[11]
#define R12 CPU->mCurrentRegisters[12]
#define R13 CPU->mCurrentRegisters[13]
#define SP CPU->mCurrentRegisters[13]
#define R14 CPU->mCurrentRegisters[14]
#define LR CPU->mCurrentRegisters[14]
#define R15 CPU->mCurrentRegisters[15]
#define PC CPU->mCurrentRegisters[15]

/** Push a value onto the stack (stmdb) */
#define PUSH(sp, val) { sp-=4; MEM->Write(sp, val); } 

/** Pop a value from the stack (ldmia) */
inline KUInt32 POP(KUInt32 &sp) { KUInt32 w; MEM->Read(sp, w); sp+=4; } 

/** Read a word from memory */
inline KUInt32 PEEK_W(KUInt32 addr) { KUInt32 w; MEM->Read(addr, w); return w; }

/** Write a word to memory */
#define POKE_W(addr, w) { MEM->Write(addr, w); }

/** Macro to define a getter for a global variable.
 \param addr address of globale variable in RAM
 \param type type of that variable
 \param name name of the variable without the leading 'g'
 */
#define GLOBAL_GET_W(addr, type, name) \
    const TMemory::VAddr g##name = addr; \
	type GetG##name() { KUInt32 w; MEM->Read(g##name, w); return (type)w; }


// --- class predeclarations


// --- typedefs


// --- function declaraitions

/** TODO: write me */
void DoDeferrals();
// Dependency tree:
//   _EnterAtomicFast
//   _ExitAtomicFast
//     DoSchedulerSWI (loops!)
//   PortDeferredSendNotify__Fv
//     (many?)
//   DeferredNotify__Fv
//     (many?)
//   DoDeferral__18TExtPageTrackerMgrFv
//     (many?)

// --- classes


// --- globals

//-/* 0x0C008400-0x0C107E18 */ Range of global variables in memory
// /* 0x0C008400-0x0C100E58 */

/** Return the number of nestings into the Fast Interrupt */
GLOBAL_GET_W(0x0C100E58, KSInt32, AtomicFIQNestCountFast);

/** Return the number of nestings into the Regular Interrupt */
GLOBAL_GET_W(0x0C100E5C, KSInt32, AtomicIRQNestCountFast);

// /* 0x0C100E60-0x0C100FE4 */

/** TODO: write docs (this is a booloean?!) */
GLOBAL_GET_W(0x0C100FE4, KUInt32, Schedule);

/** Return the number of nestings into the Atomic function */
GLOBAL_GET_W(0x0C100FE8, KSInt32, AtomicNestCount);

// /* 0x0C100FEC-0x0C100FF0 */

/** Return the number of nestings into the Fast Interrupt */
GLOBAL_GET_W(0x0C100FF0, KSInt32, AtomicFIQNestCount);

// /* 0x0C100FF4-0x0C101028 */

/** TODO: write docs (this is a booloean?!) */
GLOBAL_GET_W(0x0C101028, KUInt32, WantDeferred);

// /* 0x0C10102C-0x0C107E18 */

// --- ROM

//-/* 0x00000000-0x0071FC4C */ NewtonOS in ROM

/*
 * This injection initializes the Proteus system by setting up a few
 * variables for fast acces in later calls.
 */
T_ROM_INJECTION(0x00000000, "Initialize Proteus") {
	CPU = ioCPU;
	MEM = ioCPU->GetMemory();
	return ioUnit;
}

// /* 0x00000000-0x0014829C */
// TODO: implememt this to finish SWIs
// /* 0x00148298-0x003AD698 */

/**
 * Handle all operations that need to be executet in supervisor mode.
 *
 * This method is called from any code, priviledged or not, via the \c swi call
 * to run low level OS functions that need run in supervisor mode.
 *
 * After running the operation, every SWI call ends in the scheduler, allowing 
 * NewtonOS to switch to another task if needed. With Proteus running NewtonOS
 * in native mode inside the Einstein emulator, it is essential that task switching
 * is reimplemented so that we can switch between native and emulated tasks.
 */
T_ROM_INJECTION(0x003AD698, "SWIBoot")
{
    PUSH(SP, R1);
    PUSH(SP, R0);
    KUInt32 cmd = PEEK_W(LR-4);
    // Check if we were called through the SWI instruction
    if ( (cmd&0x0F000000) != 0x0F000000 ) {
        // FIXME:
        R0 = cmd;
        R1 = cmd & 0x0F000000;
        R15 = 0x003ADD70+4;
        return nullptr;
        //goto L003ADD70; // FIXME:
    }
    // Check if this was a conditional call
// /* 0x003AD6AC-0x003AD6B0 */
    if ( (cmd&0xF0000000) != 0xE0000000 ) {
        // FIXME:
        R0 = cmd;
        R1 = cmd & 0xF0000000;
        R15 = 0x003ADD7C+4;
        return nullptr;
        //goto L003ADD7C; // FIXME:
    }
// /* 0x003AD6B8-0x003AD6BC */
    R1 = CPU->GetSPSR();
// /* 0x003AD6B8-0x003AD6BC */
    R1 = R1 & 0x1f;
// /* 0x003AD6BC-0x003AD6CC */
    if (R1!=0x10 && R1!=0x00)
        throw "SWI from non-user mode (rebooting)";
    // FIXME:
    R0 = cmd;
    R15 = 0x003AD6CC+4;
    return nullptr;
}
// /* 0x003AD6CC-0x003AD740 */
    // r1 contains the ID of the desired swi function
// /* 0x003AD740-0x003AD744 */
    // if the ID is out of range, the OS throws a System Panic. 
// /* 0x003AD744-0x003AD74C */
    // use the function id to index into a large jump table at 0x003AD56C
// /* 0x003AD748-0x003AD750 */
    // All functions called in the dispatch above will eventually end here, in the scheduler.
    // The scheduler will check if it is time to run another thread in the multithreaded 
    // system. Thread changes are usually requested by external events or by the timer.
    // This is called preemptive multitasking. If a task does not call any SWIs for a long 
    // time, all other tasks in teh system are blocked and won;t run.
    //
    // NewtonOS has one very special event that can (and must) trigger a task change: if a
    // user task reads or writes an address that was marked by the Memory Managemnet Unit (MMU),
    // an interrupt is called that may allocate more memory to the location that was read.
    // Since allocating memory may trigger major reorganizetions, Newt puts the current
    // task to sleep and switches to teh memory management task. Only if that task is succesful,
    // the original task will be rescheduled to try the same memory access again.
    //
    // Until memory management and task switching is fully implemented, there is no use in
    // implementing any user code. Otherwies, user code could trigger a memory exeception
    // at any time and crash teh system if not handled correctly.
    //
    // Start of SWI Scheduler
T_ROM_INJECTION(0x003AD750, "SWI_Scheduler")
{
L003AD750:
    PUSH(SP, R1);
    PUSH(SP, R0);

    // bit7=IRQ, bit6=FIQ, bit0-4=mode (10=usr, 11=FIQ, 12=IRQ, 13=abt, 17=svc, 1b=und, 1f=system)
    CPU->SetCPSR( (CPU->GetCPSR()&~0xff) | 0xD3 ); // Disable interrupts, abt mode
    if (   GetGAtomicFIQNestCountFast()!=0
        || GetGAtomicIRQNestCountFast()!=0
        || GetGAtomicNestCount()!=0
        || GetGAtomicFIQNestCount()!=0)
        goto L003ADA74;
    
    // this SWI is not a nested call
    // bit7=IRQ, bit6=FIQ, bit0-4=mode (10=usr, 11=FIQ, 12=IRQ, 13=abt, 17=svc, 1b=und, 1f=system)
    CPU->SetCPSR( (CPU->GetCPSR()&~0xff) | 0x93 ); // Enable FIQ, keep IRQ disabled
    if (GetGWantDeferred()!=0)
        goto L003AD7C8;

    if (GetGSchedule()==0)
        goto L003ADA74;

L003AD7C0:
    PUSH(SP, LR);
    PUSH(SP, R12);
    PUSH(SP, R11);
    PUSH(SP, R10);
    PUSH(SP, R3);
    PUSH(SP, R2);
    goto L003AD7F8;

L003AD7C8:
    PUSH(SP, LR);
    PUSH(SP, R12);
    PUSH(SP, R11);
    PUSH(SP, R10);
    PUSH(SP, R3);
    PUSH(SP, R2);
    // bit7=IRQ, bit6=FIQ, bit0-4=mode (10=usr, 11=FIQ, 12=IRQ, 13=abt, 17=svc, 1b=und, 1f=system)
    CPU->SetCPSR( (CPU->GetCPSR()&~0xff) | 0x13 ); // Enable all interrupts

// /* 0x003AD7D8-0x003AD7DC */
    // HERE: DoDeferrals();

    // bail out at this point
    R15 = 0x003AD7D8+4;
    return nullptr;

L003AD7DC:
// /* 0x003AD7DC-0x003AD7F8 */

L003AD7F8:
// /* 0x003AD7F8-0x003AD7FC */
    // bail out at this point
    R15 = 0x003AD7F8+4;
    return nullptr;

// /* 0x003AD7FC-0x003ADA74 */

L003ADA74:
    // this SWI is somehow nested into an atomic call or an interrupt
    // bail out at this point
    R15 = 0x003ADA74+4;
    return nullptr;
}

// /* 0x003ADA74-0x003ADBB4 */
    // End of SWI scheduler??

// /* 0x003ADBB4-0x0071FC4C */

// --- RExBlock

//-/* 0x0071FC4C-0x007EE048 */ NewtonOS ROM Extensions
// /* 0x0071FC4C-0x007EE048 */

// --- Jump Table (not used here)

//-/* 0x01a00000-0x01c1085C */

// ============================================================================== //
// I've ended up calling my cat Brexit. It wakes me up meowing like crazy every
// morning because it wants to go out, but as soon as I open the door, it just
// sits there undecided and then looks angry when I put it outside.
//   -- French Europe Minister Nathalie Loiseau
// ============================================================================== //
