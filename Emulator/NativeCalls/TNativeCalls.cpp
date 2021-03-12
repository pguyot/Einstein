// ==============================
// File:			TNativeCalls.cp
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

#include <K/Defines/KDefinitions.h>
#include "Emulator/NativeCalls/TNativeCalls.h"


#if TARGET_OS_ANDROID || TARGET_OS_WIN32

// Einstein
#include "Emulator/TMemory.h"

TNativeCalls::TNativeCalls( TMemory* inMemoryIntf ) { }
TNativeCalls::~TNativeCalls( ) { }

#else

// POSIX & Co.
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#if TARGET_OS_WIN32
	#include <stdlib.h>
	#include <assert.h>
	ffi_type ffi_type_uint8; // FIXME these should be in libffi
	ffi_type ffi_type_sint8;
	ffi_type ffi_type_uint16;
	ffi_type ffi_type_sint16;
	ffi_type ffi_type_uint32;
	ffi_type ffi_type_sint32;
	ffi_type ffi_type_pointer;
#elif defined TARGET_IOS
ffi_type ffi_type_uint8; // FIXME these should be in libffi
ffi_type ffi_type_sint8;
ffi_type ffi_type_uint16;
ffi_type ffi_type_sint16;
ffi_type ffi_type_uint32;
ffi_type ffi_type_sint32;
ffi_type ffi_type_pointer;
#else
	#include <dlfcn.h>
	#include <libgen.h>
	#include <dirent.h>
#endif

// Einstein
#include "Emulator/TMemory.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#ifndef DYLIBSUFFIX
	#if TARGET_OS_OPENSTEP
		#define DYLIBSUFFIX ".dylib"
	#elif TARGET_OS_LINUX
		#define DYLIBSUFFIX ".so"
	#elif TARGET_OS_BSD
		#define DYLIBSUFFIX ".so"
	#elif TARGET_OS_CYGWIN
		#define DYLIBSUFFIX ".dll"
	#else
		#error Please define DYLIBSUFFIX
	#endif
#endif

// -------------------------------------------------------------------------- //
//  * TNativeCalls( TMemory* )
// -------------------------------------------------------------------------- //
TNativeCalls::TNativeCalls( TMemory* inMemoryIntf )
	:
		mMemoryIntf( inMemoryIntf ),
		mNativeLibs( NULL ),
		mNbNativeLibs( 0 ),
		mAllocatedNativeLibs( 0 ),
		mNativeFuncs( NULL ),
		mNbNativeFuncs( 0 ),
		mAllocatedNativeFuncs( 0 )
{
	mNativeLibs =
		(SLibraryRec*) ::malloc( sizeof(SLibraryRec) * kNativeLibsIncr );
	mNativeFuncs =
		(SFunctionRec*) ::malloc( sizeof(SFunctionRec) * kNativeFuncsIncr );
	mAllocatedNativeLibs = kNativeLibsIncr;
	mAllocatedNativeFuncs = kNativeFuncsIncr;
}

// -------------------------------------------------------------------------- //
//  * ~TNativeCalls( void )
// -------------------------------------------------------------------------- //
TNativeCalls::~TNativeCalls( void )
{
	if (mNativeLibs)
	{
		::free( mNativeLibs );
	}
	if (mNativeFuncs)
	{
		::free( mNativeFuncs );
	}
}

#if !__LP64__

