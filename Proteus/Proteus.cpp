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

// --- macros

/** Macro to define a getter for a global variable.
 \param addr address of globale variable in RAM
 \param type type of that variable
 \param name name of the variable without the leading 'g'
 */
#define GLOBAL_GET_W(addr, type, name) \
    const TMemory::VAddr g##name = addr; \
	type GetG##name() { KUInt32 w; MEM->Read(g##name, w); return (type)w; }


// --- proteus variables

TARMProcessor *CPU = nullptr;
TMemory *MEM = nullptr;

// --- class predeclarations


// --- typedefs


// --- classes


// --- globals

//-/* 0x0C008400-0x0C107E18 */ Range of global variables in memory
// /* 0x0C008400-0x0C100E58 */

/** Return the number of nestings into the Fast Interrupt */
GLOBAL_GET_W(0x0C100E58, KSInt32, AtomicFIQNestCountFast);

/** Return the number of nestings into the Regular Interrupt */
GLOBAL_GET_W(0x0C100E5C, KSInt32, AtomicIRQNestCountFast);

// /* 0x0C100E60-0x0C100FEC */

/** Return the number of nestings into the Atomic function */
GLOBAL_GET_W(0x0C100FE8, KSInt32, AtomicNestCount);

// /* 0x0C100FEC-0x0C100FF4 */

/** Return the number of nestings into the Fast Interrupt */
GLOBAL_GET_W(0x0C100FF0, KSInt32, AtomicFIQNestCount);

// /* 0x0C100FF4-0x0C107E18 */

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

// /* 0x00000000-0x003AD69C */

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
// /* 0x003AD698-0x003AD69C */
    // The start of the SWI code finds the original swi instruction and extracts
    // the SWI function code for the desired call.
// /* 0x003AD69C-0x003AD740 */
    // r1 contains the ID of the desired swi function
// /* 0x003AD740-0x003AD744 */
    // if the ID is out of range, teh OS throws a System Panic. 
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
// /* 0x003AD750-0x003AD754 */

T_ROM_INJECTION(0x003AD754, "SWI_Scheduler")
{
// /* 0x003AD754-0x003AD758 */
	CPU->SetCPSR( CPU->GetCPSR() | 0xD3 ); // Disable interrupts
// /* 0x003AD760-0x003AD768 */
	CPU->mCurrentRegisters[1] = GetGAtomicFIQNestCountFast();
	// bail out at this point
	CPU->mCurrentRegisters[15] = 0x003AD768+4;
	return nullptr;
}
// /* 0x003AD768-0x003AD790 */
    // If this call is in any way nested (SWI is called from within an interrupt, an Atomin calle, etc.)
    // then skip the scheduler.
// /* 0x003AD790-0x003ADBB4 */
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
