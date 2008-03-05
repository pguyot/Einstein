// ==============================
// File:			objects/objects_stubs.cp
// Project:			Albert
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


#include "objects_stubs.h"
#include "TObjectTable.h"
#include "TDoubleQContainer.h"


namespace Albert {

  JITInstructionProto(TObjectTable_Get_Stub)
  {
    TObjectTable *t = (TObjectTable*)ioCPU->mCurrentRegisters[0];
    KUInt32      id =                ioCPU->mCurrentRegisters[1];
    KUInt32    next =                ioCPU->mCurrentRegisters[14]+4;
    KUInt32 result = (KUInt32)t->Get(id);
    ioCPU->mCurrentRegisters[0] = result;
    MMUCALLNEXT(next);
  }
  TROMPatch pTObjectTable_Get(0x00319f14, TObjectTable_Get_Stub, "TObjectTable::Get");
  
  JITInstructionProto(TDoubleQContainer_RemoveFromQueue_Stub)
  {
    TDoubleQContainer *This = (TDoubleQContainer*)ioCPU->mCurrentRegisters[0];
    void              *data =              (void*)ioCPU->mCurrentRegisters[1];
    KUInt32            next =                     ioCPU->mCurrentRegisters[14]+4;
    KUInt32 result = (KUInt32)This->RemoveFromQueue(data);
    ioCPU->mCurrentRegisters[0] = result;
    MMUCALLNEXT(next);
  }
  TROMPatch pTDoubleQContainer_RemoveFromQueue(0x00319f14, TDoubleQContainer_RemoveFromQueue_Stub, "TDoubleQContainer::RemoveFromQueue");
  
  
} // namespace
