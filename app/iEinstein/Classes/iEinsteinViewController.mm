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
- (void)initEmulator;
@end

@implementation iEinsteinViewController

- (void)viewDidLoad 
{
	[super viewDidLoad];
}


// Action sheet delegate method.
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch ([actionSheet tag]) {
        case 1:
            if (buttonIndex == 0) {
                printf("Clearing Flash RAM file!\n");
                NSString* docdir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
                NSString* theFlashPath = [docdir stringByAppendingPathComponent:@"flash"];
                remove([theFlashPath fileSystemRepresentation]);
            }
            [self initEmulator];
            [self startEmulator];
            break;
    }
}


- (void)verifyDeleteFlashRAM
{
    UIActionSheet *actionSheet = [[UIActionSheet alloc] 
                                  initWithTitle:@"Clear Flash Memory?\r\rClearing the Flash will delete all packages that may have been installed and completely reset your Newton."                                                                
                                  delegate:self 
                                  cancelButtonTitle:@"Cancel" 
                                  destructiveButtonTitle:@"Clear the Flash!" 
                                  otherButtonTitles:nil];
    [actionSheet setTag:1];
    actionSheet.actionSheetStyle = UIActionSheetStyleDefault;
    [actionSheet showInView:self.view];
    [actionSheet release];
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
	
	[super dealloc];
}


- (void)initEmulator
{
	[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationFade];

	printf("Initializing the emulator\n");

	mNetworkManager = NULL;     // OK?
	mSoundManager = NULL;       // OK?
	mScreenManager = NULL;      // TODO: 
	mROMImage = NULL;           // TODO:
	mEmulator = NULL;           // OK
	mPlatformManager = NULL;    // OK
	mLog = NULL;                // OK

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
		return;
	}

	NSString* theROMPath = [thisBundle pathForResource:@"717006" ofType:nil];
	NSString* theImagePath = [docdir stringByAppendingPathComponent:@"717006.img"];

	NSFileManager* theFileManager = [NSFileManager defaultManager];
	if ( ![theFileManager fileExistsAtPath:theROMPath] )
	{
		mLog->LogLine("ROM file doesn't seem to exist");
		return;
	}

	mROMImage = new TFlatROMImageWithREX(
							   [theROMPath fileSystemRepresentation],
							   [einsteinRExPath fileSystemRepresentation],
							   "717006", false,
							   [theImagePath fileSystemRepresentation]);

	// Create the network manager.
	
	mNetworkManager = new TNullNetwork(mLog);

	// Create the sound manager.
	
	mSoundManager = new TCoreAudioSoundManager(mLog);

    // iPad is 1024x768. This size, and some appropriate scaling factirs, should be selectable from
    // the 'Settings' panel.
	
    static int widthLUT[]  = { 320, 640, 384,  512 };
    static int heightLUT[] = { 480, 960, 512, 1024 };
    
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	int index = [(NSNumber*)[prefs objectForKey:@"screen_resolution"] intValue];
	int newtonScreenWidth = widthLUT[index];
	int newtonScreenHeight = heightLUT[index];

	iEinsteinView* einsteinView = (iEinsteinView*)[self view];
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
    
	[einsteinView setEmulator:mEmulator];

	((TIOSScreenManager*)mScreenManager)->SetPlatformManager(mPlatformManager);
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
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	mEmulator->Run();
	
	[pool release];
}


- (void)resetEmulator
{
	mLog->LogLine("Resetting emulator");
	
	[(iEinsteinView*)[self view] reset];

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
	
	mEmulator->Stop();
}


@end