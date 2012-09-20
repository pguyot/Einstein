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

#include "Simulator/NewtonTypes.h"

// Einstein
#include "JIT.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"
#include "TROMImage.h"


namespace Sim {

	extern class SFibre* gCurrentFibre;
	extern class TARMProcessor* gCurrentCPU;
	
	KUInt32 ReadWord(KUInt32);
	inline KUInt32 ReadWord(void* p) { return ReadWord((KUInt32)p); }
	void WriteWord(KUInt32, KUInt32);
	inline void WriteWord(void* p, KUInt32 v) { WriteWord((KUInt32)p, v); }
	
	KUInt8 ReadByte(KUInt32);
	inline KUInt8 ReadByte(void* p) { return ReadByte((KUInt32)p); }
	void WriteByte(KUInt32, KUInt8);
	inline void WriteByte(void* p, KUInt8 v) { WriteByte((KUInt32)p, v); }
	
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

#define SIM_GET_SET_W_ARRAY(type, name, size) \
	private: type f##name[size]; \
	public:  type Get##name(KUInt32 ix) { return (type)Sim::ReadWord( ((KUInt32)&f##name)+4*ix ); } \
	         void Set##name(KUInt32 ix, type v) { Sim::WriteWord( ((KUInt32)&f##name)+4*ix, (KUInt32)v ); }

#define SIM_GET_SET_B(type, name) \
	private: type f##name; \
	public:  type Get##name() { return (type)Sim::ReadByte( (KUInt32)&f##name ); } \
	         void Set##name(type v) { Sim::WriteByte( (KUInt32)&f##name, (KUInt8)v ); }

#define SIM_RETURN \
	Sim::gCurrentCPU->SetRegister(15, Sim::gCurrentCPU->GetRegister(14)+4)

#define SIM_RETVAL \
	Sim::gCurrentCPU->mCurrentRegisters[0] = (KUInt32)

#define SIM_CLASS(type) \
	((type*)Sim::gCurrentCPU->GetRegister(0))

#define SIM_ARG0(type) \
	((type)Sim::gCurrentCPU->GetRegister(0))

#define SIM_ARG1(type) \
	((type)Sim::gCurrentCPU->GetRegister(1))

#define SIM_ARG2(type) \
	((type)Sim::gCurrentCPU->GetRegister(2))

#define SIM_ARG3(type) \
	((type)Sim::gCurrentCPU->GetRegister(3))

#define SIM_PUSH_REGISTERS \
	{ \
		KUInt32 _r0 = Sim::gCurrentCPU->GetRegister(0); \
		KUInt32 _r1 = Sim::gCurrentCPU->GetRegister(1); \
		KUInt32 _r2 = Sim::gCurrentCPU->GetRegister(2); \
		KUInt32 _r3 = Sim::gCurrentCPU->GetRegister(3); \
		KUInt32 _r4 = Sim::gCurrentCPU->GetRegister(4); \
		KUInt32 _r5 = Sim::gCurrentCPU->GetRegister(5); \
		KUInt32 _r6 = Sim::gCurrentCPU->GetRegister(6); \
		KUInt32 _r7 = Sim::gCurrentCPU->GetRegister(7); \
		KUInt32 _r8 = Sim::gCurrentCPU->GetRegister(8); \
		KUInt32 _r9 = Sim::gCurrentCPU->GetRegister(9); \
		KUInt32 _r10 = Sim::gCurrentCPU->GetRegister(10); \
		KUInt32 _r11 = Sim::gCurrentCPU->GetRegister(11); \
		KUInt32 _r12 = Sim::gCurrentCPU->GetRegister(12); \
		KUInt32 _r13 = Sim::gCurrentCPU->GetRegister(13); \
		KUInt32 _r14 = Sim::gCurrentCPU->GetRegister(14); \
		KUInt32 _r15 = Sim::gCurrentCPU->GetRegister(15);

#define SIM_POP_REGISTERS \
		Sim::gCurrentCPU->SetRegister(15, _r15); \
		Sim::gCurrentCPU->SetRegister(14, _r14); \
		Sim::gCurrentCPU->SetRegister(13, _r13); \
		Sim::gCurrentCPU->SetRegister(12, _r12); \
		Sim::gCurrentCPU->SetRegister(11, _r11); \
		Sim::gCurrentCPU->SetRegister(10, _r10); \
		Sim::gCurrentCPU->SetRegister(9, _r9); \
		Sim::gCurrentCPU->SetRegister(8, _r8); \
		Sim::gCurrentCPU->SetRegister(7, _r7); \
		Sim::gCurrentCPU->SetRegister(6, _r6); \
		Sim::gCurrentCPU->SetRegister(5, _r5); \
		Sim::gCurrentCPU->SetRegister(4, _r4); \
		Sim::gCurrentCPU->SetRegister(3, _r3); \
		Sim::gCurrentCPU->SetRegister(2, _r2); \
		Sim::gCurrentCPU->SetRegister(1, _r1); \
		Sim::gCurrentCPU->SetRegister(0, _r0); \
	}

#define SIM_GLOBAL_W_DECL(type, name, addr) \
	extern type g##name; \
	void GlobalSet##name(type); \
	type GlobalGet##name();

#define SIM_GLOBAL_W_IMPL(type, name, addr) \
	type g##name; \
	void GlobalSet##name(type v) { Sim::WriteWord(addr, (KUInt32)v); } \
	type GlobalGet##name() { return (type)Sim::ReadWord(addr); }


#endif
// SIMULATOR_SIM_H

