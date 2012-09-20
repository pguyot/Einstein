// ==============================
// File:                        Simulator/Sim.cp
// Project:                     Simulator
//
// Copyright 2012 by Matthias Melcher (newton@matthiasm.com).
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


#include "Simulator/Sim.h"

// Simulator
#include "Simulator/SFibre.h"
#include "Simulator/UtilityClasses/CArrayIterator.h"
#include "Simulator/Frames/FastRunState.h"

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"
#include "JIT.h"
#include "TJITGeneric_Macros.h"
#include "TROMImage.h"


namespace Sim {
	SFibre gMainFibre;
	SimDispatch gSimDispatch;
	SFibre* gCurrentFibre = 0L;
	TARMProcessor* gCurrentCPU = 0L;
};


using namespace Sim;


/*
 This function pointer is pert of JIT. It will only be set if the Simulator
 is linked to Einstein. If the Simulator is not linked, the JIT compiler
 will still compile and run normally.
 */
extern JITUnit* (*SimLink)(JITUnit* ioUnit, TARMProcessor* ioCPU, KUInt32 ix);


/**
 If the Simulator is already busy, this function will return and continue JIT
 execution. If the Simulator is idle, this function switches context into the 
 Simulator fibre and calls the Stub that will then simulate a function by
 running the corresponding native code.
 */
JITUnit* SimDispatch::Dispatch(JITUnit* ioUnit, TARMProcessor* ioCPU, KUInt32 callIndex) {
	if (gMainFibre.IsStopped()) {
		//printf("INFO: Calling Simulator\n");
		gCurrentCPU = ioCPU;
		JITFuncPtr stub = TROMPatch::GetSimulatorStubAt(callIndex);
		gMainFibre.Run(0, (void*)stub);
		return 0L;
	} else {
		printf("INFO: Simulator calling simulator\n");
		gMainFibre.pRecursions++;
		return ioUnit;
	}
	
}


/**
 Initialize the Simulator.
 */
SimDispatch::SimDispatch() {
	SimLink = Dispatch;
}


// -- memory handling


//#include <new>
//#include <iostream>
//#include <cstdlib>
//#include <stdexcept>
//#include <locale>
//
//void* operator new(std::size_t sz) throw (std::bad_alloc)
//{
//    void *result = std::malloc (sz == 0 ? 1 : sz);
//    if (result == NULL)
//        throw std::bad_alloc();
//    gNewCounter++;
//    return result;
//}
//void operator delete(void* p) throw()
//{
//    if (p == NULL)
//        return;
//    std::free (p);
//    gDeleteCounter++;
//}
//
//void* operator new(std::size_t sz, const std::nothrow_t&) throw()
//{
//    try {
//        void * result = ::operator new (sz);  // calls our overridden operator new
//        return result;
//    } catch (std::bad_alloc &) {
//		return NULL;
//    }
//}
//
//void operator delete(void* p, const std::nothrow_t&) throw()
//{
//    ::operator delete (p);
//}



KUInt32 Sim::ReadWord(KUInt32 addr)
{
	KUInt32 v;
	bool err = 0;
	do {
		err = Sim::gCurrentCPU->GetMemory()->Read(addr, v);
		if (err) {
			gCurrentCPU->mCurrentRegisters[TARMProcessor::kR15] = 0x007ffff0;
			gCurrentCPU->DataAbort();
			gMainFibre.Suspend(0);
		}
	} while (err);
	return v;
}


void Sim::WriteWord(KUInt32 addr, KUInt32 v)
{
	bool err = 0;
	do {
		err = Sim::gCurrentCPU->GetMemory()->Write(addr, v);
		if (err) {
			gCurrentCPU->mCurrentRegisters[TARMProcessor::kR15] = 0x007ffff0;
			gCurrentCPU->DataAbort();
			gMainFibre.Suspend(0);
		}
	} while (err);
}


