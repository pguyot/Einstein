// ==============================
// File:			TObjCBridgeCalls.h
// Project:			Einstein
//
// Copyright 2015 by Jake Bordens
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

#ifndef _TOBJCBRIDGECALLS_H
#define _TOBJCBRIDGECALLS_H

#include <K/Defines/KDefinitions.h>

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
class TObjCBridgeCalls
{
public:	
	///
	/// Constructor from an interface to memory.
	///
	TObjCBridgeCalls( TMemory* inMemoryIntf );

	///
	/// Destructor.
	///
	~TObjCBridgeCalls( void );
	

#if TARGET_IOS || TARGET_OS_OPENSTEP

	///
	/// Returns the number of bits of the host running the emulator.
	/// Used by the client OS to determine the size needed to hold object pointers.
	///
	KUInt32 HostGetCPUArchitecture();
	
	///
	/// Create an NSInvocation object.
	/// The selector string is of the form "+classMethod:" or "-instanceMethod:"
	///
	KUInt32 HostMakeNSInvocation(KUInt32 objPtrAddrOut,
									 KUInt32 classNamePtrAddress,
									 KUInt32 selectorStringPtrAddress);
	
	///
	/// Given an NSInvocation object, set the target object
	///
	KUInt32 HostSetInvocationTarget(KUInt32 invocationObjectAddr,
								KUInt32 targetObjectAddr);
	
	/// Set a parameter on an NSInvocation object
	KUInt32 HostSetInvocationArgument_Object(KUInt32 invocationObjectAddr,
											  KUInt32 objectAddr,
											  KUInt32 index);
	
	///
	/// Given an NSInvocation object, perform the invocation as configured.
	///
	KUInt32 HostInvoke(KUInt32 invocationObjectAddr);

	///
	/// Get return value Object from an NSInvocation
	///
	KUInt32 HostGetInvocationReturn_Object(KUInt32 returnObjectAddr,
										   KUInt32 invocationObjectAddr);
	
	
	///
	/// Release an object.  If using ARC, then use __bridge_transfer to release
	/// Otherwise, actually release the object.
	///
	KUInt32 HostReleaseObject(KUInt32 objectToRelease);

	KUInt32 HostMakeNSString(KUInt32 returnObjAddr, KUInt32 cStringAddr);
	
	/// \name Variables
	TMemory*		mMemoryIntf;			///< Interface to memory.

#endif // TARGET_IOS
};

#endif // _TNATIVEIOSCALLS_H

// ===================================================== //
// Old programmers never die, they just become managers. //
// ===================================================== //
