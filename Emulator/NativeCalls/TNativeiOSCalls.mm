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
#include "TNativeiOSCalls.h"
#include "iEinsteinAppDelegate.h"

// POSIX & Co.
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

// Einstein
#include "TMemory.h"

// -------------------------------------------------------------------------- //
//  * TNativeiOSCalls( TMemory* )
// -------------------------------------------------------------------------- //
TNativeiOSCalls::TNativeiOSCalls( TMemory* inMemoryIntf )
	:
		mMemoryIntf( inMemoryIntf )
{
	// Initialization
}

// -------------------------------------------------------------------------- //
//  * ~TNativeCalls( void )
// -------------------------------------------------------------------------- //
TNativeiOSCalls::~TNativeiOSCalls( void )
{
	// Clean-up
}

KUInt32 TNativeiOSCalls::iOSActivityWithText(KUInt32 textPtrAddr,
									      KUInt32 textLen)
{
	char* myCString = (char*)malloc(textLen);
	mMemoryIntf->FastReadBuffer(textPtrAddr, textLen, (KUInt8*)myCString);
	
	NSData* data = [NSData dataWithBytes:myCString length:textLen];
	NSMutableString* stringObj = [[[NSMutableString alloc] initWithData:data encoding:NSUTF16BigEndianStringEncoding] autorelease];
	
	// Replace classic MacOS line endings with modern ones.
	[stringObj replaceOccurrencesOfString:@"\r" withString:@"\n" options:NSLiteralSearch range:NSMakeRange(0, [stringObj length])];

	[(iEinsteinAppDelegate*)[[UIApplication sharedApplication] delegate] share:stringObj];
	
	free(myCString);
	return 123;
}

// =========================================================================== //
//         A novice was trying to fix a broken lisp machine by turning the     //
// power off and on.  Knight, seeing what the student was doing spoke sternly, //
// "You cannot fix a machine by just power-cycling it with no understanding    //
// of what is going wrong."  Knight turned the machine off and on.  The        //
// machine worked.                                                             //
// =========================================================================== //
