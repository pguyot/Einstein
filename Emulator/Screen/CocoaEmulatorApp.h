// ==============================
// File:			CocoaEmulatorApp.h
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

#ifndef _COCOAEMULATORAPP_H
#define _COCOAEMULATORAPP_H

#include <K/Defines/KDefinitions.h>
#import <Cocoa/Cocoa.h>

///
/// Protocol for the emulator application controller.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
@protocol CocoaEmulatorApp
- (void) powerChange: (BOOL) state;
- (void) backlightChange: (BOOL) state;
- (void) networkChange: (BOOL) state;
- (void) setEmulatorWindow: (NSWindow*) inWindow fullScreen: (BOOL) inFullScreen;
@end

#endif
		// _COCOAEMULATORAPP_H

// ============================================================================= //
// ===  ALL USERS PLEASE NOTE  ========================                          //
//                                                                               //
// Compiler optimizations have been made to macro expand LET into a WITHOUT-     //
// INTERRUPTS special form so that it can PUSH things into a stack in the        //
// LET-OPTIMIZATION area, SETQ the variables and then POP them back when it's    //
// done.  Don't worry about this unless you use multiprocessing.                 //
// Note that LET *could* have been defined by:                                   //
//                                                                               //
//         (LET ((LET '`(LET ((LET ',LET))                                       //
//                         ,LET)))                                               //
//         `(LET ((LET ',LET))                                                   //
//                 ,LET))                                                        //
//                                                                               //
// This is believed to speed up execution by as much as a factor of 1.01 or      //
// 3.50 depending on whether you believe our friendly marketing representatives. //
// This code was written by a new programmer here (we snatched him away from     //
// Itty Bitti Machines where we was writting COUGHBOL code) so to give him       //
// confidence we trusted his vows of "it works pretty well" and installed it.    //
// ============================================================================= //
