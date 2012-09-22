// ==============================
// File:			Newt/Frames/TInterpreter.h
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


#ifndef NEWT_FRAMES_T_INTERPRETER_H
#define NEWT_FRAMES_T_INTERPRETER_H


#include "Newt/Common/Newt.h"



class FreqFuncDef {
public:
	NEWT_GET_SET_W(char*, Name)
	NEWT_GET_SET_W(KUInt32, NumOfArgs)
};


/**
 This class manages the interpreter data stack.
 */
class SimStack {
	// FIXME: find the correct class!
public:
	NEWT_GET_SET_W(KUInt32*, Top) // instruction pointer
								 // (...)
};


/**
 This class holds the current state of the interpreter.
 */
class FastRunState
{
public:
	NEWT_GET_SET_W(KUInt8*, PC)		// instruction pointer
	NEWT_GET_SET_W(SimStack*, Stack) // stack class pointer
									// (...)
};




/**
 This class contains the byte code interpreter.
 */
class TInterpreter
{
public:
};



KUInt32 FastFreqFuncGeneral(FastRunState*, long);


#endif	// NEWT_FRAMES_T_INTERPRETER_H

