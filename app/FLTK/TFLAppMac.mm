// ==============================
// File:			TFLAppMac
// Project:			Einstein
//
// Copyright 2026 by Matthias Melcher.
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

#include "TFLAppMac.h"
#include "TFLApp.h"

#include <FL/Fl.H>

#import <Cocoa/Cocoa.h>

/**
 Target of the Quit item in the application menu.
 */
@interface TFLQuitTarget : NSObject
- (void)quit:(id)sender;
- (BOOL)validateMenuItem:(NSMenuItem*)item;
@end

@implementation TFLQuitTarget
- (void)quit:(id)sender
{
	(void) sender;
	Fl::lock();
	gApp->UserActionQuit();
	Fl::unlock();
}
- (BOOL)validateMenuItem:(NSMenuItem*)item
{
	// as FLTK does: no Quit while a modal dialog is open
	(void) item;
	return !Fl::modal();
}
@end

/**
 FLTK's Quit item sends a close event to every window. The emulator window
 takes that as Close, which keeps Einstein running while the Settings window
 is open. So the Quit item calls UserActionQuit() instead.
 */
void
TFLRedirectAppMenuQuit()
{
	static TFLQuitTarget* sTarget = [[TFLQuitTarget alloc] init];
	NSMenu* appMenu = [[[NSApp mainMenu] itemAtIndex:0] submenu];
	for (NSMenuItem* item in [appMenu itemArray])
	{
		if ([item action] == @selector(terminate:))
		{
			[item setTarget:sTarget];
			[item setAction:@selector(quit:)];
		}
	}
}
