// ==============================
// File:			iEinsteinView.h
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

#import "iEinsteinView.h"
#import "TIOSScreenManager.h"

@implementation iEinsteinView

#if !(defined kCGBitmapByteOrder32Host) && TARGET_RT_BIG_ENDIAN
#define kAlphaNoneSkipFirstPlusHostByteOrder (kCGImageAlphaNoneSkipFirst)
#else
#define kAlphaNoneSkipFirstPlusHostByteOrder (kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Host)
#endif


- (id)init 
{
	mScreenManager = NULL;
	mWidth = 320;
	mHeight = 480;
	return self;
}


- (void)setScreenManager:(TScreenManager*)sm 
{ 
	mScreenManager = sm; 
} 


- (void)drawRect:(CGRect)rect 
{
	mWidth = 320;
	mHeight = 480;

	if ( mScreenManager != NULL )
	{
		CGContextRef theContext = UIGraphicsGetCurrentContext(); 
		if ( mScreenImage == NULL )
		{
			CGColorSpaceRef theColorSpace = CGColorSpaceCreateDeviceRGB();
			
			mScreenImage = CGImageCreate(
					mWidth,
					mHeight,
					8,
					32,
					mWidth * sizeof(KUInt32),
					theColorSpace,
					kAlphaNoneSkipFirstPlusHostByteOrder,
					((TIOSScreenManager*)mScreenManager)->GetDataProvider(),
					NULL,
					false,
					kCGRenderingIntentDefault);
			
			CGColorSpaceRelease(theColorSpace);
		}
		
		CGContextDrawImage(theContext,
				CGRectMake(0.0, 0.0, rect.size.width, rect.size.height),
				mScreenImage);
	}
}


@end