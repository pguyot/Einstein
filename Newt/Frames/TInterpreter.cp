// ==============================
// File:			Newt/Frames/TInterpreter.cp
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


#include "Newt/Common/Newt.h"
#include "Newt/Common/Globals.h"
#include "Newt/Frames/TInterpreter.h"
#include "Newt/Frames/Objects.h"

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"
#include "JIT.h"
#include "TJITGeneric_Macros.h"
#include "TROMImage.h"


class RefVar;
typedef RefVar* RefArg;


bool RefIsInt(Ref v) {
	return ((v&3)==0);
}

KSInt32 RefToInt(Ref v) {
	return (v>>2);
}

// 	KUInt32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9;


// --
// Understanding the floating point emulator

// 8 registers, starting at 0x0C105A5C
// IEEE float   1 x sign,  8 x exp, 23 x mantisse, excess 127, implied 1
// IEEE double  1 x sign, 11 x exp, 52 x mantissa, excess 1023, implied 1
// ARM ext      1 x sign, 15 x exp, 16 x 0, 1 x int, 63 x mantissa, excess 16383, 1 not implied(?)


//					   Sign Exponent	J		Mantissa
//Non-trapping NAN		x	Maximum		x		1xxxxxxxxxxxxx...
//Trapping NAN			x	Maximum		x		0<non zero>
//INF					s	Maximum		0		00000000000000...
//Zero					s	0			0		00000000000000...
//Denormalised number	s	0			0		<non zero>
//Un-normalised number	s	Not			0/Max	0	xxxxxxxxxxxxxx...
//Normalised number		s	Not			0/Max	1	xxxxxxxxxxxxxx...



//T_ROM_INJECTION(0x0031C390, "stfd f4, [r0]")
//{
//	union {
//		double d;
//		KUInt8 c[8];
//		KUInt64 x;
//	};
//	int i;
//	for (i=0; i<16; i++) {
//		printf("%02X ", NewtReadByte(0x0C105A5C+4*16+i));
//	}
//	printf(" = %g\n", d);
//	KUInt32 r0 = gCurrentCPU->GetRegister(0);
//	for (i=0; i<8; i++) {
//		KUInt8 v = NewtReadByte(r0+i);
//		c[7-i] = v;
//		printf("%02X ", v);
//	}
//	printf(" = %g\n", d);
//	
//	KUInt64 dSign = x>>63, dExp = (x>>52)&0x7ff, dMant = x&0xfffffffffffff;
//	
//	printf(" = %01llx %04llx %016llx  %lld\n", dSign, dExp, dMant, dExp-1024);
//	// Exp base offsets by 1024
//	// Exp base offsets by 32768
//	for (i=0; i<8; i++) c[7-i] = NewtReadByte(0x0C105A5C+4*16+i);
//	KUInt64 eSign = x>>63; dSign = eSign;
//	KUInt64 eExp = (x>>31)&0xffff;
//	if (eExp==0) {
//		// special case
//		dExp = 0;
//	} else if (eExp>=0x8000) {
//		dExp = (((KSInt32)eExp) - 32768)/2 + 1024; // bias is 1023
//	} else {
//		dExp = (((KSInt32)eExp) - 32768)/2 + 1023;
//	}
//	
//	//	dExp = (((KSInt64)eExp) - 32768)/2 + 1024;
//		
//	printf(" = %01llx %04llx %016llx  %lld\n", dSign, dExp, dMant, eExp-32768);
//	return ioUnit;
//}



// ----------------------------------------------------------------------------


ObjectHeader* ResolveMagicPtr(Ref r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, r0);
	NewtCallJIT(0x0031DAD4);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return (ObjectHeader*)ret;
}

RefHandle* AllocateRefHandle(Ref r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x0031D26C);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return (RefHandle*)ret;
}

