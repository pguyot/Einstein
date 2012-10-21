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


typedef long Ref;

class TInterpreter;


class RefPtr
{
public:
	NEWT_GET_SET_W(Ref*, Ref)
};


class RefHandle
{
public:
	NEWT_GET_SET_W(Ref, Ref)
	NEWT_GET_SET_W(Ref, StackPos)
};


class RefVar
{
public:
	NEWT_GET_SET_W(RefHandle*, RefHandle)	//   0: reference
};


class RefStruct : public RefVar
{
public:
	//	RefStruct(RefStruct const & arg0);
	//	RefStruct(RefVar const & arg0);
	//	operator=(long const arg0);
	//	operator=(RefVar const & arg0);
	//	operator() const;
	//	RefStruct();
	//	~RefStruct();
	//	RefStruct(long const arg0);
};


class RefStack
{
	//public:
	//  RefStack();
	//  ~RefStack();
	//
	//  void		reset(long);
	//  void		pushNILs(long);
	
public:
	NEWT_GET_SET_W(Ref*, Top)			// 0:
	NEWT_GET_SET_W(Ref*, Base)			// 4:
	NEWT_GET_SET_W(Ref*, Limit)			// 8:
	NEWT_GET_SET_W(long, size)			// 12:
};


class RefStructStack : public RefStack
{
	//public:
	//  RefStructStack();
	//  ~RefStructStack();
	//
	//  void		fill(void);
	
public:
	NEWT_GET_SET_W(RefStruct*, XBase)	// 16:
	NEWT_GET_SET_W(RefStruct*, XTop)	// 20:
};


class VMState
{
public:
	Ref GetRcvrRef() { return GetRcvr()->GetRef(); }
public:
	NEWT_GET_SET_W(RefHandle*, PC);  //  0: instruction pointer
	NEWT_GET_SET_W(RefHandle*, Function);			//  4: current function
	NEWT_GET_SET_W(RefHandle*, Locals);				//  8: local variables
	NEWT_GET_SET_W(RefHandle*, Impl);				// 12:
	NEWT_GET_SET_W(RefHandle*, Rcvr);				// 16: variables used by some calls
	NEWT_GET_SET_W(RefHandle*, StackFrame);			// 20:
};


class FreqFuncDef {
public:
	NEWT_GET_SET_W(char*, Name)
	NEWT_GET_SET_W(KUInt32, NumOfArgs)
};


/**
 This class manages the interpreter data stack.
 */
class SimStack : public RefStructStack {
	// FIXME: find the correct class!
public:
};


/**
 This class holds the current state of the interpreter.
 */
class FastRunState // FIXME: same as or derived from VMState?
{
public:	// variables
	NEWT_GET_SET_W(KUInt8*, ByteCodePC)			//   0: instruction pointer
	NEWT_GET_SET_W(SimStack*, Stack)			//   4: stack class pointer
	NEWT_GET_SET_W(TInterpreter*, Interpreter)			//   8:
	RefStruct pImpl;							//  12:
	RefStruct pRcvr;							//  16:
	RefStruct pStackFrame;						//  20:
        RefStruct pUnknown24;			//  24:
	NEWT_GET_SET_W(Ref*, Literals)				//  28: Literals
	NEWT_GET_SET_W(KUInt8*, ByteCodeBase)		//  32: Ptr to BinaryData of InstructionsRef
};



class TIntrpStack : public RefStructStack
{
public:	// methods
	VMState* PrevState();
public:	// variables
};


/**
 This class contains the byte code interpreter.
 */
class TInterpreter
{
public:	// methods
  KUInt32 AlternatingLoops(long inInitialStackDepth);
  KUInt32 Run();
  KUInt32 FastRun(long inInitialStackDepth);
  KUInt32 SlowRun(long inInitialStackDepth);
	KUInt32 FastRun1(KSInt32 inInitialStackDepth, FastRunState* inState);
	KUInt32 PopHandlers();
	void SetFlags();
	void SetFastLoopFlag();
	void SetValue(long index, Ref value);
	void PushValue(Ref value);
	Ref PopValue();
	Ref PeekValue(long index);
	
	
public:	// variables
	NEWT_GET_SET_W(TInterpreter*, Next)			//   0: pointer to next interpreter in linked list
	NEWT_GET_SET_W(KUInt32, ID);				//   4:
	TIntrpStack pCtrlStack;						//   8:
	SimStack pDataStack;					//  32:
	NEWT_GET_SET_W(RefHandle*, ExceptionContext);				//  56:
	NEWT_GET_SET_W(long, ExceptionStackIndex);  //  60:
	NEWT_GET_SET_W(RefHandle*, Literals);  //  64:
	NEWT_GET_SET_W(RefHandle*, Instructions);					//  68: reference to instructions object
	NEWT_IS_SET_W(KUInt32, ReadOnly);			//  72:
	NEWT_GET_SET_W(VMState*, VMState);			//  76:
	NEWT_GET_SET_W(KSInt32, InstructionOffset);	//  80:
	NEWT_IS_SET_W(KUInt32, Ver2X);				//  84:
	NEWT_GET_SET_W(long, LocalsIndex);			//  88:
	NEWT_IS_SET_W(KUInt32, Send);				//  92:
	NEWT_IS_SET_W(KUInt32, Fast);				//  96:
	NEWT_GET_SET_W(KUInt32, TraceIndent);		// 100:
	NEWT_IS_SET_B(KUInt8, TraceGetEnabled);		// 104:
	NEWT_IS_SET_B(KUInt8, TraceFuncDisabled);	// 105:
	NEWT_GET_SET_W(KUInt32, Unknown108);		// 108:
	NEWT_GET_SET_W(KUInt32, Unknown112);		// 112:
	NEWT_GET_SET_W(KUInt32, Unknown116);		// 116:
	NEWT_GET_SET_W(KUInt32, Unknown120);		// 120:
	NEWT_IS_SET_W(KUInt32, Tracing);		// 124: tracing?
};



KUInt32 FastFreqFuncGeneral(FastRunState*, long);
KUInt32 FastBranchIfLoopNotDone(FastRunState* inState, long B);
class ArrayObject* ObjectPtr(Ref inRef);
KUInt32 _RINTError(Ref r0);
KUInt32 FastFindVar(FastRunState* inState, long inB);


#endif	// NEWT_FRAMES_T_INTERPRETER_H

