//
//  iEinsteinAppDelegate.h
//  iEinstein
//
//  Created by Matthias Melcher on 12.09.10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@class iEinsteinViewController;

@interface iEinsteinAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    iEinsteinViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet iEinsteinViewController *viewController;

@end

