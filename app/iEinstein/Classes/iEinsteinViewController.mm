// ==============================
// File:			iEinsteinViewController.mm
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

#import "iEinsteinViewController.h"

#include "Emulator/TEmulator.h"
#include "Emulator/Log/TStdOutLog.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/Printer/TIOSPrinterManager.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/Screen/TIOSScreenManager.h"
#include "Emulator/Sound/TCoreAudioSoundManager.h"

@interface
iEinsteinViewController ()
- (int)initEmulator;
@end

@implementation iEinsteinViewController

- (void)viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	// Initialization moved to viewDidLayoutSubviews to ensure correct bounds
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
}

- (void)viewDidLayoutSubviews
{
	[super viewDidLayoutSubviews];
#ifdef USE_STORYBOARDS
	// Only initialize once, after layout is complete so we have correct bounds
	if (!mEmulatorInitialized) {
		mEmulatorInitialized = YES;
		[self initEmulator];
	}
#endif
}

// Action sheet delegate method.
- (void)actionSheet:(UIActionSheet*)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	switch ([actionSheet tag])
	{
		case 1:
		case 4:
			if (buttonIndex == 0)
			{
				printf("Clearing Flash RAM file!\n");
				NSString* docdir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
				NSString* theFlashPath = [docdir stringByAppendingPathComponent:@"flash"];
				remove([theFlashPath fileSystemRepresentation]);
				NSString* theLastInstallPath = [docdir stringByAppendingPathComponent:@".lastInstall"];
				remove([theLastInstallPath fileSystemRepresentation]);
				[self resetEmulator];
				[self startEmulator];
			} else if ([actionSheet tag] == 1)
				[self startEmulator];
			else
			{
				[self initEmulator];
			}
			break;
		case 2:
			switch (buttonIndex)
			{
				case 0:
					break;
				case 1:
					[self stopEmulator];
					break;
				default:
					break;
			}
			break;
		case 3:
			[[UIApplication sharedApplication] performSelector:@selector(terminateWithSuccess)];
			break;
		default:
			break;
	}
}

- (void)verifyDeleteFlashRAM:(int)withTag;
{
	UIActionSheet* actionSheet = [[UIActionSheet alloc]
				 initWithTitle:@"Clear Flash Memory?\r\r"
								"Clearing the Flash will delete all packages that may "
								"have been installed and completely reset your Newton."
					  delegate:self
			 cancelButtonTitle:@"Cancel"
		destructiveButtonTitle:@"Clear the Flash!"
			 otherButtonTitles:nil];
	[actionSheet setTag:withTag];
	actionSheet.actionSheetStyle = UIActionSheetStyleDefault;
	[actionSheet showInView:self.view];
#if !__has_feature(objc_arc)
	[actionSheet release];
#endif
}

- (void)explainMissingROM:(id)sender
{
	UIActionSheet* actionSheet = [[UIActionSheet alloc]
				 initWithTitle:@"Newton ROM not found.\r\r"
								"Einstein Emulator requires an MP2x00 US ROM image. "
								"The ROM file must be named 717006.rom and copied to "
								"this device using the iTunes File Sharing feature.\r\r"
								"For more information please read the instructions at "
								"https://github.com/pguyot/Einstein/wiki/Build-Instructions"
					  delegate:self
			 cancelButtonTitle:nil
		destructiveButtonTitle:@"Quit Einstein"
			 otherButtonTitles:nil];
	[actionSheet setTag:3];
	[actionSheet showInView:self.view];
#if !__has_feature(objc_arc)
	[actionSheet release];
#endif
}

//- (void)setNeedsDisplayInNewtonRect:(NSValue*)v

