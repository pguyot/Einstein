// ==============================
// File:			TCocoaAppController.mm
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
#import "TCocoaAppController.h"

// ANSI & POSIX
#include <stdlib.h>
#include <stdio.h>

// Einstein
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/Network/TTapNetwork.h"
#include "Emulator/Sound/TCoreAudioSoundManager.h"
#include "Emulator/Sound/TPortAudioSoundManager.h"
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Screen/CocoaScreenProxy.h"
#include "Emulator/Screen/TCocoaScreenManager.h"
#include "Emulator/Screen/TX11ScreenManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TStdOutLog.h"

#import "TCocoaUserDefaults.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define machinestr(tag)	\
	(((tag) == 717006) ? "717006" : \
		(((tag) == 737041) ? "737041" : \
			"747129"))

static TCocoaAppController* gInstance = nil;

@implementation TCocoaAppController

// -------------------------------------------------------------------------- //
//  * initialize
// -------------------------------------------------------------------------- //
+ (void) initialize
{
	NSString* theAppSupportDir = [self getAppSupportDirectory];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSDictionary *appDefaults =
		[NSDictionary
			dictionaryWithObjectsAndKeys:
			[theAppSupportDir stringByAppendingPathComponent: @"newton.rom"],
			kROMImagePathKey,
			[theAppSupportDir stringByAppendingPathComponent: @"internal.flash"],
			kInternalFlashPathKey,
			[NSNumber numberWithInt: 717006],
			kMachineKey,
			[NSNumber numberWithInt: TScreenManager::kDefaultPortraitWidth],
			kScreenWidthKey,
			[NSNumber numberWithInt: TScreenManager::kDefaultPortraitHeight],
			kScreenHeightKey,
			[NSNumber numberWithBool: NO],
			kFullScreenKey,
			[NSNumber numberWithInt: 0x40],
			kRAMSizeKey,
			[NSNumber numberWithInt: kCoreAudioDriverTag],
			kAudioDriverKey,
			[NSNumber numberWithInt: kCocoaScreenDriverTag],
			kScreenDriverKey,
			[NSNumber numberWithInt: kUsermodeNetworkDriverTag],
			kNetworkDriverKey,
			[NSNumber numberWithBool:NO],
			kDontShowAtStartupKey,
			NULL
			];
	[defaults registerDefaults:appDefaults];
}

// -------------------------------------------------------------------------- //
//  * init
// -------------------------------------------------------------------------- //
- init
{
	if ((self = [super init]))
	{
		mProxy = [[CocoaScreenProxy new] init];
		mRAMSize = 0x40;	// 4 MB.
		mROMPath = NULL;
		mREx0Path = NULL;
		mNetworkManager = NULL;
		mSoundManager = NULL;
		mScreenManager = NULL;
		mROMImage = NULL;
		mEmulator = NULL;
		mPlatformManager = NULL;
		mLog = NULL;
		
		mToolbarPowerItem = NULL;
		mPowerState = YES;
		mToolbarBacklightItem = NULL;
		mBacklightState = NO;

		// Retrieve toolbar dynamic images.
		mToolbarPowerOnImage = [NSImage imageNamed:@"button_power_on"];
		mToolbarPowerOffImage = [NSImage imageNamed:@"button_power"];
		mToolbarBacklightOnImage = [NSImage imageNamed:@"button_backlight_on"];
		mToolbarBacklightOffImage = [NSImage imageNamed:@"button_backlight"];
		
		// Single instance.
		gInstance = self;
	}

	return self;
}

// -------------------------------------------------------------------------- //
//  * (void)dealloc
// -------------------------------------------------------------------------- //
- (void) dealloc
{
	if (mEmulator)
	{
		delete mEmulator;
	}
	if (mScreenManager)
	{
		delete mScreenManager;
	}
	if (mNetworkManager)
	{
		delete mNetworkManager;
	}
	if (mSoundManager)
	{
		delete mSoundManager;
	}
	if (mROMImage)
	{
		delete mROMImage;
	}
	if (mLog)
	{
		delete mLog;
	}
	
	[super dealloc];
}

