// ==============================
// File:                        Newt/Common/Globals.cp
// Project:                     Simulator
//
// Copyright 1999-2012 by Newton Research Group and others
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


#include "Newt/Common/Globals.h"


FreqFuncDef* GlobalGetFreqFuncInfo() { return (FreqFuncDef*)0x0C1051F4; }

NEWT_GLOBAL_W_IMPL(KUInt32,			FramesFunctionProfilingEnabled,	0x0C10535C);
NEWT_GLOBAL_W_IMPL(KUInt32,			FramesFunctionProfiler,			0x0C105360);
NEWT_GLOBAL_W_IMPL(Ref,				FunctionFrame,					0x0C10544C);
NEWT_GLOBAL_W_IMPL(Ref,				FreqFuncs,						0x0C105450);
NEWT_GLOBAL_W_IMPL(KUInt32,			FramesBreakPointsEnabled,		0x0C105460);
NEWT_GLOBAL_W_IMPL(KUInt32,			AccurateStackTrace,				0x0C105468);
NEWT_GLOBAL_W_IMPL(Ref,				CacheObjPtrRef,					0x0c10554c);
NEWT_GLOBAL_W_IMPL(ObjectHeader*,	CacheObjPtrPtr,					0x0c105550);
