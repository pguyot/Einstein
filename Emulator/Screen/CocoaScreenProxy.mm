// ==============================
// File:			CocoaScreenProxy.mm
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

#import "CocoaScreenProxy.h"

@implementation CocoaScreenProxy

// ------------------------------------------------------------------------- //
//  * (id) init
// ------------------------------------------------------------------------- //
- (id) init
{
#ifdef __MWERKS__
	#pragma unused ( _cmd )
#endif
	if ((self = [super init]))
	{
		// Récupération de la boucle courante.
		mRunLoop = [NSRunLoop currentRunLoop];
	}
	return self;
}

// ------------------------------------------------------------------------- //
//  * sendInvocation: (NSInvocation*)
// ------------------------------------------------------------------------- //
- (void) sendInvocation: (NSInvocation*) inInvocation
{
	NSTimer* theTimer = [NSTimer
			timerWithTimeInterval: 0
			invocation: inInvocation
			repeats: NO];
	
	[mRunLoop addTimer: theTimer forMode: NSDefaultRunLoopMode];
}

// ------------------------------------------------------------------------- //
//  * createInvocationWithTarget: (id) inTarget withSelector: (SEL)
// ------------------------------------------------------------------------- //
- (NSInvocation*) createInvocationWithTarget: (id) inTarget
								withSelector: (SEL) inSelector
{
	NSMethodSignature* theSignature;
	NSInvocation* theResult;
	
	// Récupération de la signature.
	theSignature = [inTarget methodSignatureForSelector: inSelector];
	
	// Création de l'invocation.
	theResult = [NSInvocation invocationWithMethodSignature: theSignature];
	
	// On règle le sélecteur.
	[theResult setSelector: inSelector];
	
	// On règle la cible.
	[theResult setTarget: inTarget];
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * setNeedsDisplay: (BOOL) forView: (NSView*)
// ------------------------------------------------------------------------- //
- (void) setNeedsDisplay: (BOOL) inNeedsDisplay forView: (NSView*) inView
{
	if (mRunLoop == [NSRunLoop currentRunLoop])
	{
		[inView setNeedsDisplay: inNeedsDisplay];
	} else {
		// Création de l'invocation.
		NSInvocation* theInvocation
			= [self createInvocationWithTarget: inView withSelector:
						@selector(setNeedsDisplay:)];
		
		[theInvocation setArgument: &inNeedsDisplay atIndex: 2];
		
		// On invoque.
		[self sendInvocation: theInvocation];
	}
}

// ------------------------------------------------------------------------- //
//  * setHidden: (BOOL) forView: (NSView*)
// ------------------------------------------------------------------------- //
- (void) setHidden: (BOOL) inSetHidden forView: (NSView*) inView
{
	if (mRunLoop == [NSRunLoop currentRunLoop])
	{
		[inView setHidden: inSetHidden];
	} else {
		// Création de l'invocation.
		NSInvocation* theInvocation
			= [self createInvocationWithTarget: inView withSelector:
						@selector(setHidden:)];
		
		[theInvocation setArgument: &inSetHidden atIndex:2];
		
		// On invoque.
		[self sendInvocation: theInvocation];
	}
}

// ------------------------------------------------------------------------- //
//  * forwardPowerChange: (BOOL) toListener: (id<CocoaEmulatorApp>)
// ------------------------------------------------------------------------- //
- (void) forwardPowerChange: (BOOL) inState toListener: (id<CocoaEmulatorApp>) inApp
{
	if (mRunLoop == [NSRunLoop currentRunLoop])
	{
		[inApp powerChange: inState];
	} else {
		// Création de l'invocation.
		NSInvocation* theInvocation
			= [self createInvocationWithTarget: inApp withSelector:
						@selector(powerChange:)];
		
		[theInvocation setArgument: &inState atIndex:2];
		
		// On invoque.
		[self sendInvocation: theInvocation];
	}
}

// ------------------------------------------------------------------------- //
//  * forwardBacklightChange: (BOOL) toListener: (id<CocoaEmulatorApp>)
// ------------------------------------------------------------------------- //
- (void) forwardBacklightChange: (BOOL) inState toListener: (id<CocoaEmulatorApp>) inApp
{
	if (mRunLoop == [NSRunLoop currentRunLoop])
	{
		[inApp backlightChange: inState];
	} else {
		// Création de l'invocation.
		NSInvocation* theInvocation
			= [self createInvocationWithTarget: inApp withSelector:
						@selector(backlightChange:)];
		
		[theInvocation setArgument: &inState atIndex:2];
		
		// On invoque.
		[self sendInvocation: theInvocation];
	}
}

// ------------------------------------------------------------------------- //
//  * setStringValue: (NSString*) forControl: (NSControl*)
// ------------------------------------------------------------------------- //
- (void) setStringValue: (NSString*) inString forControl: (NSControl*) inControl
{
	if (mRunLoop == [NSRunLoop currentRunLoop])
	{
		[inControl setStringValue: inString];
	} else {
		// Création de l'invocation.
		NSInvocation* theInvocation
			= [self createInvocationWithTarget: inControl withSelector:
						@selector(setStringValue:)];
		
		[theInvocation setArgument: &inString atIndex:2];
		
		// On invoque.
		[self sendInvocation: theInvocation];
	}
}

@end

// =========================================== //
// Don't sweat it -- it's only ones and zeros. //
//                 -- P. Skelly                //
// =========================================== //
