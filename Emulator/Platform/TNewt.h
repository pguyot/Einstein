// ==============================
// File:			TNewt.h
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

#ifndef _T_NEWT_H
#define _T_NEWT_H

#include <K/Defines/KDefinitions.h>

#include <map>
#include <string>

// Einstein
class TEmulator;
class TMemory;
class TARMProcessor;

// Some very basic types
typedef KUInt32 VAddr;
typedef KUInt32 NewtRef;

// Some constant Refs
static const NewtRef kNewtRefNIL = 0x0002;
static const NewtRef kNewtRefTRUE = 0x001A;
static const NewtRef kNewtSymbolClass = 0x55552;

static const VAddr kNewtNullptr = 0;

// A list of useful error codes
static const KSInt32 kNSErrUndefinedMethod = -48809;
static const KSInt32 kNSErrNotASymbol = -48410;

#define kNSErrBase					(-48000)
#define kNSErrNotAFrame				(kNSErrBase - 400)	// Expected a frame
#define kNSErrNotAnArray			(kNSErrBase - 401)	// Expected an array
#define kNSErrNotAString			(kNSErrBase - 402)	// Expected a string
#define kNSErrNotAPointer			(kNSErrBase - 403)	// Expected a pointer, array, or binary object
#define kNSErrNotANumber			(kNSErrBase - 404)	// Expected a number
#define kNSErrNotAReal				(kNSErrBase - 405)	// Expected a real
#define kNSErrNotAnInteger			(kNSErrBase - 406)	// Expected an integer
#define kNSErrNotACharacter			(kNSErrBase - 407)	// Expected a character
#define kNSErrNotABinaryObject		(kNSErrBase - 408)	// Expected a binary object
#define kNSErrNotAPathExpr			(kNSErrBase - 409)	// Expected a path expression (or a symbol or integer)
#define kNSErrNotASymbol			(kNSErrBase - 410)	// Expected a symbol
#define kNSErrNotAFunction			(kNSErrBase - 411)	// Expected a function
#define kNSErrNotAFrameOrArray		(kNSErrBase - 412)	// Expected a frame or an array
#define kNSErrNotAnArrayOrNil		(kNSErrBase - 413)	// Expected an array or NIL
#define kNSErrNotAStringOrNil		(kNSErrBase - 414)	// Expected a string or NIL
#define kNSErrNotABinaryObjectOrNil	(kNSErrBase - 415)	// Expected a binary object or NIL
#define kNSErrUnexpectedFrame		(kNSErrBase - 416)	// Unexpected frame
#define kNSErrUnexpectedBinaryObject	(kNSErrBase - 417)	// Unexpected binary object
#define kNSErrUnexpectedImmediate	(kNSErrBase - 418)	// Unexpected immediate
#define kNSErrNotAnArrayOrString	(kNSErrBase - 419)	// Expected an array or string
#define kNSErrNotAVBO				(kNSErrBase - 420)	// Expected a vbo
#define kNSErrNotAPackage			(kNSErrBase - 421)	// Expected a package
#define kNSErrNotNil				(kNSErrBase - 422)	// Expected a NIL
#define kNSErrNotASymbolOrNil		(kNSErrBase - 423)  // expected NIL or a Symbol
#define kNSErrNotTrueOrNil			(kNSErrBase - 424)  // expected NIL or True
#define kNSErrNotAnIntegerOrArray	(kNSErrBase - 425)  // expected an integer or an array


/**
 A namespace that provides access to a running Newton system.

 This interface allows us to read and create NewtonScript objects right inside
 a running OS. Some functions are reimplemented on the host machine and use
 the memory manager to access emulated RAM and ROM, and some functions
 call code inside the ROM via a minimalized JIT emulator loop. These functions
 can only be called from within the JIT itself, for example from within
 TPlatformManager::NewtonScriptCall(RefArg rcvr, RefArg arg0, RefArg arg1)

 \note At some point, we want to cahe sybols, so we don't doe the expensive NewtonOS lookup.
 */
