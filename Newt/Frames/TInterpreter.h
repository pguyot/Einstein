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

class RefVar;
typedef RefVar* RefArg;

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


class TRefStack
{
public:
	//  TRefStack();
	//  ~TRefStack();
	//
	//  void		reset(long);
  KUInt32 TRefStack::PushNILs(long r1);
	
public:
	NEWT_GET_SET_W(Ref*, Top)			// 0:
	NEWT_GET_SET_W(Ref*, Base)			// 4:
	NEWT_GET_SET_W(Ref*, Limit)			// 8:
	NEWT_GET_SET_W(long, size)			// 12:
};


class RefStructStack : public TRefStack
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
  KUInt32 NewState();
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
  KUInt32 CallCFuncPtr(long* r1, long r2);
  KUInt32 Call(RefArg r1, long r2);
	
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


class TObjectHeap
{
public:
  KUInt32 AllocateArray(RefArg r1, long r2);
  KUInt32 AllocateFrame();
  KUInt32 AllocateObject(long r1, unsigned long r2);
  KUInt32 AllocateMap(RefArg r1, long r2);
};


class TRefStructStack
{
public:
  KUInt32 Fill();
};


  // TODO
KUInt32 SetArraySlotError(long r0, long r1, char* r2);
KUInt32 EQ1(long r0, long r1);
KUInt32 AllocateFrameWithMap(RefArg map);
KUInt32 XGetVariable(RefArg cotext, RefArg name, long* exist, int lookup);
KUInt32 UnsafeGetFrameSlot(Ref r0, Ref r1, Ref* r2);
KUInt32 ThrowExInterpreterWithSymbol(long r0, RefArg r1);
class ObjectHeader* ResolveMagicPtr(Ref r0);
RefHandle* AllocateRefHandle(Ref r0);
KUInt32 DisposeRefHandle(RefHandle* r0);
KUInt32 Throw(const char *label, long err, KUInt32 r2);
KUInt32 FastComplicatedAref(FastRunState* r0, Ref r1, Ref r2, int r3);
KUInt32 FastComplicatedEqual(FastRunState* r0);
KUInt32 FastComplicatedNotEqual(FastRunState* r0);
KUInt32 FastComplicatedSetAref(FastRunState* r0, Ref r1, Ref r2, int r3);
KUInt32 FastFindAndSetVar(FastRunState* r0, Ref r1);
KUInt32 FastInvoke(FastRunState* r0, Ref r1);
KUInt32 FastIterDone(FastRunState* r0, Ref r1);
KUInt32 GetTag(RefArg r0, long r1, long* r2);
KUInt32 ForEachLoopReset(RefArg r0, RefArg r1);
KUInt32 GetFrameSlotRef(long r0, long r1);
KUInt32 Length(long r0);
KUInt32 ICacheClear();
KUInt32 ForEachLoopDone(RefArg r0);
KUInt32 FastNewHandlers(FastRunState* r0, Ref r1);
KUInt32 FastResend(FastRunState* r0, Ref r1);
KUInt32 FastResendIfDefined(FastRunState* r0, Ref r1);
KUInt32 FastSend(FastRunState* r0, Ref r1);
KUInt32 FastSendIfDefined(FastRunState* r0, Ref r1);
KUInt32 FastSetLexScope(FastRunState* r0, Ref r1);
KUInt32 FastUnary1Ext(FastRunState* r0, Ref r1);
KUInt32 FastUndefined(FastRunState* r0, Ref r1);
Ref GetFramePath(RefArg r0, RefArg r1);
KUInt32 NumberAdd(RefArg r0, RefArg r1);
KUInt32 NumberSubtract(RefArg r0, RefArg r1);
KUInt32 ThrowBadTypeWithFrameData(long r0, RefArg r1);
KUInt32 FindOffset(long r0, long r1);
KUInt32 AddSlot(RefArg r0, RefArg r1);
KUInt32 ThrowExFramesWithBadValue(long r0, RefArg r1);
KUInt32 ThrowRefException(char* r0, RefArg r1);
KUInt32 _RINTError(Ref r0);
KUInt32 SetFramePath(RefArg r0, RefArg r1, RefArg r2);
KUInt32 SetFramePathFor1XFunctions(RefArg r0, RefArg r1, RefArg r2);
KUInt32 ThrowOutOfBoundsException(RefArg r0, Ref r1);
class ForwardingObject* ObjectPtr1(Ref r0, Ref r1, KSInt32 r2);
KUInt32 FastDoCall(FastRunState* inState, Ref fnRef, Ref r2);
KUInt32 rt_sdiv(KUInt32 r0, KUInt32 r1);
KUInt32 RemoveSecP(unsigned long r0);
KUInt32 AddNewSecPNJT(unsigned long r0, unsigned long r1, unsigned long r2, KUInt32 r3, unsigned char frame0);
KUInt32 Clone(RefArg r0);
KUInt32 Throw(KUInt32 r0, KUInt32 r1, KUInt32 r2);


  // DONE
KUInt32 FastCall(FastRunState* r0, Ref r1);
KUInt32 SetArraySlotRef(long r0, long r1, long r2);
KUInt32 EQRef(long r0, long r1);
KUInt32 ForEachLoopNext(RefArg r0);
KUInt32 AllocateFrame();
KUInt32 DirtyObject(long r0);
KUInt32 SymbolHash(long r0);
KUInt32 symcmp(char* r0, char* r1);
KUInt32 SymbolCompare(long r0, long r1);
KUInt32 SetFrameSlot(RefArg r0, RefArg r1, RefArg r2);
KUInt32 GetArraySlotError(long r0, long r1, char* r2);
KUInt32 GetArraySlotRef(long r0, long r1);
KUInt32 FastIterNext(FastRunState* r0, long r1);
KUInt32 AllocateArray(RefArg r0, long r1);
KUInt32 FastMakeArray(FastRunState* r0, long r1);
KUInt32 FastMakeFrame(FastRunState* inState, long inB);
KUInt32 ObjectFlags(Ref obj);
class ForwardingObject* ObjectPtr(Ref inRef);
KUInt8* BinaryData(Ref ref);
KUInt32 FastFreqFuncGeneral(FastRunState* inState, KSInt32);
KUInt32 FastBranchIfLoopNotDone(FastRunState* inState, long B);
KUInt32 FastFindVar(FastRunState* inState, long inB);
KUInt32 FixedMultiply(KUInt32 in0, KUInt32 in1);


#endif	// NEWT_FRAMES_T_INTERPRETER_H

