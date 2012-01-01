// ==============================
// File:                        sys/memory.h
// Project:                     Simulator
//
// Copyright 2003-2008 by Matthias Melcher (newton@matthiasm.com).
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

#ifndef SIMULATOR_SYS_MEMORY_H
#define SIMULATOR_SYS_MEMORY_H

#include <K/Defines/KDefinitions.h>
#include "Simulator/sys/types.h"
#include "Emulator/TARMProcessor.h"

/*
 * This file provide macros that help interfacing Simulator in emulation mode
 * to connect to Einstein.
 */

namespace Simulator {

#define DECLARE_REGISTERS(cpu) \
  KUInt32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr, pc;
  
#define FETCH_REGISTERS(cpu) \
  r0  = cpu->mCurrentRegisters[TARMProcessor::kR0]; \
  r1  = cpu->mCurrentRegisters[TARMProcessor::kR1]; \
  r2  = cpu->mCurrentRegisters[TARMProcessor::kR2]; \
  r3  = cpu->mCurrentRegisters[TARMProcessor::kR3]; \
  r4  = cpu->mCurrentRegisters[TARMProcessor::kR4]; \
  r5  = cpu->mCurrentRegisters[TARMProcessor::kR5]; \
  r6  = cpu->mCurrentRegisters[TARMProcessor::kR6]; \
  r7  = cpu->mCurrentRegisters[TARMProcessor::kR7]; \
  r8  = cpu->mCurrentRegisters[TARMProcessor::kR8]; \
  r9  = cpu->mCurrentRegisters[TARMProcessor::kR9]; \
  r10 = cpu->mCurrentRegisters[TARMProcessor::kR10]; \
  r11 = cpu->mCurrentRegisters[TARMProcessor::kR11]; \
  r12 = cpu->mCurrentRegisters[TARMProcessor::kR12]; \
  sp  = cpu->mCurrentRegisters[TARMProcessor::kR13]; \
  lr  = cpu->mCurrentRegisters[TARMProcessor::kR14]; \
  pc  = cpu->mCurrentRegisters[TARMProcessor::kR15];
  
#define RESTORE_REGISTERS(cpu) \
  cpu->mCurrentRegisters[TARMProcessor::kR0]  = r0; \
  cpu->mCurrentRegisters[TARMProcessor::kR1]  = r1; \
  cpu->mCurrentRegisters[TARMProcessor::kR2]  = r2; \
  cpu->mCurrentRegisters[TARMProcessor::kR3]  = r3; \
  cpu->mCurrentRegisters[TARMProcessor::kR4]  = r4; \
  cpu->mCurrentRegisters[TARMProcessor::kR5]  = r5; \
  cpu->mCurrentRegisters[TARMProcessor::kR6]  = r6; \
  cpu->mCurrentRegisters[TARMProcessor::kR7]  = r7; \
  cpu->mCurrentRegisters[TARMProcessor::kR8]  = r8; \
  cpu->mCurrentRegisters[TARMProcessor::kR9]  = r9; \
  cpu->mCurrentRegisters[TARMProcessor::kR10] = r10; \
  cpu->mCurrentRegisters[TARMProcessor::kR11] = r11; \
  cpu->mCurrentRegisters[TARMProcessor::kR12] = r12; \
  cpu->mCurrentRegisters[TARMProcessor::kR13] = sp; \
  cpu->mCurrentRegisters[TARMProcessor::kR14] = lr; \
  cpu->mCurrentRegisters[TARMProcessor::kR15] = pc+4;
  
  extern KUInt32 getMem32(KUInt32 addr);
  extern void setMem32(KUInt32 addr, KUInt32 val);
  extern KUInt8 getMem8(KUInt32 addr);
  extern void setMem8(KUInt32 addr, KUInt8 val);

  extern void callEmulator(KUInt32 addr);
  
} // namespace


#endif
// SIMULATOR_SYS_MEMORY_H

