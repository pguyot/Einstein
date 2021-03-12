// ==============================
// File:			TNativeCalls.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TNATIVECALLS_H
#define _TNATIVECALLS_H

#include <K/Defines/KDefinitions.h>

// libffi
// FIXME: actually, this could work in MSWindows
#if !TARGET_OS_ANDROID && !TARGET_OS_MAC && !TARGET_OS_WIN32
  #include "ffi.h"
#endif

#include "NativeCallsDefines.h"

class TMemory;

///
/// Class for native calls, the first step towards virtualization.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test	aucun test défini.
///
class TNativeCalls
{
public:	
	///
	/// Constructor from an interface to memory.
	///
	TNativeCalls( TMemory* inMemoryIntf );

	///
	/// Destructor.
	///
	~TNativeCalls( void );
  
#if !TARGET_OS_ANDROID && !TARGET_OS_WIN32
 
#if !__LP64__

	///
	/// Open a native library.
	/// Return -1 if the library couldn't be found.
	/// Tries many things:
	/// path, path + suffix, path + suffix + anything.
	/// If path doesn't start with /, prepend /lib and /usr/lib.
	///
	/// \param inPathAddr	address of the path of the library to open.
	/// \return an index or -1 if it failed.
	///
	KUInt32	OpenLib( KUInt32 inPathAddr );

	///
	/// Close a library.
	///
	/// \param inLibRef	reference returned by OpenLib.
	///
	void	CloseLib( KUInt32 inLibRef );

	///
	/// Prepare the FFI structure for a given function.
	/// Return the index of the structure or -1 if it failed.
	///
	KUInt32	PrepareFFIStructure(
				KUInt32 inNativeLib,
				KUInt32 inSymbol,
				KUInt32 inNbArgs);

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_uint8(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt8 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_uint8;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt8 = inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_sint8(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KSInt8 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_sint8;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt8 = (KUInt8) inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_uint16(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt16 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_uint16;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt16 = inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_sint16(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KSInt16 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_sint16;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt16 = (KUInt16) inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_uint32(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_uint32;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt32 = inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_sint32(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KSInt32 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_sint32;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt32 = (KUInt32) inValue;
		}
	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_uint64(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt64 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_uint64;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt64 = inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_sint64(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KSInt64 inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_uint64;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fInt64 = (KUInt64) inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_float(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				float inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_float;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fFloat = inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_double(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				double inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_double;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fDouble = inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_longdouble(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				long double inValue)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_longdouble;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fLongDouble = inValue;
		}

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_string(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inStringAddr,
				KUInt32 inStringSize);


	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_binary(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inBinaryAddr,
				KUInt32 inBinarySize);

	///
	/// Set an argument type for a FFI structure.
	///
	void	SetArgValue_pointer(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUIntPtr inPointer)
		{
			mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_pointer;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fPtr = (void*) inPointer;
			mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fToFree = false;
		}

	///
	/// Set the result type.
	///
	void	SetResultType(
				KUInt32 inFFIStructure,
				EFFI_Type inType);

	///
	/// Call the function.
	///
	void	Call_void(KUInt32 inFFIStructure);

	///
	/// Call the function.
	///
	KUInt32	Call_int(KUInt32 inFFIStructure);

	///
	/// Call the function.
	///
	double	Call_real(KUInt32 inFFIStructure);

	///
	/// Call the function.
	///
	void	Call_string(KUInt32 inFFIStructure, KUInt32 inStringAddr, KUInt32 inSize);

	///
	/// Call the function.
	///
	KUIntPtr	Call_pointer(KUInt32 inFFIStructure);

	///
	/// Get the NativeCalls error message (with dlerror).
	///
	void		GetErrorMessage(KUInt32 inStringAddr, KUInt32 inSize);

	///
	/// Return Errno
	///
	KUInt32	GetErrno( void );

	///
	/// Get an argument's value after the call.
	///
	void	GetOutArgValue_string(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inStringAddr,
				KUInt32 inStringSize);

	///
	/// Get an argument's value after the call.
	///
	void	GetOutArgValue_binary(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inBinaryAddr,
				KUInt32 inBinarySize);

	///
	/// Dispose the FFI structure.
	///
	void	DisposeFFIStructure(KUInt32 inFFIStructure);
#endif // if !__LP64__
	
private:
	/// If the binary should be disposed
	struct SPointer {
		void*		fPtr;
		Boolean		fToFree;
	};

	/// Storage for non pointer arguments
	union SStorage {
		long double	fLongDouble;
		double		fDouble;
		float		fFloat;
		KUInt64		fInt64;
		KUInt32		fInt32;
		KUInt16		fInt16;
		KUInt8		fInt8;
		SPointer	fPointer;
	};

	struct SFunctionRec {
		KUInt32			fNbArgs;
		ffi_type*		fArgTypes[kNativeCalls_MaxArgs];
		SStorage		fArgValues[kNativeCalls_MaxArgs];
		void*			fArgValuesPtr[kNativeCalls_MaxArgs];
		ffi_type*		fResultType;
		void*			fFuncPtr;
		Boolean			fFreeRec;
	};

	struct SLibraryRec {
		void*			fHandle;
		Boolean			fFreeRec;
	};
	
	enum {
		kNativeLibsIncr		= 5,
		kNativeFuncsIncr	= 10,
	};

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TNativeCalls( const TNativeCalls& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TNativeCalls& operator = ( const TNativeCalls& inCopy );

#if !__LP64__
	///
	/// Call the function.
	///
	void	Call(KUInt32 inFFIStructure, SStorage* outResult);

	///
	/// Actually open a library. Return NULL if the library couldn't be found.
	/// Tries: path, path + suffix, path + suffix + anything.
	///
	/// \param inPath	path of the library to open.
	/// \return a handle or NULL
	///
	void*	DoOpenLib( const char* inPath );
#endif
	
	/// \name Variables
	TMemory*		mMemoryIntf;			///< Interface to memory.
	SLibraryRec*	mNativeLibs;			///< Data on open native libraries.
	KUInt32			mNbNativeLibs;			///< Number of native libraries.
	KUInt32			mAllocatedNativeLibs;	///< Capacity of native libraries.
	SFunctionRec*	mNativeFuncs;			///< Data on native functions.
	KUInt32			mNbNativeFuncs;			///< Number of native functions.
	KUInt32			mAllocatedNativeFuncs;	///< Capacity of native functions.
#endif
};

#endif
		// _TNATIVECALLS_H

// ===================================================== //
// Old programmers never die, they just become managers. //
// ===================================================== //
