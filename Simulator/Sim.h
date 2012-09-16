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

#endif
// SIMULATOR_SIM_H

