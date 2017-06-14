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
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Sound/TCoreAudioSoundManager.h"
#include "Emulator/Screen/TIOSScreenManager.h"
#include "Emulator/Platform/TPlatformManager.h"

@interface iEinsteinViewController ()
- (int)initEmulator;
@end

@implementation iEinsteinViewController

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
#ifdef USE_STORYBOARDS
	[self initEmulator];
#endif
}


// Action sheet delegate method.
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch ([actionSheet tag]) {
        case 1:
        case 4:
            if (buttonIndex == 0) {
                printf("Clearing Flash RAM file!\n");
                NSString* docdir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
                NSString* theFlashPath = [docdir stringByAppendingPathComponent:@"flash"];
                remove([theFlashPath fileSystemRepresentation]);
                NSString* theLastInstallPath = [docdir stringByAppendingPathComponent:@".lastInstall"];
                remove([theLastInstallPath fileSystemRepresentation]);
                [self resetEmulator];
                [self startEmulator];
            } else if ([actionSheet tag]==1)
                [self startEmulator];
            else {
                [self initEmulator];
            }
            break;
        case 2:
            switch (buttonIndex) {
                case 0: break;
                case 1: [self stopEmulator]; break;            
            }
            break;
        case 3:
            [[UIApplication sharedApplication] performSelector:@selector(terminateWithSuccess)];
            break;
    }
}


