// ==============================
// File:                        objects/objects.h
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

#ifndef ALBERT_OBJECTS_OBJECTS_H
#define ALBERT_OBJECTS_OBJECTS_H

#include <K/Defines/KDefinitions.h>
#include <Albert/sys/types.h>


namespace Albert {

  
  class TObject;
  
  const KUInt32 kBuiltInSMemMsgId = 1;
  const KUInt32 kBuiltInSMemId = 2;
  const KUInt32 kBuiltInSMemMonitorFaultId = 3;

  extern ObjectId LocalToGlobalId(ObjectId);
  extern NewtonErr ConvertIdToObj(KernelTypes inType, ObjectId inId, TObject **outObj);
  extern TObject *IdToObj(KernelTypes inType, ObjectId inId);

  inline KSInt32 ObjectType(ObjectId id) { return (id & 0x0f); }


} // namespace


#endif
// ALBERT_OBJECTS_OBJECTS_H

