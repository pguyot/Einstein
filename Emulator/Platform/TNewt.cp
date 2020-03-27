// ==============================
// File:			TNewt.cp
// Project:			Einstein
//
// Copyright 2020, Matthias Melcher.
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

#include "TNewt.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// K
#include <K/Unicode/UUTF16CStr.h>

// Einstein
#include "TMemory.h"
#include "TARMProcessor.h"
#include "TEmulator.h"


using namespace TNewt;

TEmulator *TNewt::mEmulator = nullptr;
TMemory *TNewt::mMemory = nullptr;
TARMProcessor *TNewt::mCPU = nullptr;

void TNewt::SetEmulator(TEmulator *inEmulator)
{
	mEmulator = inEmulator;
	mMemory = inEmulator->GetMemory();
	mCPU = inEmulator->GetProcessor();
}

/*
 Ags is a "C" string that indicates the type of the argument list
 - s: 0-terminated "C" string
 ? u: 0-terminated "C" string in utf8 that is passed as a MSB utf16 string
 ? i: 32 bit integer
 - d: 64 bit double precission floating point valu
 ? R: a NewtonsScript Ref
 */
KUInt32 TNewt::CallNewton(VAddr functionVector, const char *args, ...)
{
	KUInt32 regs[16];
	va_list ap;
	KUInt32 sp = mCPU->GetRegister(13);
	KUInt32 pc = functionVector;
	KUInt32 r0 = 0;

	// backing up all registers
	memcpy(regs, mCPU->mCurrentRegisters, sizeof(regs));

	// Convert the list of arguments into ARM function call convention
	va_start (ap, args);
	for (int i=0; ;i++) {
		char c = args[i];
		if (c==0) break;
		if (i>3) {
			fprintf(stderr, "ERROR in TNewt::CallNewton: too many arguments\n");
			break;
		}
		switch (c) {
			case 's': { // C utf-8 string
				const char *s = va_arg(ap, char*);
				if (s) {
					KUInt32 n = (KUInt32)strlen(s)+1, n4 = (n+3) & ~3;
					sp -= n4;
					mMemory->FastWriteString(sp, &n, s);
					mCPU->SetRegister(i, sp);
				} else {
					mCPU->SetRegister(i, 0);
				}
				break; }
			case 'd': { // double precission floating point
				union { double d; KUInt32 i[2]; } u;
				u.d = va_arg(ap, double);
				// FIXME: make sure that this order is the same on LSB and MSB machines (tested ok for Intelorder (LSB))
				mCPU->SetRegister(i+1, u.i[0]);
				mCPU->SetRegister(  i, u.i[1]);
				// FIXME: wrong argument counting!
				i++;
				break; }
			case 'A': { // NewtRefArg
				KUInt32 v = va_arg(ap, KUInt32);
				sp -= 4;
				mMemory->Write(sp, v);
				mCPU->SetRegister(i, sp);
				sp -= 4;
				break; }
			case '0': { // zero indirection, just copy to register
				KUInt32 v = va_arg(ap, KUInt32);
				mCPU->SetRegister(i, v);
				break; }
			case 'i': { // int
				KUInt32 v = va_arg(ap, KUInt32);
				mCPU->SetRegister(i, v);
				break; }
			default:
				fprintf(stderr, "ERROR in TNewt::CallNewton: unspported argument type '%c'\n", c);
				break;
		}
	}
	va_end(ap);

	// Preset the return address for the emulator loop
    mCPU->SetRegister(13, sp);
	mCPU->SetRegister(14, 0x00fffffc); // magic address at the end of the REx
	mCPU->SetRegister(15, pc);

	TJITGeneric *mJit = mMemory->GetJITObject();
	JITUnit* theJITUnit = mJit->GetJITUnitForPC( mCPU, mMemory, pc+4);
	for(;;)
	{
		theJITUnit = theJITUnit->fFuncPtr( theJITUnit, mCPU );
		if (mCPU->mCurrentRegisters[15]==0x00fffffc+4) break;
	}

	r0 = mCPU->GetRegister(0);

	// Restoring all registers
	memcpy(mCPU->mCurrentRegisters, regs, sizeof(regs));
	return r0;
}

// Tested and OK
NewtRef TNewt::MakeString(const char *txt)
{
	return (NewtRef)CallNewton(0x0180099c, "s", txt);
}

// Tested and OK
NewtRef TNewt::MakeSymbol(const char *txt)
{
	return (NewtRef)CallNewton(0x018029cc, "s", txt);
}

// Tested and OK
NewtRef TNewt::MakeReal(double v)
{
	return (NewtRef)CallNewton(0x01800998, "d", v);
}

// TODO: implement argument types and RefVar
NewtRefVar TNewt::AllocateRefHandle(NewtRef r)
{
	return (NewtRefVar)CallNewton(0x01800818, "0", r); // AllocateRefHandle_Fl
}

