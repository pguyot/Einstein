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
- (void)initEmulator;
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
//- (void)openEinsteinMenu;

@end

extern void openEinsteinMenu(iEinsteinViewController*);

#if 0


// ==============================
// File:			TCocoaAppController.h
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

#ifndef _TCOCOAAPPCONTROLLER_H
#define _TCOCOAAPPCONTROLLER_H

#include <K/Defines/KDefinitions.h>
#import <Cocoa/Cocoa.h>

#import "Emulator/Screen/TCocoaScreenView.h"
#import "Emulator/Screen/CocoaScreenProxy.h"
#import "Emulator/Screen/CocoaEmulatorApp.h"
#import "TCocoaROMDumperController.h"
#import "TCocoaSetupController.h"

class TBufferLog;
class TNetworkManager;
class TSoundManager;
class TScreenManager;
class TROMImage;
class TEmulator;
class TPlatformManager;
class TLog;

///
/// Protocol pour le contrôleur de l'application Einstein.app.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
@interface TCocoaAppController : NSObject <CocoaEmulatorApp>
{
  IBOutlet NSMenu* mMenuBar;
  IBOutlet TCocoaSetupController* mSetupController;
  IBOutlet TCocoaROMDumperController* mROMDumperController;
  IBOutlet NSUserDefaultsController* mUserDefaultsController;
  
  CocoaScreenProxy*	mProxy;
  
  Boolean				mQuit;
  int					mRAMSize;
  NSString*			mFlashPath;
  NSString*			mROMPath;
  NSString*			mREx0Path;
  
  TNetworkManager*	mNetworkManager;
  TSoundManager*		mSoundManager;
  TScreenManager*		mScreenManager;
  TROMImage*			mROMImage;
  TEmulator*			mEmulator;
  TPlatformManager*	mPlatformManager;
  TLog*				mLog;
  
  NSToolbarItem*		mToolbarPowerItem;
  NSImage*			mToolbarPowerOnImage;
  NSImage*			mToolbarPowerOffImage;
  BOOL				mPowerState;
  NSToolbarItem*		mToolbarBacklightItem;
  NSImage*			mToolbarBacklightOnImage;
  NSImage*			mToolbarBacklightOffImage;
  BOOL				mBacklightState;
  NSToolbarItem*		mToolbarNetworkItem;
  NSImage*			mToolbarNetworkOnImage;
  NSImage*			mToolbarNetworkOffImage;
  BOOL				mNetworkState;
}

// Initialize.
- (id)init;

// Release
- (void)dealloc;

// NSNibAwaking
- (void)awakeFromNib;

// Other delegate methods.
- (void)applicationWillTerminate:(NSNotification*)notification;

// Actions.
- (IBAction)installPackage:(id)sender;
- (IBAction)powerButton:(id)sender;
- (IBAction)backlightButton:(id)sender;
- (IBAction)networkButton:(id)sender;

// AppleScript
- (id)commandInstallPackage:(NSURL*)inFileURL;
- (id)commandDoNewtonScript:(NSString*)inText;

// Public methods.
- (NSString*)openFile;
- (NSString*)saveFile;
- (void)startEmulator;
- (void)resetEmulator;
- (BOOL)canStartEmulator;
- (void)abortWithMessage:(NSString*)message;
+ (TCocoaAppController*)getInstance;

// Einstein Emulator App interface.
- (void)powerChange:(BOOL)power;
- (void)backlightChange:(BOOL)state;
- (void)networkChange:(BOOL)state;
- (void)setEmulatorWindow:(NSWindow*)inWindow fullScreen:(BOOL)inFullScreen;

// Private (sic) methods.
+ (NSString*)getAppSupportDirectory;
- (void)runEmulator;
- (void)installPackageFile:(NSString*)inPath;
- (void)setupToolbar:(NSWindow*)inWindow;
@end

#endif
// _TCOCOAAPPCONTROLLER_H

// ============================================================== //
// As of next Thursday, UNIX will be flushed in favor of TOPS-10. //
// Please update your programs.                                   //
// ============================================================== //

#endif