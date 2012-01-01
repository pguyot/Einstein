// ==============================
// File:                        gfx/TRect.cp
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


#include "TObjectTable.h"
#include "TObject.h"


using namespace Simulator;


TObject *Simulator::TObjectTable::Get(ObjectId id)
{  
  TObject *obj = GetEntry((id>>4)&kObjectTableMask);
  for ( ; obj; obj = obj->GetNext() )
  {
    ObjectId oid = obj->GetId();
    if (oid==id) {
      if (oid!=0) {
        return obj;
      } else {
        return 0L;
      }
    }
  }
  return 0L;
}

