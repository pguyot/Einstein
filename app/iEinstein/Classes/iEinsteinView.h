//
//  iEinsteinViewController.h
//  iEinstein
//
//  Created by Matthias Melcher on 12.09.10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#include <K/Defines/KDefinitions.h>


@interface iEinsteinView : UIView {

  id	mScreenManager;
  CGImageRef				mScreenImage;
  //BOOL					mVisible;
  int						mWidth;
  int						mHeight;
  //EOrientation			mOrientation;
  //KUInt32					mPreviousMods;
  
}

- (id)init;

- (void)dealloc;

- (void)SetScreenManager:(id)sm;
  
@end