KUInt32 DisposeRefHandle(RefHandle* r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x0031D2B0);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 Throw(const char *label, long err, KUInt32 r2)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)label);
	gCurrentCPU->SetRegister(1, (KUInt32)err);
	gCurrentCPU->SetRegister(2, (KUInt32)r2);
	NewtCallJIT(0x000B00C8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastCall(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ED508);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastComplicatedAref(FastRunState* r0, Ref r1, Ref r2, int r3)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	gCurrentCPU->SetRegister(2, (KUInt32)r2);
	gCurrentCPU->SetRegister(3, (KUInt32)r3);
	NewtCallJIT(0x002EF1F0);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastComplicatedEqual(FastRunState* r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x002EF73C);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastComplicatedNotEqual(FastRunState* r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x002EF8A8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastComplicatedSetAref(FastRunState* r0, Ref r1, Ref r2, int r3)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	gCurrentCPU->SetRegister(2, (KUInt32)r2);
	gCurrentCPU->SetRegister(3, (KUInt32)r3);
	NewtCallJIT(0x002EF314);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastFindVar(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EDAF8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastFindAndSetVar(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EDBF8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastInvoke(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ED4EC);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastIterDone(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ECD18);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastIterNext(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ECCA0);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastMakeArray(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EDD60);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastMakeFrame(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EDE40);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastNewHandlers(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EDEE8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastResend(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ED6F4);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastResendIfDefined(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ED798);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastSend(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ED598);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastSendIfDefined(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ED644);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastSetLexScope(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002ED840);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastUnary1Ext(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EDA8C);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 FastUndefined(FastRunState* r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EE090);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


Ref GetFramePath(RefArg r0, RefArg r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x0031EEE8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 NumberAdd(RefArg r0, RefArg r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002B5018);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 NumberSubtract(RefArg r0, RefArg r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002B6790);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 TInterpreter::PopHandlers()
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)this);
	NewtCallJIT(0x002F5674);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


VMState* TIntrpStack::PrevState()
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)this);
	NewtCallJIT(0x001A4820);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return (VMState*)ret;
}


KUInt32 _RINTError(Ref r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x0031CF3C);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 SetFramePath(RefArg r0, RefArg r1, RefArg r2)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	gCurrentCPU->SetRegister(2, (KUInt32)r2);
	NewtCallJIT(0x0031F5DC);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 SetFramePathFor1XFunctions(RefArg r0, RefArg r1, RefArg r2)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	gCurrentCPU->SetRegister(2, (KUInt32)r2);
	NewtCallJIT(0x0031F5E4);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 ThrowExFramesWithBadValue(Ref r0, RefArg r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x0031CA58);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 ThrowOutOfBoundsException(RefArg r0, Ref r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002EF144);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


ArrayObject* ObjectPtr1(Ref r0, Ref r1, KSInt32 r2)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, r0);
	gCurrentCPU->SetRegister(1, r1);
	gCurrentCPU->SetRegister(2, r2);
	NewtCallJIT(0x0031DC54);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return (ArrayObject*)ret;
}


KUInt32 FastDoCall(FastRunState* inState, Ref fnRef, KUInt32 r2)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)inState);
	gCurrentCPU->SetRegister(1, (KUInt32)fnRef);
	gCurrentCPU->SetRegister(2, r2);
	NewtCallJIT(0x002ECFE8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 rt_sdiv(KUInt32 r0, KUInt32 r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, r0);
	gCurrentCPU->SetRegister(1, r1);
	NewtCallJIT(0x0038CA10);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


KUInt32 TInterpreter::SlowRun(long r1)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)this);
	gCurrentCPU->SetRegister(1, (KUInt32)r1);
	NewtCallJIT(0x002F1EE0);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}


// ----------------------------------------------------------------------------


void TInterpreter::SetFastLoopFlag()
{
	if (   GlobalGetAccurateStackTrace()
		|| IsTracing()
		|| GlobalGetFramesBreakPointsEnabled()
		|| !IsReadOnly() )
	{
		SetFast(0);
	} else if (GlobalGetFramesFunctionProfilingEnabled()!='ffpe') {
		SetFast(1);
	} else if (GlobalGetFramesFunctionProfiler()!=0) {
		SetFast(0);
	} else {
		SetFast(1);
	}
}


KUInt32 ObjectFlags(Ref obj)
{
	return ObjectPtr(obj)->GetFlags();
}


void TInterpreter::SetFlags()
{
	Ref funcRef = GetVMState()->GetFunction()->GetRef();
	ArrayObject* funcArray = ObjectPtr(funcRef);
	Ref myInstructions = funcArray->GetSlot(1); // _instructions
	GetInstructions()->SetRef(myInstructions);
	Ref myLiterals = funcArray->GetSlot(2); // _literals
	GetLiterals()->SetRef(myLiterals);
	KUInt32 flags = ObjectFlags(GetInstructions()->GetRef()) & 80;
	SetReadOnly( (flags!=0) );
	Ref stackFrameRef = GetVMState()->GetStackFrame()->GetRef();
	if ( (stackFrameRef&4) ) {
		SetLocalsIndex(stackFrameRef>>8);
		SetVer2X(1);
	} else {
		SetVer2X(0);
	}
	return SetFastLoopFlag();
}


ArrayObject* ObjectPtr(Ref inRef)
{
	Ref type = inRef & 3;
	if (type==1) { // pointer
		if ( (inRef<0x3800000) || (inRef>=0x60000000 && inRef<0x68000000) ) {
			// in ROM of packet memory? Directly convert into a pointer
			return (ArrayObject*)(inRef&0xfffffffc);
		} else {
			// Cached?
			if (inRef == GlobalGetCacheObjPtrRef()) {
				return (ArrayObject*)GlobalGetCacheObjPtrPtr();
			}
			// No? Fall through.
		}
	} else if (type==3) {
		return (ArrayObject*)ResolveMagicPtr(inRef);
	}
	return ObjectPtr1(inRef, type, 0);
}


