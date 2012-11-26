// ==============================
// File:                        SNewt/Common/Glue.cp
// Project:                     Simulator
//
// Copyright 1999-2012 by Newton Research Group and others
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


#include "Newt/Common/Glue.h"

// Simulator
#include "Newt/UtilityClasses/CArrayIterator.h"
#include "Newt/Frames/TInterpreter.h"

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"
#include "JIT.h"
#include "TJITGeneric_Macros.h"
#include "TROMImage.h"

//#define NEWT_GLUE_VERBATIM
#define NEWT_GLUE_VERBATIM if (0)

NewtGlueFibre gMainFibre;
NewtGlueDispatch gSimDispatch;
NewtGlueFibre* gCurrentFibre = 0L;
TARMProcessor* gCurrentCPU = 0L;


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
JITUnit* NewtGlueDispatch::Dispatch(JITUnit* ioUnit, TARMProcessor* ioCPU, KUInt32 callIndex) {
	if (gMainFibre.IsStopped()) {
		//printf("INFO: Calling Simulator\n");
		gCurrentCPU = ioCPU;
		JITFuncPtr stub = TROMPatch::GetSimulatorStubAt(callIndex);
		KSInt32 ret = gMainFibre.Run(callIndex, (void*)stub);
		if (ret) {
			NEWT_GLUE_VERBATIM printf("INFO: Called Simulator, but changed my mind (%s)\n", TROMPatch::GetNameAt(callIndex));
			return ioUnit;
		} else {
			NEWT_GLUE_VERBATIM printf("INFO: Called Simulator (%s)\n", TROMPatch::GetNameAt(callIndex));
			return 0L;
		}
	} else {
		NEWT_GLUE_VERBATIM printf("INFO: Simulator calling simulator (%s)\n", TROMPatch::GetNameAt(callIndex));
		gMainFibre.pRecursions++;
		return ioUnit;
	}
	
}


/**
 Initialize the Simulator.
 */
