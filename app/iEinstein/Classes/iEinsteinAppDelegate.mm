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

#ifdef USE_STORYBOARDS
// Reuse the window variable leveraged by the NIB version of the code
@synthesize window = window;
@synthesize viewController = viewController;
#endif

+ (void)initialize
{
	NSDictionary* defaults = [NSDictionary dictionaryWithObjectsAndKeys:
											   [NSNumber numberWithInt:0], @"screen_resolution",
										   [NSNumber numberWithBool:NO], @"clear_flash_ram",
										   @"{8,9.88897}", @"printable_size",
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

#ifdef USE_STORYBOARDS
	// When using Storyboards, we get back a full heirarchy, so no need to add it
	// to the veiwe and make it visible.
	viewController = (iEinsteinViewController*) self.window.rootViewController;
#else
	[window addSubview:[viewController view]];
	[window makeKeyAndVisible];
#endif

	NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	bool clearFlash = [(NSNumber*) [prefs objectForKey:@"clear_flash_ram"] boolValue];
	if (clearFlash)
	{
		// User requested to clear the Flash Memory
		// Clear this setting from the preferences.
		[prefs setValue:[NSNumber numberWithBool:NO] forKey:@"clear_flash_ram"];
		[prefs synchronize];
		[viewController stopEmulator];
		// Pop up a dialog making sure that the user really wants to that!
		[viewController verifyDeleteFlashRAM:4];
	}

#ifndef USE_STORYBOARDS
	// When using storyboards, we can't init the emulator until the view heirarchy
	// is fully built.  Do it in the iEinsteinViewController.
	[viewController initEmulator];
#endif

	return YES;
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options
{
	if ([url.scheme isEqualToString:@"file"]) {
		[url startAccessingSecurityScopedResource];
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex:0];
		NSString *srcPath = url.path;
		NSString *filename = [srcPath lastPathComponent];
		NSString *dstPath = [documentsDirectory stringByAppendingPathComponent:filename];
		NSError *error = nil;
		
		if(![srcPath isEqualToString:dstPath]) {
			if ([[NSFileManager defaultManager] fileExistsAtPath:dstPath]) {
				[[NSFileManager defaultManager] removeItemAtPath:dstPath error:&error];
				if(error){
					NSLog(@"%@ %ld %@",[error domain],(long)[error code],[[error userInfo] description]);
				}
				else{
					NSLog(@"File removed.");
				}
			}
			[[NSFileManager defaultManager] copyItemAtPath:srcPath toPath:dstPath error:&error];
		}
		
		if(error){
			 NSLog(@"%@ %ld %@",[error domain],(long)[error code],[[error userInfo] description]);
		}
		else {
			NSLog(@"Pkg File copied.");
			[viewController installNewPackages];
			[[NSFileManager defaultManager] removeItemAtPath:dstPath error:&error];
			if(error){
				NSLog(@"%@ %ld %@",[error domain],(long)[error code],[[error userInfo] description]);
			}
			else{
				NSLog(@"Installed Pkg file removed.");
			}
		}
		return YES;
	}
	else {
		 [[UIApplication sharedApplication] openURL:url options:@{} completionHandler:{}];
		return YES;
	}
	return NO;
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
	bool clearFlash = [(NSNumber*) [prefs objectForKey:@"clear_flash_ram"] boolValue];
	if (clearFlash)
	{
		// User requested to clear the Flash Memory
		// Clear this setting from the preferences.
		[prefs setValue:[NSNumber numberWithBool:NO] forKey:@"clear_flash_ram"];
		[prefs synchronize];
		[viewController stopEmulator];
		// Pop up a dialog making sure that the user really wants to that!
		[viewController verifyDeleteFlashRAM:1];
		// replying to the dialog will start the emulator
	} else
	{
		[viewController startEmulator];
	}
	// The platform manager will install new packages as soon as the emlator booted.
	// See: void TNativePrimitives::ExecutePlatformDriverNative( KUInt32 inInstruction )
	// [viewController installNewPackages];
}

- (void)share:(NSString*)data
{
	// dispatch_semaphore_t sema = dispatch_semaphore_create(0);
	dispatch_async(dispatch_get_main_queue(), ^{
		NSArray* objectsToShare = @[ data ];

		UIActivityViewController* activityVC = [[UIActivityViewController alloc] initWithActivityItems:objectsToShare applicationActivities:nil];

		NSArray* excludeActivities = @[ UIActivityTypeAssignToContact,
			UIActivityTypeSaveToCameraRoll,
			UIActivityTypeAddToReadingList,
			UIActivityTypePostToFlickr,
			UIActivityTypePostToVimeo ];

		activityVC.excludedActivityTypes = excludeActivities;
		/*activityVC.completionWithItemsHandler = ^(NSString *activityType, BOOL completed, NSArray *returnedItems, NSError *activityError) {
			dispatch_semaphore_signal(sema);
		};
		 */
		if ([activityVC respondsToSelector:@selector(popoverPresentationController)])
		{
			// iOS8
			UIView* view = (UIView*) viewController.einsteinView;
			CGRect fullView = view.frame;
			activityVC.popoverPresentationController.sourceView = view;
			activityVC.popoverPresentationController.sourceRect = CGRectMake(fullView.size.width / 2 - 10, 10, 20, 20);
		}

		[viewController presentViewController:activityVC animated:YES completion:nil];
	});

	// dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
	// dispatch_release(sema);
}

- (void)dealloc
{
#if !__has_feature(objc_arc)
	[viewController release];
	[window release];
	[super dealloc];
#endif
}

@end
