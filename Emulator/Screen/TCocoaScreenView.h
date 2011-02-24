// ==============================
// File:			TCocoaScreenView.h
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

#ifndef _TCOCOASCREENVIEW_H
#define _TCOCOASCREENVIEW_H

#include <K/Defines/KDefinitions.h>
#import <Cocoa/Cocoa.h>

class TCocoaScreenManager;

enum EOrientation {
	kNormal,
	k90Clockwise,
	k180Clockwise,
	k270Clockwise
};

@interface TCocoaScreenView : NSView
{
	TCocoaScreenManager*	mScreenManager;
	CGImageRef				mScreenImage;
	BOOL					mVisible;
	int						mWidth;
	int						mHeight;
	EOrientation			mOrientation;
	KUInt32					mPreviousMods;
}

// Initialisation.
- (id)initWithFrame:(NSRect)frameRect screenManager:(TCocoaScreenManager*)inScreenManager;

// Interface with the screen manager.
- (void)setScreenWidth:(int) inWidth height:(int) inHeight orientation:(EOrientation) inOrientation;
- (void)drawRect:(NSRect)rect;

// Mouse events.
- (BOOL)mouseDownCanMoveWindow;
- (void)mouseDown:(NSEvent *)theEvent;
- (BOOL)mouseInView;

// Drag & drop of packages.
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender;
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender;

// Other
- (BOOL)isOpaque;

// Private functions.
- (NSPoint)convertPenPoint:(NSPoint)inMousePoint;
@end

#endif
		// _TCOCOASCREENVIEW_H

// ========================================= //
// If it has syntax, it isn't user friendly. //
// ========================================= //
