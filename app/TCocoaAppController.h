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
#import "TCocoaMonitorController.h"

class TBufferLog;
class TNetworkManager;
class TSoundManager;
class TScreenManager;
class TSerialPortManager;
class TROMImage;
class TEmulator;
class TPlatformManager;
class TLog;
class TMacMonitor;
class TSymbolList;
class TFileManager;

///
/// Protocol pour le contrôleur de l'application Einstein.app.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
@interface TCocoaAppController : NSObject <CocoaEmulatorApp, NSApplicationDelegate, NSToolbarDelegate, NSFileManagerDelegate>
{
	IBOutlet NSMenu* mMenuBar;
	IBOutlet TCocoaSetupController* mSetupController;
	IBOutlet TCocoaROMDumperController* mROMDumperController;
	IBOutlet TCocoaMonitorController* mMonitorController;
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
	TSerialPortManager* mExtrSerialPortManager;
	TROMImage*			mROMImage;
	TEmulator*			mEmulator;
	TPlatformManager*	mPlatformManager;
	TFileManager*       mFileManager;
    TLog*				mLog;
	TMacMonitor*		mMonitor;
	TBufferLog*			mMonitorLog;
	TSymbolList*		mSymbolList;
	
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
+ (void)initialize;
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
- (IBAction)showMonitor:(id)sender;

// AppleScript
- (id)commandInstallPackage:(NSURL*)inFileURL;
- (id)commandDoNewtonScript:(NSString*)inText;

// Public methods.
- (NSString*)openFile;
- (NSString*)saveFile;
- (void)startEmulator;
- (BOOL)canStartEmulator;
- (void)abortWithMessage:(NSString*)message;
+ (TCocoaAppController*)getInstance;

// Einstein Emulator App interface.
- (void)powerChange:(BOOL)power;
- (void)backlightChange:(BOOL)state;
- (void)networkChange:(BOOL)state;
- (void)setEmulatorWindow:(NSWindow*)inWindow fullScreen:(BOOL)inFullScreen;

@end

#endif
		// _TCOCOAAPPCONTROLLER_H

// ============================================================== //
// As of next Thursday, UNIX will be flushed in favor of TOPS-10. //
// Please update your programs.                                   //
// ============================================================== //