// -------------------------------------------------------------------------- //
//  * (TCocoaAppController*)getInstance
// -------------------------------------------------------------------------- //
+ (TCocoaAppController*) getInstance
{
	return gInstance;
}

// -------------------------------------------------------------------------- //
//  * (void)awakeFromNib
// -------------------------------------------------------------------------- //
- (void)awakeFromNib
{
	NSUserDefaults* defaults = [mUserDefaultsController defaults];
	if ([defaults boolForKey: kDontShowAtStartupKey]
		&& ![defaults boolForKey: kFullScreenKey]
		&& [self canStartEmulator])
	{
		[self startEmulator];
	} else {
		[mSetupController openSetupWindow];
  	}
}

// -------------------------------------------------------------------------- //
//  * (BOOL)canStartEmulator
// -------------------------------------------------------------------------- //
- (BOOL)canStartEmulator
{
	BOOL canStart = NO;

	NSUserDefaults* defaults = [mUserDefaultsController defaults];
	NSString* thePath = [defaults stringForKey: kROMImagePathKey];
	if (thePath != nil)
	{
		NSFileManager* theFileManager = [NSFileManager defaultManager];
		canStart = [theFileManager fileExistsAtPath: thePath];
	}
	
	return canStart;
}

// -------------------------------------------------------------------------- //
//  * (void)startEmulator
// -------------------------------------------------------------------------- //
- (void)startEmulator
{
	NSUserDefaults* defaults = [mUserDefaultsController defaults];

	// Create the ROM.
	NSString* einsteinRExPath;
	NSBundle* thisBundle = [NSBundle bundleForClass:[self class]];
	if (!(einsteinRExPath = [thisBundle pathForResource:@"Einstein" ofType:@"rex"]))
	{
		[self abortWithMessage: @"Couldn't load Einstein REX"];
		return;
	}
	
	NSString* theROMPath = [defaults stringForKey: kROMImagePathKey];
	if (theROMPath == nil)
	{
		[self abortWithMessage: @"No path set for ROM"];
		return;
	}

	NSFileManager* theFileManager = [NSFileManager defaultManager];
	if (![theFileManager fileExistsAtPath: theROMPath])
	{
		[self abortWithMessage: @"ROM file doesn't seem to exist"];
		return;
	}
	
	NSString* theREX0Path = nil;
	if ([theROMPath hasSuffix: @".aif"])
	{
		int theLength = [theROMPath length];
		theREX0Path = [[theROMPath substringToIndex: (theLength - 4)] stringByAppendingString: @".rex"];
		if (![theFileManager fileExistsAtPath: theREX0Path])
		{
			theREX0Path = nil;
		}
	} else if ([theROMPath hasSuffix: @" image"]) {
		int theLength = [theROMPath length];
		theREX0Path = [[theROMPath substringToIndex: (theLength - 6)] stringByAppendingString: @" high"];
		if (![theFileManager fileExistsAtPath: theREX0Path])
		{
			theREX0Path = nil;
		}
	}
	
	int theMachine = [defaults integerForKey: kMachineKey];
	if (theREX0Path)
	{
		mROMImage = new TAIFROMImageWithREXes(
							[theROMPath UTF8String],
							[theREX0Path UTF8String],
							[einsteinRExPath UTF8String],
							machinestr(theMachine) );
	} else {
		mROMImage = new TFlatROMImageWithREX(
							[theROMPath UTF8String],
							[einsteinRExPath UTF8String],
							machinestr(theMachine) );
	}
	
	// Create a log if possible
#ifdef _DEBUG
	mLog = 0L; // new TStdOutLog(); 
#endif
	
	// Create the network manager.
	int indexNetworkDriver = [defaults integerForKey: kNetworkDriverKey];
	if (indexNetworkDriver == kUsermodeNetworkDriverTag)
	{
		mNetworkManager = new TUsermodeNetwork(new TStdOutLog());
	} else if (indexNetworkDriver == kTapNetworkDriverTag) {
		mNetworkManager = new TTapNetwork(mLog);
	} else {
		mNetworkManager = new TNullNetwork(mLog);
	}
	
	// Create the sound manager.
	int indexAudioDriver = [defaults integerForKey: kAudioDriverKey];
	if (indexAudioDriver == kCoreAudioDriverTag)
	{
		mSoundManager = new TCoreAudioSoundManager(mLog);
#if OPTION_PORT_AUDIO          
        } else if (indexAudioDriver == kPortAudioDriverTag) {
		mSoundManager = new TPortAudioSoundManager(mLog);
#endif
	} else {
		mSoundManager = new TNullSoundManager(mLog);
	}

	// Create the screen manager.
	Boolean fullScreen = [defaults boolForKey: kFullScreenKey] == YES ? true : false;
	Boolean screenIsLandscape = true;
	int indexScreenDriver = [defaults integerForKey: kScreenDriverKey];
	if (indexScreenDriver == kCocoaScreenDriverTag)
	{
		int theWidth;
		int theHeight;
		if (fullScreen)
		{
			// Mac/eMate orientation.
			NSRect theRect = [[NSScreen mainScreen] frame];
			if (theRect.size.width > theRect.size.height)
			{
				screenIsLandscape = true;
				theWidth = (int) theRect.size.height;
				theHeight = (int) theRect.size.width;
			} else {
				screenIsLandscape = false;
				theWidth = (int) theRect.size.width;
				theHeight = (int) theRect.size.height;
			}
		} else {
			theWidth = [defaults integerForKey: kScreenWidthKey];
			theHeight = [defaults integerForKey: kScreenHeightKey];
		}
		mScreenManager = new TCocoaScreenManager(
									mProxy,
									self,
									mLog,
									theWidth,
									theHeight,
									fullScreen,
									screenIsLandscape);
#ifdef OPTION_X11_SCREEN
	} else {
		KUInt32 theWidth;
		KUInt32 theHeight;
		if (fullScreen)
		{
			KUInt32 theScreenWidth;
			KUInt32 theScreenHeight;
			TX11ScreenManager::GetScreenSize(&theScreenWidth, &theScreenHeight);
			if (theScreenWidth >= theScreenHeight)
			{
				screenIsLandscape = true;
				theWidth = theScreenHeight;
				theHeight = theScreenWidth;
			} else {
				screenIsLandscape = false;
				theWidth = theScreenWidth;
				theHeight = theScreenHeight;
			}
		} else {
			theWidth = [defaults integerForKey: kScreenWidthKey];
			theHeight = [defaults integerForKey: kScreenHeightKey];
		}
		mScreenManager = new TX11ScreenManager(
									mLog,
									theWidth,
									theHeight,
									fullScreen,
									screenIsLandscape);
#endif
	}

	// Create the emulator.
	int ramSize = [defaults integerForKey: kRAMSizeKey];
	const char* theFlashPath =
		[[defaults stringForKey: kInternalFlashPathKey] UTF8String];
	mEmulator = new TEmulator(
				mLog, mROMImage, theFlashPath,
				mSoundManager, mScreenManager, mNetworkManager, ramSize << 16 );
	mPlatformManager = mEmulator->GetPlatformManager();
	if (indexScreenDriver == kCocoaScreenDriverTag)
	{
		((TCocoaScreenManager*) mScreenManager)
			->SetPlatformManager( mPlatformManager );
	}
	
	// Close the window.
	[mSetupController closeSetupWindow];
	
	// Start the thread.
	[NSThread detachNewThreadSelector: @selector(runEmulator) toTarget: self withObject: NULL];
}