KUInt8 Sim::ReadByte(KUInt32 addr)
{
	KUInt8 v;
	bool err = 0;
	do {
		err = Sim::gCurrentCPU->GetMemory()->ReadB(addr, v);
		if (err) {
			gCurrentCPU->mCurrentRegisters[TARMProcessor::kR15] = 0x007ffff0;
			gCurrentCPU->DataAbort();
			gMainFibre.Suspend(0);
		}
	} while (err);
	return v;
}


void Sim::WriteByte(KUInt32 addr, KUInt8 v)
{
	bool err = 0;
	do {
		err = Sim::gCurrentCPU->GetMemory()->WriteB(addr, v);
		if (err) {
			gCurrentCPU->mCurrentRegisters[TARMProcessor::kR15] = 0x007ffff0;
			gCurrentCPU->DataAbort();
			gMainFibre.Suspend(0);
		}
	} while (err);
}


// --- Resume patch

// - This is called by the JIT to resume the Simulator after an interrupt or exception
T_ROM_INJECTION(0x007ffff0, "Resume Simulator") {
	gCurrentCPU = ioCPU;
	gMainFibre.Resume(0);
	return 0;
}


// --- Suspend Simulator

// - This is used to switch context from the Simulator to JIT when an exception occurs
void Suspend() {
	gCurrentCPU->SetRegister(15, 0x007ffff0);
	gMainFibre.Suspend(0);
	return;
}

// - When in the Simulator, call the JIT compiler for yet unimplemented functions. Use with care!
void Sim::CallJIT(KUInt32 addr) {
	gCurrentCPU->SetRegister(15, addr+4);		// Call this function
	gCurrentCPU->SetRegister(14, 0x007ffff0);	// Return to the Simulator afterwards
	gMainFibre.Suspend(0);
	return;
}


// --- testing ping pong

/*
 This is an example that show how to use the simulator to call a function using
 the JIT compiler.
 */
KUInt32 RSect(KUInt32 r0, KUInt32 r1, KUInt32 r2, KUInt32 r3)
{
	KUInt32 ret;
	SIM_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, r0);
	gCurrentCPU->SetRegister(1, r1);
	gCurrentCPU->SetRegister(2, r2);
	gCurrentCPU->SetRegister(3, r3);
	Sim::CallJIT(0x003408D0);
	ret = gCurrentCPU->GetRegister(0);
	SIM_POP_REGISTERS
	return ret;
}

KUInt32 SectRect(KUInt32 r0, KUInt32 r1, KUInt32 r2)
{
	KUInt32 ret = 0;
	ret = RSect(r2, 2, r0, r1);
	return ret;
}

// Uncomment the lines below to activate the JIT->Sim->JIT sequence.
//T_SIM_INJECTION(0x00340D70, "SectRect(Rect,Rect,Rect)") {
//	printf("*** SectRect\n");
//	SIM_RETVAL SectRect(SIM_ARG0(KUInt32), SIM_ARG1(KUInt32), SIM_ARG2(KUInt32));
//	SIM_RETURN;
//}



// --- simulator calls




//T_ROM_INJECTION(0x002ECD94, "FastFreqFuncGeneral(FastRunState*, long)") {
//	printf("*** FastFreqFuncGeneral(FastRunState*, long)\n");
//	return ioUnit;
//}

//T_ROM_INJECTION(0x002EE138, "FastRun") {
//	printf("*** Fast Run\n");
//	return ioUnit;
//}
//
//T_ROM_INJECTION(0x002F1EE0, "SlowRun") {
//	printf("*** Slow Run\n");
//	return ioUnit;
//}



/*
 
 Next task: implement TInterpreter::FastRun1(...)
	0x002EE138 to 0x002EF140 (1026 lines of code)
 
 Jump table for byte code starts at 0x002EE1E8 and is 207 entries big.
 
 There is also a slow version. Is that used when debugging?
	0x002F1EE0: t_unknown TInterpreter::SlowRun(...)
 
 Jump table at: 0x002F2034
 
 
 Look at 0x002ECD94: t_unknown FastFreqFuncGeneral(...)
 
 */