// -------------------------------------------------------------------------- //
//  * OpenLib( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TNativeCalls::OpenLib( KUInt32 inPathAddr )
{
	KUInt32 theResult = (KUInt32) -1;
	
	// Get the path.
	char theArgPath[PATH_MAX + 1];
	char thePath[PATH_MAX + 1];
	KUInt32 pathSize = PATH_MAX;
	if (!mMemoryIntf->FastReadString(inPathAddr, &pathSize, theArgPath))
	{
		void* theHandle = NULL;
		
		do {
			if (theArgPath[0] != '/')
			{
				/* try with /lib/<inPath> */
				(void) snprintf(
							thePath,
							sizeof(thePath),
							"/lib/%s",
							theArgPath );
				thePath[PATH_MAX - 1] = 0;
				theHandle = DoOpenLib(thePath);
				if (theHandle) break;
		
				/* try with /usr/lib/<inPath> */
				(void) snprintf(
							thePath,
							sizeof(thePath),
							"/usr/lib/%s",
							theArgPath );
				thePath[PATH_MAX - 1] = 0;
				theHandle = DoOpenLib(thePath);
				if (theHandle) break;
			}
	
			/* try directly */
			theHandle = DoOpenLib(theArgPath);
			if (theHandle) break;
		} while (false);
		
		if (theHandle)
		{
			// Store the handle in a new record.
			KUInt32 indexLib;
			for (indexLib = 0; indexLib < mNbNativeLibs; indexLib++)
			{
				if (mNativeLibs[indexLib].fFreeRec)
				{
					theResult = indexLib;
					break;
				}
			}
			
			if (theResult == (KUInt32) -1)
			{
				// Create a new record
				if (mNbNativeLibs == mAllocatedNativeLibs)
				{
					// Increase the capacity.
					mAllocatedNativeLibs += kNativeLibsIncr;
					mNativeLibs = (SLibraryRec*) ::realloc(
											mNativeLibs,
											sizeof(SLibraryRec) * mAllocatedNativeLibs);
				}
				theResult = mNbNativeLibs;
				mNbNativeLibs++;
			}
			
			mNativeLibs[theResult].fHandle = theHandle;
			mNativeLibs[theResult].fFreeRec = false;
		}
	} // else: a memory error occurred.
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * DoOpenLib( const char* )
// -------------------------------------------------------------------------- //
void*
TNativeCalls::DoOpenLib( const char* inPath )
{
#if TARGET_OS_WIN32
  // FIXME We will implement this later
  assert(0);
  return 0L;
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
  return 0L;
# else
	void* theResult = NULL;
	char* theDirNameBuf = NULL;
	char* theBaseNameBuf = NULL;
	do {
		char thePath[PATH_MAX];
		char* theDirName;
		char* theBaseName;
		size_t baseNameLen;
		size_t pathLen;
		DIR* theDir;

		/* try to open the library without any suffix */
		theResult = dlopen(inPath, RTLD_LAZY);
		if (theResult) break;
		
		/* add the suffix */
		pathLen = strlen(inPath);
		if (pathLen >= (PATH_MAX - 1)) break;

		(void) memcpy(
					thePath,
					inPath,
					pathLen);

		(void) strncpy(
					&thePath[pathLen],
					DYLIBSUFFIX,
					PATH_MAX - pathLen);
		thePath[PATH_MAX - 1] = 0;
		theResult = dlopen(thePath, RTLD_LAZY);
		if (theResult) break;

		/* try to look at path + anything */
		theDirNameBuf = strdup(inPath);
		theDirName = dirname(theDirNameBuf);
		
		/* iterate on the directory */
		theDir = opendir(theDirName);
		if (theDir != NULL)
		{
			theBaseNameBuf = strdup(thePath);
			theBaseName = basename(theBaseNameBuf);
			baseNameLen = strlen(theBaseName);
			pathLen = strlen(thePath);

			struct dirent* theEntry;
			do {
				theEntry = readdir(theDir);
				if (theEntry == NULL)
				{
					break;
				}
				if (memcmp(theEntry->d_name, theBaseName, baseNameLen) == 0)
				{
					/* the name begins with what we have */
					strncpy(
						&thePath[pathLen],
						&(theEntry->d_name)[baseNameLen],
						PATH_MAX - pathLen);
					thePath[PATH_MAX - 1] = 0;
					theResult = dlopen(thePath, RTLD_LAZY);
					if (theResult) break;
				}
			} while (true);
			
			closedir(theDir);
		}
	} while (false);
	
	if (theDirNameBuf != NULL)
	{
		free(theDirNameBuf);
	}
	if (theBaseNameBuf != NULL)
	{
		free(theBaseNameBuf);
	}
	
	return theResult;
#endif
}

// -------------------------------------------------------------------------- //
//  * CloseLib( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::CloseLib( KUInt32 inLibRef )
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	(void) dlclose( mNativeLibs[inLibRef].fHandle );
	mNativeLibs[inLibRef].fFreeRec = true;

	// If it's the last, empty it up in the queue.
	KUInt32 lastNativeLib = mNbNativeLibs - 1;
	if (lastNativeLib == inLibRef)
	{
		do {
			lastNativeLib--;
		} while (mNativeLibs[lastNativeLib].fFreeRec);
		mNbNativeLibs = lastNativeLib + 1;
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * PrepareFFIStructure( KUInt32, const char*, KUInt32 inNbArgs )
// -------------------------------------------------------------------------- //
KUInt32
TNativeCalls::PrepareFFIStructure(
				KUInt32 inNativeLib,
				KUInt32 inSymbolAddr,
				KUInt32 inNbArgs)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
	return 0;
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
  return 0;
# else
	KUInt32 theResult = mNbNativeFuncs;
	// Get a new record.
	if (theResult == mAllocatedNativeFuncs)
	{
		// Increase the capacity.
		mAllocatedNativeFuncs += kNativeFuncsIncr;
		mNativeFuncs = (SFunctionRec*) ::realloc(
								mNativeFuncs,
								sizeof(SFunctionRec) * mAllocatedNativeFuncs);
	}
	
	char symbolName[kNativeCalls_SymbolMaxLen];
	KUInt32 symbolSize = sizeof(symbolName);
	mMemoryIntf->FastReadString(inSymbolAddr, &symbolSize, symbolName);
	symbolName[kNativeCalls_SymbolMaxLen - 1] = 0;
	
	SFunctionRec* theStructure = &mNativeFuncs[theResult];
	mNbNativeFuncs++;
	theStructure->fNbArgs = inNbArgs;

	// Set the argument types to NULL & set the pointers.
	KUInt32 indexArgs;
	for (indexArgs = 0; indexArgs < inNbArgs; indexArgs++)
	{
		theStructure->fArgTypes[indexArgs] = NULL;
		theStructure->fArgValuesPtr[indexArgs] =
			&theStructure->fArgValues[indexArgs];
	}

	void* theLibHandle = mNativeLibs[inNativeLib].fHandle;
	void* theFuncPtr = dlsym(theLibHandle, symbolName);
	if (theFuncPtr == NULL)
	{
		theStructure->fFreeRec = true;
		mNbNativeFuncs--;
		theResult = (KUInt32) -1;
	} else {
		theStructure->fFuncPtr = theFuncPtr;
		theStructure->fResultType = NULL;
		theStructure->fFreeRec = false;
	}
	
	return theResult;
#endif
}

// -------------------------------------------------------------------------- //
//  * SetArgValue_string( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::SetArgValue_string(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inStringAddr,
				KUInt32 inSize)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	// Allocate the string.
	KUInt8* theString = (KUInt8*) ::malloc(inSize + 1);
	if (mMemoryIntf->FastReadBuffer(inStringAddr, inSize, theString))
	{
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fPtr = NULL;
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fToFree = false;
		::free(theString);
	} else {
		mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_pointer;
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fPtr = theString;
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fToFree = true;
	
		// Set the null terminator.
		theString[inSize] = 0;
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * SetArgValue_binary( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::SetArgValue_binary(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inStringAddr,
				KUInt32 inSize)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	// Allocate the binary.
	KUInt8* theBinary = (KUInt8*) ::malloc(inSize);
	if (mMemoryIntf->FastReadBuffer(inStringAddr, inSize, theBinary))
	{
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fPtr = NULL;
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fToFree = false;
		::free(theBinary);
	} else {
		mNativeFuncs[inFFIStructure].fArgTypes[inArgIndex] = &ffi_type_pointer;
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fPtr = theBinary;
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fToFree = true;
	}	
#endif
}

// -------------------------------------------------------------------------- //
//  * SetResultType( KUInt32, EFFI_Type )
// -------------------------------------------------------------------------- //
void
TNativeCalls::SetResultType(
				KUInt32 inFFIStructure,
				EFFI_Type inType)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	ffi_type* theType = NULL;
	switch (inType)
	{
		case k_void:
			theType = &ffi_type_void;
			break;
			
		case k_uint8:
			theType = &ffi_type_uint8;
			break;

		case k_sint8:
			theType = &ffi_type_sint8;
			break;

		case k_uint16:
			theType = &ffi_type_uint16;
			break;

		case k_sint16:
			theType = &ffi_type_sint16;
			break;

		case k_uint32:
			theType = &ffi_type_uint32;
			break;

		case k_sint32:
			theType = &ffi_type_sint32;
			break;

		case k_uint64:
			theType = &ffi_type_uint64;
			break;

		case k_sint64:
			theType = &ffi_type_sint64;
			break;

		case k_float:
			theType = &ffi_type_float;
			break;

		case k_double:
			theType = &ffi_type_double;
			break;

		case k_longdouble:
			theType = &ffi_type_longdouble;
			break;

		case k_string:
		case k_binary:
		case k_pointer:
		case k_iostring:
		case k_iobinary:
			theType = &ffi_type_pointer;
			break;
	}
	
	mNativeFuncs[inFFIStructure].fResultType = theType;
#endif
}

