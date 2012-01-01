// ==============================
// File:                        objects/TMonitor.h
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

#ifndef SIMULATOR_OBJECTS_TMONITOR_H
#define SIMULATOR_OBJECTS_TMONITOR_H

#include <K/Defines/KDefinitions.h>
#include <Simulator/sys/types.h>
#include <Simulator/sys/einstein.h>
#include <Simulator/sys/macros.h>
#include <Simulator/objects/TObject.h>


namespace Simulator {

  class TTask;
  class TDoubleQContainer;

  class TMonitor : public TObject {
  public:
    SIMULATOR_CLASS_MEMBER( KUInt32, QueueCount );                           // +10
    SIMULATOR_CLASS_MEMBER( KUInt32, Suspended );                            // +14
    SIMULATOR_CLASS_MEMBER( void*, ProcContext );                            // +18
    SIMULATOR_CLASS_MEMBER( ObjectId, MsgId );                               // +1c
    SIMULATOR_CLASS_MEMBER( TTask*, Caller );                                // +20
    SIMULATOR_CLASS_MEMBER( TDoubleQContainer*, Queue );                     // +24
    KUInt32 unknown28;
    KUInt32 unknown2c;
    KUInt32 unknown30;
    KUInt32 unknown34;
    SIMULATOR_CLASS_MEMBER( MonitorProcPtr, Proc );                          // +38
    SIMULATOR_CLASS_MEMBER( TTask*, MonitorTask );                           // +3c
    SIMULATOR_CLASS_MEMBER( ObjectId, MonitorTaskId );                       // +40
    SIMULATOR_CLASS_MEMBER_BYTE( BOOL, IsFaultMonitor );                     // +44
    SIMULATOR_CLASS_MEMBER_BYTE( BOOL, RebootProtected );                    // +45
    SIMULATOR_CLASS_MEMBER_BYTE( BOOL, IsCopying );                          // +46
  };
  
} // namespace


#endif
// SIMULATOR_OBJECTS_TMONITOR_H

