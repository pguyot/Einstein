// ==============================
// File:			UJITGenericRetargetSupport.cp
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

#include "UJITGenericRetargetSupport.h"

#include "Emulator/TARMProcessor.h"
#include "Emulator/TMemory.h"
#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"
#include "Monitor/TSymbolList.h"


#define HANDLE_DATA_ABORT_LOCALLY 0

/**
 * Read emulated memory from a virtual address.
 * This function handles all exceptions that may happen during this process.
 * The function may invoke the scheduler and switch tasks.
 */
KUInt32
UJITGenericRetargetSupport::ManagedMemoryReadAligned(TARMProcessor* ioCPU, KUInt32 inAddress)
{
	KUInt32 theData;
	TMemory *theMemoryInterface = ioCPU->GetMemory();
#if HANDLE_DATA_ABORT_LOCALLY
	while (theMemoryInterface->ReadAligned(inAddress, theData)) {
		//ManagedDataAbort();
		fprintf(stderr, "PANIC: memory access failed in simulation\n");
	}
#else
	if (theMemoryInterface->ReadAligned(inAddress, theData)) {
		ioCPU->DataAbort();
		throw "DataAbort";
	}
#endif
	return theData;
}


/**
 * Read emulated memory from a virtual address.
 * This function handles all exceptions that may happen during this process.
 * The function may invoke the scheduler and switch tasks.
 */
KUInt32
UJITGenericRetargetSupport::ManagedMemoryRead(TARMProcessor* ioCPU, KUInt32 inAddress)
{
	KUInt32 theData;
	TMemory *theMemoryInterface = ioCPU->GetMemory();
#if HANDLE_DATA_ABORT_LOCALLY
	while (theMemoryInterface->Read(inAddress, theData)) {
		//ManagedDataAbort();
		fprintf(stderr, "PANIC: memory access failed in simulation\n");
	}
#else
	if (theMemoryInterface->Read(inAddress, theData)) {
		ioCPU->DataAbort();
		throw "DataAbort";
	}
#endif
	return theData;
}


/**
 * Read emulated memory from a virtual address.
 * This function handles all exceptions that may happen during this process.
 * The function may invoke the scheduler and switch tasks.
 */
KUInt8
UJITGenericRetargetSupport::ManagedMemoryReadB(TARMProcessor* ioCPU, KUInt32 inAddress)
{
	KUInt8 theData;
	TMemory *theMemoryInterface = ioCPU->GetMemory();
#if HANDLE_DATA_ABORT_LOCALLY
	while (theMemoryInterface->ReadB(inAddress, theData)) {
		//ManagedDataAbort();
		fprintf(stderr, "PANIC: memory access failed in simulation\n");
	}
#else
	if (theMemoryInterface->ReadB(inAddress, theData)) {
		ioCPU->DataAbort();
		throw "DataAbort";
	}
#endif
	return theData;
}


/**
 * Write emulated memory at a virtual address.
 * This function handles all exceptions that may happen during this process.
 * The function may invoke the scheduler and switch tasks.
 */
void
UJITGenericRetargetSupport::ManagedMemoryWriteAligned(TARMProcessor* ioCPU, KUInt32 inAddress, KUInt32 inData)
{
	TMemory *theMemoryInterface = ioCPU->GetMemory();
#if HANDLE_DATA_ABORT_LOCALLY
	while (theMemoryInterface->WriteAligned(inAddress, inData)) {
		//ManagedDataAbort();
		fprintf(stderr, "PANIC: memory access failed in simulation\n");
	}
#else
	if (theMemoryInterface->WriteAligned(inAddress, inData)) {
		ioCPU->DataAbort();
		throw "DataAbort";
	}
#endif
}


/**
 * Write emulated memory at a virtual address.
 * This function handles all exceptions that may happen during this process.
 * The function may invoke the scheduler and switch tasks.
 */
void
UJITGenericRetargetSupport::ManagedMemoryWrite(TARMProcessor* ioCPU, KUInt32 inAddress, KUInt32 inData)
{
	TMemory *theMemoryInterface = ioCPU->GetMemory();
#if HANDLE_DATA_ABORT_LOCALLY
	while (theMemoryInterface->Write(inAddress, inData)) {
		//ManagedDataAbort();
		fprintf(stderr, "PANIC: memory access failed in simulation\n");
	}
#else
	if (theMemoryInterface->Write(inAddress, inData)) {
		ioCPU->DataAbort();
		throw "DataAbort";
	}
#endif
}