// -------------------------------------------------------------------------- //
//  * Call( KUInt32, SStorage* )
// -------------------------------------------------------------------------- //
void
TNativeCalls::Call(KUInt32 inFFIStructure, SStorage* outResult)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	ffi_cif theCif;
	SFunctionRec* theStructure = &mNativeFuncs[inFFIStructure];
	if (theStructure->fFuncPtr == NULL)
	{
		KPrintf("Cannot call a null function.\n");
	} else {
		if (ffi_prep_cif(
				&theCif,
				FFI_DEFAULT_ABI,
				theStructure->fNbArgs,
				theStructure->fResultType,
				theStructure->fArgTypes) == FFI_OK)
		{
			ffi_call(
				&theCif, (void (*)()) theStructure->fFuncPtr,
				outResult, theStructure->fArgValuesPtr);
		} else {
			// FIXME: handle errors.
			KPrintf("Error with ffi_prep_cif\n");
		}
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * Call_void( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::Call_void(KUInt32 inFFIStructure)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	SStorage theResult;
	Call(inFFIStructure, &theResult);
#endif
}

// -------------------------------------------------------------------------- //
//  * Call_int( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TNativeCalls::Call_int(KUInt32 inFFIStructure)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
	return 0;
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
  return 0;
# else
	KUInt32 theReturnValue;
	SStorage theResult;
	ffi_type* theType = mNativeFuncs[inFFIStructure].fResultType;
	Call(inFFIStructure, &theResult);
	if ((theType == &ffi_type_uint8) || (theType == &ffi_type_sint8))
	{
		theReturnValue = theResult.fInt8;
	} else if ((theType == &ffi_type_uint16) || (theType == &ffi_type_sint16)) {
		theReturnValue = theResult.fInt16;
	} else if ((theType == &ffi_type_uint32) || (theType == &ffi_type_sint32)) {
		theReturnValue = theResult.fInt32;
//	} else if ((theType == &ffi_type_uint64) || (theType == &ffi_type_sint64)) {
//		theReturnValue = theResult.fInt64;
	} else {
		theReturnValue = 0;
	}
	
	return theReturnValue;
#endif
}

