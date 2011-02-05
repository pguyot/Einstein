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

#include "Emulator/JIT/TJITPerformance.h"


@implementation iEinsteinAppDelegate


+ (void)initialize
{
	NSDictionary* defaults = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithInt:0], @"screen_resolution", 
			[NSNumber numberWithBool:NO], @"clear_flash_ram",
			nil];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaults];
	[[NSUserDefaults standardUserDefaults] synchronize];
}


- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions 
{
    
    fprintf(stderr, "UIApplication::didFinishLaunchingWithOptions -> check for added files\n");
    
    // Override point for customization after app launch
    
    // Get the user preferences
	
    //[self setAutoRotate:[defaults boolForKey:@"auto_rotate"]];
    
	[window addSubview:[viewController view]];
	[window makeKeyAndVisible];

    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    bool clearFlash = [(NSNumber*)[prefs objectForKey:@"clear_flash_ram"] boolValue];
    if (clearFlash) {
        // User requested to clear the Flash Memory
        // Clear this setting from the preferences.
        [prefs setValue:[NSNumber numberWithBool:NO] forKey:@"clear_flash_ram"];
        [prefs synchronize];
        [viewController stopEmulator];
        // Pop up a dialog making sure that the user really wants to that!
        [viewController verifyDeleteFlashRAM:4];
    }

    [viewController initEmulator];

    return YES;
}


- (void)applicationWillResignActive:(UIApplication*)application
{
	[viewController stopEmulator];
}


- (void)applicationDidBecomeActive:(UIApplication*)application 
{    
    if (![viewController allResourcesFound])
        return;
    
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    bool clearFlash = [(NSNumber*)[prefs objectForKey:@"clear_flash_ram"] boolValue];
    if (clearFlash) {
        // User requested to clear the Flash Memory
        // Clear this setting from the preferences.
        [prefs setValue:[NSNumber numberWithBool:NO] forKey:@"clear_flash_ram"];
        [prefs synchronize];
        [viewController stopEmulator];
        // Pop up a dialog making sure that the user really wants to that!
        [viewController verifyDeleteFlashRAM:1];
        // replying to the dialog will start the emulator
    } else {
        [viewController startEmulator];
    }
    [viewController installNewPackages];
}


- (void)dealloc 
{
    [viewController release];
    [window release];
    [super dealloc];
}


@end