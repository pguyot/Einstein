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
#import "TCocoaMonitorController.h"

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
#ifdef OPTION_X11_SCREEN
#include "Emulator/Screen/TX11ScreenManager.h"
#endif
#include "Emulator/Serial/TPipesSerialPortManager.h"
#include "Emulator/Serial/TPtySerialPortManager.h"
#include "Emulator/Serial/TBasiliskIISerialPortManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TBufferLog.h"
#include "Emulator/Log/TStdOutLog.h"

#import "Monitor/TMacMonitor.h"
#import "Monitor/TSymbolList.h"

#include "TCocoaFileManager.h"
#import "TCocoaUserDefaults.h"

#ifdef JIT_PERFORMANCE
#include "Emulator/JIT/TJITPerformance.h"
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

static TCocoaAppController* gInstance = nil;


@interface TCocoaAppController ()

+ (NSString*)getAppSupportDirectory;
- (void)runEmulator;
- (void)installPackageFile:(NSString*)inPath;
- (void)setupToolbar:(NSWindow*)inWindow;
- (BOOL)validateSelector:(SEL)selector;

@end


@implementation TCocoaAppController

// -------------------------------------------------------------------------- //
//  * initialize
// -------------------------------------------------------------------------- //
+ (void) initialize
{
	NSString* theAppSupportDir = [self getAppSupportDirectory];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	NSDictionary *appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
			[theAppSupportDir stringByAppendingPathComponent:@"newton.rom"], kROMImagePathKey,
			[theAppSupportDir stringByAppendingPathComponent:@"internal.flash"], kInternalFlashPathKey,
			[NSNumber numberWithInt:717006], kMachineKey,
			[NSNumber numberWithInt:TScreenManager::kDefaultPortraitWidth], kScreenWidthKey,
			[NSNumber numberWithInt:TScreenManager::kDefaultPortraitHeight], kScreenHeightKey,
			[NSNumber numberWithBool:NO], kFullScreenKey,
			[NSNumber numberWithInt:0x40], kRAMSizeKey,
			[NSNumber numberWithInt:kCoreAudioDriverTag], kAudioDriverKey,
			[NSNumber numberWithInt:kCocoaScreenDriverTag], kScreenDriverKey,
			[NSNumber numberWithInt:kUsermodeNetworkDriverTag], kNetworkDriverKey,
			[NSNumber numberWithInt:kBasiliskIISerialDriverTag], kSerialDriverKey,
			[NSNumber numberWithBool:NO], kDontShowAtStartupKey,
			[NSNumber numberWithBool:NO], kEnableListenersKey,
			nil];
	
	[defaults registerDefaults:appDefaults];
}