// -------------------------------------------------------------------------- //
//  * (IBAction)insertNE2000Card:(id)
// -------------------------------------------------------------------------- //
- (IBAction)insertNE2000Card:(id)sender
{
  mPlatformManager->SendNE2000CardEvent();
}


// -------------------------------------------------------------------------- //
//  * (IBAction)powerButton:(id)
// -------------------------------------------------------------------------- //
- (IBAction)powerButton:(id)sender
{
	mPlatformManager->SendPowerSwitchEvent();
}

// -------------------------------------------------------------------------- //
//  * (IBAction)backlightButton:(id)
// -------------------------------------------------------------------------- //
- (IBAction)backlightButton:(id)sender
{
	mPlatformManager->SendBacklightEvent();
}

// -------------------------------------------------------------------------- //
//  * (IBAction)installPackage:(id)
// -------------------------------------------------------------------------- //
- (IBAction)installPackage:(id)sender
{
	// Ask for a file.
	NSString* theFile = [self openFile];
	
	// Install it.
	[self installPackageFile: theFile];
}

// -------------------------------------------------------------------------- //
//  * (void)installPackageFile:(NSString*)
// -------------------------------------------------------------------------- //
- (void)installPackageFile:(NSString*)inPath
{
	mPlatformManager->InstallPackage([inPath UTF8String]);
}