// -------------------------------------------------------------------------- //
//  * Call_real( KUInt32 )
// -------------------------------------------------------------------------- //
double
TNativeCalls::Call_real(KUInt32 inFFIStructure)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
	return 0.0;
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
  return 0.0;
# else
	double theReturnValue;
	SStorage theResult;
	ffi_type* theType = mNativeFuncs[inFFIStructure].fResultType;
	Call(inFFIStructure, &theResult);
	if (theType == &ffi_type_float)
	{
		theReturnValue = theResult.fFloat;
	} else if (theType == &ffi_type_double) {
		theReturnValue = theResult.fDouble;
	} else if (theType == &ffi_type_longdouble) {
		theReturnValue = theResult.fLongDouble;
	} else {
		theReturnValue = 0.0;
	}
	
	return theReturnValue;
#endif
}

// -------------------------------------------------------------------------- //
//  * Call_string( KUInt32, char*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::Call_string(
	KUInt32 inFFIStructure,
	KUInt32 inResultAddr,
	KUInt32 inSize)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	SStorage theResult;
	Call(inFFIStructure, &theResult);
	
	KUInt32 size = inSize;
	mMemoryIntf->FastWriteString(
		inResultAddr,
		&size,
		(const char*) theResult.fPointer.fPtr);
#endif
}

