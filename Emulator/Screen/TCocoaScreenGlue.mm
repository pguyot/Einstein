// ==============================
// File:			TCocoaScreenGlue.mm
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

#include "TCocoaScreenGlue.h"
#import "CocoaEmulatorApp.h"
#import "CocoaScreenProxy.h"
#import "TCocoaPowerButtonNSView.h"
#include "TCocoaScreenManager.h"
#import "TCocoaScreenView.h"

// Prototypes for private methods.
void EmulatorWindow_FinishCreate(
	TCocoaScreenManager* inManager,
	NSWindow* inWindow,
	NSView* inView,
	NSView* inText);

// -------------------------------------------------------------------------- //
//  * EmulatorText_Create( int, int )
// -------------------------------------------------------------------------- //
id
EmulatorText_Create(int inWindowWidth, int inWindowHeight)
{
	NSRect theFrame;
	theFrame.origin.x = 0;
	theFrame.origin.y = 0;
	theFrame.size.width = inWindowWidth;
	theFrame.size.height = 20;
	NSTextField* theTextField =
		[[NSTextField alloc] initWithFrame:theFrame];
	[theTextField setHidden:NO];
	[theTextField setStringValue:@"Booting"];

	NSCell* theCell = [theTextField cell];
	NSFont* theFont =
		[NSFont
			fontWithName:[[theCell font] fontName]
					size:[NSFont systemFontSizeForControlSize:NSMiniControlSize]];
	[theCell setFont:theFont];
	[theCell setControlSize:NSMiniControlSize];
	[theTextField sizeToFit];

	[theTextField setAutoresizingMask:
					  NSViewMinXMargin | NSViewMaxXMargin
				  | NSViewMinYMargin | NSViewMaxYMargin];

	[theTextField setEditable:NO];
	[theTextField setSelectable:NO];
	[theTextField setAlignment:NSCenterTextAlignment];

	[theTextField setDrawsBackground:NO];
	[theTextField setBordered:NO];

	// Center the control in the view.
	NSRect theRect = [theTextField frame];
	theRect.origin.x = 0;
	theRect.origin.y = (inWindowHeight + theRect.size.height) / 2;
	theRect.size.width = inWindowWidth;
	[theTextField setFrame:theRect];

	return theTextField;
}

// -------------------------------------------------------------------------- //
//  * EmulatorText_SetEinsteinOffString( id, bool )
// -------------------------------------------------------------------------- //
void
EmulatorText_SetScreenOffString(id inProxy, id inEmulatorText)
{
	[((CocoaScreenProxy*) inProxy) setStringValue:@"Screen is off (please be patient)" forControl:inEmulatorText];
}

// -------------------------------------------------------------------------- //
//  * EmulatorText_SetEinsteinOffString( id, bool )
// -------------------------------------------------------------------------- //
void
EmulatorText_SetEinsteinOffString(id inProxy, id inEmulatorText)
{
	[((CocoaScreenProxy*) inProxy) setStringValue:@"Einstein is off" forControl:inEmulatorText];
}

// -------------------------------------------------------------------------- //
//  * TCocoaScreenView_Create( TCocoaScreenManager*, int, int )
// -------------------------------------------------------------------------- //
id
TCocoaScreenView_Create(TCocoaScreenManager* inManager, int inWidth, int inHeight)
{
	NSRect theFrame;
	theFrame.origin.x = 0;
	theFrame.origin.y = 0;
	theFrame.size.width = inWidth;
	theFrame.size.height = inHeight;
	TCocoaScreenView* theScreenView =
		[[TCocoaScreenView alloc] initWithFrame:theFrame
								  screenManager:inManager];
	[theScreenView setHidden:YES];

	[theScreenView setAutoresizingMask:
					   NSViewMinXMargin | NSViewWidthSizable
				   | NSViewMaxXMargin | NSViewMinYMargin
				   | NSViewHeightSizable | NSViewMaxYMargin];

	return theScreenView;
}

// -------------------------------------------------------------------------- //
//  * TCocoaScreenView_SetNeedsDisplay( id )
// -------------------------------------------------------------------------- //
void
TCocoaScreenView_SetNeedsDisplay(id inProxy, id inView)
{
	[((CocoaScreenProxy*) inProxy) setNeedsDisplay:YES forView:inView];
}

