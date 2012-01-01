// ==============================
// File:                        objects/TObjectTable.h
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

#ifndef SIMULATOR_OBJECTS_TOBJECTTABLE_H
#define SIMULATOR_OBJECTS_TOBJECTTABLE_H

#include <K/Defines/KDefinitions.h>
#include <Simulator/sys/types.h>
#include <Simulator/sys/einstein.h>
#include <Simulator/sys/macros.h>


namespace Simulator {

  class TObject;

  class TObjectTable {
  public:
    static const int kObjectTableSize = 0x80;
    static const int kObjectTableMask = 0x7F;
    
    /**
     * Return the object in the table with the given Id.
     */
    TObject *Get(ObjectId id);
    
  private:    
    SIMULATOR_CLASS_MEMBER( GetScavengeProcPtr, Scavenge );                  // +00
    SIMULATOR_CLASS_MEMBER( TObject*, ThisObj );                             // +04
    SIMULATOR_CLASS_MEMBER( TObject*, PrevObj );                             // +08
    SIMULATOR_CLASS_MEMBER( KUInt32, Index );                                // +0c
    SIMULATOR_CLASS_MEMBER_ARRAY( TObject*, Entry, kObjectTableSize );       // +10
  };
  

} // namespace


#endif
// SIMULATOR_OBJECTS_TOBJECTTABLE_H