- (void)openEinsteinMenu:(NSValue*)v
{
	UIActionSheet* actionSheet = [[UIActionSheet alloc]
				 initWithTitle:@"Einstein Menu"
					  delegate:self
			 cancelButtonTitle:@"Cancel"
		destructiveButtonTitle:@"Quit Emulator"
			 otherButtonTitles:nil];
	[actionSheet setTag:2];
	actionSheet.actionSheetStyle = UIActionSheetStyleDefault;
	[actionSheet showInView:self.view];
#if !__has_feature(objc_arc)
	[actionSheet release];
#endif
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning
{
	// Releases the view if it doesn't have a superview.
	[super didReceiveMemoryWarning];

	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload
{
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)installNewPackages
{
	if (mPlatformManager)
		mPlatformManager->InstallNewPackages();
}

- (void)dealloc
{
	if (mEmulator)
	{
		delete mEmulator;
		mEmulator = NULL;
	}

	if (mScreenManager)
	{
		delete mScreenManager;
		mScreenManager = NULL;
	}

	if (mNetworkManager)
	{
		delete mNetworkManager;
		mNetworkManager = NULL;
	}

	if (mSoundManager)
	{
		delete mSoundManager;
		mSoundManager = NULL;
	}

	delete mPrinterManager;
	mPrinterManager = NULL;

	if (mROMImage)
	{
		delete mROMImage;
		mROMImage = NULL;
	}

	if (mLog)
	{
		delete mLog;
		mLog = NULL;
	}

#if !__has_feature(objc_arc)
	[_einsteinView release];
	[super dealloc];
#endif
}

- (int)initEmulator
{
#ifndef USE_STORYBOARDS
	[[UIApplication sharedApplication] setStatusBarHidden:YES
											withAnimation:UIStatusBarAnimationFade];
#endif

	printf("Initializing the emulator\n");

	mNetworkManager = NULL;
	mSoundManager = NULL;
	mScreenManager = NULL;
	mPrinterManager = NULL;
	mROMImage = NULL;
	mEmulator = NULL;
	mPlatformManager = NULL;
	mLog = NULL;

	// Create a log if possible
	//#ifdef _DEBUG
	mLog = new TStdOutLog();
	//#endif

	NSString* docdir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

	// Create the ROM.

	NSString* einsteinRExPath = nil;
	NSBundle* thisBundle = [NSBundle mainBundle];

	if (!(einsteinRExPath = [thisBundle pathForResource:@"Einstein" ofType:@"rex"]))
	{
		//[self abortWithMessage: @"Couldn't load Einstein REX"];
		mLog->LogLine("Couldn't load Einstein REX");
		return 0;
	}

	NSString* theROMPath = [docdir stringByAppendingPathComponent:@"717006.rom"];
	NSString* theDebugROMPath = [docdir stringByAppendingPathComponent:@"717006.aif"];
	NSString* theDebugHighROMPath = [docdir stringByAppendingPathComponent:@"717006.rex"];
	NSString* theImagePath = [docdir stringByAppendingPathComponent:@"717006.img"];

	NSFileManager* theFileManager = [NSFileManager defaultManager];

	if ([theFileManager fileExistsAtPath:theROMPath])
	{
#if 0
        mROMImage = new TFlatROMImageWithREX(
							[theROMPath fileSystemRepresentation],
							[einsteinRExPath fileSystemRepresentation],
							"717006", false,
							[theImagePath fileSystemRepresentation]);
#else
		mROMImage = new TFlatROMImageWithREX(
			[theROMPath fileSystemRepresentation],
			[einsteinRExPath fileSystemRepresentation]);
#endif
	} else if ([theFileManager fileExistsAtPath:theDebugROMPath]
		&& [theFileManager fileExistsAtPath:theDebugHighROMPath])
	{
#if 0
        mROMImage = new TAIFROMImageWithREXes(
                            [theDebugROMPath fileSystemRepresentation],
                            [theDebugHighROMPath fileSystemRepresentation],
                            [einsteinRExPath fileSystemRepresentation],
                            "717006" );
#else
		mROMImage = new TAIFROMImageWithREXes(
			[theDebugROMPath fileSystemRepresentation],
			[theDebugHighROMPath fileSystemRepresentation],
			[einsteinRExPath fileSystemRepresentation]);
#endif
	} else
	{
		fprintf(stderr, "ROM file required here:\n %s\nor here:\n %s\n %s\n\n",
			[theROMPath fileSystemRepresentation],
			[theDebugROMPath fileSystemRepresentation],
			[theDebugHighROMPath fileSystemRepresentation]);

		// Defer this call to explainMissingROM because self.view is not in the
		// visible view hierarchy yet. (We arrive here from viewWillAppear due to
		// other order-of-operations issues -- see issue #31)

		[self performSelector:@selector(explainMissingROM:) withObject:self afterDelay:0];

		mROMImage = 0L;
		return 0;
	}

	// Create the network manager.

	mNetworkManager = new TNullNetworkManager(mLog);

	// Create the sound manager.

	mSoundManager = new TCoreAudioSoundManager(mLog);

#ifdef USE_STORYBOARDS
	iEinsteinView* einsteinView = self.einsteinView;
#else
	iEinsteinView* einsteinView = (iEinsteinView*) [self view];
#endif

	// Determine screen resolution based on use_full_screen setting
	NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	BOOL useFullScreen = [(NSNumber*) [prefs objectForKey:@"use_full_screen"] boolValue];
	int newtonScreenWidth, newtonScreenHeight;

	if (useFullScreen) {
		// Full screen mode: use native view bounds for resolution
		CGRect viewBounds = [einsteinView bounds];

		// Use point dimensions (not pixels) for the Newton resolution.
		// The Newton is always in portrait orientation internally (width < height).
		int viewWidth = (int)viewBounds.size.width;
		int viewHeight = (int)viewBounds.size.height;

		// Ensure width is the smaller dimension (portrait mode for Newton)
		if (viewWidth < viewHeight) {
			newtonScreenWidth = viewWidth;
			newtonScreenHeight = viewHeight;
		} else {
			newtonScreenWidth = viewHeight;
			newtonScreenHeight = viewWidth;
		}

		// On iPad, use 2x scaling (halve resolution, scale up when drawing)
		if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
			newtonScreenWidth /= 2.0;
			newtonScreenHeight /= 2.0;
		} else {
			newtonScreenWidth /= 1.25;
			newtonScreenHeight /= 1.25;
		}

		// Round width to an even number (required for pixel operations)
		newtonScreenWidth = (newtonScreenWidth + 1) & ~1;
	} else {
		// Classic mode: use LUT-based resolution from Settings
		static int widthLUT[] = { 320, 640, 384, 786, 640, 320, 750, 375, 1080, 540 };
		static int heightLUT[] = { 480, 960, 512, 1024, 1136, 568, 1134, 567, 1920, 960 };

		int index = [(NSNumber*) [prefs objectForKey:@"screen_resolution"] intValue];
		newtonScreenWidth = widthLUT[index];
		newtonScreenHeight = heightLUT[index];
	}

	// Store current settings for change detection
	lastKnownFullScreenMode = useFullScreen;
	lastKnownScreenResolution = [(NSNumber*) [prefs objectForKey:@"screen_resolution"] intValue];

	Boolean isLandscape = (newtonScreenWidth > newtonScreenHeight);

	mScreenManager = new TIOSScreenManager(
		einsteinView,
		self,
		mLog,
		newtonScreenWidth, newtonScreenHeight,
		true,
		isLandscape);

	[einsteinView setScreenManager:mScreenManager useFullScreen:useFullScreen];

	// Create the printer manager.
	mPrinterManager = new TIOSPrinterManager(mLog);

	// Create the emulator.

	NSString* theFlashPath = [docdir stringByAppendingPathComponent:@"flash"];
	printf("Flash file is %s\n", [theFlashPath fileSystemRepresentation]);

	mEmulator = new TEmulator(
		mLog, mROMImage, [theFlashPath fileSystemRepresentation],
		mSoundManager, mScreenManager, mNetworkManager, 0x40 << 16, mPrinterManager);

	mEmulator->SerialPorts.Initialize(TSerialPorts::kTcpClientDriver,
		TSerialPorts::kNullDriver,
		TSerialPorts::kNullDriver,
		TSerialPorts::kNullDriver);
	mPlatformManager = mEmulator->GetPlatformManager();
	mPlatformManager->SetDocDir([docdir fileSystemRepresentation]);
	mPrinterManager->SetMemory(mEmulator->GetMemory());

	[einsteinView setEmulator:mEmulator];

	((TIOSScreenManager*) mScreenManager)->SetPlatformManager(mPlatformManager);

	return 1;
}

- (void)startEmulator
{
	// Check if screen settings have changed since last time
	NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	[prefs synchronize];

	BOOL currentFullScreenMode = [(NSNumber*) [prefs objectForKey:@"use_full_screen"] boolValue];
	int currentScreenResolution = [(NSNumber*) [prefs objectForKey:@"screen_resolution"] intValue];

	BOOL needsReset = NO;

	if (currentFullScreenMode != lastKnownFullScreenMode) {
		mLog->LogLine("Full screen mode changed by Settings.");
		needsReset = YES;
	} else if (!currentFullScreenMode && currentScreenResolution != lastKnownScreenResolution) {
		// Only check resolution change in classic mode (not full screen)
		mLog->LogLine("Newton screen resolution changed by Settings.");
		needsReset = YES;
	}

	if (needsReset) {
		lastKnownFullScreenMode = currentFullScreenMode;
		lastKnownScreenResolution = currentScreenResolution;
		[self resetEmulator];
	}

	// Start the thread.
	mLog->LogLine("Detaching emulator thread");
	[NSThread detachNewThreadSelector:@selector(emulatorThread) toTarget:self withObject:nil];
}

- (void)emulatorThread
{
#if !__has_feature(objc_arc)
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#endif
	mEmulator->Run();
#if !__has_feature(objc_arc)
	[pool release];
#endif
}

- (int)allResourcesFound
{
	return (mEmulator != 0L)
		& (mNetworkManager != 0L)
		& (mSoundManager != 0L)
		& (mScreenManager != 0L)
		& (mROMImage != 0L);
}

- (void)resetEmulator
{
	mLog->LogLine("Resetting emulator");

#ifdef USE_STORYBOARDS
	[self.einsteinView reset];
#else
	[(iEinsteinView*) [self view] reset];
#endif

	delete mEmulator;
	mEmulator = NULL;

	delete mNetworkManager;
	mNetworkManager = NULL;

	delete mSoundManager;
	mSoundManager = NULL;

	delete mScreenManager;
	mScreenManager = NULL;

	if (mPrinterManager != NULL)
	{
		delete mPrinterManager;
		mPrinterManager = NULL;
	}

	delete mROMImage;
	mROMImage = NULL;

	// Emulator deletes platform manager in its destructor
	mPlatformManager = NULL;

	delete mLog;
	mLog = NULL;

	[self initEmulator];
}

- (void)stopEmulator
{
	mLog->LogLine("Stopping emulator thread");

	if (mEmulator)
		mEmulator->Stop();
}

/*
void openEinsteinMenu(iEinsteinViewController *ctrl)
{
	[ctrl openEinsteinMenu];
}
*/

// for iOS7+ hide the status bar
- (BOOL)prefersStatusBarHidden
{
	return YES;
}

@end