// -------------------------------------------------------------------------- //
//  * TCocoaScreenView_SetNeedsDisplayInRect( id, id, NSRect )
// -------------------------------------------------------------------------- //
void
TCocoaScreenView_SetNeedsDisplayInRect(id inProxy, id inView, NSRect inRect)
{
	[((CocoaScreenProxy*) inProxy) setNeedsDisplayInRect:inRect forView:inView];
}

// -------------------------------------------------------------------------- //
//  * TCocoaScreenView_SetHidden( id, bool )
// -------------------------------------------------------------------------- //
void
TCocoaScreenView_SetHidden(id inProxy, id inView, bool inHidden)
{
	[((CocoaScreenProxy*) inProxy) setHidden:(inHidden ? YES : NO) forView:inView];
}

// -------------------------------------------------------------------------- //
//  * EmulatorApp_PowerChange( id, id, bool )
// -------------------------------------------------------------------------- //
void
EmulatorApp_PowerChange(id inProxy, id inApp, bool inState)
{
	[((CocoaScreenProxy*) inProxy)
		forwardPowerChange:(inState ? YES : NO)
				toListener:((id<CocoaEmulatorApp>) inApp)];
}

// -------------------------------------------------------------------------- //
//  * EmulatorApp_BacklightChange( id, id, bool )
// -------------------------------------------------------------------------- //
void
EmulatorApp_BacklightChange(id inProxy, id inApp, bool inState)
{
	[((CocoaScreenProxy*) inProxy)
		forwardBacklightChange:(inState ? YES : NO)
					toListener:((id<CocoaEmulatorApp>) inApp)];
}

// -------------------------------------------------------------------------- //
//  * EmulatorWindow_FinishCreate( TCocoaScreenManager*, NSWindow*, ... )
// -------------------------------------------------------------------------- //
void
EmulatorWindow_FinishCreate(
	TCocoaScreenManager* inScreenManager,
	NSWindow* inWindow,
	NSView* inView,
	NSView* inText)
{
	// Create the power button view.
	NSView* thePowerButtonView = NULL;
	if (inScreenManager != NULL)
	{
		thePowerButtonView = [[TCocoaPowerButtonNSView alloc]
			initWithFrame:[inView frame]
			screenManager:inScreenManager];
#if !__has_feature(objc_arc)
		[thePowerButtonView autorelease];
#endif
	}

	// Add the widgets.
	NSView* theContentView = [inWindow contentView];
	[theContentView addSubview:inText];
	if (thePowerButtonView != NULL)
	{
		[theContentView addSubview:thePowerButtonView];
	}
	[theContentView addSubview:inView];
}

// -------------------------------------------------------------------------- //
//  * EmulatorWindow_CreateFullScreen( id, id, id )
// -------------------------------------------------------------------------- //
id
EmulatorWindow_CreateFullScreen(
	TCocoaScreenManager* inScreenManager,
	id inApp,
	id inView,
	id inText)
{
	NSWindow* theWindow;
	NSScreen* theMainScreen = [NSScreen mainScreen];
	NSRect theScreenRect = [theMainScreen frame];
	theWindow = [[NSWindow alloc]
		initWithContentRect:theScreenRect
				  styleMask:NSBorderlessWindowMask
					backing:NSBackingStoreBuffered
					  defer:YES
					 screen:theMainScreen];
	[theWindow setLevel:CGShieldingWindowLevel()];
	[theWindow setHasShadow:NO];
	[theWindow setBackgroundColor:[NSColor lightGrayColor]];

	EmulatorWindow_FinishCreate(inScreenManager, theWindow, (NSView*) inView, (NSView*) inText);

	// Tell the app about the window.
	[((id<CocoaEmulatorApp>) inApp) setEmulatorWindow:theWindow fullScreen:YES];

	return theWindow;
}

// -------------------------------------------------------------------------- //
//  * EmulatorWindow_Create( id, int, int, id, id )
// -------------------------------------------------------------------------- //
id
EmulatorWindow_Create(id inApp, int inWindowWidth, int inWindowHeight, id inView, id inText)
{
	NSWindow* theWindow;
	NSRect theWindowRect;
	theWindowRect.origin.x = 0;
	theWindowRect.origin.y = 0;
	theWindowRect.size.width = inWindowWidth;
	theWindowRect.size.height = inWindowHeight;
	theWindow = [[NSWindow alloc]
		initWithContentRect:theWindowRect
				  styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask
					backing:NSBackingStoreBuffered
					  defer:YES];
	[theWindow setContentSize:theWindowRect.size];
	[theWindow center];
	[theWindow setTitle:@"Einstein Platform"];

	EmulatorWindow_FinishCreate(NULL, theWindow, (NSView*) inView, (NSView*) inText);

	// Tell the app about the window.
	[((id<CocoaEmulatorApp>) inApp) setEmulatorWindow:theWindow fullScreen:NO];

	return theWindow;
}

