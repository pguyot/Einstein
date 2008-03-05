// ==============================
// File:                        sys/einstein.cp
// Project:                     Albert
//
// Copyright 2003-2008 by Matthias Melcher (albert@matthiasm.com).
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


#include <Albert/sys/einstein.h>
#include "Emulator/TMemory.h"
#include "Emulator/TARMProcessor.h"


namespace Albert {

  
TARMProcessor *CPUInterface = 0L;

 
  KUInt32 getMem32(KUInt32 addr)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    KUInt32 val;
    if (theMemoryInterface->ReadAligned(addr, val))
    {
      printf("### DATA ABORT ERROR reading 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
    return val;
  }
  
  void setMem32(KUInt32 addr, KUInt32 val)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    if (theMemoryInterface->WriteAligned(addr, val))
    {
      printf("### DATA ABORT ERROR writing 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
  }
  
  KUInt8 getMem8(KUInt32 addr)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    KUInt8 val;
    if (theMemoryInterface->ReadB(addr, val))
    {
      printf("### DATA ABORT ERROR reading 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
    return val;
  }
  
  void setMem8(KUInt32 addr, KUInt8 val)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    if (theMemoryInterface->WriteB(addr, val))
    {
      printf("### DATA ABORT ERROR writing 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
  }
  
  KUInt32 getPhysMem32(KUInt32 addr)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    Boolean fault;
    KUInt32 val = theMemoryInterface->ReadP(addr, fault);
    if (fault)
    {
      printf("### DATA ABORT ERROR reading 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
    return val;
  }
  
  void setPhysMem32(KUInt32 addr, KUInt32 val)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    if (theMemoryInterface->WriteP(addr, val))
    {
      printf("### DATA ABORT ERROR writing 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
  }
  
  KUInt8 getPhysMem8(KUInt32 addr)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    KUInt8 val;
    if (theMemoryInterface->ReadBP(addr, val))
    {
      printf("### DATA ABORT ERROR reading 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
    return val;
  }
  
  void setPhysMem8(KUInt32 addr, KUInt8 val)
  {
    TMemory* theMemoryInterface = CPUInterface->GetMemory();
    if (theMemoryInterface->WriteBP(addr, val))
    {
      printf("### DATA ABORT ERROR writing 0x%08x (pc=0x%08x): please implement DataAbortHandler now.\n", (unsigned int)addr, (unsigned int)CPUInterface->mCurrentRegisters[15]);
		}
  }
  
}