/**
 * Write emulated memory at a virtual address.
 * This function handles all exceptions that may happen during this process.
 * The function may invoke the scheduler and switch tasks.
 */
void
UJITGenericRetargetSupport::ManagedMemoryWriteB(TARMProcessor* ioCPU, KUInt32 inAddress, KUInt8 inData)
{
	TMemory *theMemoryInterface = ioCPU->GetMemory();
#if HANDLE_DATA_ABORT_LOCALLY
	while (theMemoryInterface->WriteB(inAddress, inData)) {
		//ManagedDataAbort();
		fprintf(stderr, "PANIC: memory access failed in simulation\n");
	}
#else
	if (theMemoryInterface->WriteB(inAddress, inData)) {
		ioCPU->DataAbort();
		throw "DataAbort";
	}
#endif
}


/**
 * Throw an exception if the simulation receives an unexpected program counter.
 */
void
UJITGenericRetargetSupport::UnexpectedPC(TARMProcessor* ioCPU)
{
	throw "UnexpectedPC";
}


/**
 * Throw an exception to return from simulation back to emulation.
 */
void
UJITGenericRetargetSupport::ReturnToEmulator(TARMProcessor* ioCPU, KUInt32 inPC)
{
	ioCPU->SetRegister(TARMProcessor::kR15, inPC+4);
	throw "ReturnToEmulator";
}

/**
 * Look up the given PC in the list of simulated functions. If the function
 * exist, stay in the simulation and call it. If it doesn't exist, fall back
 * into emulation.
 */
void
UJITGenericRetargetSupport::JumpToCalculatedAddress(TARMProcessor* ioCPU, KUInt32 inPC, KUInt32 ret)
{
	static KUInt32 cnt = 0;
	// FIXME: implement the lookup for known functions
	ioCPU->SetRegister(TARMProcessor::kR15, inPC);
	if (inPC>=0x01000000) {
		KUInt32 instruction = ManagedMemoryRead(ioCPU, inPC-4);
		if ((instruction & 0xff000000)==0xea000000) {
			KUInt32 offset = (instruction & 0x007FFFFF) << 2;
			if (instruction & 0x00800000) {
				offset |= 0xFE000000;
			}
			KUInt32 delta = offset + 8;
			inPC = inPC + delta;
		}
	}
	if (inPC<0x01000000) {
		KUInt32 instruction = ManagedMemoryRead(ioCPU, inPC-4);
		if ((instruction & 0xffe00000)==0xefc00000) {
			KUInt32 simIndex = instruction & 0x001fffff;
			JITSimPtr nativeTarget = (JITSimPtr)TJITGenericROMPatch::GetSimulatorFunctionAt(simIndex);
			if (nativeTarget) {
				ioCPU->SetRegister(TARMProcessor::kR15, inPC);
				return nativeTarget(ioCPU, ret);
			} else {
				printf("SIM_INFO(%d): no native function registered at 0x%08lX\n", (unsigned int)++cnt, (unsigned long)inPC);
			}
		} else {
			// FIXME: remove some noise
			if (inPC<0x00800000 || inPC>0x008fffff) {
				printf("SIM_INFO: not a native target at 0x%08X, pc=0x%08X, lr=0x%08X\n", (unsigned int)inPC-4, (unsigned int)ioCPU->mCurrentRegisters[TARMProcessor::kR14], (unsigned int)ioCPU->mCurrentRegisters[TARMProcessor::kR15]);
				char sym[1024];
				if (TSymbolList::List->GetSymbolExact(inPC-4, sym)) {
					printf("SIM_INFO(%d): try: rt cjit %s\n", (unsigned int)++cnt, sym);
				} else {
					printf("SIM_INFO(%d): try: rt cjit %08lX-%08lX vt_%08lX\n", (unsigned int)++cnt, (unsigned long)inPC-4, (unsigned long)inPC, (unsigned long)inPC-4);
				}
			}
		}
	} else {
		printf("SIM_INFO(%ld): target outside of ROM at 0x%08lX\n", (unsigned long)++cnt, (unsigned long)inPC);
	}
	throw "JumpToCalculatedAddress";
}

#endif
