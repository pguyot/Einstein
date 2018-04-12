// ==============================
// File:			TCocoaSetupController.mm
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

#include <K/Defines/KDefinitions.h>
#import "TCocoaSetupController.h"

// Einstein
#import "TCocoaUserDefaults.h"
#import "TCocoaAppController.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

@implementation TCocoaSetupController

// -------------------------------------------------------------------------- //
//  * (IBAction)chooseFlashFile:(id)
// -------------------------------------------------------------------------- //
- (IBAction)chooseFlashFile:(id)sender
{
	NSString* thePath = [mAppController saveFile];
	if (thePath)
	{
		[[mUserDefaultsController values]
			setValue: thePath
			forKey: kInternalFlashPathKey];
	}
}

// -------------------------------------------------------------------------- //
//  * (IBAction)chooseROMFile:(id)
// -------------------------------------------------------------------------- //
- (IBAction)chooseROMFile:(id)sender
{
	NSString* thePath = [mAppController openFile];
	if (thePath)
	{
		[[mUserDefaultsController values]
			setValue: thePath
			forKey: kROMImagePathKey];

		[mStartSaveButton setEnabled: YES];
	}
}

// -------------------------------------------------------------------------- //
//  * (void)setupRAMSizeWidgets
// -------------------------------------------------------------------------- //
- (void)setupRAMSizeWidgets
{
	int theRAMSize = (int)[[mUserDefaultsController defaults] integerForKey: kRAMSizeKey];
	[mRAMSizeSlider setIntValue: theRAMSize];
	[self updateRAMSizeTextField: theRAMSize];
}

// -------------------------------------------------------------------------- //
//  * (IBAction)updateRAMSizeSlider:(id)
// -------------------------------------------------------------------------- //
- (IBAction)updateRAMSizeSlider:(id)sender
{
	int ramSize = [mRAMSizeSlider intValue];

	[[mUserDefaultsController values]
			setValue: [NSNumber numberWithInt: ramSize]
			forKey: kRAMSizeKey];

	[self updateRAMSizeTextField: ramSize];
}

// -------------------------------------------------------------------------- //
//  * (void)updateRAMSizeTextField: (int)
// -------------------------------------------------------------------------- //
- (void)updateRAMSizeTextField: (int) inValue
{
	// Compute the actual ram size in MB.
	int nbOfMB = inValue / 16;
	int nbOfKB = (inValue % 16) * 64;
	
	if (nbOfKB == 0)
	{
		[mRAMSizeTextField setStringValue: [NSString stringWithFormat: @"%i MB", nbOfMB]];
	} else {
		[mRAMSizeTextField setStringValue: [NSString stringWithFormat: @"%i MB %i KB", nbOfMB, nbOfKB]];
	}
}

// -------------------------------------------------------------------------- //
//  * (IBAction)saveOrStart:(id)
// -------------------------------------------------------------------------- //
- (IBAction)saveOrStart:(id)sender
{
	// Save the preferences.
	[mUserDefaultsController save: sender];
	[[mUserDefaultsController defaults] synchronize];
	
	if (buttonsAreStartAndQuit)
	{
		// Disable the button.
		[mStartSaveButton setEnabled: NO];
		
		// Start the emulator
		[mAppController startEmulator];
	} else {
		[self closeSetupWindow];
	}
}

// -------------------------------------------------------------------------- //
//  * (IBAction)revertOrQuit:(id)
// -------------------------------------------------------------------------- //
- (IBAction)revertOrQuit:(id)sender
{
	// Revert the preferences.
	[mUserDefaultsController revert: sender];

	if (buttonsAreStartAndQuit)
	{
		// Quit the emulator
		[NSApp terminate: sender];
	} else {
		[self closeSetupWindow];
	}
}

// -------------------------------------------------------------------------- //
//  * (void)awakeFromNib
// -------------------------------------------------------------------------- //
- (void)awakeFromNib
{
	[mSetupWindow center];
}

// -------------------------------------------------------------------------- //
//  * (void)openSetupWindow
// -------------------------------------------------------------------------- //
- (void)openSetupWindow
{
	buttonsAreStartAndQuit = true;
	[mStartSaveButton setHidden: NO];
	[mStartSaveButton setEnabled: [mAppController canStartEmulator]];
	[mStartSaveButton setTitle: @"Start"];
	[mQuitRevertButton setHidden: NO];
	[mQuitRevertButton setEnabled: YES];
	[mQuitRevertButton setTitle: @"Quit"];
	[mOnlyAtRestartTextField setHidden: YES];
	[self setupRAMSizeWidgets];
	[mSetupWindow makeKeyAndOrderFront: self];
	[mUserDefaultsController setAppliesImmediately: YES];
}

// -------------------------------------------------------------------------- //
//  * (IBAction)openPreferences:(id)sender
// -------------------------------------------------------------------------- //
- (IBAction)openPreferences:(id)sender
{
	buttonsAreStartAndQuit = false;
	[mStartSaveButton setHidden: NO];
	[mStartSaveButton setEnabled: YES];
	[mStartSaveButton setTitle: @"Save"];
	[mQuitRevertButton setHidden: NO];
	[mQuitRevertButton setEnabled: YES];
	[mQuitRevertButton setTitle: @"Revert"];
	[mOnlyAtRestartTextField setHidden: NO];
	[self setupRAMSizeWidgets];
	[mSetupWindow makeKeyAndOrderFront: self];
	[mUserDefaultsController setAppliesImmediately: NO];
}

// -------------------------------------------------------------------------- //
//  * (void)closeSetupWindow
// -------------------------------------------------------------------------- //
- (void)closeSetupWindow
{
	[mSetupWindow close];
}

// -------------------------------------------------------------------------- //
//  * (BOOL)validateMenuItem:(NSMenuItem*)menuItem
// -------------------------------------------------------------------------- //
- (BOOL)validateMenuItem:(NSMenuItem*)menuItem
{
	if ([menuItem action] == @selector(openPreferences:))
	{
		return ![mSetupWindow isVisible];
	}
	
	return YES;
}

// -------------------------------------------------------------------------- //
//  * (void)updateStartButtonState
// -------------------------------------------------------------------------- //
- (void)updateStartButtonState
{
	[mStartSaveButton setEnabled: [mAppController canStartEmulator]];
}

@end

// =================================================================== //
// In English, every word can be verbed.  Would that it were so in our //
// programming languages.                                              //
// =================================================================== //