- (void)verifyDeleteFlashRAM:(int)withTag;
{
    UIActionSheet *actionSheet = [[UIActionSheet alloc] 
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


- (void)explainMissingROM
{
    UIActionSheet *actionSheet = [[UIActionSheet alloc] 
                                  initWithTitle:@"Newton ROM not found.\r\r"
                                  "Einstein Emulator requires an MP2x00 US ROM image. "
                                  "The ROM file must be named 717006.rom and copied to "
                                  "this device using the iTunes File Sharing feature.\r\r"
                                  "For more information please read the instructions at "
                                  "https://github.com/pguyot/Einstein/wiki/Build-Instructions"
                                  delegate:self 
                                  cancelButtonTitle:@"Quit Einstein" 
                                  destructiveButtonTitle:nil 
                                  otherButtonTitles:nil];
    [actionSheet setTag:3];
    actionSheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
    [actionSheet showInView:self.view];
#if !__has_feature(objc_arc)
    [actionSheet release];
#endif
}


//- (void)setNeedsDisplayInNewtonRect:(NSValue*)v

- (void)openEinsteinMenu:(NSValue*)v
{
    UIActionSheet *actionSheet = [[UIActionSheet alloc] 
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
	if ( mEmulator )
	{
		delete mEmulator;
		mEmulator = NULL;
	}
	
	if ( mScreenManager )
	{
		delete mScreenManager;
		mScreenManager = NULL;
	}
	
	if ( mNetworkManager )
	{
		delete mNetworkManager;
		mNetworkManager = NULL;
	}
	
	if ( mSoundManager )
	{
		delete mSoundManager;
		mSoundManager = NULL;
	}
	
	if ( mROMImage )
	{
		delete mROMImage;
		mROMImage = NULL;
	}
	
	if ( mLog )
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
	[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationFade];
#endif

	printf("Initializing the emulator\n");

	mNetworkManager = NULL;
	mSoundManager = NULL;
	mScreenManager = NULL;
	mROMImage = NULL;
	mEmulator = NULL;
	mPlatformManager = NULL;
	mLog = NULL;

	// Create a log if possible
	//#ifdef _DEBUG
	mLog = new TStdOutLog(); 
	//#endif

	NSString* docdir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];

	// Create the ROM.
	
	NSString* einsteinRExPath = nil;
	NSBundle* thisBundle = [NSBundle mainBundle];

	if ( !(einsteinRExPath = [thisBundle pathForResource:@"Einstein" ofType:@"rex"]) )
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
    
    if ([theFileManager fileExistsAtPath:theROMPath]) {
        mROMImage = new TFlatROMImageWithREX(
							[theROMPath fileSystemRepresentation],
							[einsteinRExPath fileSystemRepresentation],
							"717006", false,
							[theImagePath fileSystemRepresentation]);
    } else if ([theFileManager fileExistsAtPath:theDebugROMPath]
             &&[theFileManager fileExistsAtPath:theDebugHighROMPath]) {
        mROMImage = new TAIFROMImageWithREXes(
                            [theDebugROMPath fileSystemRepresentation],
                            [theDebugHighROMPath fileSystemRepresentation],
                            [einsteinRExPath fileSystemRepresentation],
                            "717006" );
    } else {
        fprintf(stderr, "ROM file required here:\n %s\nor here:\n %s\n %s\n\n",
                [theROMPath fileSystemRepresentation],
                [theDebugROMPath fileSystemRepresentation],
                [theDebugHighROMPath fileSystemRepresentation]);
		//[self abortWithMessage: @"ROM file not found"];
        [self explainMissingROM];
        mROMImage = 0L;
        return 0;
    }

	// Create the network manager.
	
	mNetworkManager = new TNullNetwork(mLog);

	// Create the sound manager.
	
	mSoundManager = new TCoreAudioSoundManager(mLog);

    // iPad is 1024x768. This size, and some appropriate scaling factors, should be selectable from
    // the 'Settings' panel.
	
    static int widthLUT[]  = { 320, 640, 384,  786,  640, 320,  750, 375, 1080, 540 };
    static int heightLUT[] = { 480, 960, 512, 1024, 1136, 568, 1134, 567, 1920, 960 };
    
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	int index = [(NSNumber*)[prefs objectForKey:@"screen_resolution"] intValue];
	int newtonScreenWidth = widthLUT[index];
	int newtonScreenHeight = heightLUT[index];

#ifdef USE_STORYBOARDS
    // When using storyboards as configured, the einsteinView is a subview of self.view
    // This facilitates other subviews in the future.
    iEinsteinView* einsteinView = self.einsteinView;
#else
    // When using NIBs, the einsteinView is self.view
	iEinsteinView* einsteinView = (iEinsteinView*)[self view];
#endif
	Boolean isLandscape = (newtonScreenWidth > newtonScreenHeight);
	
	mScreenManager = new TIOSScreenManager(
							einsteinView,
							self,
							mLog,
							newtonScreenWidth, newtonScreenHeight,
							true,
							isLandscape);

	[einsteinView setScreenManager:mScreenManager];

	// Create the emulator.

	NSString* theFlashPath = [docdir stringByAppendingPathComponent:@"flash"];
	printf("Flash file is %s\n", [theFlashPath fileSystemRepresentation]);

	mEmulator = new TEmulator(
					mLog, mROMImage, [theFlashPath fileSystemRepresentation],
					mSoundManager, mScreenManager, mNetworkManager, 0x40 << 16);
					
	mPlatformManager = mEmulator->GetPlatformManager();
    mPlatformManager->SetDocDir([docdir fileSystemRepresentation]);
    
	[einsteinView setEmulator:mEmulator];

	((TIOSScreenManager*)mScreenManager)->SetPlatformManager(mPlatformManager);
    
    return 1;
}


- (void)startEmulator
{
	// See if screen resolution has changed since last time
	
	NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	[prefs synchronize];
	int currentScreenResolution = [(NSNumber*)[prefs objectForKey:@"screen_resolution"] intValue];
	
	if ( currentScreenResolution != lastKnownScreenResolution )
	{
		// Reboot emulator
		
		mLog->LogLine("Newton screen resolution changed by Settings.");
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
    return (mEmulator!=0L)
         & (mNetworkManager!=0L)
         & (mSoundManager!=0L)
         & (mScreenManager!=0L)
         & (mROMImage!=0L);
}

- (void)resetEmulator
{
	mLog->LogLine("Resetting emulator");

#ifdef USE_STORYBOARDS
    [self.einsteinView reset];
#else
    [(iEinsteinView*)[self view] reset];
#endif
    
	delete mEmulator;
	mEmulator = NULL;

	delete mNetworkManager;
	mNetworkManager = NULL;
	
	delete mSoundManager;
	mSoundManager = NULL;
	
	delete mScreenManager;
	mScreenManager = NULL;
	
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

	if ( mEmulator )
		mEmulator->Stop();
}

/*
void openEinsteinMenu(iEinsteinViewController *ctrl)
{
    [ctrl openEinsteinMenu];
}
*/

// for iOS7+ hide the status bar
- (BOOL)prefersStatusBarHidden {
	return YES;
}

@end
