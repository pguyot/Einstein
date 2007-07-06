// ==============================
// File:			TEinsteinApplication.mm
// Project:			Einstein
//
// Copyright 2004-2007 by Paul Guyot (pguyot@kallisys.net).
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
#import "TEinsteinApplication.h"

@implementation TEinsteinApplication

- (id)init
{
	if ((self = [super init]))
	{
		mView = nil;
	}
	return self;
}

- (void)sendEvent:(NSEvent *)anEvent
{
	if (mView && [mView mouseInView])
	{
		NSEventType theType = [anEvent type];
		if (theType == NSKeyDown)
		{
			[mView doKeyDown: anEvent];
		} else if (theType == NSKeyUp) {
			[mView doKeyUp: anEvent];
		} else if (theType == NSFlagsChanged) {
			[mView doFlagsChanged: anEvent];
		} else {
			[super sendEvent: anEvent];
		}
	} else {
		[super sendEvent: anEvent];
	}
}

- (void)registerView:(TCocoaScreenView *)inView
{
	mView = inView;
}

- (void)unregisterView
{
	mView = nil;
}

@end

// ====================================================== //
// There are three kinds of people: men, women, and unix. //
// ====================================================== //
