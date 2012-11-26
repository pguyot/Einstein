// ==============================
// File:			Newt/Common/Globals.h
// Project:			Einstein
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


#ifndef NEWT_COMMON_GLOBALS_H
#define NEWT_COMMON_GLOBALS_H


#include "Newt/Common/Glue.h"
#include "Newt/Common/Types.h"
#include "Newt/Frames/TInterpreter.h"


class ObjectHeader;


FreqFuncDef* GlobalGetFreqFuncInfo(); //NEWT_GLOBAL_W_DECL(FreqFuncDef*, FreqFuncInfo, 0x0C1051F4);

NEWT_GLOBAL_W_DECL(Ref,				VarFrame,						0x0C10180C);

NEWT_GLOBAL_W_DECL(KUInt32,			FramesFunctionProfilingEnabled,	0x0C10535C);
NEWT_GLOBAL_W_DECL(KUInt32,			FramesFunctionProfiler,			0x0C105360);
NEWT_GLOBAL_W_DECL(Ref,				FunctionFrame,					0x0C10544C);
NEWT_GLOBAL_W_DECL(Ref,				FreqFuncs,						0x0C105450);
NEWT_GLOBAL_W_DECL(KUInt32,			FramesBreakPointsEnabled,		0x0C105460);
NEWT_GLOBAL_W_DECL(KUInt32,			AccurateStackTrace,				0x0C105468);
NEWT_GLOBAL_W_DECL(TObjectHeap*,	Heap,							0x0c105548);
NEWT_GLOBAL_W_DECL(Ref,				CacheObjPtrRef,					0x0C10554C);
NEWT_GLOBAL_W_DECL(ObjectHeader*,	CacheObjPtrPtr,					0x0C105550);


#endif	// NEWT_COMMON_GLOBALS_H

