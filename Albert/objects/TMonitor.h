// ==============================
// File:                        objects/TMonitor.h
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

#ifndef ALBERT_OBJECTS_TMONITOR_H
#define ALBERT_OBJECTS_TMONITOR_H

#include <K/Defines/KDefinitions.h>
#include <Albert/sys/types.h>
#include <Albert/sys/einstein.h>
#include <Albert/sys/macros.h>
#include <Albert/objects/TObject.h>


namespace Albert {

  class TTask;
  class TDoubleQContainer;

  class TMonitor : public TObject {
  public:
    ALBERT_CLASS_MEMBER( KUInt32, QueueCount );                           // +10
    ALBERT_CLASS_MEMBER( KUInt32, Suspended );                            // +14
    ALBERT_CLASS_MEMBER( void*, ProcContext );                            // +18
    ALBERT_CLASS_MEMBER( ObjectId, MsgId );                               // +1c
    ALBERT_CLASS_MEMBER( TTask*, Caller );                                // +20
    ALBERT_CLASS_MEMBER( TDoubleQContainer*, Queue );                     // +24
    KUInt32 unknown28;
    KUInt32 unknown2c;
    KUInt32 unknown30;
    KUInt32 unknown34;
    ALBERT_CLASS_MEMBER( MonitorProcPtr, Proc );                          // +38
    ALBERT_CLASS_MEMBER( TTask*, MonitorTask );                           // +3c
    ALBERT_CLASS_MEMBER( ObjectId, MonitorTaskId );                       // +40
    ALBERT_CLASS_MEMBER_BYTE( BOOL, IsFaultMonitor );                     // +44
    ALBERT_CLASS_MEMBER_BYTE( BOOL, RebootProtected );                    // +45
    ALBERT_CLASS_MEMBER_BYTE( BOOL, IsCopying );                          // +46
  };
  
} // namespace


#endif
// ALBERT_OBJECTS_TMONITOR_H

