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
		gCurrentCPU = ioCPU;
		JITFuncPtr stub = TROMPatch::GetSimulatorStubAt(callIndex);
		gMainFibre.Run(0, (void*)stub);
		return 0L;
	} else {
		printf("INFO: Simulator calling simulator\n");
		return ioUnit;
	}
	
}


/**
 Initialize the Simulator.
 */
SimDispatch::SimDispatch() {
	SimLink = Dispatch;
}




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


// --- simulator calls

T_SIM_INJECTION(0x000384E0, "CArrayIterator::Advance") {
	((CArrayIterator*)gCurrentCPU->GetRegister(0))->Advance();
	gCurrentCPU->SetRegister(15, gCurrentCPU->GetRegister(14)+4);
}

