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


TEmulator *TNewt::mEmulator = nullptr;	///< Reference back to the Emulator
TMemory *TNewt::mMemory = nullptr;		///< Reference back to emulated memeory
TARMProcessor *TNewt::mCPU = nullptr;	///< Reference back to the emulated CPU


/**
 This object is used to look up and call host functions by symbol name.

 This is a list of function calls from NewtonOS via teh EinsteinGlue.ntkc into functions
 inside the EInstein emulator. All calls have exactly one argument which can contain
 literals but alos complex hierarchies of arrays and frames. If the argument is unused,
 it should be NIL.

 On the Einstein side of thiks, these functions shall be names in camlecase starting
 with 'NS...'. They commonly make extensive use of the tools in the TNewt namespace
 to read paramaters from Newton memory into host space, and generate results that
 the calling NewtonScript can understand.

 A typical NewtonScript call into Einstein look like this:
	local index := call EInstein.Platform with ('GetSerialPortDriver, 'extr);

 Also available are
	call Einstein.Glue with (nil);
 which returns the version number of EinsteinGlue.ntkc, and
	call Einstein.Version with (nil);
 which return -1 if this is not running on Einstein, 0 if this is Einstein, but without TNewt, or
 simpe integer version number that will increase over time.

 The source code for EinsteinGlue.ntkc is in /Dirvers/packages/NTKGlue
 */
PlatformCallMap TNewt::CallMap = {
	{ "getserialportdrivernames", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSGetDriverNames(arg); }
	},
	{ "getserialportdriverlist", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSGetDriverList(arg); }
	},
	{ "getserialportdriveroptions", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSGetDriverOptions(arg); }
	},
	{ "setserialportdriveroptions", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSSetDriverOptions(arg); }
		// for TCP, send { serPort:'extr, serverAddr:"127.0.0.1", sereverPort:"3679" }
		// fprintf(stderr, "TODO: set serial port driver options\n");
	},
	{ "getsymbol", [](RefArg, RefArg arg)->NewtRef {
		// Just some silly testing of our implementatiosn
		RefVar array( AllocateArray(3) );
		RefVar text( MakeString("Will this ever work?") );
		SetArraySlot( array, 1, text );
		SetArraySlotRef(array.Ref(), 2, MakeReal(3.141592654));
		return array.Ref(); }
	},
};


/**
 Connect the TNewt namespace to the emulator, called by TEmulator upon creation.
 */
void TNewt::SetEmulator(TEmulator *inEmulator)
{
	mEmulator = inEmulator;
	mMemory = inEmulator->GetMemory();
	mCPU = inEmulator->GetProcessor();
}


/**
 Add a Ref to the Garbe Collection system and mark it used.
 */
VAddr TNewt::AllocateRefHandle(NewtRef r)
{
	return (VAddr)CallNewton(0x01800818, "0", r);
}

/**
 Mark the corresponding Ref unused and ready to be deleted (unless linked otherwise).
 */
void TNewt::DisposeRefHandle(VAddr v)
{
	CallNewton(0x01800888, "0", v);
}


