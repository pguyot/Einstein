// ==============================
// File:			TCocoaSetupController.h
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

#ifndef _TCOCOASETUPCONTROLLER_H
#define _TCOCOASETUPCONTROLLER_H

#include <K/Defines/KDefinitions.h>

#import <Cocoa/Cocoa.h>

class TBufferLog;
class TNetworkManager;
class TSoundManager;
class TScreenManager;
class TROMImage;
class TEmulator;
class TPlatformManager;
@class TCocoaAppController;

///
/// Protocol for the setup dialog controller.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
@interface TCocoaSetupController : NSObject
{
	IBOutlet NSSlider* mRAMSizeSlider;
	IBOutlet NSTextField* mRAMSizeTextField;
	IBOutlet NSWindow* mSetupWindow;
	IBOutlet NSButton* mStartSaveButton;
	IBOutlet NSButton* mQuitRevertButton;
	IBOutlet NSTextField* mOnlyAtRestartTextField;
	IBOutlet NSUserDefaultsController* mUserDefaultsController;
	IBOutlet TCocoaAppController* mAppController;
	IBOutlet NSMenuItem* mPreferencesMenuItem;
	
	BOOL	buttonsAreStartAndQuit;
}

// NSNibAwaking
- (void)awakeFromNib;

// Actions.
- (IBAction)chooseFlashFile:(id)sender;
- (IBAction)chooseROMFile:(id)sender;
- (IBAction)updateRAMSizeSlider:(id)sender;
- (IBAction)saveOrStart:(id)sender;
- (IBAction)revertOrQuit:(id)sender;
- (IBAction)openPreferences:(id)sender;

// NSMenuValidation protocol
- (BOOL)validateMenuItem:(NSMenuItem*)menuItem;

// Public methods.
- (void)openSetupWindow;
- (void)closeSetupWindow;
- (void)updateStartButtonState;

// Private methods.
- (void)setupRAMSizeWidgets;
- (void)updateRAMSizeTextField:(int) inValue;
@end

#endif
		// _TCOCOASETUPCONTROLLER_H

// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //
