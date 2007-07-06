// ==============================
// File:			TCocoaScreenGlue.h
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

#ifndef _TCOCOASCREENVIEWGLUE_H
#define _TCOCOASCREENVIEWGLUE_H

#include <K/Defines/KDefinitions.h>

// ObjC
#include <objc/objc.h>

// Einstein
#include "Emulator/Screen/TScreenManager.h"

class TCocoaScreenManager;

id		EmulatorText_Create( int inWindowWidth, int inWindowHeight );
void	EmulatorText_SetScreenOffString( id inProxy, id inEmulatorText );
void	EmulatorText_SetEinsteinOffString( id inProxy, id inEmulatorText );
id		TCocoaScreenView_Create(
			TCocoaScreenManager* inManager,
			int inWindowWidth,
			int inWindowHeight);
void	TCocoaScreenView_SetNeedsDisplay( id inProxy, id inView );
void	TCocoaScreenView_SetHidden( id inProxy, id inView, bool inHidden );
void	EmulatorApp_PowerChange( id inProxy, id inListener, bool inState );
void	EmulatorApp_BacklightChange( id inProxy, id inListener, bool inState );
id		EmulatorWindow_CreateFullScreen(
			TCocoaScreenManager* inManager,
			id inApp,
			id inView,
			id inText);
id		EmulatorWindow_Create(
			id inApp,
			int inWidth,
			int inHeight,
			id inView,
			id inText);
void	EmulatorWindow_MakeFront( id inWindow );
void	EmulatorWindow_Close( id inWindow );
void	EmulatorWindow_SetFirstResponder( id inWindow, id inView );
void	ResizeForRotation( id inWindow, id inView, int inWidth, int inHeight );
void	RotateView(
			id inView,
			TScreenManager::EOrientation inScreenOrientation);

#endif
		// _TCOCOASCREENVIEWGLUE_H

// ========================================================================= //
// No, I'm not interested in developing a powerful brain.  All I'm after is  //
// just a mediocre brain, something like the president of American Telephone //
// and Telegraph Company.                                                    //
//                 -- Alan Turing on the possibilities of a thinking         //
//                    machine, 1943.                                         //
// ========================================================================= //
