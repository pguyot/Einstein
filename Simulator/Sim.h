// ==============================
// File:                        Simulator/Sim.h
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

#ifndef SIMULATOR_SIM_H
#define SIMULATOR_SIM_H

#include <K/Defines/KDefinitions.h>
#include <K/Threads/TFibre.h>


// Einstein
#include "JIT.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"


namespace Sim {

	extern class SFibre* gCurrentFibre;
	extern class TARMProcessor* gCurrentCPU;
	
	KUInt32 ReadWord(KUInt32);
	void WriteWord(KUInt32, KUInt32);
	
	KUInt8 ReadByte(KUInt32);
	void WriteByte(KUInt32, KUInt8);
	
	void CallJIT(KUInt32 addr);
	
	class SimDispatch
	{
	public:
		static JITUnit* Dispatch(JITUnit* ioUnit, TARMProcessor* ioCPU, KUInt32 callIndex);
		SimDispatch();
	};
	

} // namespace


#define T_SIM_INJECTION(addr, name) \
	extern void p##addr(); \
	TROMSimulatorInjection i##addr(addr, p##addr, name); \
	void p##addr()

#define SIM_GET_SET_W(type, name) \
	private: type f##name; \
	public:  type Get##name() { return (type)Sim::ReadWord( (KUInt32)&f##name ); } \
	         void Set##name(type v) { Sim::WriteWord( (KUInt32)&f##name, (KUInt32)v ); }

#define SIM_GET_SET_B(type, name) \
	private: type f##name; \
	public:  type Get##name() { return (type)Sim::ReadByte( (KUInt32)&f##name ); } \
	         void Set##name(type v) { Sim::WriteByte( (KUInt32)&f##name, (KUInt8)v ); }

#define SIM_RETURN \
	gCurrentCPU->SetRegister(15, gCurrentCPU->GetRegister(14)+4)

#define SIM_RETVAL \
	gCurrentCPU->mCurrentRegisters[0] = (KUInt32)

#define SIM_CLASS(type) \
	((type*)gCurrentCPU->GetRegister(0))

#define SIM_ARG0(type) \
	((type)gCurrentCPU->GetRegister(0))

#define SIM_ARG1(type) \
	((type)gCurrentCPU->GetRegister(1))

#define SIM_ARG2(type) \
	((type)gCurrentCPU->GetRegister(2))

#define SIM_ARG3(type) \
	((type)gCurrentCPU->GetRegister(3))

#define SIM_PUSH_REGISTERS \
	{ \
		KUInt32 _r0 = gCurrentCPU->GetRegister(0); \
		KUInt32 _r1 = gCurrentCPU->GetRegister(1); \
		KUInt32 _r2 = gCurrentCPU->GetRegister(2); \
		KUInt32 _r3 = gCurrentCPU->GetRegister(3); \
		KUInt32 _r4 = gCurrentCPU->GetRegister(4); \
		KUInt32 _r5 = gCurrentCPU->GetRegister(5); \
		KUInt32 _r6 = gCurrentCPU->GetRegister(6); \
		KUInt32 _r7 = gCurrentCPU->GetRegister(7); \
		KUInt32 _r8 = gCurrentCPU->GetRegister(8); \
		KUInt32 _r9 = gCurrentCPU->GetRegister(9); \
		KUInt32 _r10 = gCurrentCPU->GetRegister(10); \
		KUInt32 _r11 = gCurrentCPU->GetRegister(11); \
		KUInt32 _r12 = gCurrentCPU->GetRegister(12); \
		KUInt32 _r13 = gCurrentCPU->GetRegister(13); \
		KUInt32 _r14 = gCurrentCPU->GetRegister(14); \
		KUInt32 _r15 = gCurrentCPU->GetRegister(15);

#define SIM_POP_REGISTERS \
		gCurrentCPU->SetRegister(15, _r15); \
		gCurrentCPU->SetRegister(14, _r14); \
		gCurrentCPU->SetRegister(13, _r13); \
		gCurrentCPU->SetRegister(12, _r12); \
		gCurrentCPU->SetRegister(11, _r11); \
		gCurrentCPU->SetRegister(10, _r10); \
		gCurrentCPU->SetRegister(9, _r9); \
		gCurrentCPU->SetRegister(8, _r8); \
		gCurrentCPU->SetRegister(7, _r7); \
		gCurrentCPU->SetRegister(6, _r6); \
		gCurrentCPU->SetRegister(5, _r5); \
		gCurrentCPU->SetRegister(4, _r4); \
		gCurrentCPU->SetRegister(3, _r3); \
		gCurrentCPU->SetRegister(2, _r2); \
		gCurrentCPU->SetRegister(1, _r1); \
		gCurrentCPU->SetRegister(0, _r0); \
	}

#endif
// SIMULATOR_SIM_H