// -------------------------------------------------------------------------- //
//  * (id)commandInstallPackage:(NSURL*)
// -------------------------------------------------------------------------- //
- (id)commandInstallPackage:(NSURL*)inFileURL
{
	mPlatformManager->InstallPackage([[inFileURL path] UTF8String]);
	return NULL;
}

// -------------------------------------------------------------------------- //
//  * (id)commandDoNewtonScript:(NSString*)
// -------------------------------------------------------------------------- //
- (id)commandDoNewtonScript:(NSString*)inText
{
	mPlatformManager->EvalNewtonScript([inText UTF8String]);
	return NULL;
}

// -------------------------------------------------------------------------- //
//  * (void)abortWithMessage:(NSString*)
// -------------------------------------------------------------------------- //
- (void)abortWithMessage:(NSString*)message
{
	// Show an alert.
	NSAlert* theAlert = [[NSAlert alloc] init];
	[theAlert setMessageText: @"Einstein Emulator must exit"];
	[theAlert setInformativeText: message];
	[theAlert setAlertStyle: NSCriticalAlertStyle];
	
	(void) [theAlert runModal];
	::abort();
}

// -------------------------------------------------------------------------- //
//  * (void)runEmulator
// -------------------------------------------------------------------------- //
- (void)runEmulator
{
	mEmulator->Run();
	// Quit if the emulator quitted.
	mQuit = true;
	[[NSApplication sharedApplication] terminate: self];
}

// -------------------------------------------------------------------------- //
//  * (void) applicationWillTerminate: (NSNotification *) notification
// -------------------------------------------------------------------------- //
- (void) applicationWillTerminate: (NSNotification *) notification
{
	if (!mQuit)
	{
		mEmulator->Quit();
	}
}

