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
    /* This is how we access preferences!
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [self setShouldPlaySounds:[defaults boolForKey:@"play_sounds_preference"]];
     */
    
	[window addSubview:[viewController view]];
	[window makeKeyAndVisible];

	return YES;
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
