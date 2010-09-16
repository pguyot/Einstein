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
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Screen/TIOSScreenManager.h"


@implementation iEinsteinViewController


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
  NSSet *ts = [event allTouches];
  UITouch *t = [ts anyObject];
  if (t) {
    CGPoint pt = [t locationInView: [self view]];
    mScreenManager->PenDown(480-pt.y, pt.x);
  }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
  NSSet *ts = [event allTouches];
  UITouch *t = [ts anyObject];
  if (t) {
    CGPoint pt = [t locationInView: [self view]];
    mScreenManager->PenDown(480-pt.y, pt.x);
  }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
  mScreenManager->PenUp();
}


/*
 mScreenManager->PenDown(
 (KUInt16) penLoc.x,
 (KUInt16) penLoc.y);
*/

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
  [super viewDidLoad];
  
  //char buf[2048];
  //getcwd(buf, 2048);
  //printf("Current dir is %s\n", buf);
  NSString *str = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
  printf("Document dir is %s\n", [str UTF8String]);
  
  [self startEmulator];
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
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

/*
+ (void) initialize
{
}
*/

// (never called?!)
- (id) init 
{
  if ((self = [super init]))
  {
    mNetworkManager = NULL;
    mSoundManager = NULL;
    mScreenManager = NULL;
    mROMImage = NULL;
    mEmulator = NULL;
    mPlatformManager = NULL;
    mLog = NULL;
  }
  return self;
}


// -------------------------------------------------------------------------- //
//  * (void)startEmulator
// -------------------------------------------------------------------------- //
- (void)startEmulator
{
  [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationFade];

  printf("Starting the emulator\n");
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
  
  NSString *docdir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
  mLog->LogLine("Hallo Welt!");
  
  // Create the ROM.
  NSString* einsteinRExPath;
  NSBundle* thisBundle = [NSBundle bundleForClass:[self class]];
  if (!(einsteinRExPath = [thisBundle pathForResource:@"Einstein" ofType:@"rex"]))
  {
    //[self abortWithMessage: @"Couldn't load Einstein REX"];
    mLog->LogLine("Couldn't load Einstein REX");
    return;
  }
  
  NSString* theROMPath = [thisBundle pathForResource:@"717006" ofType:0L];
  NSString* theImagePath = [docdir stringByAppendingString:@"/717006.img"];
  
  NSFileManager* theFileManager = [NSFileManager defaultManager];
  if (![theFileManager fileExistsAtPath: theROMPath])
  {
    mLog->LogLine("ROM file doesn't seem to exist");
    return;
  }
  
  mROMImage = new TFlatROMImageWithREX(
                                       [theROMPath UTF8String],
                                       [einsteinRExPath UTF8String],
                                       "717006", false,
                                       [theImagePath UTF8String]);
  
  // Create the network manager.
  mNetworkManager = new TNullNetwork(mLog);
  
  // Create the sound manager.
  mSoundManager = new TNullSoundManager(mLog);
  
  mScreenManager = new TIOSScreenManager(
                                           (iEinsteinView*)[self view],
                                           self,
                                           mLog,
                                           320,
                                           480,
                                           true,
                                           false);

  [(iEinsteinView*)[self view] setScreenManager:mScreenManager];
  
  // Create the emulator.
  char theFlashPath[2048];
  sprintf(theFlashPath, "%s/flash", [docdir UTF8String]);
  printf("Flash file is %s\n", theFlashPath);
  
  mEmulator = new TEmulator(
                            mLog, mROMImage, theFlashPath,
                            mSoundManager, mScreenManager, mNetworkManager, 0x40 << 16 );
  mPlatformManager = mEmulator->GetPlatformManager();
  
  ((TIOSScreenManager*) mScreenManager)->SetPlatformManager( mPlatformManager );
  
  // Start the thread.
  [NSThread detachNewThreadSelector: @selector(runEmulator) toTarget: self withObject: NULL];
}



// -------------------------------------------------------------------------- //
//  * (void)runEmulator
// -------------------------------------------------------------------------- //
- (void)runEmulator
{
  mEmulator->Run();
  // Quit if the emulator quitted.
  //mQuit = true;
  // FIXME: bad idea! this crashes!
  [[UIApplication sharedApplication] terminate: self];
}


@end