namespace TNewt
{

// Back references into the emulator
extern TEmulator *mEmulator;
extern TMemory *mMemory;
extern TARMProcessor *mCPU;

// Setthe all back references into the emulator.
void SetEmulator(TEmulator *inEmulator);

// Add a Ref to the Garbe Collection system and mark it used
VAddr AllocateRefHandle(NewtRef);

// Mark the corresponding Ref unused and ready to be deleted (unless linked otherwise)
void DisposeRefHandle(VAddr);

/**
 Create and dispose RefHandles automatically.
 */
class RefVar {
protected:
	KUInt32 mRefHandle;
	Boolean mAllocated;
	RefVar(KUInt32 inRefHandle, Boolean inAllocated);
public:
	// Named Constrcutor
	static RefVar FromPtr(KUInt32 ptr);
	// Standard Constructor
	RefVar(NewtRef ref);
	// Destructor
	~RefVar();
	// Return the address of the RefHandle in emulated memory
	KUInt32 Handle() const;
	// Return the Ref for this Handle
	NewtRef Ref() const;
};

// Use this to pass arguments to functions
typedef const RefVar &RefArg;

KUInt32 LockedBinaryPtr(RefArg);

void UnlockRefArg(RefArg);


// Call a NewtonOS function in ROM via the Apple sanction jumptable
KUInt32 CallNewton(VAddr functionVector, const char *args, ...);

// Print any RefVar
void Print(RefArg, int depth=2, int indent = 0);

// Print any Ref
void PrintRef(NewtRef, int depth=2, int indent = 0);

// Create a string object from a C style ASCII string
NewtRef MakeString(const char *);

// Create a symbol from a C style ASCII string
NewtRef MakeSymbol(const char *);

// Return true if the Ref is a floating point value
Boolean RefIsReal(NewtRef);

// Return a floating point value
double RefToReal(NewtRef);

// Create a binary object containing a double precission floating point value
NewtRef MakeReal(double);

// Allocate an empty Frame object
NewtRef AllocateFrame();

// Add a Slot to an array at the specified position
NewtRef AddArraySlot(RefArg, RefArg);

// Allocate an array of a given size, marked with a symbol
NewtRef AllocateArray(RefArg, KUInt32);

// Allocate an array of the give size, marked 'Array
NewtRef AllocateArray(KUInt32);

// Quickly set a slot in an array
NewtRef SetArraySlotRef(NewtRef, KUInt32, NewtRef);

// Set a slot in an array
NewtRef SetArraySlot(RefArg, KUInt32, RefArg);

// Set a slot in a Frame
NewtRef SetFrameSlot(RefArg, RefArg, RefArg);

// Get the slot in a frame
NewtRef GetFrameSlotRef(NewtRef frame, NewtRef symbol);

// Get the slot in a frame
NewtRef GetFrameSlot(RefArg frame, RefArg symbol);

// Return true if the Ref is an integer
Boolean RefIsInt(NewtRef);

// Convert the Ref to a signed integer value, no error checking
KSInt32 RefToInt(NewtRef);

// Convert a signed integer into a Ref
NewtRef MakeInt(KSInt32);

// Return true, if the Ref is an NSSymbol
Boolean RefIsSymbol(NewtRef);

// Copy the name of a symbol into a buffer in host space
Boolean SymbolToCString(NewtRef, char *buf, int size);

// Copy the name of a symbol and convert it to all lower case characters
Boolean SymbolToLowerCaseCString(NewtRef, char *buf, int size);

// Return true if the Ref is a utf16 string
Boolean RefIsString(NewtRef);

// Return the number of characters in the string
KUInt32 RefStringLength(NewtRef);

// Return a string in utf-8
Boolean RefToString(NewtRef, char *buf, int size);

// Return true if the Ref is a pointer into NewtonOS memeory
Boolean RefIsPointer(NewtRef);

// Convert a Ref into a pointer
KUInt32 RefToPointer(NewtRef);

// Convert a pointer into a Ref
NewtRef MakePointer(KUInt32);

// Return true if the Ref is an array
Boolean RefIsArray(NewtRef);

// Return true if the Ref is a frame
Boolean RefIsFrame(NewtRef);

// Return true if the Ref is a frame
Boolean RefIsBinary(NewtRef);

KUInt32 RefArrayGetNumSlots(NewtRef r);
NewtRef RefArrayGetSlot(NewtRef r, int i);


// FIXME: don't use thsi yet, it will hang the emulator
NewtRef ThrowBadTypeWithFrameData(long, RefArg);

// Jump table from the Platform Manager calls into their implementation
typedef NewtRef (*PlatformCall)(RefArg rcvr, RefArg arg);
typedef std::map<std::string, PlatformCall> PlatformCallMap;
extern PlatformCallMap CallMap;

}

#endif
// _T_NEWT_H

// ======================================================================= //
// FORTRAN is not a flower but a weed -- it is hardy, occasionally blooms, //
// and grows in every computer.                                            //
//                 -- A.J. Perlis                                          //
// ======================================================================= //