// ------------------------------------------------------------------------- //
//  * setupToolbar: (NSWindow*)
// ------------------------------------------------------------------------- //
- (void) setupToolbar: (NSWindow*) inWindow
{
	NSToolbar *toolbar = [[NSToolbar alloc] initWithIdentifier:@"mainToolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];	
	[toolbar setAutosavesConfiguration:YES];
	[inWindow setToolbar:[toolbar autorelease]];
}

// ------------------------------------------------------------------------- //
//  * toolbar: (NSToolbar *) itemForItemIdentifier: (NSString *) ... 
// ------------------------------------------------------------------------- //
- (NSToolbarItem *) toolbar: (NSToolbar *)toolbar
					itemForItemIdentifier:(NSString *)itemIdentifier
					willBeInsertedIntoToolbar:(BOOL)flag
{
#pragma unused ( toolbar )

	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier];
	
	if ( [itemIdentifier isEqualToString:@"Preferences"] ) {
		[item setLabel:NSLocalizedString(@"Preferences",nil)];
		[item setPaletteLabel:[item label]];
		[item setTarget:self];
		[item setAction:@selector(showPreferences:)];
		[item setImage:[NSImage imageNamed:@"button_prefs"]];
		[item setEnabled:NO];
	} else if ( [itemIdentifier isEqualToString:@"Install"] ) {
		[item setLabel:NSLocalizedString(@"Install Package",nil)];
		[item setPaletteLabel:[item label]];
		[item setTarget:self];
		[item setAction:@selector(installPackage:)];
		[item setImage:[NSImage imageNamed:@"button_install"]];
		[item setEnabled:YES];
	} else if ( [itemIdentifier isEqualToString:@"Power"] ) {
		if (flag == YES)
		{
			if (mToolbarPowerItem != NULL)
			{
				[mToolbarPowerItem release];
				mToolbarPowerItem = NULL;
			}
			mToolbarPowerItem = item;
			[mToolbarPowerItem retain];
		}
		[item setLabel:NSLocalizedString(@"Power",nil)];
		[item setPaletteLabel:[item label]];
		[item setTarget:self];
		[item setAction:@selector(powerButton:)];
		if (flag == YES)
		{
			[item setImage: 
				(mPowerState == YES) ?
					mToolbarPowerOnImage : mToolbarPowerOffImage];
		} else {
			[item setImage: mToolbarPowerOnImage];
		}
		[item setEnabled: YES];
	} else if ( [itemIdentifier isEqualToString:@"Backlight"] ) {
		if (flag == YES)
		{
			if (mToolbarBacklightItem != NULL)
			{
				[mToolbarBacklightItem release];
				mToolbarBacklightItem = NULL;
			}
			mToolbarBacklightItem = item;
			[mToolbarBacklightItem retain];
		}
		[item setLabel:NSLocalizedString(@"Backlight",nil)];
		[item setPaletteLabel:[item label]];
		[item setTarget:self];
		[item setAction:@selector(backlightButton:)];
		if (flag == YES)
		{
			[item setImage: 
				(mBacklightState == YES) ?
					mToolbarBacklightOnImage : mToolbarBacklightOffImage];
		} else {
			[item setImage: mToolbarBacklightOffImage];
		}
		[item setEnabled: YES];
	}
	return [item autorelease];
}

// ------------------------------------------------------------------------- //
//  * toolbarAllowedItemIdentifiers: (NSToolbar*)
// ------------------------------------------------------------------------- //
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) toolbar
{
#pragma unused ( toolbar )

	return [NSArray arrayWithObjects:NSToolbarSeparatorItemIdentifier,
					NSToolbarSpaceItemIdentifier,
					NSToolbarFlexibleSpaceItemIdentifier,
					NSToolbarCustomizeToolbarItemIdentifier, 
					@"Preferences", @"Install", @"Power", @"Backlight", nil];
}

// ------------------------------------------------------------------------- //
//  * toolbarDefaultItemIdentifiers: (NSToolbar*)
// ------------------------------------------------------------------------- //
- (NSArray *) toolbarDefaultItemIdentifiers: (NSToolbar*) inToolbar
{
#pragma unused ( inToolbar )

	return [NSArray arrayWithObjects:
			@"Power",
			@"Backlight",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Install",
			NSToolbarCustomizeToolbarItemIdentifier,
			nil];
}