// TODO: implement argument types and RefVar
void TNewt::DisposeRefHandle(NewtRefVar v) // DisposeRefHandle_FP9RefHandle
{
	CallNewton(0x01800888, "0", v);
}

// TODO: implement argument types and RefVar
NewtRef TNewt::AllocateFrame()
{
	return (NewtRef)CallNewton(0x0180080c, ""); // AllocateFrame_Fv
}

// TODO: implement argument types and RefVar
KUInt32 TNewt::AddSlot(NewtRefArg frame, NewtRefArg slot)
{
	return CallNewton(0x018007fc, "AA", frame, slot); // AddSlot__FRC6RefVarTl
}

NewtRef TNewt::AddArraySlot(NewtRefArg array, NewtRefArg value)
{
	return (NewtRef)CallNewton(0x018007f4, "AA"); // AddArraySlot__FRC6RefVarT1
}

// Works
NewtRef TNewt::AllocateArray(NewtRefArg symbol, KUInt32 nSlots)
{
	return (NewtRef)CallNewton(0x01800804, "Ai", symbol, nSlots); // AllocateArray__FRC6RefVarl
}

NewtRef TNewt::AllocateArray(KUInt32 nSlots)
{
	NewtRefVar sym = AllocateRefHandle(MakeSymbol("array"));
	NewtRef arrRef = AllocateArray(sym, nSlots);
	DisposeRefHandle(sym);
	return arrRef;
}

// works
NewtRef TNewt::SetArraySlotRef(NewtRef array, KUInt32 index, NewtRef value)
{
	return (NewtRef)CallNewton(0x01800a24, "0i0", array, index, value);
}

// works
NewtRef TNewt::SetArraySlot(NewtRefArg array, KUInt32 index, NewtRefArg value)
{
	return (NewtRef)CallNewton(0x018029e4, "AiA", array, index, value); // SetArraySlot__FRC6RefVarlT1
}

// works
NewtRef TNewt::SetFrameSlot(NewtRefArg frame, NewtRefArg key, NewtRefArg value)
{
	return (NewtRef)CallNewton(0x01800a34, "AAA", frame, key, value);
}

bool TNewt::RefIsInt(NewtRef r)
{
	return ((r&3)==0);
}

KSInt32 TNewt::RefToInt(NewtRef r)
{
	KSInt32 v = (KSInt32)r;
	return v>>2;
}

NewtRef TNewt::MakeInt(KSInt32 v)
{
	return ((KUInt32)v<<2) & 0xFFFFFFFC;
}

bool TNewt::RefIsSymbol(NewtRef r)
{
	if (!TNewt::RefIsPointer(r)) return false;
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 newtPtrClass = 0;
	mMemory->Read(p+8, newtPtrClass);
	if (newtPtrClass!=kNewtSymbolClass) return false;
	return true;
}

bool TNewt::SymbolToCString(NewtRef r, char *buf, int bufSize)
{
	if (!TNewt::RefIsPointer(r))
		return false;

	KUInt32 p = TNewt::RefToPointer(r);

	KUInt32 newtPtrClass = 0;
	mMemory->Read(p+8, newtPtrClass);
	if (newtPtrClass!=kNewtSymbolClass)
		return false;

	KUInt32 newtSize = 0;
	mMemory->Read(p, newtSize);
	int strSize = (newtSize>>8)-16;
	if (strSize>bufSize)
		return false;

	mMemory->FastReadBuffer(p+16, strSize, (KUInt8*)buf);

	for (int i=0; i<strSize; i++)
		buf[i] = tolower(buf[i]&0x7f);

	return true;
}

//bool TNewt::RefIsString(NewtRef r)
//{
//	// TODO: write this
//	return false;
//}
//
//KUInt32 TNewt::RefStringLength(NewtRef r)
//{
//	// TODO: write this
//	return 0;
//}
//
//char *TNewt::RefToStringDup(NewtRef r)
//{
//	// TODO: write this
//	return 0;
//}
//
//NewtRef TNewt::RefReplaceString(NewtRef, const char*)
//{
//	// TODO: write this
//	return kNewtRefNIL;
//}

bool TNewt::RefIsPointer(NewtRef r)
{
	return ((((KUInt32)r)&3)==1);
}

KUInt32 TNewt::RefToPointer(NewtRef r)
{
	return ((KUInt32)r)&0xFFFFFFFC;
}

NewtRef TNewt::MakePointer(KUInt32 r)
{
	return (NewtRef)((r&0xFFFFFFFC)|1);
}


NewtRef TNewt::RefVar::Ref()
{
	NewtRef ref;
	mMemory->Read(mRefHandle, ref);
	return ref;
}


// ====================================================================== //
// Nurse Donna:    Oh, Groucho, I'm afraid I'm gonna wind up an old maid. //
// Groucho:        Well, bring her in and we'll wind her up together.     //
// Nurse Donna:    Do you believe in computer dating?                     //
// Groucho:        Only if the computers really love each other.          //
// ====================================================================== //