/**
 Call functions inside the Newton ROM.

 Apple has left us a fixed jump table for over 3000 functions in ROM. They are
 meant to be called by native applications running on physical MessagePad.
 Since these jump tables are guaranteed to stay the same on every ROM, we
 use them to jump from Einstein right into ROM code.

 'args'' is a "C" string that indicates the type of the argument list
   's': 0-terminated "C" string
   'd': 64 bit double precission floating point value
   'A': RefArg.Handler()
   '0': Zero indirection, just use the 32-bit value
   'i': integer

 ARM 'C' calling conventions
   - the first four arguments are passed using R0 to R3
   - the following argument are passed through the stack
   - 'double' values take two registers or 64 bit on the stack
   - in 'C++' methods, 'this' is the first argument, followed by the rest
   - values are returned in R0 or R0/R1 for 'double'

 \note we need to implement a utf-8 to utf-16 option
 \note we need to set up an exception handler or exceptions will hang the machine
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
	int outArg = 0;
	va_start (ap, args);
	for (int i=0; ;i++) {
		char c = args[i];
		if (c==0) break;
		if (outArg>3 || (c=='d' && outArg>2)) {
			fprintf(stderr,
					"ERROR in TNewt::CallNewton: too many arguments "
					"(fix this by implementing arguments on the stack).\n");
			break;
		}
		switch (c) {
			case 's': { // C utf-8 string
				const char *s = va_arg(ap, char*);
				if (s) {
					KUInt32 n = (KUInt32)strlen(s)+1, n4 = (n+3) & ~3;
					sp -= n4;
					mMemory->FastWriteString(sp, &n, s);
					mCPU->SetRegister(outArg++, sp);
				} else {
					mCPU->SetRegister(outArg++, 0);
				}
				break; }
			case 'd': { // double precission floating point (needs two regsiters!
				union { double d; KUInt32 i[2]; } u;
				u.d = va_arg(ap, double);
				// FIXME: make sure that this order is the same on LSB and MSB machines (tested ok for Intelorder (LSB))
				mCPU->SetRegister(outArg++, u.i[1]);
				mCPU->SetRegister(outArg++, u.i[0]);
				break; }
			case 'A': { // TNewt::RefArg
				KUInt32 v = va_arg(ap, KUInt32);
				sp -= 4;
				mMemory->Write(sp, v);
				mCPU->SetRegister(outArg++, sp);
				sp -= 4;
				break; }
			case '0': { // zero indirection, just copy to register
				KUInt32 v = va_arg(ap, KUInt32);
				mCPU->SetRegister(outArg++, v);
				break; }
			case 'i': { // int
				KUInt32 v = va_arg(ap, KUInt32);
				mCPU->SetRegister(outArg++, v);
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

	// If we want to allow any of the code taht we will be calling to throw
	// exception, the current system will crash. We need to add an exception
	// handler before calling anything in this space. If an exception
	// occurs, a longjump is executed in Newton space, making it impossible
	// to end the emulation loop here.
	
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

/**
 Print any RefVar.
 \param arg print this
 \param depth optional print depth
 \param indent indent text this much for formatting
 */
void TNewt::Print(RefArg arg, int depth, int indent)
{
	// TODO: write me
}

/**
 Print any Ref.
 \param ref print this
 \param depth optional print depth
 \param indent indent text this much for formatting
 */
void TNewt::PrintRef(NewtRef ref, int depth, int indent)
{
	// TODO: write me
}

/**
 Create a string object from a C style ASCII string.
 */
NewtRef TNewt::MakeString(const char *txt)
{
	return (NewtRef)CallNewton(0x0180099c, "s", txt);
}

/**
 Create a symbol from a C style ASCII string.
 */
NewtRef TNewt::MakeSymbol(const char *txt)
{
	return (NewtRef)CallNewton(0x018029cc, "s", txt);
}

/**
 Create a binary object containing a double precission floating point value.
 */
NewtRef TNewt::MakeReal(double v)
{
	return (NewtRef)CallNewton(0x01800998, "d", v);
}

/**
 Allocate an empty Frame object.
 */
NewtRef TNewt::AllocateFrame()
{
	return (NewtRef)CallNewton(0x0180080c, ""); // AllocateFrame_Fv
}

/**
 Add a Slot to an array at the specified position.
 */
NewtRef TNewt::AddArraySlot(RefArg array, RefArg value)
{
	return CallNewton(0x018007f4, "AA", array.Handle(), value.Handle()); // AddArraySlot__FRC6RefVarT1
}

/**
 Allocate an array of a given size, marked with a symbol.
 */
NewtRef TNewt::AllocateArray(RefArg symbol, KUInt32 nSlots)
{
	return (NewtRef)CallNewton(0x01800804, "Ai", symbol.Handle(), nSlots); // AllocateArray__FRC6RefVarl
}

/**
 Allocate an array of the give size, marked 'Array.
 */
NewtRef TNewt::AllocateArray(KUInt32 nSlots)
{
	RefVar sym(MakeSymbol("array"));
	NewtRef arrRef = AllocateArray(sym, nSlots);
	return arrRef;
}

/**
 Quickly set a slot in an array.
 */
NewtRef TNewt::SetArraySlotRef(NewtRef array, KUInt32 index, NewtRef value)
{
	return CallNewton(0x01800a24, "0i0", array, index, value);
}

/**
 Set a slot in an array.
 */
NewtRef TNewt::SetArraySlot(RefArg array, KUInt32 index, RefArg value)
{
	return CallNewton(0x018029e4, "AiA", array.Handle(), index, value.Handle()); // SetArraySlot__FRC6RefVarlT1
}

