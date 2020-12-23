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
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// K
#include <K/Unicode/UUTF16CStr.h>

// Einstein
#include "Emulator/TMemory.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"


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

 On the Einstein side, these functions shall be names in camelcase starting
 with 'NS...'. They commonly make extensive use of the tools in the TNewt namespace
 to read paramaters from Newton memory into host space, and generate results that
 the calling NewtonScript can understand.

 A typical NewtonScript call into Einstein look like this:
	local index := call Einstein.Platform with ('GetSerialPortDriver, 'extr);

 Also available are
	call Einstein.Glue with (nil);
 which returns the version number of EinsteinGlue.ntkc, and
	call Einstein.Version with (nil);
 which return -1 if this is not running on Einstein, 0 if this is Einstein, but without TNewt, or
 simpe integer version number that will increase over time.

 The source code for EinsteinGlue.ntkc is in /Dirvers/packages/NTKGlue
 */
PlatformCallMap TNewt::CallMap = {
	{ "print", [](RefArg, RefArg arg)->NewtRef {
		TNewt::Print(arg); return kNewtRefNIL; }
	},
	{ "getserialportdrivernames", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSGetDriverNames(arg); }
	},
	{ "getserialportdriverlist", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSGetDriverList(arg); }
	},
	{ "getserialportdriverandoptions", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSGetDriverAndOptions(arg); }
	},
	{ "setserialportdriverandoptions", [](RefArg, RefArg arg)->NewtRef {
		return mEmulator->SerialPorts.NSSetDriverAndOptions(arg); }
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


// WITH_LOCKED_BINARY
KUInt32 TNewt::LockedBinaryPtr(RefArg arg)
{
	return (VAddr)CallNewton(0x018029bc, "A", arg.Handle());
}

// END_WITH_LOCKED_BINARY
void TNewt::UnlockRefArg(RefArg arg)
{
	CallNewton(0x01802b54, "A", arg.Handle());
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
	PrintRef(arg.Ref(), depth, indent);
}

/**
 Print any Ref.
 \param ref print this
 \param depth optional print depth
 \param indent indent text this much for formatting
 */
void TNewt::PrintRef(NewtRef ref, int depth, int indent)
{
	static char space[] = "                                        ";
	char buffer[128];

	if (depth<0) {
		printf("%.*s0x%08x\n", indent, space, ref);
		return;
	}

	if (RefIsInt(ref)) {
		printf("%.*s%d\n", indent, space, RefToInt(ref));
	} else if (ref==kNewtRefNIL) {
		printf("%.*snil\n", indent, space);
	} else if (ref==kNewtRefTRUE) {
		printf("%.*strue\n", indent, space);
	} else if (RefIsReal(ref)) {
		printf("%.*s%f\n", indent, space, RefToReal(ref));
	} else if (RefIsString(ref)) {
		RefToString(ref, buffer, 128);
		printf("%.*s\"%s\"\n", indent, space, buffer);
	} else if (RefIsSymbol(ref)) {
		SymbolToCString(ref, buffer, 128);
		printf("%.*s'%s\n", indent, space, buffer);
//	} else if (RefIsChar(ref)) {
//		KUInt32 u = SymbolToChar(ref);
//		printf("%.*s#%s\n", unicodeToUtf8(u));
	} else if (RefIsPointer(ref)) {
		VAddr obj = RefToPointer(ref);
		if (RefIsBinary(ref)) { // TODO: use 'ObjIsBinary()' etc.
			NewtRef s = RefArrayGetSlot(ref, -1);
			SymbolToCString(s, buffer, 128);
			KUInt32 n = RefArrayGetNumSlots(ref);
			printf("%.*s{ Binary: '%s, %d bytes }\n", indent, space, buffer, n*4);
		} else if (RefIsArray(ref)) {
			NewtRef sym = RefArrayGetSlot(ref, -1);
			SymbolToCString(sym, buffer, 128);
			printf("%.*s[ '%s\n", indent, space, buffer);
			KUInt32 n = RefArrayGetNumSlots(ref);
			for (KUInt32 i=0; i<n; i++) {
				NewtRef s = RefArrayGetSlot(ref, i);
				PrintRef(s, depth-1, indent+2);
			}
			printf("%.*s]\n", indent, space);
		} else if (RefIsFrame(ref)) {
			NewtRef sym = RefArrayGetSlot(ref, -1);
			SymbolToCString(sym, buffer, 128);
			printf("%.*s{ '%s\n", indent, space, buffer);
			KUInt32 n = RefArrayGetNumSlots(ref);
			NewtRef map = RefArrayGetSlot(ref, -1);
			for (KUInt32 i=0; i<n; i++) {
				NewtRef key = RefArrayGetSlot(map, i+1);
				NewtRef value = RefArrayGetSlot(ref, i);
				SymbolToCString(key, buffer, 128);
				printf("%.*s%s:\n", indent+2, space, buffer);
				PrintRef(value, depth-1, indent+4);
			}
			printf("%.*s}\n", indent, space);
		} else {
			KUInt32 flags = 0;
			mMemory->Read(obj, flags);
			printf("WARNING: TNewt::PrintRef: Unknown Object type at 0x%08x: 0x%08x\n", obj, flags);
		}
	} else {
		printf("WARNING: TNewt::PrintRef: Unknown Ref type: 0x%08x\n", ref);
	}
	/*
	kObjSlotted		= 0x01, \ 0 = binary, 1 = array, 2 = large binary, 3 = frame
	kObjFrame		= 0x02, /
	kObjFree		= 0x04,
	kObjMarked		= 0x08,
	kObjLocked		= 0x10,
	kObjForward		= 0x20, -> Handled in RefToPointer
	kObjReadOnly	= 0x40,
	kObjDirty		= 0x80,
	*/
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
 Return true if the Ref is a floating point value.
 */
bool TNewt::RefIsReal(NewtRef r)
{
	// TODO: duplicating code!
	if (!TNewt::RefIsPointer(r))
		return false;
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 type = 0;
	mMemory->Read(p, type);
	if ((type&3)!=0) return false;
	KUInt32 newtPtrClass = 0;
	mMemory->Read(p+8, newtPtrClass);
	if (!TNewt::RefIsPointer(newtPtrClass))
		return false;

	char buffer[80];
	SymbolToLowerCaseCString(newtPtrClass, buffer, 80);
	return (strcmp(buffer, "real")==0);
}

/**
 Return a floating point value.
 */
double TNewt::RefToReal(NewtRef r)
{
	if (!TNewt::RefIsReal(r))
		return false;
	KUInt32 p = TNewt::RefToPointer(r);

	union { double d; KUInt32 i[2]; } u;
	mMemory->Read(p+12, u.i[1]);
	mMemory->Read(p+16, u.i[0]);
	return u.d;
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
 Get the slot in a frame.
 */
NewtRef TNewt::GetFrameSlotRef(NewtRef frame, NewtRef symbol)
{
	return CallNewton(0x0180092c, "00", frame, symbol);
}

/**
 Get the slot in a frame.
 */
NewtRef TNewt::GetFrameSlot(RefArg frame, RefArg symbol)
{
	return CallNewton(0x01802988, "AA", frame.Handle(), symbol.Handle());
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
	KUInt32 type = 0;
	mMemory->Read(p, type);
	if ((type&3)!=0) return false;
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
	*buf = 0;
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

	return true;
}

/**
 Copy the name of a symbol and convert it to all lower case characters.
 */
bool TNewt::SymbolToLowerCaseCString(NewtRef r, char *buf, int size)
{
	bool ret;
	if ( (ret = SymbolToCString(r, buf, size)) ) {
		int strSize = (int) strlen(buf);
		for (int i=0; i<strSize; i++)
			buf[i] = tolower(buf[i]&0x7f);
	}
	return ret;
}

/**
 Return true if the Ref is a utf16 string.
 */
bool TNewt::RefIsString(NewtRef r)
{
	if (!TNewt::RefIsPointer(r))
		return false;
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 type = 0;
	mMemory->Read(p, type);
	if ((type&3)!=0) return false;
	KUInt32 newtPtrClass = 0;
	mMemory->Read(p+8, newtPtrClass);
	if (!TNewt::RefIsPointer(newtPtrClass))
		return false;

	char buffer[80];
	SymbolToLowerCaseCString(newtPtrClass, buffer, 80);
	return (strcmp(buffer, "string")==0);
}

/**
 Return the number of characters in the string.
 */
KUInt32 TNewt::RefStringLength(NewtRef r)
{
    if (!TNewt::RefIsString(r))
        return false;
    KUInt32 p = TNewt::RefToPointer(r);
    KUInt32 newtSize = 0;
    mMemory->Read(p, newtSize);
    int strSize = ((newtSize>>8)-12)/2;
	return strSize;
}

// Return a string in utf-8
bool TNewt::RefToString(NewtRef r, char *buf, int bufSize)
{
	*buf = 0;
	if (!TNewt::RefIsString(r))
		return false;

	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 newtSize = 0;
	mMemory->Read(p, newtSize);
	int strSize = ((newtSize>>8)-12)/2;
	if (strSize>=bufSize)
		strSize = bufSize-1;
    // TODO: this is only a bad ASCII interpretation, write the utf8 conversion!
	for (int i=0; i<strSize; i++) {
		KUInt8 c;
		mMemory->ReadB(p+13+2*i, c);
		buf[i] = c;
	}
	buf[strSize] = 0;
	return true;
}

/**
 Return true if the Ref is a pointer into NewtonOS memeory.
 */
bool TNewt::RefIsPointer(NewtRef r)
{
	return ((((KUInt32)r)&3)==1);
}

/**
 Convert a Ref into a pointer.
 If the result points toa forwarding object, follow the link until we find the actual object.
 */
KUInt32 TNewt::RefToPointer(NewtRef r)
{
	for (;;) {
		// Make sure that this is a Ref, return kNewtNullptr if not
		if ((r&3)!=1) {
			printf("ERROR: TNewt::RefToPointer: not a pointer (0x%08x)!\n", r);
			return kNewtNullptr;
		}
		VAddr ptr = (r&~3);
		KUInt32 objFlags;
		// return 'true' if there was a fault reading this address
		if (mMemory->Read(ptr, objFlags)==true) {
			printf("ERROR: TNewt::RefToPointer: invalid object address for flags: 0x%08x!\n", ptr);
			return kNewtNullptr;
		}
		// check, if the 'forward' flag is clear
		if ( (objFlags&0x20)==0 ) // 0x00000c20
			return ptr;
		// read the Ref that forwards to the requested object
		if (mMemory->Read(ptr+8, r)==true) {
			printf("ERROR: TNewt::RefToPointer: invalid object address for indirection: 0x%08x!\n", ptr+8);
			return kNewtNullptr;
		}
		// repeat until satisfied
	}
}

/**
 Convert a pointer into a Ref
 */
NewtRef TNewt::MakePointer(KUInt32 r)
{
	return (NewtRef)((r&0xFFFFFFFC)|1);
}

/**
 Return true if the Ref is an array
 */
bool TNewt::RefIsArray(NewtRef r)
{
	if (!TNewt::RefIsPointer(r))
		return false;
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 size = 0;
	mMemory->Read(p, size);
	return ( (size&0x03)==0x01 );
}

/**
 Return true if the Ref is a frame.
 */
bool TNewt::RefIsFrame(NewtRef r)
{
	if (!TNewt::RefIsPointer(r))
		return false;
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 size = 0;
	mMemory->Read(p, size);
	return ( (size&0x03)==0x03 );
}

/**
 Return true if the Ref is a frame.
 */
bool TNewt::RefIsBinary(NewtRef r)
{
	if (!TNewt::RefIsPointer(r))
		return false;
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 size = 0;
	mMemory->Read(p, size);
	return ( (size&0x03)==0x00 );
}

KUInt32 TNewt::RefArrayGetNumSlots(NewtRef r)
{
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 size = 0;
	mMemory->Read(p, size);
	return ((size>>8)-12)/4;
}

NewtRef TNewt::RefArrayGetSlot(NewtRef r, int i)
{
	KUInt32 p = TNewt::RefToPointer(r);
	KUInt32 ret = 0;
	mMemory->Read(p+12+4*i, ret);
	return ret;
}


/**
 FIXME: don't use this yet, it will hang the emulator.
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