NewtGlueDispatch::NewtGlueDispatch() {
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



KUInt32 NewtReadWord(KUInt32 addr)
{
	KUInt32 v;
	bool err = 0;
	do {
		err = gCurrentCPU->GetMemory()->Read(addr, v);
		if (err) {
			gCurrentCPU->mCurrentRegisters[TARMProcessor::kR15] = 0x007ffff0;
			gCurrentCPU->DataAbort();
			gMainFibre.Suspend(0);
		}
	} while (err);
	return v;
}


void NewtWriteWord(KUInt32 addr, KUInt32 v)
{
	bool err = 0;
	do {
		err = gCurrentCPU->GetMemory()->Write(addr, v);
		if (err) {
			gCurrentCPU->mCurrentRegisters[TARMProcessor::kR15] = 0x007ffff0;
			gCurrentCPU->DataAbort();
			gMainFibre.Suspend(0);
		}
	} while (err);
}


KUInt8 NewtReadByte(KUInt32 addr)
{
	KUInt8 v;
	bool err = 0;
	do {
		err = gCurrentCPU->GetMemory()->ReadB(addr, v);
		if (err) {
			gCurrentCPU->mCurrentRegisters[TARMProcessor::kR15] = 0x007ffff0;
			gCurrentCPU->DataAbort();
			gMainFibre.Suspend(0);
		}
	} while (err);
	return v;
}


void NewtWriteByte(KUInt32 addr, KUInt8 v)
{
	bool err = 0;
	do {
		err = gCurrentCPU->GetMemory()->WriteB(addr, v);
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
void NewtCallJIT(KUInt32 addr) {
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
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, r0);
	gCurrentCPU->SetRegister(1, r1);
	gCurrentCPU->SetRegister(2, r2);
	gCurrentCPU->SetRegister(3, r3);
	NewtCallJIT(0x003408D0);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}

KUInt32 SectRect(KUInt32 r0, KUInt32 r1, KUInt32 r2)
{
	KUInt32 ret = 0;
	ret = RSect(r2, 2, r0, r1);
	return ret;
}

// Uncomment the lines below to activate the JIT->Sim->JIT sequence.
//NEWT_INJECTION(0x00340D70, "SectRect(Rect,Rect,Rect)") {
//	printf("*** SectRect\n");
//	NEWT_RETVAL SectRect(NEWT_ARG0(KUInt32), NEWT_ARG1(KUInt32), NEWT_ARG2(KUInt32));
//	NEWT_RETURN;
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


//T_ROM_INJECTION(0x0038e2cc, "Read Double from memory") {
//	union {
//		struct { KUInt32 r0, r1; };
//		struct { KUInt8 b0, b1, b2, b3, b4, b5, b6, b7; };
//		double d;
//	} v;
//	v.r0 = ioCPU->GetRegister(2);
//	v.r1 = ioCPU->GetRegister(0);
//	printf("read double 0x%08lX 0x%08lX (%g)\n", ioCPU->GetRegister(0), ioCPU->GetRegister(2), v.d);
//	printf("%02X%02X%02X%02X%02X%02X%02X%02X\n", v.b7, v.b6, v.b5, v.b4, v.b3, v.b2, v.b1, v.b0);
//	printf("%d %d %g\n", v.b7>>7, ((v.b7&127)<<4)|(v.b6>>4), 0);
//	printf("read double 0x%08lX 0x%08lX (%g)\n", ioCPU->GetRegister(0), ioCPU->GetRegister(2), v.d);
//	return ioUnit;
//}
//
//T_ROM_INJECTION(0x0038e30c, "Write f0 to memory") {
//	printf("write f0 0x%08lX 0x%08lX 0x%08lX\n", ioCPU->GetRegister(0), ioCPU->GetRegister(1), ioCPU->GetRegister(2));
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


NewtGlueFibre::NewtGlueFibre()
:	TFibre()
{
	pRecursions = 0;
}


KSInt32 NewtGlueFibre::Task(KSInt32 inIndex, void* inUserData)
{
	pRecursions = 0;
	pTaskIndex = inIndex;
	pSP = gCurrentCPU->GetRegister(13);
	NewtGlueTask task = (NewtGlueTask)inUserData;
	KSInt32 ret = task();
	if (pRecursions) {
		NEWT_GLUE_VERBATIM printf("Performance Info: %ld recursion attempts during simulator call (%s)\n", pRecursions, TROMPatch::GetNameAt(inIndex));
		pRecursions = 0;
	}
	return ret;
}

#if 0

T_ROM_INJECTION(0x00394560, "longjmp")
{
	KUInt32 oldSP = ioCPU->GetRegister(13);
	KUInt32 newSP = NewtReadWord(ioCPU->GetRegister(0)+12*4);
	KUInt32 newPC = NewtReadWord(ioCPU->GetRegister(0)+13*4);
	NEWT_GLUE_VERBATIM printf("INFO: longmp called from 0x%08lX (old sp: 0x%08lX, new sp: 0x%08lX, d=%ld, new pc: 0x%08lX)\n",
		   ioCPU->GetRegister(14),
		   oldSP,
		   newSP,
		   oldSP-newSP,
		   newPC);
	if (gMainFibre.IsSuspended()) {
		NEWT_GLUE_VERBATIM fprintf(stderr, "INFO: Fibre SP is 0x%08lX\n", gMainFibre.pSP);
		if (oldSP<gMainFibre.pSP && newSP<gMainFibre.pSP)  // and same stack
			NEWT_GLUE_VERBATIM fprintf(stderr, "INFO: Fibre is not affected\n");
		if (oldSP>gMainFibre.pSP && newSP>gMainFibre.pSP) // and same stack
			NEWT_GLUE_VERBATIM fprintf(stderr, "ERROR: Fibre will not run again!\n");
	}
	NEWT_GLUE_VERBATIM fprintf(stderr, "INFO: trying to fix the issue. This is untested and may cause inconsistencies.\n");
	if (gMainFibre.IsSuspended() && oldSP<gMainFibre.pSP && newSP>gMainFibre.pSP) {
		NEWT_GLUE_VERBATIM fprintf(stderr, "ALERT: longjmp crosses Fibre stack at 0x%08lX, Fibres may no longer work (%s)!\n", gMainFibre.pSP, TROMPatch::GetNameAt(gMainFibre.pTaskIndex));
		NEWT_GLUE_VERBATIM fprintf(stderr, "INFO: trying to fix the issue. This is untested and may cause inconsistencies.\n");
		gMainFibre.Abort(0);
	}
	return ioUnit;
}

#endif