// -------------------------------------------------------------------------- //
//  * Call_pointer( KUInt32 )
// -------------------------------------------------------------------------- //
KUIntPtr
TNativeCalls::Call_pointer(KUInt32 inFFIStructure)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
	return 0;
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
  return 0;
# else
	SStorage theResult;
	Call(inFFIStructure, &theResult);
	
	return (KUIntPtr) theResult.fPointer.fPtr;
#endif
}

// -------------------------------------------------------------------------- //
//  * GetOutArgValue_string( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::GetOutArgValue_string(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inStringAddr,
				KUInt32 inStringSize)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	char* theArgCopy = (char*)
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fPtr;

	// Copy back the string.
	KUInt32 count = inStringSize;
	(void) mMemoryIntf->FastWriteString(inStringAddr, &count, theArgCopy);
#endif
}

// -------------------------------------------------------------------------- //
//  * GetOutArgValue_binary( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::GetOutArgValue_binary(
				KUInt32 inFFIStructure,
				KUInt32 inArgIndex,
				KUInt32 inBinaryAddr,
				KUInt32 inBinarySize)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	KUInt8* theArgCopy = (KUInt8*)
		mNativeFuncs[inFFIStructure].fArgValues[inArgIndex].fPointer.fPtr;

	// Copy back the binary.
	(void) mMemoryIntf->FastWriteBuffer(inBinaryAddr, inBinarySize, theArgCopy);
#endif
}

// -------------------------------------------------------------------------- //
//  * DisposeFFIStructure( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::DisposeFFIStructure(KUInt32 inFFIStructure)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	SFunctionRec* theStructure = &mNativeFuncs[inFFIStructure];

	// Free the arguments.
	KUInt32 indexArgs;
	KUInt32 nbArgs = theStructure->fNbArgs;
	for (indexArgs = 0; indexArgs < nbArgs; indexArgs++)
	{
		if ((theStructure->fArgTypes[indexArgs] == &ffi_type_pointer)
			&& (theStructure->fArgValues[indexArgs].fPointer.fToFree))
		{
			::free(theStructure->fArgValues[indexArgs].fPointer.fPtr);
		}
	}
	
	theStructure->fFreeRec = true;
	
	// If it's the last, empty it up in the queue.
	KUInt32 newNbNativeFuncs = mNbNativeFuncs - 1;
	if (inFFIStructure == newNbNativeFuncs)
	{
		SFunctionRec* theCrsr = theStructure;
		for (; newNbNativeFuncs > 0; newNbNativeFuncs--)
		{
			theCrsr--;
			if (!theCrsr->fFreeRec)
			{
				break;
			}
		}
		mNbNativeFuncs = newNbNativeFuncs;
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * GetErrorMessage( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNativeCalls::GetErrorMessage(KUInt32 inStringAddr, KUInt32 inSize)
{
#if TARGET_OS_WIN32
	// FIXME We will implement this later
	assert(0);
#elif defined TARGET_IOS
  // FIXME We will implement this later
  assert(0);
# else
	KUInt32 theSize = inSize;
	const char* theErrStr = dlerror();
	if (theErrStr != NULL)
	{
		(void) mMemoryIntf->FastWriteString(inStringAddr, &theSize, theErrStr);
	} else {
		(void) mMemoryIntf->FastWriteString(inStringAddr, &theSize, "no error");
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * GetErrno( void )
// -------------------------------------------------------------------------- //
KUInt32
TNativeCalls::GetErrno( void)
{
	return errno;
}

#endif //!__LP64__

#endif

// =========================================================================== //
//         A novice was trying to fix a broken lisp machine by turning the     //
// power off and on.  Knight, seeing what the student was doing spoke sternly, //
// "You cannot fix a machine by just power-cycling it with no understanding    //
// of what is going wrong."  Knight turned the machine off and on.  The        //
// machine worked.                                                             //
// =========================================================================== //