// -------------------------------------------------------------------------- //
//  * EmulatorWindow_MakeFront( id )
// -------------------------------------------------------------------------- //
void
EmulatorWindow_MakeFront(id inWindow)
{
	[((NSWindow*) inWindow) makeKeyAndOrderFront:NULL];
}

// -------------------------------------------------------------------------- //
//  * EmulatorWindow_Resize( id, int, int )
// -------------------------------------------------------------------------- //
void
EmulatorWindow_Resize(id inWindow, int inWidth, int inHeight)
{
	NSSize theSize;
	theSize.width = inWidth;
	theSize.height = inHeight;
	[((NSWindow*) inWindow) setContentSize:theSize];
}

// -------------------------------------------------------------------------- //
//  * EmulatorWindow_Close( id )
// -------------------------------------------------------------------------- //
void
EmulatorWindow_Close(id inWindow)
{
	[((NSWindow*) inWindow) close];
}

// -------------------------------------------------------------------------- //
//  * EmulatorWindow_SetFirstResponder( id, id )
// -------------------------------------------------------------------------- //
void
EmulatorWindow_SetFirstResponder(id inWindow, id inView)
{
	dispatch_async(dispatch_get_main_queue(), ^{
		[((NSWindow*) inWindow) makeFirstResponder:inView];
	});
}

// -------------------------------------------------------------------------- //
//  * ResizeForRotation( id, id, int, int )
// -------------------------------------------------------------------------- //
void
ResizeForRotation(id inWindow, id inView, int inWidth, int inHeight)
{
	NSSize theSize;
	theSize.width = inWidth;
	theSize.height = inHeight;
	[((TCocoaScreenView*) inView) setScreenWidth:inWidth height:inHeight orientation:kNormal];
	dispatch_async(dispatch_get_main_queue(), ^{
		[((NSWindow*) inWindow) setContentSize:theSize];
	});
}

// -------------------------------------------------------------------------- //
//  * RotateView( id, EOrientation )
// -------------------------------------------------------------------------- //
void
RotateView(id inView, TScreenManager::EOrientation inScreenOrientation)
{
	TCocoaScreenView* theView = (TCocoaScreenView*) inView;
	NSRect theFrame = [theView frame];

	// Compute the new size.
	Boolean portraitMode = false;
	EOrientation theOrientation = kNormal;
	switch (inScreenOrientation)
	{
		case TScreenManager::kOrientation_AppleTop:
			// kPortrait
			theOrientation = k90Clockwise;
			portraitMode = true;
			break;

		case TScreenManager::kOrientation_AppleRight:
			// kLandscape
			// Normal mac/emate orientation.
			theOrientation = kNormal;
			portraitMode = false;
			break;

		case TScreenManager::kOrientation_AppleBottom:
			// kPortraitFlip
			theOrientation = k270Clockwise;
			portraitMode = true;
			break;

		case TScreenManager::kOrientation_AppleLeft:
			// kLandscapeFlip
			theOrientation = k180Clockwise;
			portraitMode = false;
			break;
	}

	int portraitWidth = theFrame.size.width;
	int portraitHeight = theFrame.size.height;
	if (portraitWidth > portraitHeight)
	{
		int tmp = portraitHeight;
		portraitHeight = portraitWidth;
		portraitWidth = tmp;
	}

	int theWidth;
	int theHeight;
	if (portraitMode)
	{
		theWidth = portraitWidth;
		theHeight = portraitHeight;
	} else
	{
		theWidth = portraitHeight;
		theHeight = portraitWidth;
	}

	[theView setScreenWidth:theWidth height:theHeight orientation:theOrientation];
}

// ========================================================================== //
// DOS Air:                                                                   //
// All the passengers go out onto the runway, grab hold of the plane, push it //
// until it gets in the air, hop on, jump off when it hits the ground again.  //
// Then they grab the plane again, push it back into the air, hop on, et      //
// cetera.                                                                    //
// ========================================================================== //
