// ==============================
// File:                        objects/TTask.h
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

#ifndef SIMULATOR_OBJECTS_TTASK_H
#define SIMULATOR_OBJECTS_TTASK_H

#include <K/Defines/KDefinitions.h>
#include <Simulator/sys/types.h>
#include <Simulator/sys/einstein.h>
#include <Simulator/sys/macros.h>
#include <Simulator/objects/TObject.h>


namespace Simulator {


  class TTask : public TObject {
  public:    
    SIMULATOR_CLASS_MEMBER_ARRAY( KUInt32, Register, 16 );                   // +10
    SIMULATOR_CLASS_MEMBER( KUInt32, PSR );                                  // +50
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown54 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown58 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown5c );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown60 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown64 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown68 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown6c );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown70 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown74 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown78 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown7c );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown80 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown84 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown88 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown8c );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown90 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown94 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown98 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown9c );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowna0 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowna4 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowna8 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownac );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownb0 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownb4 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownb8 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownbc );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownc0 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownc4 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownc8 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowncc );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownd0 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownd4 );                            // +54
    SIMULATOR_CLASS_MEMBER( ObjectId, MonitorId );                           // +D8
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowndc );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowne0 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowne4 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknowne8 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownec );                            // +54
    SIMULATOR_CLASS_MEMBER( ObjectId, SharedMemId );                         // +f0
    SIMULATOR_CLASS_MEMBER( ObjectId, SharedMemMsgId );                      // +f4
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownf8 );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknownfc );                            // +54
    SIMULATOR_CLASS_MEMBER( KUInt32, unknown100 );                           // +54
  };
  
} // namespace


#endif
// SIMULATOR_OBJECTS_TTASK_H