// -------------------------------------------------------------------------- //
//  * init
// -------------------------------------------------------------------------- //
- (id)init
{
	if ((self = [super init]))
	{
		mProxy = [[CocoaScreenProxy alloc] init];
		mRAMSize = 0x40;	// 4 MB.
		mROMPath = NULL;
		mREx0Path = NULL;
		mNetworkManager = NULL;
		mSoundManager = NULL;
		mScreenManager = NULL;
		mExtrSerialPortManager = NULL;
		mROMImage = NULL;
		mEmulator = NULL;
		mPlatformManager = NULL;
		mLog = NULL;
		
		mToolbarPowerItem = NULL;
		mPowerState = YES;
		mToolbarBacklightItem = NULL;
		mBacklightState = NO;
		mToolbarNetworkItem = NULL;
		mNetworkState = NO;

		// Retrieve toolbar dynamic images.
		mToolbarPowerOnImage = [NSImage imageNamed:@"button_power_on"];
		mToolbarPowerOffImage = [NSImage imageNamed:@"button_power"];
		mToolbarBacklightOnImage = [NSImage imageNamed:@"button_backlight_on"];
		mToolbarBacklightOffImage = [NSImage imageNamed:@"button_backlight"];
		mToolbarNetworkOnImage = [NSImage imageNamed:@"button_network_in"];
		mToolbarNetworkOffImage = [NSImage imageNamed:@"button_network"];
		
		// Single instance.
		gInstance = self;
		
		[NSApp setDelegate:self];
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
	if (mExtrSerialPortManager)
	{
		delete mExtrSerialPortManager;
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
	if (mFileManager)
	{
		delete mFileManager;
	}
	
#if !__has_feature(objc_arc)
	[super dealloc];
#endif
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
	NSUserDefaults *defaults = [mUserDefaultsController defaults];
	
	if ( [defaults boolForKey:kOpenMonitorAtLaunch] )
	{
		[self showMonitor:self];
	}

	if ([defaults boolForKey:kDontShowAtStartupKey]
			&& ![defaults boolForKey:kFullScreenKey]
			&& [self canStartEmulator])
	{
		[self startEmulator];
	}
	else
	{
		[mSetupController openSetupWindow];
	}
}

// -------------------------------------------------------------------------- //
//  * (BOOL)canStartEmulator
// -------------------------------------------------------------------------- //
- (BOOL)canStartEmulator
{
	BOOL canStart = NO;

	NSUserDefaults *defaults = [mUserDefaultsController defaults];
	
	// Confirm that there is a file at the path where the ROM is supposed to be
	NSString *romPath = [defaults stringForKey:kROMImagePathKey];
	
	if ( romPath != nil )
	{
		canStart = [[NSFileManager defaultManager] fileExistsAtPath:romPath];
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
	NSBundle* thisBundle = [NSBundle mainBundle];
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
	
	// Special cases for ROM files ending with the extension ".aif" or " image"
	
	NSString* theREX0Path = nil;
	if ([theROMPath hasSuffix: @".aif"])
	{
		NSUInteger theLength = [theROMPath length];
		theREX0Path = [[theROMPath substringToIndex: (theLength - 4)] stringByAppendingString: @".rex"];
		if (![theFileManager fileExistsAtPath: theREX0Path])
		{
			theREX0Path = nil;
		}
	} else if ([theROMPath hasSuffix: @" image"]) {
		NSUInteger theLength = [theROMPath length];
		theREX0Path = [[theROMPath substringToIndex: (theLength - 6)] stringByAppendingString: @" high"];
		if (![theFileManager fileExistsAtPath: theREX0Path])
		{
			theREX0Path = nil;
		}
	}
	
	// Load the ROM
	
	NSInteger theMachine = [defaults integerForKey: kMachineKey]; //e.g; 717006
	NSString* machineStr = [NSString stringWithFormat:@"%ld", theMachine];
	
	if (theREX0Path)
	{
		// .aif or "image" ROM file
		
		mROMImage = new TAIFROMImageWithREXes(
							[theROMPath UTF8String],
							[theREX0Path UTF8String],
							[einsteinRExPath UTF8String],
							[machineStr UTF8String]);
	} else {
		// .rom file
		
		mROMImage = new TFlatROMImageWithREX(
							[theROMPath UTF8String],
							[einsteinRExPath UTF8String],
							[machineStr UTF8String]);
	}
	
	// Create a log if possible
#ifdef _DEBUG
	mLog = 0L; // new TStdOutLog();
	//mLog = new TStdOutLog();
#endif
	
	// Create the network manager.
	NSInteger indexNetworkDriver = [defaults integerForKey: kNetworkDriverKey];
	if (indexNetworkDriver == kUsermodeNetworkDriverTag)
	{
		mNetworkManager = new TUsermodeNetwork(new TStdOutLog());
	} else if (indexNetworkDriver == kTapNetworkDriverTag) {
		mNetworkManager = new TTapNetwork(mLog);
	} else {
		mNetworkManager = new TNullNetwork(mLog);
	}
	
	// Create the sound manager.
	NSInteger indexAudioDriver = [defaults integerForKey: kAudioDriverKey];
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
	NSInteger indexScreenDriver = [defaults integerForKey: kScreenDriverKey];
	if (indexScreenDriver == kCocoaScreenDriverTag)
	{
		KUInt32 theWidth;
		KUInt32 theHeight;
		if (fullScreen)
		{
			// Mac/eMate orientation.
			NSRect theRect = [[NSScreen mainScreen] frame];
			if (theRect.size.width > theRect.size.height)
			{
				screenIsLandscape = true;
				theWidth = (KUInt32) theRect.size.height;
				theHeight = (KUInt32) theRect.size.width;
			} else {
				screenIsLandscape = false;
				theWidth = (KUInt32) theRect.size.width;
				theHeight = (KUInt32) theRect.size.height;
			}
		} else {
			theWidth = (KUInt32)[defaults integerForKey: kScreenWidthKey];
			theHeight = (KUInt32)[defaults integerForKey: kScreenHeightKey];
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

	// Create the serial port manager
	NSInteger indexSerialPortDriver = [defaults integerForKey: kSerialDriverKey];
	if (indexSerialPortDriver == kBasiliskIISerialDriverTag)
	{
		mExtrSerialPortManager = new TBasiliskIISerialPortManager(
			mLog,
			TSerialPortManager::kExternalSerialPort);
	} else if (indexSerialPortDriver == kPtySerialDriverTag) {
		mExtrSerialPortManager = new TPtySerialPortManager(
			mLog,
			TSerialPortManager::kExternalSerialPort);
	} else if (indexSerialPortDriver == kPipesSerialDriverTag) {
		mExtrSerialPortManager = new TPipesSerialPortManager(
			mLog,
			TSerialPortManager::kExternalSerialPort);
	} else {
		mExtrSerialPortManager = new TSerialPortManager(
			mLog,
			TSerialPortManager::kExternalSerialPort);
	}

	// Create the emulator.
	KUInt32 ramSize = (KUInt32)[defaults integerForKey: kRAMSizeKey];
	const char* theFlashPath =
		[[defaults stringForKey: kInternalFlashPathKey] UTF8String];
	mEmulator = new TEmulator(
							  mLog, mROMImage, theFlashPath,
							  mSoundManager, mScreenManager, mNetworkManager,
							  ramSize << 16,
							  mExtrSerialPortManager );
	
	if ([defaults boolForKey: kEnableListenersKey])
	{
		mFileManager = new TCocoaFileManager();
		mFileManager->SetLog(mLog);
		mFileManager->SetMemory(mEmulator->GetMemory());
		mEmulator->SetFileManager(mFileManager);
	}
	
	mPlatformManager = mEmulator->GetPlatformManager();
	if (indexScreenDriver == kCocoaScreenDriverTag)
	{
		((TCocoaScreenManager*) mScreenManager)
			->SetPlatformManager( mPlatformManager );
	}
	
	mMonitorLog = new TBufferLog();
	
	NSString* theDataPath = [theROMPath stringByDeletingLastPathComponent];
#ifdef _DEBUG
	// If there is a file called symbols.txt next to the ROM file, we can
	// load it to symbolicate addresses in the monitor
	
	NSString* theSymbolPath = [theDataPath stringByAppendingPathComponent:@"symbols.txt"];
	mSymbolList = TSymbolList::List = new TSymbolList([theSymbolPath fileSystemRepresentation]);
#else
	mSymbolList = NULL;
#endif
	
	mMonitor = new TMacMonitor(mMonitorLog, mEmulator, mSymbolList, theDataPath.UTF8String);
	[mMonitorController setMonitor:mMonitor];
	
	// FIXME: delete this to keep the Monitor closed
	//[mMonitorController showWindow:self];
	
	// Close the window.
	[mSetupController closeSetupWindow];
	
	// Create the Overlay text window
	mScreenManager->OverlayClear();
	mScreenManager->OverlayOn();
	mScreenManager->OverlayPrintAt(0, 0, "Booting...", true);
	mScreenManager->OverlayPrintProgress(1, 0);
	mScreenManager->OverlayFlush();
	
	// FIXME: to launch with the last saved state, enable these commands (will reboot! Something's missing!)
	//mMonitor->LoadEmulatorState();
	//mEmulator->GetProcessor()->SetRegister(15, 0x800AAC); //0x800AB4
	
	// Start the thread.
	[NSThread detachNewThreadSelector:@selector(runEmulator) toTarget: self withObject: NULL];
//	[mMonitorController performSelector:@selector(executeCommand:) withObject:@"load x" afterDelay:1];
	[mMonitorController performSelector:@selector(executeCommand:) withObject:@"run" afterDelay:1];
//	[mMonitorController performSelector:@selector(executeCommand:) withObject:@"revert" afterDelay:1];

	[mMonitorController update];
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
#ifdef JIT_PERFORMANCE 
	// branchDestCount currently holds all commands that are executed.
	FILE *f;
	f = fopen("/tmp/p1.txt", "wb");
	//branchDestCount.print(f, TJITPerfHitCounter::kStyleMostHit|TJITPerfHitCounter::kStyleHex, mSymbolList, 1000);
	//branchDestCount.print(f, TJITPerfHitCounter::kStyleAllHit|TJITPerfHitCounter::kStyleHex, mSymbolList);
	branchDestCount.print(f, TJITPerfHitCounter::kStyleNonZeroOnly|TJITPerfHitCounter::kStyleSymbolsOnly|TJITPerfHitCounter::kStyleDontSort|TJITPerfHitCounter::kStyleHex, mSymbolList);
	fclose(f);
	//f = fopen("/tmp/p2.txt", "wb");
	//branchLinkDestCount.print(f, TJITPerfHitCounter::kStyleMostHit|TJITPerfHitCounter::kStyleHex, mSymbolList, 1000);
	//fclose(f);
#endif
}

// -------------------------------------------------------------------------- //
//  * (IBAction)networkButton:(id)
// -------------------------------------------------------------------------- //
- (IBAction)networkButton:(id)sender
{
	mPlatformManager->SendNetworkCardEvent();
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
	// This runs in an NSThread
#if !__has_feature(objc_arc)
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#else
	@autoreleasepool {
#endif
	mMonitor->Run();
	
	// Quit if the emulator quitted.
	mQuit = true;
	[[NSApplication sharedApplication] terminate: self];
#if !__has_feature(objc_arc)
	[pool release];
#else
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * (void) applicationWillTerminate: (NSNotification *) notification
// -------------------------------------------------------------------------- //
- (void) applicationWillTerminate: (NSNotification *) notification
{
	if (!mQuit)
	{
		if ( mEmulator )
		{
			mEmulator->Quit();
		}
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
#if !__has_feature(objc_arc)
	[toolbar autorelease];
#endif
	[inWindow setToolbar:toolbar];
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
#if !__has_feature(objc_arc)
				[mToolbarPowerItem release];
#endif
				mToolbarPowerItem = NULL;
			}
			mToolbarPowerItem = item;
#if !__has_feature(objc_arc)
			[mToolbarPowerItem retain];
#endif
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
#if !__has_feature(objc_arc)
				[mToolbarBacklightItem release];
#endif
				mToolbarBacklightItem = NULL;
			}
			mToolbarBacklightItem = item;
#if !__has_feature(objc_arc)
			[mToolbarBacklightItem retain];
#endif
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
	} else if ( [itemIdentifier isEqualToString:@"Network"] ) {
		if (flag == YES)
		{
			if (mToolbarNetworkItem != NULL)
			{
#if !__has_feature(objc_arc)
				[mToolbarNetworkItem release];
#endif
				mToolbarNetworkItem = NULL;
			}
			mToolbarNetworkItem = item;
#if !__has_feature(objc_arc)
			[mToolbarNetworkItem retain];
#endif
		}
		[item setLabel:NSLocalizedString(@"Network",nil)];
		[item setPaletteLabel:[item label]];
		[item setTarget:self];
		[item setAction:@selector(networkButton:)];
		if (flag == YES)
		{
			[item setImage: 
			 (mNetworkState == YES) ?
				mToolbarNetworkOnImage : mToolbarNetworkOffImage];
		} else {
			[item setImage: mToolbarNetworkOffImage];
		}
		[item setEnabled: YES];
	}
#if !__has_feature(objc_arc)
	[item autorelease];
#endif
	return item;
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
			@"Preferences", @"Install", @"Power", 
			@"Backlight", @"Network", nil];
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
			@"Network",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Install",
			NSToolbarCustomizeToolbarItemIdentifier,
			nil];
}

// ------------------------------------------------------------------------- //
//  * getAppSupportDirectory
// ------------------------------------------------------------------------- //
+ (NSString *)getAppSupportDirectory
{
	NSString *result = nil;

	// Returns where to place the file internal.flash -- usually ~/Library/Application Support/Einstein Platform/
	
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	
	if ( paths && [paths count] > 0 )
	{
		NSString *appSupFolderPath = [paths firstObject];
		NSFileManager *theFileManager = [NSFileManager defaultManager];
		
		BOOL isDir;

		result = [appSupFolderPath stringByAppendingPathComponent:@"Einstein Platform"];
		
		if ( ![theFileManager fileExistsAtPath:result isDirectory:&isDir] )
		{
			// Try creating the "Einstein Platform" folder
			
			if ( [theFileManager createDirectoryAtPath:result withIntermediateDirectories:NO attributes:nil error:nil] == NO )
			{
				return NSHomeDirectory();
			}
		}

		if ( isDir == NO )
		{
			return NSHomeDirectory();
		}
	}
	else
	{
		return NSHomeDirectory();
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
	if ([thePanel runModal] == NSModalResponseOK)
	{
		theResult = [[thePanel URL] path];
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
	if ([thePanel runModal] == NSModalResponseOK)
	{
		theResult = [[thePanel URL] path];
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
//  * (void) networkChange: (BOOL)
// -------------------------------------------------------------------------- //
- (void) networkChange: (BOOL) state
{
	mNetworkState = state;
	if (mToolbarNetworkItem != NULL)
	{
		[mToolbarNetworkItem setImage: 
		 (mNetworkState == YES) ?
			  mToolbarNetworkOnImage : mToolbarNetworkOffImage];
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


- (IBAction)showMonitor:(id)sender
{
	[mMonitorController showWindow:self];
}


- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	return [self validateSelector:[menuItem action]];
}


- (BOOL)validateSelector:(SEL)selector
{
	if ( selector == @selector(installPackage:) 
			|| selector == @selector(networkButton:) 
			|| selector == @selector(backlightButton:) 
			|| selector == @selector(powerButton:) )
	{
		return (mEmulator && mEmulator->IsRunning());
	}
	return YES;
}


- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
	return [self validateSelector:[theItem action]];
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
	return YES;
}

@end

// ============================================================================= //
// The proof that IBM didn't invent the car is that it has a steering wheel	  //
// and an accelerator instead of spurs and ropes, to be compatible with a horse. //
//				 -- Jac Goudsmit											   //
// ============================================================================= //
