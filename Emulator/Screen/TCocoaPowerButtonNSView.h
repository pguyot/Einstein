// ==============================
// File:			TCocoaPowerButtonNSView.h
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

#ifndef _TCOCOAPOWERBUTTONNSVIEW_H
#define _TCOCOAPOWERBUTTONNSVIEW_H

#include <K/Defines/KDefinitions.h>
#import <Cocoa/Cocoa.h>

class TCocoaScreenManager;

@interface TCocoaPowerButtonNSView : NSView
{
	TCocoaScreenManager*	mScreenManager;
}

// Initialisation.
- (id)initWithFrame:(NSRect)frameRect screenManager:(TCocoaScreenManager*)inScreenManager;

// Mouse events.
- (BOOL)mouseDownCanMoveWindow;
- (void)mouseDown:(NSEvent *)theEvent;
@end

#endif
		// _TCOCOAPOWERBUTTONNSVIEW_H

// ====================================================================== //
// This quote is taken from the Diamondback, the University of Maryland   //
// student newspaper, of Tuesday, 3/10/87.                                //
//                                                                        //
//         One disadvantage of the Univac system is that it does not use  //
//         Unix, a recently developed program which translates from one   //
//         computer language to another and has a built-in editing system //
//         which identifies errors in the original program.               //
// ====================================================================== //
