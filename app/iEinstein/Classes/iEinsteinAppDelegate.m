//
//  iEinsteinAppDelegate.m
//  iEinstein
//
//  Created by Matthias Melcher on 12.09.10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "iEinsteinAppDelegate.h"
#import "iEinsteinViewController.h"

@implementation iEinsteinAppDelegate

@synthesize window;
@synthesize viewController;


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    
    // Override point for customization after app launch    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
	
	return YES;
}


- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}


@end
