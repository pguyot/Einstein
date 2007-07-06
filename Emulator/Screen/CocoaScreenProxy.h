// ==============================
// File:			CocoaScreenProxy.h
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

#ifndef _COCOASCREENPROXY_H
#define _COCOASCREENPROXY_H

#include <K/Defines/KDefinitions.h>
#import <Cocoa/Cocoa.h>

class TCocoaScreenManager;
#import "CocoaEmulatorApp.h"

///
/// Class to interface with Cocoa screen widgets.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 121 $
///
/// \test	aucun test défini.
///
@interface CocoaScreenProxy : NSObject
{
	NSRunLoop*				mRunLoop;			///< Event loop.
}

// Initialization.
- (id)init;

// Thread-safeness
- (void)sendInvocation:(NSInvocation*)inInvocation;
- (NSInvocation*) createInvocationWithTarget: (id) inTarget withSelector: (SEL) inSelector;
- (void)setNeedsDisplay:(BOOL)inNeedsDisplay forView:(NSView*)inView;
- (void)setHidden:(BOOL)inSetHidden forView:(NSView*)inView;
- (void)setStringValue: (NSString*)inString forControl: (NSControl*) inControl;
- (void)forwardPowerChange: (BOOL)inState toListener: (id<CocoaEmulatorApp>)inApp;
- (void)forwardBacklightChange: (BOOL)inState toListener: (id<CocoaEmulatorApp>)inApp;

@end

#endif
		// _COCOASCREENPROXY_H

// ============================================================================== //
// Today is a good day for information-gathering.  Read someone else's mail file. //
// ============================================================================== //
