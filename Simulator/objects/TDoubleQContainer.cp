// ==============================
// File:                        objects/TDoubleQContainer.cp
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


#include "TDoubleQContainer.h"
#include "TDoubleQItem.h"


using namespace Simulator;


BOOL Simulator::TDoubleQContainer::RemoveFromQueue(void *item)
{
  if (!item)
    return 0;  
  
  TDoubleQItem *it = (TDoubleQItem*)(((char*)item) + GetOffsetToDoubleQItem());
  if (it->GetContainer()!=this) 
    return 0;
  
  TDoubleQItem *next = it->GetNext();
  TDoubleQItem *prev = it->GetPrev();
  TDoubleQItem *head = GetHead();
  if (it==head) 
  {
    TDoubleQItem *tail = GetTail();
    if (head!=tail) {
      SetHead(next);
      next->SetPrev(0);
    } else {
      SetHead(0);
      SetTail(0);
    }
  } else {  
    prev->SetNext(next);
    TDoubleQItem *tail = GetTail();
    if (it!=tail) {
      next->SetPrev(prev);
    } else {
      SetTail(prev);
    }
  }
  
  it->SetPrev(0);
  it->SetNext(0);
  it->SetContainer(0);
  return 1;
}


BOOL TDoubleQContainer::DeleteFromQueue(void *item)
{
  BOOL ok = RemoveFromQueue(item);
  if (!ok) {
    return 0;
  }
  DestructorProcPtr proc = GetDestructor();
  if (proc) {
#if 0
    proc(GetDestructorInstance(), item);
#else
    TARMProcessor::current->mCurrentRegisters[TARMProcessor::kR0] = (KUInt32)GetDestructorInstance();
    TARMProcessor::current->mCurrentRegisters[TARMProcessor::kR1] = (KUInt32)item;
    callEmulator((KUInt32)proc); // DestructorProcPtr(void*, void*);
#endif
  }
  return 1;
}


using namespace Simulator;