// ------------------------------------------------------------------------- //
//  * getAppSupportDirectory
// ------------------------------------------------------------------------- //
+ (NSString*) getAppSupportDirectory
{
	NSString* result = nil;

	// We'll use the file internal.flash into ~/Library/Application Support/Einstein Platform/
	FSRef appSupFolderRef;
	OSStatus theErr;
	do {
		theErr = FSFindFolder(
					kUserDomain,
					kApplicationSupportFolderType,
					kCreateFolder,
					&appSupFolderRef );
		if (theErr != noErr)
		{
			break;
		}
		
		UInt8 cFlashPath[PATH_MAX];
		theErr = FSRefMakePath(&appSupFolderRef, cFlashPath, sizeof(cFlashPath));
		if (theErr != noErr)
		{
			break;
		}

		NSFileManager* theFileManager = [NSFileManager defaultManager];
		NSString* appSupFolderPath =
			[theFileManager
					stringWithFileSystemRepresentation: (const char*) cFlashPath
					length: strlen((const char*) cFlashPath) ];
		BOOL isDir;
		if (![theFileManager fileExistsAtPath: appSupFolderPath isDirectory: &isDir])
		{
			if ([theFileManager createDirectoryAtPath: appSupFolderPath withIntermediateDirectories:NO attributes: nil error:nil] != YES)
			{
				theErr = errno;
				break;
			}
		} else if (!isDir) {
			theErr = ENOTDIR;
			break;
		}

		result =
			[appSupFolderPath
				stringByAppendingPathComponent: @"Einstein Platform"];
		if (![theFileManager fileExistsAtPath: result isDirectory: &isDir])
		{
			if ([theFileManager createDirectoryAtPath: result withIntermediateDirectories:NO attributes: nil error:nil] != YES)
			{
				theErr = errno;
				break;
			}
		} else if (!isDir) {
			theErr = ENOTDIR;
			break;
		}
	} while (false);
	
	if ((result == nil) || (theErr != noErr))
	{
		result = NSHomeDirectory();
	}
	
	return result;
}

// -------------------------------------------------------------------------- //
//  * (NSString*)openFile
// -------------------------------------------------------------------------- //
- (NSString*)openFile
{
	NSString* theResult = nil;
	NSOpenPanel* thePanel = [NSOpenPanel openPanel];
	if ([thePanel runModalForTypes: nil] == NSOKButton)
	{
		theResult = [thePanel filename];
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * (NSString*)saveFile
// -------------------------------------------------------------------------- //
- (NSString*)saveFile
{
	NSString* theResult = nil;
	NSSavePanel* thePanel = [NSSavePanel savePanel];
	if ([thePanel runModal] == NSOKButton)
	{
		theResult = [thePanel filename];
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * (void) powerChange: (BOOL)
// -------------------------------------------------------------------------- //
- (void) powerChange: (BOOL) power
{
	mPowerState = power;
	if (power == YES)
	{
		[mToolbarPowerItem setImage: mToolbarPowerOnImage];
	} else {
		[mToolbarPowerItem setImage: mToolbarPowerOffImage];
	}
}

// -------------------------------------------------------------------------- //
//  * (void) backlightChange: (BOOL)
// -------------------------------------------------------------------------- //
- (void) backlightChange: (BOOL) state
{
	mBacklightState = state;
	if (mToolbarBacklightItem != NULL)
	{
		[mToolbarBacklightItem setImage: 
			(mBacklightState == YES) ?
				mToolbarBacklightOnImage : mToolbarBacklightOffImage];
	}
}

// -------------------------------------------------------------------------- //
//  * (void) setEmulatorWindow: (NSWindow*) fullScreen: (BOOL)
// -------------------------------------------------------------------------- //
- (void) setEmulatorWindow: (NSWindow*) inWindow fullScreen: (BOOL) inFullScreen
{
	if (!inFullScreen)
	{
		[self setupToolbar: inWindow];
	}
}

@end

// ============================================================================= //
// The proof that IBM didn't invent the car is that it has a steering wheel      //
// and an accelerator instead of spurs and ropes, to be compatible with a horse. //
//                 -- Jac Goudsmit                                               //
// ============================================================================= //
