// ==============================
// File:			TEinsteinApplication.h
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

#ifndef _TEINSTEINAPPLICATION_H
#define _TEINSTEINAPPLICATION_H

#include <K/Defines/KDefinitions.h>
#import <Cocoa/Cocoa.h>

#import "Emulator/Screen/TCocoaScreenView.h"

///
/// Sous-classe de NSApplication.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 131 $
///
/// \test	aucun test défini.
///
@interface TEinsteinApplication : NSApplication
{
	TCocoaScreenView*	mView;
}

// - init
- (id)init;

// - events
- (void)sendEvent:(NSEvent *)anEvent;

// - register/unregister the emulator view.
- (void)registerView:(TCocoaScreenView *)inView;
- (void)unregisterView;

@end

#endif
		// _TEINSTEINAPPLICATION_H

// =============================== //
// System going down in 5 minutes. //
// =============================== //
