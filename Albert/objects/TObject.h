// ==============================
// File:                        objects/TObject.h
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

#ifndef ALBERT_OBJECTS_TOBJECT_H
#define ALBERT_OBJECTS_TOBJECT_H

#include <K/Defines/KDefinitions.h>
#include <Albert/sys/types.h>
#include <Albert/sys/einstein.h>
#include <Albert/sys/macros.h>


namespace Albert {


  class TObject {
  public:
    /*
                CObject(ObjectId id = 0)  { fId = id; }
                CObject(const CObject &inCopy)	{ fId = inCopy.fId; }
    
    operator    ObjectId() const          { return fId; }
    
    void        setOwner(ObjectId id)     { fOwnerId = id; }
    ObjectId    owner(void) const         { return fOwnerId; }
    
    void        assignToTask(ObjectId id) { fAssignedOwnerId = id; }
    ObjectId    assignedOwner(void) const { return fAssignedOwnerId; }
    */
    
    ALBERT_CLASS_MEMBER( ObjectId, Id );                                  // +00
    ALBERT_CLASS_MEMBER( TObject*, Next );                                // +04
    ALBERT_CLASS_MEMBER( ObjectId, OwnerId );                             // +08
    ALBERT_CLASS_MEMBER( ObjectId, AssignedOwnerId );                     // +0c
  };
  
} // namespace


#endif
// ALBERT_OBJECTS_TOBJECT_H

