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

@interface iEinsteinViewController ()
- (void)initEmulator;
@end

@implementation iEinsteinViewController

- (void)viewDidLoad 
{
	[super viewDidLoad];

	NSString* str = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	printf("Document dir is %s\n", [str UTF8String]);
	
	[self initEmulator];
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
	mLog->LogLine("Hallo Welt!");

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
	
	iEinsteinView* einsteinView = (iEinsteinView*)[self view];
	Boolean isLandscape = ([einsteinView newtonScreenWidth] > [einsteinView newtonScreenHeight]);
	
	mScreenManager = new TIOSScreenManager(
							   einsteinView,
							   self,
							   mLog,
							   [einsteinView newtonScreenWidth],
							   [einsteinView newtonScreenHeight],
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

	((TIOSScreenManager*)mScreenManager)->SetPlatformManager(mPlatformManager);
}


- (void)startEmulator
{
	// Start the thread.
	
	mLog->LogLine("Detaching emulator thread");
	[NSThread detachNewThreadSelector:@selector(emulatorThread) toTarget:self withObject:nil];
}


- (void)emulatorThread
{
	mEmulator->Run();
}


- (void)stopEmulator
{
	mLog->LogLine("Stopping emulator thread");
	mEmulator->Stop();
}


@end