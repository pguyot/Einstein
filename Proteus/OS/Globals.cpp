// ==============================
// File:			Globals.cpp
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
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


#include "Globals.h"


namespace NewtOS {


//-/* 0x0C008400-0x0C107E18 */ Range of global variables in memory

	TImageParamBlock *gParamBlockFromImage = (TImageParamBlock*)0x0C008400;

// /* 0x0C0084E4-0x0C100E58 */

GLOBAL_GETSET_W(0x0C100E58, KSInt32, AtomicFIQNestCountFast);

GLOBAL_GETSET_W(0x0C100E5C, KSInt32, AtomicIRQNestCountFast);

// /* 0x0C100E60-0x0C100E6C */

	GLOBAL_GETSET_P(0x0C100E6C, InterruptObject*, SchedulerIntObj);

// /* 0x0C100E70-0x0C100FE4 */

GLOBAL_GETSET_W(0x0C100FE4, KUInt32, Schedule);

GLOBAL_GETSET_W(0x0C100FE8, KSInt32, AtomicNestCount);

// /* 0x0C100FE8-0x0C100FF0 */

GLOBAL_GETSET_W(0x0C100FF0, KSInt32, AtomicFIQNestCount);

// /* 0x0C100FF4-0x0C100FF8 */

GLOBAL_GETSET_P(0x0C100FF8, TTask*, CurrentTask);

// /* 0x0C100FFC-0x0C101028 */

GLOBAL_GETSET_W(0x0C101028, KUInt32, WantDeferred);

// /* 0x0C10102C-0x0C101040 */

GLOBAL_GETSET_W(0x0C101040, KUInt32, CopyDone);

// /* 0x0C101044-0x0C101054 */

	GLOBAL_GETSET_W(0x0C101054, ObjectId, CurrentTaskId);

	GLOBAL_GETSET_W(0x0C101058, ObjectId, gCurrentMonitorId);

	GLOBAL_GETSET_P(0x0C10105C, void*, CurrentGlobals);

// /* 0x0C101060-0x0C101A2C */

	GLOBAL_GETSET_W(0x0C101A2C, KUInt32, WantSchedulerToRun);

	GLOBAL_GETSET_W(0x0C101A30, KUInt32, SchedulerRunning);

// /* 0x0C101A34-0x0C107E18 */

} // namespace


// ======================================================================= //
//                       - insert insane quote here -                      //
// ======================================================================= //