KUInt8* BinaryData(Ref ref)
{
	ArrayObject* obj = ObjectPtr(ref);
	if ( (obj->GetSizeAndFlags() & 3) != 2 ) {
		return (KUInt8*)obj->PtrToSlots();
	} else {
		KUInt32 ret;
		NEWT_PUSH_REGISTERS
		gCurrentCPU->SetRegister(0, (KUInt32)obj->PtrToSlot(1));
		gCurrentCPU->SetRegister(1, obj->GetSlot(0));
		NewtCallJIT(NewtReadWord(obj->GetSlot(0)+4));
		ret = gCurrentCPU->GetRegister(0);
		NEWT_POP_REGISTERS
		return (KUInt8*)ret;
	}
}


KUInt32 FastFreqFuncGeneral(FastRunState* inState, KSInt32)
{
	SimStack*	currentStack;
	Ref*		sp;
	Ref			arg1, arg2, result;
	
	KUInt8* currentPC = inState->GetByteCodePC();
	KUInt16 B = (NewtReadByte(currentPC)<<8) + NewtReadByte(currentPC+1);
	inState->SetByteCodePC(currentPC+2);
	
	switch (B) {
		case 7: // MUL
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				result = arg1 * (arg2>>2);
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break; // Continue with FastDoCall()
			}
		case 9: // DIV
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				if (arg1) {
					result = 4 * ( (arg2>>2) / (arg1>>2) );
				} else {
					result = 4 * rt_sdiv( arg1>>2, arg2>>2 );
					// FIXME: sdiv calls sdiv0 and throws an exception if dividing by 0. Go implement Throw()
				}
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break;
			}
		case 10: // LESS-THAN
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				if (arg2<arg1) {
					result = 26;
				} else {
				    result = 2;
				}
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break;
			}
		case 11: // GREATER-THAN
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				if (arg2>arg1) {
					result = 26;
				} else {
				    result = 2;
				}
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break;
			}
		case 13: // LESS-OR-EQUAL
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				if (arg2<=arg1) {
					result = 26;
				} else {
				    result = 2;
				}
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break;
			}
		case 12: // GREATER-OR-EQUAL
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				if (arg2>=arg1) {
					result = 26;
				} else {
				    result = 2;
				}
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break;
			}
		case 14: // BIT-AND
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				result = arg1 & arg2;
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break; // Continue with FastDoCall()
			}
		case 15: // BIT-OR
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				result = arg1 | arg2;
				NewtWriteWord(sp-2, result);
				currentStack->SetTop(sp-1);
				return 0;
			} else {
				break; // Continue with FastDoCall()
			}
		case 16: // BIT-NOT
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg1 = (~arg1) & 0xfffffffc;
			NewtWriteWord(sp-1, arg1);
			return 0;
	}
	
	// if this command was not handled in the switch/case above, we continue here
	Ref freqFuncsRef = GlobalGetFreqFuncs();
	ArrayObject* freqFuncs = ObjectPtr(freqFuncsRef);
	Ref thisFunc = freqFuncs->GetSlot(B);
	
	FreqFuncDef* funcInfo = GlobalGetFreqFuncInfo();
	KUInt32 numArgs = funcInfo[B].GetNumOfArgs();
	
	return FastDoCall(inState, thisFunc, numArgs);
}


