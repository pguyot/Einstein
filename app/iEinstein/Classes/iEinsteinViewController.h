// ==============================
// File:			iEinsteinViewController.h
// Project:			Einstein
//
// Copyright 2010 by Matthias Melcher (einstein@matthiasm.com).
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

#ifndef EINSTEIN_VIEW_CONTROLLER_H
#define EINSTEIN_VIEW_CONTROLLER_H

#include <K/Defines/KDefinitions.h>
#import <UIKit/UIKit.h>


class TNetworkManager;
class TSoundManager;
class TScreenManager;
class TROMImage;
class TEmulator;
class TPlatformManager;
class TLog;

@interface iEinsteinViewController : UIViewController < UIActionSheetDelegate >
{
	TNetworkManager*	mNetworkManager;
	TSoundManager*		mSoundManager;
	TScreenManager*		mScreenManager;
	TROMImage*			mROMImage;
	TEmulator*			mEmulator;
	TPlatformManager*	mPlatformManager;
	TLog*				mLog;  
	int					lastKnownScreenResolution;
  
}

// Release
- (void) dealloc;

// NSNibAwaking
//- (void) awakeFromNib;

// Other delegate methods.
//- (void) applicationWillTerminate: (NSNotification *) notification;

// Actions.
//- (IBAction) installPackage:(id)sender;
//- (IBAction) powerButton:(id)sender;
//- (IBAction) backlightButton:(id)sender;
//- (IBAction) networkButton:(id)sender;

// AppleScript
//- (id)commandInstallPackage:(NSURL*)inFileURL;
//- (id)commandDoNewtonScript:(NSString*)inText;

// Public methods.
//- (NSString*)openFile;
//- (NSString*)saveFile;
- (int)initEmulator;
- (void)startEmulator;
- (void)stopEmulator;
- (void)resetEmulator;
//- (BOOL)canStartEmulator;
//- (void)abortWithMessage:(NSString*)message;
//+ (TCocoaAppController*)getInstance;

// Einstein Emulator App interface.
//- (void) powerChange: (BOOL) power;
//- (void) backlightChange: (BOOL) state;
//- (void) networkChange: (BOOL) state;
//- (void) setEmulatorWindow: (NSWindow*) inWindow fullScreen: (BOOL) inFullScreen;

// Private (sic) methods.
//+ (NSString*)getAppSupportDirectory;
//- (void)installPackageFile:(NSString*)inPath;
//- (void)setupToolbar: (NSWindow*) inWindow;

- (void)verifyDeleteFlashRAM;
- (void)explainMissingROM;
//- (void)openEinsteinMenu;

- (int)allResourcesFound;

@end

extern void openEinsteinMenu(iEinsteinViewController*);


// ============================================================== //
// As of next Thursday, UNIX will be flushed in favor of TOPS-10. //
// Please update your programs.                                   //
// ============================================================== //

#endif