/**
 Set a slot in a Frame.
 */
NewtRef TNewt::SetFrameSlot(RefArg frame, RefArg key, RefArg value)
{
	return CallNewton(0x01800a34, "AAA", frame.Handle(), key.Handle(), value.Handle());
}

/**
 Return true if the Ref is an integer.
 */
bool TNewt::RefIsInt(NewtRef r)
{
	return ((r&3)==0);
}

/**
 Convert the Ref to a signed integer value, no error checking.
 */
KSInt32 TNewt::RefToInt(NewtRef r)
{
	KSInt32 v = (KSInt32)r;
	return v>>2;
}

/**
 Convert a signed integer into a Ref.
 */
NewtRef TNewt::MakeInt(KSInt32 v)
{
	return ((KUInt32)v<<2) & 0xFFFFFFFC;
}

/**
 Return true, if the Ref is an NSSymbol.
 */
bool TNewt::RefIsSymbol(NewtRef r)
{
	if (!TNewt::RefIsPointer(r)) return false;
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 newtPtrClass = 0;
	mMemory->Read(p+8, newtPtrClass);
	if (newtPtrClass!=kNewtSymbolClass) return false;
	return true;
}

/**
 Copy the name of a symbol into a buffer in host space.
 */
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

/**
 Return true if the Ref is a utf16 string.
 */
//bool TNewt::RefIsString(NewtRef r)
//{
//	// TODO: write this
//	return false;
//}

/**
 Return the number of characters in the string.
 */
//KUInt32 TNewt::RefStringLength(NewtRef r)
//{
//	// TODO: write this
//	return 0;
//}

/**
 FIXME: return a string that must be fee'd by the caller.
 */
//char *TNewt::RefToStringDup(NewtRef r)
//{
//	// TODO: write this
//	return 0;
//}

/**
 Return true if the Ref is a pointer into NewtonOS memeory.
 */
bool TNewt::RefIsPointer(NewtRef r)
{
	return ((((KUInt32)r)&3)==1);
}

/**
 Convert a Ref into a pointer.
 */
KUInt32 TNewt::RefToPointer(NewtRef r)
{
	return ((KUInt32)r)&0xFFFFFFFC;
}

/**
 Convert a pointer into a Ref
 */
NewtRef TNewt::MakePointer(KUInt32 r)
{
	return (NewtRef)((r&0xFFFFFFFC)|1);
}

/**
 FIXME: don't use thsi yet, it will hang the emulator.
 */
NewtRef TNewt::ThrowBadTypeWithFrameData(long err, RefArg var)
{
	return (NewtRef)CallNewton(0x01802b28, "0A", err, var.Handle());
}


/**
 Create a RefVar from scratch. Not for the casual coder.
 */
TNewt::RefVar::RefVar(KUInt32 inRefHandle, bool inAllocated)
:	mRefHandle( inRefHandle )
,	mAllocated( inAllocated )
{
}

/**
 Create a RefVar from a Ref, protecting the Ref from deletion.
 */
TNewt::RefVar::RefVar(NewtRef ref)
{
	mAllocated = true;
	mRefHandle = AllocateRefHandle( ref );
}

/**
 Destroy the RefVar, making the Ref free for deletion (unless used elsewhere).
 */
TNewt::RefVar::~RefVar()
{
	if (mAllocated)
		DisposeRefHandle( mRefHandle );
}

/**
 Return the address of teh RefVar in Newton memory.
 */
KUInt32 TNewt::RefVar::Handle() const {
	return mRefHandle;
}

/**
 Return the Ref that we protect.
 */
NewtRef TNewt::RefVar::Ref() const
{
	NewtRef ref;
	mMemory->Read(mRefHandle, ref);
	return ref;
}

/**
 Named Constructor: create a RefArg form a RefVar.
 */
RefVar RefVar::FromPtr(KUInt32 ptr)
{
	KUInt32 hdl;
	mMemory->Read(ptr, hdl);
	return RefVar(hdl, false);
}




// ====================================================================== //
// Nurse Donna:    Oh, Groucho, I'm afraid I'm gonna wind up an old maid. //
// Groucho:        Well, bring her in and we'll wind her up together.     //
// Nurse Donna:    Do you believe in computer dating?                     //
// Groucho:        Only if the computers really love each other.          //
// ====================================================================== //