KUInt32 TInterpreter::FastRun1(KSInt32 inInitialStackDepth, FastRunState* inState)
{
	// this local variable must live in Einstein space
	NEWT_LOCAL(RefVar, tmpRef);

	// these can live in native space
	RefStruct* Impl = &inState->pImpl;
	RefStruct* Rcvr = &inState->pRcvr;
	RefStruct* StackFrame = &inState->pStackFrame;
	
	for (;;) {
		
		Ref instrRef = GetInstructions()->GetRef();
		KUInt8* instrPtr = BinaryData(instrRef);
		inState->SetByteCodeBase(instrPtr);
		instrPtr += GetInstructionOffset();
		inState->SetByteCodePC(instrPtr);
		
		Ref* literalsHdl;
		Ref literalsRef = GetLiterals()->GetRef();
		if (literalsRef==NILREF) {
			literalsHdl = 0;
		} else {
			literalsHdl = ObjectPtr(literalsRef)->PtrToSlots();
		}
		inState->SetLiterals(literalsHdl);
		
		Ref* Locals = pDataStack.GetBase() + GetLocalsIndex();
		
		for (;;) {
			KUInt8 A = NewtReadByte(instrPtr++);
			KUInt32 B = A & 0x07;
			KUInt32 functionType = 0;
			
			switch (A) {
					// pop
					// x --
				case 000:
					PopValue();
					break;
					
					// dup
					// x -- x x
				case 001: {
					Ref val = PeekValue(0);
					PushValue(val);
					break; }
					
					// return
					// --
				case 002: {
					if (IsVer2X()) {
						Ref retVal = PeekValue(0);
						VMState* vm = GetVMState();
						Ref stackFrame = vm->GetStackFrame()->GetRef();
						stackFrame = (stackFrame>>8) +3;
						SimStack* stack = inState->GetStack();
						Ref* newTop = stack->GetBase() + stackFrame;
						stack->SetTop(newTop+1);
						NewtWriteWord(newTop, retVal);
					}
					VMState* newState = pCtrlStack.PrevState();
					SetVMState(newState);
					SetInstructionOffset(newState->GetPC()->GetRef()>>2);
					
					KSInt32 stackDepth = pCtrlStack.GetTop() - pCtrlStack.GetBase() - 1;
					if (stackDepth<0)
						stackDepth += 3;
					stackDepth = (stackDepth>>2)-1;
					if (stackDepth<GetExceptionStackIndex())
						PopHandlers();
					
					Ref func = GetVMState()->GetFunction()->GetRef();
					if (func!=NILREF) {
						if (GetInstructionOffset()!=-1)
							SetFlags();
					}
					functionType = 1;
					goto exitInnerLoop; }
					
					// push-self
					// -- RCVR
				case 003: {
					Ref rcvrRef = GetVMState()->GetRcvrRef();
					PushValue(rcvrRef);
					break; }
					
					// push
					// -- literal
				case 037:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 030:
				case 031:
				case 032:
				case 033:
				case 034:
				case 035:
				case 036: {
					Ref* literals = inState->GetLiterals();
					Ref lit = NewtReadWord(literals+B);
					PushValue(lit);
					break; }
					
					// push-constant
					// -- value (signed)
				case 047:
					B = ((KSInt8)NewtReadByte(instrPtr++)) * 256;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 040:
				case 041:
				case 042:
				case 043:
				case 044:
				case 045:
				case 046:
					PushValue(B);
					break;
					
					// branch-if-true
					// value --
				case 0147:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
				case 0140:
				case 0141:
				case 0142:
				case 0143:
				case 0144:
				case 0145:
				case 0146: {
					Ref val = PopValue();
					if (val!=2) {
						instrPtr = inState->GetByteCodeBase() + B;
					}
					break; }
					
					// branch-if-false
					// value --
				case 0157:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
				case 0150:
				case 0151:
				case 0152:
				case 0153:
				case 0154:
				case 0155:
				case 0156: {
					Ref val = PopValue();
					if (val==2) {
						instrPtr = inState->GetByteCodeBase() + B;
					}
					break; }
					// branch
					// --
				case 0137:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
				case 0130:
				case 0131:
				case 0132:
				case 0133:
				case 0134:
				case 0135:
				case 0136: {
					instrPtr = inState->GetByteCodeBase() + B;
					break; }
					
					// get-var
					// -- result
				case 0177:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0170:
				case 0171:
				case 0172:
				case 0173:
				case 0174:
				case 0175:
				case 0176:
					if (IsVer2X()) {
						Ref var = NewtReadWord(Locals+B);
						PushValue(var);
					} else {
						Ref locals = GetVMState()->GetLocals()->GetRef();
						ArrayObject *localsArray = ObjectPtr(locals);
						PushValue(localsArray->GetSlot(B));
					}
					break;
					
					// set-var
					// value --
				case 0247:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0240:
				case 0241:
				case 0242:
				case 0243:
				case 0244:
				case 0245:
				case 0246:
					if (IsVer2X()) {
						Ref val = PopValue();
						NewtWriteWord(Locals+B, val);
					} else {
						Ref val = PopValue();
						Ref locals = GetVMState()->GetLocals()->GetRef();
						ArrayObject *localsArray = ObjectPtr(locals);
						localsArray->SetSlot(B, val);
					}
					break;
					
					// incr-var
					// addend -- addend value
				case 0267:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0260:
				case 0261:
				case 0262:
				case 0263:
				case 0264:
				case 0265:
				case 0266: {
					Ref* slot;
					if (IsVer2X()) {
						slot = Locals+B;
					} else {
						Ref localsRef = GetVMState()->GetLocals()->GetRef();
						ArrayObject* locals = ObjectPtr(localsRef);
						slot = locals->PtrToSlot(B);
					}
					Ref var = NewtReadWord(slot);
					Ref add = PeekValue(0);
					if (((var|add)&3)==0) {	// are bot refs of type INT?
						var = var + add;
						NewtWriteWord(slot, var);
						PushValue(var);
					} else {
						if (var&3) {
							_RINTError(var);
						}
						if (add&3) {
							_RINTError(add);
						}
					}
					break; }
					
					// get-path (nil object allowed)
					// object pathExpr -- value
				case 0220: {
					Ref path = PopValue();
					Ref obj = PopValue();
					if (obj==NILREF) { // NIL
						PushValue(NILREF);
					} else {
						RefHandle* hImpl = Impl->GetRefHandle();
						hImpl->SetRef(path);
						RefHandle* hRcvr = Rcvr->GetRefHandle();
						hRcvr->SetRef(obj);
						Ref val = GetFramePath(Rcvr, Impl);
						PushValue(val);
					}
					break; }
					
					// get-path (nil object throws exception)
					// object pathExpr -- value
				case 0221: {
					Ref obj = PeekValue(-1);
					if (obj==2) {
						Ref path = PeekValue(0);
						RefHandle* hImpl = Impl->GetRefHandle();
						hImpl->SetRef(path);
						ThrowExFramesWithBadValue(-48204, Impl);
						return 0;
					} else {
						Ref path = PopValue();
						RefHandle* hImpl = Impl->GetRefHandle();
						hImpl->SetRef(path);
						RefHandle* hRcvr = Rcvr->GetRefHandle();
						hRcvr->SetRef(obj);
						Ref val = GetFramePath(Rcvr, Impl);
						SetValue(0, val);
					}
					break; }
					
					// set_path (don't push value)
					// object pathExpr value --
				case 0230: {
					Ref val = PopValue();
					RefHandle* hImpl = Impl->GetRefHandle();
					hImpl->SetRef(val);
					Ref path = PopValue();
					RefHandle* hRcvr = Rcvr->GetRefHandle();
					hRcvr->SetRef(path);
					Ref obj = PopValue();
					RefHandle* hStackFrame = StackFrame->GetRefHandle();
					hStackFrame->SetRef(obj);
					if ( IsVer2X() && (((KUInt32)instrPtr)>0x800000) ) {
						SetFramePath(StackFrame, Rcvr, Impl);
					} else {
						SetFramePathFor1XFunctions(StackFrame, Rcvr, Impl);
					}
					break; }
					
					// set_path (and push value)
					// object pathExpr value -- result
				case 0231: {
					Ref val = PopValue();
					RefHandle* hImpl = Impl->GetRefHandle();
					hImpl->SetRef(val);
					Ref path = PopValue();
					RefHandle* hRcvr = Rcvr->GetRefHandle();
					hRcvr->SetRef(path);
					Ref obj = PopValue();
					RefHandle* hStackFrame = StackFrame->GetRefHandle();
					hStackFrame->SetRef(obj);
					if ( IsVer2X() && (((KUInt32)instrPtr)>0x800000) ) {
						SetFramePath(StackFrame, Rcvr, Impl);
					} else {
						SetFramePathFor1XFunctions(StackFrame, Rcvr, Impl);
					}
					hImpl = Impl->GetRefHandle();
					PushValue(hImpl->GetRef());
					break; }
					
					// freq-func aref
					// object index -- element
				case 0302: {
					Ref ix = PopValue();
					if ( (ix&3)==0 ) {
						ix = ix>>2;
					} else {
						_RINTError(ix);
					}
					KSInt32 index = ix;
					
					Ref objectRef = PeekValue(0);
					ArrayObject* object = ObjectPtr(objectRef);
					if (object->GetType()==1) {
						if ((index<0) || (index>=object->GetNumSlots())) {
							PopValue();
							ThrowOutOfBoundsException(NEWT_LOCAL_PTR(tmpRef), index);
							return 0;
						}
						SetValue(0, object->GetSlot(index));
					} else {
						Impl->GetRefHandle()->SetRef(PeekValue(0));
						inState->SetByteCodePC(instrPtr);
						FastComplicatedAref(inState, index, object->GetFlags(), IsVer2X());
						instrPtr = inState->GetByteCodePC();
					}
					break; }
					
					// freq-func set-aref
					// object index element -- element
				case 0303: {
					Ref element = PopValue();
					Rcvr->GetRefHandle()->SetRef(element);
					
					Ref ix = PopValue();
					if ( (ix&3)==0 ) {
						ix = ix>>2;
					} else {
						_RINTError(ix);
					}
					KSInt32 index = ix;
					
					Ref objectRef = PeekValue(0);
					ArrayObject* object = ObjectPtr(objectRef);
					if (object->GetType()==1) {
						if ((index<0) || (index>=object->GetNumSlots())) {
							PopValue();
							ThrowOutOfBoundsException(NEWT_LOCAL_PTR(tmpRef), index);
							return 0;
						}
						object->SetSlot(index, element);
						SetValue(0, element);
					} else {
						Impl->GetRefHandle()->SetRef(PeekValue(0));
						inState->SetByteCodePC(instrPtr);
						FastComplicatedSetAref(inState, index, object->GetFlags(), IsVer2X());
						instrPtr = inState->GetByteCodePC();
					}
					break; }
					
					// freq-func add
					// num1 num2 -- result
				case 0300: {
					Ref val1 = PopValue();
					Ref val2 = PeekValue(0);
					if ((val1&3)==0 && (val2&3)==0) {
						SetValue(0, val2+val1);
					} else {
						Impl->GetRefHandle()->SetRef(val2);
						Rcvr->GetRefHandle()->SetRef(val1);
						val1 = NumberAdd(Impl, Rcvr);
						SetValue(0, val1);
					}
					break; }
					
					// freq-func subtract
					// num1 num2 -- result
				case 0301: {
					Ref val1 = PopValue();
					Ref val2 = PeekValue(0);
					if ((val1&3)==0 && (val2&3)==0) {
						SetValue(0, val2-val1);
					} else {
						Impl->GetRefHandle()->SetRef(val2);
						Rcvr->GetRefHandle()->SetRef(val1);
						val1 = NumberSubtract(Impl, Rcvr);
						SetValue(0, val1);
					}
					break; }
					
					// freq-func equals
					// obj1 obj2 -- result
				case 0304: {
					Ref val1 = PopValue();
					Ref val2 = PeekValue(0);
					if ((val1|val2)==1) {
						if (val1!=val2) {
							val1 = 2;	// NIL
						} else {
							val1 = 26;	// TRUE
						}
						SetValue(0, val1);
					} else {
						Impl->GetRefHandle()->SetRef(val2);
						Rcvr->GetRefHandle()->SetRef(val1);
						inState->SetByteCodePC(instrPtr);
						FastComplicatedEqual(inState);
						instrPtr = inState->GetByteCodePC();
					}
					break; }
					
					// freq-func equals
					// obj1 obj2 -- result
				case 0306: {
					Ref val1 = PopValue();
					Ref val2 = PeekValue(0);
					if ((val1|val2)==1) {
						if (val1==val2) {
							val1 = 2;	// NIL
						} else {
							val1 = 26;	// TRUE
						}
						SetValue(0, val1);
					} else {
						Impl->GetRefHandle()->SetRef(val2);
						Rcvr->GetRefHandle()->SetRef(val1);
						inState->SetByteCodePC(instrPtr);
						FastComplicatedNotEqual(inState);
						instrPtr = inState->GetByteCodePC();
					}
					break; }
					
					// freq-func not
					// object -- result
				case 0305: {
					Ref val = PeekValue(0);
					if (val==NILREF) {	// NIL
						val = TRUEREF;	// TRUE
					} else {
						val = NILREF;	// NIL
					}
					SetValue(0, val);
					break; }
					
					// set-lex-scope
					// func -- closure
				case 004: {
					inState->SetByteCodePC(instrPtr);
					FastSetLexScope(inState, (KUInt32)pDataStack.GetBase());
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// iter-next
					// iterator --
				case 005: {
					inState->SetByteCodePC(instrPtr);
					FastIterNext(inState, (KUInt32)pDataStack.GetBase());
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// iter-done
					// iterator -- done
				case 006: {
					inState->SetByteCodePC(instrPtr);
					FastIterDone(inState, (KUInt32)pDataStack.GetBase());
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// pop-handlers
					// --
				case 007: {
					inState->SetByteCodePC(instrPtr);
					FastUnary1Ext(inState, (KUInt32)pDataStack.GetBase());
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// find-var
					// -- result
					// get name from literals then look up variable in
					// 1	lexical environment
					// 2	receiver
					// 3	globals
				case 0167:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0160:
				case 0161:
				case 0162:
				case 0163:
				case 0164:
				case 0165:
				case 0166: {
					inState->SetByteCodePC(instrPtr);
					FastFindVar(inState, B);
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// find-and-set-var
					// value --
				case 0257:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0250:
				case 0251:
				case 0252:
				case 0253:
				case 0254:
				case 0255:
				case 0256: {
					inState->SetByteCodePC(instrPtr);
					FastFindAndSetVar(inState, B);
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// make-array
					// b == 0xffff		size class -- array
					// b <  0xffff		val1 val2 ... valn class -- array
				case 0217:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0210:
				case 0211:
				case 0212:
				case 0213:
				case 0214:
				case 0215:
				case 0216: {
					inState->SetByteCodePC(instrPtr);
					FastMakeArray(inState, B);
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// make-frame
					// val1 val2 ... valn map -- frame
				case 0207:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0200:
				case 0201:
				case 0202:
				case 0203:
				case 0204:
				case 0205:
				case 0206: {
					inState->SetByteCodePC(instrPtr);
					FastMakeFrame(inState, B);
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// call
					// arg1 arg2 ... argn name -- result
				case 057:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 050:
				case 051:
				case 052:
				case 053:
				case 054:
				case 055:
				case 056: {
					inState->SetByteCodePC(instrPtr);
					functionType = FastCall(inState, B);
					instrPtr = inState->GetByteCodePC();
					if (functionType) {
						goto exitInnerLoop;
					}
					break; }
					
					// invoke
					// arg1 arg2 ... argn func -- result
				case 067:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 060:
				case 061:
				case 062:
				case 063:
				case 064:
				case 065:
				case 066: {
					inState->SetByteCodePC(instrPtr);
					functionType = FastInvoke(inState, B);
					instrPtr = inState->GetByteCodePC();
					if (functionType) {
						goto exitInnerLoop;
					}
					break; }
					
					// send
					// arg1 arg2 ... argn receiver name -- result
				case 077:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 070:
				case 071:
				case 072:
				case 073:
				case 074:
				case 075:
				case 076: {
					inState->SetByteCodePC(instrPtr);
					functionType = FastSend(inState, B);
					instrPtr = inState->GetByteCodePC();
					if (functionType) {
						goto exitInnerLoop;
					}
					break; }
					
					// send-if-defined
					// arg1 arg2 ... argn receiver name -- result
				case 0107:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0100:
				case 0101:
				case 0102:
				case 0103:
				case 0104:
				case 0105:
				case 0106: {
					inState->SetByteCodePC(instrPtr);
					functionType = FastSendIfDefined(inState, B);
					instrPtr = inState->GetByteCodePC();
					if (functionType) {
						goto exitInnerLoop;
					}
					break; }
					
					// resend
					// arg1 arg2 ... argn name -- result
				case 0117:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0110:
				case 0111:
				case 0112:
				case 0113:
				case 0114:
				case 0115:
				case 0116: {
					inState->SetByteCodePC(instrPtr);
					functionType = FastResend(inState, B);
					instrPtr = inState->GetByteCodePC();
					if (functionType) {
						goto exitInnerLoop;
					}
					break; }
					
					// resend-if-defined
					// arg1 arg2 ... argn name -- result
				case 0127:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0120:
				case 0121:
				case 0122:
				case 0123:
				case 0124:
				case 0125:
				case 0126: {
					inState->SetByteCodePC(instrPtr);
					functionType = FastResendIfDefined(inState, B);
					instrPtr = inState->GetByteCodePC();
					if (functionType) {
						goto exitInnerLoop;
					}
					break; }
					
					// branch-if-loop-not-done
					// incr index limit --
				case 0277:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0270:
				case 0271:
				case 0272:
				case 0273:
				case 0274:
				case 0275:
				case 0276: {
					inState->SetByteCodePC(instrPtr);
					FastBranchIfLoopNotDone(inState, B);
					instrPtr = inState->GetByteCodePC();
					break; }
					
					// freq-func
					// arg1 arg2 ... argn -- result
				case 0307: {
					inState->SetByteCodePC(instrPtr);
					KUInt32 ret = FastFreqFuncGeneral(inState, (KUInt32)pDataStack.GetBase());
					instrPtr = inState->GetByteCodePC();
					if (ret) {
						goto exitInnerLoop;
					}
					break; }
					
					// new-handlers
					// sym1 PC1 sym2 PC2 ... symn PCn --
				case 0317:
					B  = NewtReadByte(instrPtr++) << 8;
					B |= NewtReadByte(instrPtr++);
					// fall through
				case 0310:
				case 0311:
				case 0312:
				case 0313:
				case 0314:
				case 0315:
				case 0316: {
					inState->SetByteCodePC(instrPtr);
					FastNewHandlers(inState, B);
					instrPtr = inState->GetByteCodePC();
					break; }
					
				default: {
					inState->SetByteCodePC(instrPtr);
					FastUndefined(inState, (KUInt32)pDataStack.GetBase());
					instrPtr = inState->GetByteCodePC();
					break; }
					
					
			} // switch
			
		} // forever (inner)
		
	exitInnerLoop:
		KSInt32 stackDepth = pCtrlStack.GetTop() - pCtrlStack.GetBase() - 1;
		if (stackDepth<0)
			stackDepth += 3;
		stackDepth = (stackDepth>>2)-1;
		if (stackDepth<inInitialStackDepth)
			return 1;
		if (!IsFast())
			return 0;
	} // forever (outer)
	
}


// NOTE: the original function takes a Ref** !
void TInterpreter::SetValue(long index, Ref val)
{
	Ref* sp = pDataStack.GetTop();
	NewtWriteWord(sp+index-1, val);
}


// NOTE: the original function takes a Ref** !
void TInterpreter::PushValue(Ref val)
{
	Ref* sp = pDataStack.GetTop();
	NewtWriteWord(sp, val);
	sp++;
	pDataStack.SetTop(sp);
}


Ref TInterpreter::PeekValue(long index)
{
	Ref* sp = pDataStack.GetTop();
	return NewtReadWord(sp + index - 1);
}


Ref TInterpreter::PopValue()
{
	Ref* sp = pDataStack.GetTop();
	sp--;
	pDataStack.SetTop(sp);
	return NewtReadWord(sp);
}


KUInt32 TInterpreter::FastRun(long inInitialStackDepth)
{
	NEWT_LOCAL(FastRunState, tState);
	FastRunState* state = NEWT_LOCAL_PTR(tState);

	state->pImpl.SetRefHandle(AllocateRefHandle(NILREF));
	state->pRcvr.SetRefHandle(AllocateRefHandle(NILREF));
	state->pStackFrame.SetRefHandle(AllocateRefHandle(NILREF));
	state->pUnknown24.SetRefHandle(AllocateRefHandle(NILREF));
	state->SetInterpreter(this);
	state->SetStack(&pDataStack);
	KUInt32 ret = FastRun1(inInitialStackDepth, state);
	DisposeRefHandle(state->pUnknown24.GetRefHandle());
	DisposeRefHandle(state->pStackFrame.GetRefHandle());
	DisposeRefHandle(state->pRcvr.GetRefHandle());
	DisposeRefHandle(state->pImpl.GetRefHandle());
	return ret;
}


KUInt32 TInterpreter::AlternatingLoops(long inInitialStackDepth)
{
	KUInt32 ret;
	KUInt32 runFast = IsFast();
	for (;;) {
		if (runFast) {
			ret = FastRun(inInitialStackDepth);
			if (ret)
				return ret;
		} else {
			ret = SlowRun(inInitialStackDepth);
			if (ret)
				return ret;
		}
		runFast = !runFast;
	}
}


// incr index limit
KUInt32 FastBranchIfLoopNotDone(FastRunState* inState, long B)
{
	SimStack* stack = inState->GetStack();
	Ref* top = stack->GetTop();
	
	// pop loop limit from the stack
	Ref limitRef = NewtReadWord(top-1);
	if (!RefIsInt(limitRef))
		_RINTError(limitRef);
	long limit = RefToInt(limitRef);
	
	// pop loop index from the stack
	Ref indexRef = NewtReadWord(top-2);
	if (!RefIsInt(indexRef))
		_RINTError(indexRef);
	long index = RefToInt(indexRef);
	
	// pop loop increment from the stack
	Ref incrRef = NewtReadWord(top-3);
	if (!RefIsInt(incrRef))
		_RINTError(incrRef);
	long incr = RefToInt(incrRef);
	
	// update stack top
	stack->SetTop(top-3);
	
	// don't allow a zero incrment
	if (incr==0) {
		Throw("evt.ex.fr.intrp", -48804, 0); // FOR loop BY expression has value zero
		return 0;
	}
	
	// did the index pass the limit?
	if ( (incr<0 && limit<=index) || (incr>0 && limit>=index) ) {
		// if not, set the PC to the start of the loop
		KUInt8* newPC = inState->GetByteCodeBase() + B;
		inState->SetByteCodePC(newPC);
	}
	
	return 0;
}


// FIXME: next is KUInt32 TInterpreter::Run()
// This method contains a setjmp call to prepare for a later Throw() call.
// Something we have not solved yet.



// ----------------------------------------------------------------------------


NEWT_INJECTION(0x002ED9B8, "FastBranchIfLoopNotDone(FastRunState*, long)") {
	NEWT_RETVAL FastBranchIfLoopNotDone(NEWT_ARG0(FastRunState*), NEWT_ARG1(long));
	NEWT_RETURN;
}

NEWT_INJECTION(0x0031E684, "BinaryData(Ref)") {
	NEWT_RETVAL BinaryData(NEWT_ARG0(Ref));
	NEWT_RETURN;
}

NEWT_INJECTION(0x002F42A0, "TInterpreter::PeekValue(...)") {
	NEWT_RETVAL NEWT_CLASS(TInterpreter)->PeekValue(NEWT_ARG1(long));
	NEWT_RETURN;
}

NEWT_INJECTION(0x002F428C, "TInterpreter::PopValue(...)") {
	NEWT_RETVAL NEWT_CLASS(TInterpreter)->PopValue();
	NEWT_RETURN;
}

NEWT_INJECTION(0x002EE138, "TInterpreter::FastRun1(...)") {
	NEWT_RETVAL NEWT_CLASS(TInterpreter)->FastRun1(NEWT_ARG1(long), NEWT_ARG2(FastRunState*));
	NEWT_RETURN;
}

NEWT_INJECTION(0x002EE0A8, "TInterpreter::FastRun(...)") {
	NEWT_RETVAL NEWT_CLASS(TInterpreter)->FastRun(NEWT_ARG1(long));
	NEWT_RETURN;
}

NEWT_INJECTION(0x0031DD54, "ObjectPtr(Ref)") {
	NEWT_RETVAL ObjectPtr(NEWT_ARG0(Ref));
	NEWT_RETURN;
}

NEWT_INJECTION(0x002ECD94, "FastFreqFuncGeneral(FastRunState*, long)") {
	NEWT_RETVAL FastFreqFuncGeneral(NEWT_ARG0(FastRunState*), NEWT_ARG1(Ref));
	NEWT_RETURN;
}

NEWT_INJECTION(0x002F1CEC, "TInterpreter::SetFastLoopFlag(...)") {
	NEWT_CLASS(TInterpreter)->SetFastLoopFlag();
	NEWT_RETURN;
}

NEWT_INJECTION(0x0031E290, "ObjectFlags(...)") {
	NEWT_RETVAL ObjectFlags(NEWT_ARG0(Ref));
	NEWT_RETURN;
}

NEWT_INJECTION(0x002F543C, "TInterpreter::SetFlags(...)") {
	NEWT_CLASS(TInterpreter)->SetFlags();
	NEWT_RETURN;
}

NEWT_INJECTION(0x002F1D68, "TInterpreter::AlternatingLoops(...)") {
	NEWT_RETVAL NEWT_CLASS(TInterpreter)->AlternatingLoops(NEWT_ARG1(long));
	NEWT_RETURN;
}

//NEWT_INJECTION(0x002F1DB4, "TInterpreter::Run(...)") {
//	NEWT_RETVAL NEWT_CLASS(TInterpreter)->Run();
//	NEWT_RETURN;
//}

