// ==============================
// File:			TObjCBridgeCalls.cp
// Project:			Einstein
//
// Copyright 2015 by Jake Bordens.
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

#include "TObjCBridgeCalls.h"
#import <Foundation/Foundation.h>

// POSIX & Co.
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

// Einstein
#include "Emulator/TMemory.h"

// Helper functions
inline void WriteObjectToEmulatorMemory(TMemory* mMemoryIntf, KUInt32 address, id object);
inline id ReadObjectFromEmulatorMemory(TMemory* mMemoryIntf, KUInt32 address);
NSString* ReadUTF16(TMemory* memoryIntf, KUInt32 address);

// -------------------------------------------------------------------------- //
//  * TNativeiOSCalls( TMemory* )
// -------------------------------------------------------------------------- //
TObjCBridgeCalls::TObjCBridgeCalls( TMemory* inMemoryIntf )
	:
		mMemoryIntf( inMemoryIntf )
{
	// Initialization
}

#if TARGET_IOS || TARGET_OS_OPENSTEP

KUInt32 TObjCBridgeCalls::HostGetCPUArchitecture() {
	return sizeof(void*);
}


KUInt32 TObjCBridgeCalls::HostMakeNSInvocation(KUInt32 objPtrAddrOut,
										  KUInt32 classNamePtrAddress,
										  KUInt32 selectorStringPtrAddress) {

	// Get the first (16-bit) character from the selector to determine
	// if it is an instance or class member
	char classOrInstanceCharacter[2];
	mMemoryIntf->FastReadBuffer(selectorStringPtrAddress, 2, (KUInt8*)&classOrInstanceCharacter);

	// classOrInstanceCharacter should be '\0+' or '\0-' -- make sure the first character is \0
	if (classOrInstanceCharacter[0] != '\0')
		return -1;
	
	NSString* className = ReadUTF16(mMemoryIntf, classNamePtrAddress);
	NSString* selectorName = ReadUTF16(mMemoryIntf, selectorStringPtrAddress+2);
	Class classObj = NSClassFromString(className);

	SEL selector = NSSelectorFromString(selectorName);
	NSInvocation* invocation = nil;
	if (classOrInstanceCharacter[1] == '+') {
		//This is class Method

		NSMethodSignature* signature = [classObj methodSignatureForSelector:selector];
		if (signature == nil) return -1;
		invocation = [NSInvocation invocationWithMethodSignature:signature];
		invocation.selector = selector;
		invocation.target = classObj;
	} else if (classOrInstanceCharacter[1] == '-') {
		//This is an instance Method
		NSMethodSignature* signature = [classObj instanceMethodSignatureForSelector:selector];
		if (signature == nil) return -1;
		invocation = [NSInvocation invocationWithMethodSignature:signature];
		invocation.selector = selector;
		//Target to be set by a later setTarget call.
	}
	
	if (invocation == nil)
		return -1;
	
	WriteObjectToEmulatorMemory(mMemoryIntf, objPtrAddrOut, invocation);
	return 0;
}

KUInt32 TObjCBridgeCalls::HostSetInvocationTarget(KUInt32 invocationObjectAddr,
											     KUInt32 targetObjectAddr) {
	
	NSInvocation* invocation = ReadObjectFromEmulatorMemory(mMemoryIntf, invocationObjectAddr);
	
	invocation.target = ReadObjectFromEmulatorMemory(mMemoryIntf, targetObjectAddr);
	
	return 0;
}

KUInt32 TObjCBridgeCalls::HostSetInvocationArgument_Object(KUInt32 invocationObjectAddr,
													  KUInt32 objectAddr,
													  KUInt32 index) {
	
	NSInvocation* invocation = ReadObjectFromEmulatorMemory(mMemoryIntf, invocationObjectAddr);

	id object = ReadObjectFromEmulatorMemory(mMemoryIntf, objectAddr);
	[invocation setArgument:&object atIndex:index];
	
	return 0;
}

KUInt32 TObjCBridgeCalls::HostInvoke(KUInt32 invocationObjectAddr) {

	NSInvocation* invocation = ReadObjectFromEmulatorMemory(mMemoryIntf, invocationObjectAddr);
	[invocation invoke];
	
	return 0;
}

KUInt32 TObjCBridgeCalls::HostGetInvocationReturn_Object(KUInt32 returnObjectAddr,
														 KUInt32 invocationObjectAddr) {
	
	NSInvocation* invocation = ReadObjectFromEmulatorMemory(mMemoryIntf, invocationObjectAddr);

	// See http://stackoverflow.com/questions/22018272 for why this needs to be __unsafe_unretained
	id __unsafe_unretained returnObj;
	[invocation getReturnValue:&returnObj];
	WriteObjectToEmulatorMemory(mMemoryIntf, returnObjectAddr, returnObj);
	
	return 0;

}

KUInt32 TObjCBridgeCalls::HostReleaseObject(KUInt32 objectToReleaseAddr) {
	// Not using the helper function ReadObjectFromEmulatorMemory here, because
	// It uses the __bridge cast which is a no-op from an ARC perspective.
	// We want a __bridge_transfer cast which decrements the counter (release).
	void* objectPtr;
	mMemoryIntf->FastReadBuffer(objectToReleaseAddr, sizeof(id), (KUInt8*)&objectPtr);
	
#if !__has_feature(objc_arc)
    id object = (id)objectPtr;
	[object release];
#else
    id object = (__bridge_transfer id)objectPtr;
#endif
	object = nil;
	
	return 0;
}

KUInt32 TObjCBridgeCalls::HostMakeNSString(KUInt32 returnObjAddr, KUInt32 cStringAddr) {

	NSString* string = ReadUTF16(mMemoryIntf, cStringAddr);
	WriteObjectToEmulatorMemory(mMemoryIntf, returnObjAddr, string);
	
	return 0;
}

// Helper function to pull a NSString from the emulator's memory.
// There's probably a faster way to do this.
NSString* ReadUTF16(TMemory* memoryIntf, KUInt32 address) {
	UInt16 buffer;
	NSMutableData* data = [[NSMutableData alloc] init];
	while (1) {
		memoryIntf->FastReadBuffer(address, 2, (KUInt8*)&buffer);
		if (buffer==0) break;
		[data appendBytes:&buffer length:2];
		address += 2;
	}
	NSString* returnVal = [[NSString alloc] initWithData:data encoding:NSUTF16BigEndianStringEncoding];
#if !__has_feature(objc_arc)
	[returnVal autorelease];
	[data release];
#endif
	
	return returnVal;
	
}

inline void WriteObjectToEmulatorMemory(TMemory* mMemoryIntf, KUInt32 address, id object) {
#if !__has_feature(objc_arc)
	[object retain];
    void* objectPtr = ((void*)object);
#else
	void* objectPtr = ((__bridge_retained void*)object);
#endif
	mMemoryIntf->FastWriteBuffer(address, sizeof(id), (KUInt8*)&objectPtr);
}

inline id ReadObjectFromEmulatorMemory(TMemory* mMemoryIntf, KUInt32 address) {
	void* objectPtr;
	mMemoryIntf->FastReadBuffer(address, sizeof(id), (KUInt8*)&objectPtr);
	return (__bridge id)objectPtr;
}

#endif

// =========================================================================== //
// Jakes-MBP:Einstein jake$ fortune											   //
// -bash: fortune: command not found                                           //
// =========================================================================== //
