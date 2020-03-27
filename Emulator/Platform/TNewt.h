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

// Einstein

class TEmulator;
class TMemory;
class TARMProcessor;

typedef KUInt32 VAddr;
typedef KUInt32 NewtRef;
typedef KUInt32 NewtRefVar;	// FIXME: nonono!
typedef const NewtRefVar &NewtRefArg;

static const NewtRef kNewtRefNIL = 0x0002;
static const NewtRef kNewtRefTRUE = 0x001A;
static const NewtRef kNewtSymbolClass = 0x55552;

static const KSInt32 kNSErrUndefinedMethod = -48809;
static const KSInt32 kNSErrNotASymbol = -48410;

/**
 A static class that provides access to a running Newton system.

 This interface allows us to read and create NewtonScript objects right inside
 a running OS. Some functions are reimplemented on the host machine and use
 the memory manager to access emulated RAM and ROM, and some functions
 call code inside the ROM via a minimalized JIT emulator loop. These functions
 can only be called from within the JIT itself, for example from within
 TPlatformManager::NewtonScriptCall(NewtRef rcvr, NewtRef arg0, NewtRef arg1)
 */
namespace TNewt
{
// TODO: Symbol caching!

	extern TEmulator *mEmulator;
	extern TMemory *mMemory;
	extern TARMProcessor *mCPU;

	void SetEmulator(TEmulator *inEmulator);

	KUInt32 CallNewton(VAddr functionVector, const char *args, ...);
	NewtRef MakeString(const char *);
	NewtRef MakeSymbol(const char *);
	NewtRef MakeReal(double);
	NewtRefVar AllocateRefHandle(NewtRef);
	void DisposeRefHandle(NewtRefVar);
	NewtRef AllocateFrame();
	KUInt32 AddSlot(NewtRefArg, NewtRefArg);
	NewtRef AddArraySlot(NewtRefArg, NewtRefArg);
	NewtRef AllocateArray(NewtRefArg, KUInt32);
	NewtRef AllocateArray(KUInt32);
	NewtRef SetArraySlotRef(NewtRef, KUInt32, NewtRef);
	NewtRef SetArraySlot(NewtRefArg, KUInt32, NewtRefArg);
	NewtRef SetFrameSlot(NewtRefArg, NewtRefArg, NewtRefArg);

	bool RefIsInt(NewtRef);
	KSInt32 RefToInt(NewtRef);
	NewtRef MakeInt(KSInt32);

	bool RefIsSymbol(NewtRef);
	bool SymbolToCString(NewtRef, char *buf, int size);

	bool RefIsString(NewtRef);
	KUInt32 RefStringLength(NewtRef);
	char *RefToStringDup(NewtRef);
	NewtRef RefReplaceString(NewtRef, const char*);

	bool RefIsPointer(NewtRef);
	KUInt32 RefToPointer(NewtRef);
	NewtRef MakePointer(KUInt32);

class RefVar {
	KUInt32 mRefHandle;
public:
	RefVar(NewtRef ref) {
		mRefHandle = AllocateRefHandle( ref );
	}
	~RefVar() {
		DisposeRefHandle( mRefHandle );
	}
	operator NewtRefArg() {
		return mRefHandle;
	}
};

};

#endif
		// _T_NEWT_H

// ======================================================================= //
// FORTRAN is not a flower but a weed -- it is hardy, occasionally blooms, //
// and grows in every computer.                                            //
//                 -- A.J. Perlis                                          //
// ======================================================================= //
