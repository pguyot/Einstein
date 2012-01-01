// ==============================
// File:			objects/objects_stubs.cp
// Project:			Simulator
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


// Simulator
#include <Simulator/sys/macros.h>
#include "TObjectTable.h"
#include "TDoubleQContainer.h"
#include "objects.h"

// Einstein
#include "JIT.h"
#include "TROMImage.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"


namespace Simulator {
  

  SIMULATOR_METHOD_STUB(0x00319f14, TObjectTable, Get)
  {
    TObjectTable *t = (TObjectTable*)ioCPU->mCurrentRegisters[0];
    KUInt32      id =                ioCPU->mCurrentRegisters[1];
    KUInt32    next =                ioCPU->mCurrentRegisters[14]+4;
    KUInt32 result = (KUInt32)t->Get(id);
    ioCPU->mCurrentRegisters[0] = result;
    MMUCALLNEXT(next);
  }
  
  SIMULATOR_METHOD_STUB(0x0009c7c4, TDoubleQContainer, RemoveFromQueue)
  {
    TDoubleQContainer *This = (TDoubleQContainer*)ioCPU->mCurrentRegisters[0];
    void              *data =              (void*)ioCPU->mCurrentRegisters[1];
    KUInt32            next =                     ioCPU->mCurrentRegisters[14]+4;
    KUInt32 result = (KUInt32)This->RemoveFromQueue(data);
    ioCPU->mCurrentRegisters[0] = result;
    MMUCALLNEXT(next);
  }
  
  SIMULATOR_METHOD_STUB(0x0009c844, TDoubleQContainer, DeleteFromQueue)
  {
    TDoubleQContainer *This = (TDoubleQContainer*)ioCPU->mCurrentRegisters[0];
    void              *data =              (void*)ioCPU->mCurrentRegisters[1];
    KUInt32            next =                     ioCPU->mCurrentRegisters[14]+4;
    KUInt32 result = (KUInt32)This->DeleteFromQueue(data);
    ioCPU->mCurrentRegisters[0] = result;
    MMUCALLNEXT(next);
  }

  SIMULATOR_FUNCTION_STUB(0x00191e80, LocalToGlobalId)
  {
    ObjectId a = (ObjectId)ioCPU->mCurrentRegisters[0];
    ObjectId result = LocalToGlobalId(a);
    ioCPU->mCurrentRegisters[0] = (KUInt32)result;
    KUInt32 next = ioCPU->mCurrentRegisters[14]+4;
    MMUCALLNEXT(next);
  }

  SIMULATOR_FUNCTION_STUB(0x00191f14, ConvertIdToObj)
  {
    KernelTypes inType  = (KernelTypes)ioCPU->mCurrentRegisters[0];
    ObjectId inId       = (ObjectId)ioCPU->mCurrentRegisters[1];
    TObject **outObj    = (TObject**)ioCPU->mCurrentRegisters[2];
    KUInt32 next        = ioCPU->mCurrentRegisters[14]+4;
    NewtonErr result = ConvertIdToObj(inType, inId, outObj);
    ioCPU->mCurrentRegisters[0] = (KUInt32)result;
    MMUCALLNEXT(next);
  }

  
} // namespace
