// ==============================
// File:			iEinsteinAppDelegate.mm
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

#import "iEinsteinAppDelegate.h"
#import "iEinsteinViewController.h"

@implementation iEinsteinAppDelegate


+ (void)initialize
{
	NSDictionary* defaults = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithInt:320], @"NewtonScreenWidth", 
			[NSNumber numberWithInt:480], @"NewtonScreenHeight", 
			nil];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaults];
}


- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions 
{
    // Override point for customization after app launch
    
    // Get the user preferences
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [self setScreenResolution:[defaults integerForKey:@"screen_resolution"]];
    //[self setAutoRotate:[defaults boolForKey:@"auto_rotate"]];
    //[self setClearFlashRAM:[defaults boolForKey:@"clear_flash_ram"]];
    [defaults setValue:[NSNumber numberWithInt:false] forKey:@"clear_flash_ram"];
    
	[window addSubview:[viewController view]];
	[window makeKeyAndVisible];

	return YES;
}

- (void)setScreenResolution:(int)index
{
    static int widthLUT[]  = { 320, 640, 384,  512 };
    static int heightLUT[] = { 480, 960, 512, 1024 };
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setValue:[NSNumber numberWithInt:widthLUT[index]]  forKey:@"NewtonScreenWidth"];
    [defaults setValue:[NSNumber numberWithInt:heightLUT[index]] forKey:@"NewtonScreenHeight"];
}

- (void)applicationWillResignActive:(UIApplication*)application
{
	[viewController stopEmulator];
}


- (void)applicationDidBecomeActive:(UIApplication*)application 
{
	[viewController startEmulator];
}


- (void)dealloc 
{
    [viewController release];
    [window release];
    [super dealloc];
}


@end
