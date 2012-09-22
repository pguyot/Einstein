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



KUInt32 ObjectPtr(KUInt32 r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, r0);
	NewtCallJIT(0x0031DD54);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
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



KUInt32 FastFreqFuncGeneral(FastRunState* inState, KSInt32)
{
	SimStack*	currentStack;
	KUInt32*	sp;
	Ref			arg1, arg2, result;

	KUInt8* currentPC = inState->GetPC();
	KUInt16 B = (NewtReadByte(currentPC)<<8) + NewtReadByte(currentPC+1);
	inState->SetPC(currentPC+2);

	switch (B) {
		case 7: // MUL
			currentStack = inState->GetStack();
			sp = currentStack->GetTop();
			arg1 = NewtReadWord(sp-1);
			arg2 = NewtReadWord(sp-2);
			if ( ISINT(arg1) && ISINT(arg2) ) {
				result = arg1 * (arg2/4);
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
					result = 4 * ( (arg2/4) / (arg1/4) );
				} else {
					result = 4 * rt_sdiv( arg1/4, arg2/4 );
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
	ArrayObject* freqFuncs = (ArrayObject*)ObjectPtr(freqFuncsRef);
	Ref thisFunc = freqFuncs->GetSlot(B);
	
	FreqFuncDef* funcInfo = GlobalGetFreqFuncInfo();
	KUInt32 numArgs = funcInfo[B].GetNumOfArgs();
	
	return FastDoCall(inState, thisFunc, numArgs);
}


NEWT_INJECTION(0x002ECD94, "FastFreqFuncGeneral(FastRunState*, long)") {
	NEWT_RETVAL FastFreqFuncGeneral(NEWT_ARG0(FastRunState*), NEWT_ARG1(Ref));
	NEWT_RETURN;
}

