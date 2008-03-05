// ==============================
// File:                        sys/globals.cp
// Project:                     Albert
//
// Copyright 2003-2008 by Newton Research Group, Matthias Melcher (albert@matthiasm.com).
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

#include "globals.h"

using namespace Albert;

/*
  extern class CTask              *gIdleTask;             // 0C100FC4
  extern class CObjectTable       *Albert::gObjectTable;  // 0C100FC8
  extern class CPort              *gNullPort;             // 0C100FCC +08
  extern class CScheduler         *gKernelScheduler;      // 0C100FD0 +0C
  extern BOOL                      gScheduleRequested;    // 0C100FD4 +10
  extern long                      gHoldScheduleLevel;    // 0C100FD8 +14
  extern class CTimerEngine       *gTimerEngine;          // 0C100FE0 +1c
  extern BOOL                      gDoSchedule;           // 0C100FE4 +20
  extern class CTask              *gCurrentTask;          // 0C100FF8 +34
  extern class CTask              *gCurrentTimedTask;     // 0C100FFC +38
  extern class CTask              *gCurrentMemCountTask;  // 0C101000 +3c
  extern class CTask              *gCurrentTaskSaved;     // 0C101004 +40
  extern BOOL                      gWantDeferred;         // 0C101028
  extern class CDoubleQContainer  *gCopyTasks;            // 0C101034 +70
  extern class CDoubleQContainer  *gBlockedOnMemory;      // 0C101038	+74
  extern class CDoubleQContainer  *gDeferredSends;        // 0C10103C	+78
  extern BOOL                      gCopyDone;             // 0C101040
  extern class CDoubleQContainer  *gTimerDeferred;        // 0C101048	(0x30)
  extern long                      gRebootProtectCount;   // 0C10104C
  extern BOOL                      gWantReboot;           // 0C101050
*/

