// ==============================
// File:                        sys/types.h
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

#ifndef SIMULATOR_SYS_TYPES_H
#define SIMULATOR_SYS_TYPES_H

#include <K/Defines/KDefinitions.h>
#include "Simulator/sys/error.h"


namespace Simulator {

  /* FIXME
   * Currently this is a loose collection of typedefs that is created as we go.
   * At som point, we must weed this out and create proper Simulator OS code.
   */
  
  typedef KSInt32 ObjectId;
  typedef KSInt32 Fixed;
  typedef KSInt32 NewtonErr;
  typedef KUInt32 BOOL;
  typedef KUInt32 ULong;
  
  typedef KUInt32 GetScavengeProcPtr; // FIXME
  
  typedef void (*DestructorProcPtr)(void *, void *);
  typedef KUInt32 (*MonitorProcPtr)(void *, int, void *);

  enum KernelTypes {
    kNoType = 0,
    kBaseType,
    kPortType,
    kTaskType,
    kEnvironmentType,
    kDomainType,
    kSemListType,
    kSemGroupType,
    kSharedMemType,
    kSharedMemMsgType,
    kMonitorType,
    kPhysType,
    kExtPhysType
  };
  
} // namespace


#endif
// SIMULATOR_SYS_TYPES_H

