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

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"
#include "JIT.h"
#include "TJITGeneric_Macros.h"
#include "TROMImage.h"


using namespace Sim;


SFibre* Sim::gCurrentFibre;
TARMProcessor* Sim::gCurrentCPU = 0L;
Sim::SFibre gMainFibre;

void Init()
{
}


JITInstructionProto(Dispatch)
{
  return 0L;
}


#define T_SIM_INJECTION(addr, name) \
extern JITInstructionProto(p##addr); \
TROMInjection i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr)


T_SIM_INJECTION(0x00000000, "RESET") {
    fprintf(stderr, "RESET called with r0=0x%08x\n", ioCPU->mCurrentRegisters[TARMProcessor::kR0]);
    return ioUnit;
}


//KUInt32 Astrlen(char *str)
//{
//	char *s = str;
//	for (;;) {
//		KUInt8 b;
//		gCurrentCPU->GetMemory()->ReadB( (KUInt32)str, b );
//		if (b==0) break;
//		s++;
//	}
//	return s-str;
//}
//
//T_SIM_INJECTION(0x0002E6E0, "Astrlen") {
//	gCurrentCPU = ioCPU;
//	ioCPU->SetRegister(0, Astrlen( (char*)ioCPU->GetRegister(0) ) );
//	return 0L;
//}


// --- Resume patch

// - This is called by the JIT to resume the Simulator after an interrupt or exception
T_ROM_INJECTION(0x007ffff0, "Resume Simulator") {
	gCurrentCPU = ioCPU;
	gMainFibre.Resume(0);
    return 0;
}


// --- Suspend Simulator

// - This is used to switch context from the Simulator to JIT when an exception occurs
/*
	if (theMemoryInterface->WritePAligned( theAddress, ioCPU->mCurrentRegisters[indexReg] ))
	{
		SETPC(GETPC());
		ioCPU->DataAbort();
		MMUCALLNEXT_AFTERSETPC;
	}
*/
void Suspend() {
	gCurrentCPU->SetRegister(15, 0x007ffff0);
	gMainFibre.Suspend(0);
	return;
}


// --- sample simulator call, all spelled out

// - this code simulates "void CArrayIterator::Advance(void)"
void CArrayIterator_Advance(void* This)
{
	KUInt8 tmp1; // flag byte, incrmenting iterator if != 0
	KUInt32 tmp3, tmp4;

	::Suspend();

	gCurrentCPU->GetMemory()->Read( ((KUInt32)This)+4, tmp3 ); // current index
	gCurrentCPU->GetMemory()->ReadB( ((KUInt32)This)+16, tmp1 );
	if (tmp1!=0) {
		gCurrentCPU->GetMemory()->Read( ((KUInt32)This)+12, tmp4 ); // max index
		if (tmp3<tmp4) {
			tmp3++;
			gCurrentCPU->GetMemory()->Write( ((KUInt32)This)+4, tmp3 );
			return;
		}
	} else {
		gCurrentCPU->GetMemory()->Read( ((KUInt32)This)+8, tmp4 ); // min index
		if (tmp3>tmp4) {
			tmp3--;
			gCurrentCPU->GetMemory()->Write( ((KUInt32)This)+4, tmp3 );
			return;
		}
	}
	gCurrentCPU->GetMemory()->Write( ((KUInt32)This)+4, -1 ); // invalid index
	return;
}

// - This stub connects the Fibre to the Simulator
// The Stub is needed for each function call
void CArrayIterator_Advance_Stub() {
	CArrayIterator_Advance( (void*)gCurrentCPU->GetRegister(0) );
	gCurrentCPU->SetRegister(15, gCurrentCPU->GetRegister(14)+4);
}

// - This function connects the JIT to the Simulator
// This function is need only once, but it must dispatch all calls
T_SIM_INJECTION(0x000384E0, "CArrayIterator::Advance") {
	if (gMainFibre.IsStopped()) {
		gCurrentCPU = ioCPU;
		KSInt32 ret = gMainFibre.Run(0, (void*)CArrayIterator_Advance_Stub);
		//while (ret) {
		//	ret = gMainFibre.Resume(0);
		//}
		return 0L;
	} else {
		printf("INFO: Simulator calling simulator\n");
		return ioUnit;
	}
}

//0x000384E0: t_unknown CArrayIterator::Advance(...)
/*
 Advance__14CArrayIteratorFv:                    @ 0x000384E0: t_unknown CArrayIterator::Advance(...)
 @ label = 'Advance__14CArrayIteratorFv'
 @ ARM R0 = type: 'CArrayIterator'*
 @ name = 'Advance'
 @ class = 'CArrayIterator'
 ldrb    r1, [r0, #16]                   @ 0x000384E0 0xE5D01010 - ....
 mvn     r2, #0                          @ 0x000384E4 0xE3E02000 - ....
 teq     r1, #0                          @ 0x000384E8 0xE3310000 - .1..
 ldr     r1, [r0, #4]                    @ 0x000384EC 0xE5901004 - ....
 beq     L00038508                       @ 0x000384F0 0x0A000004 - ....
 ldr     r3, [r0, #12]                   @ 0x000384F4 0xE590300C - ..0.
 cmp     r1, r3                          @ 0x000384F8 0xE1510003 - .Q..
 addlt   r1, r1, #1                      @ 0x000384FC 0xB2811001 - ....
 blt     L00038518                       @ 0x00038500 0xBA000004 - ....
 b       L00038520                       @ 0x00038504 0xEA000005 - ....
 L00038508:
 ldr     r3, [r0, #8]                    @ 0x00038508 0xE5903008 - ..0.
 cmp     r1, r3                          @ 0x0003850C 0xE1510003 - .Q..
 ble     L00038520                       @ 0x00038510 0xDA000002 - ....
 sub     r1, r1, #1                      @ 0x00038514 0xE2411001 - .A..
 L00038518:
 str     r1, [r0, #4]!                   @ 0x00038518 0xE5A01004 - ....
 mov     pc, lr                          @ 0x0003851C 0xE1A0F00E - ....
 L00038520:
 str     r2, [r0, #4]!                   @ 0x00038520 0xE5A02004 - ....
 mov     pc, lr                          @ 0x00038524 0xE1A0F00E - ....
*/
	
// stub(ioUnit, ioCPU);


