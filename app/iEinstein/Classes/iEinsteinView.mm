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
					mWidth, mHeight,
					8, 32, mWidth * sizeof(KUInt32),
					theColorSpace,
					kAlphaNoneSkipFirstPlusHostByteOrder,
					((TIOSScreenManager*)mScreenManager)->GetDataProvider(),
					NULL, false,
					kCGRenderingIntentDefault);
			
			CGColorSpaceRelease(theColorSpace);

			CGRect screenBounds = [[UIScreen mainScreen] bounds];
			CGRect r = [self frame];
			
			if ( screenBounds.size.width > mWidth && screenBounds.size.height > mHeight )
			{
				int mod = (int)r.size.height % (int)r.size.width;
				r.size.width = r.size.width - (mod / 2);
				r.size.height = r.size.width + (r.size.width / 2);
				r.origin.x += (screenBounds.size.width - r.size.width) / 2;
				r.origin.y += (screenBounds.size.height - r.size.height) / 2;
			}

			screenImageRect = CGRectIntegral(r);
		}
		
		CGContextSetInterpolationQuality(theContext, kCGInterpolationNone);
		CGContextDrawImage(theContext, screenImageRect, mScreenImage);
	}
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* t = [touches anyObject];
	if ( t ) 
	{
		CGPoint p = [t locationInView:self];
        CGRect r = screenImageRect;
		int x = (1.0 - ((p.y - r.origin.y) / r.size.height)) * mHeight;
		int y = ((p.x - r.origin.x) / r.size.width) * mWidth;
		mScreenManager->PenDown(x, y);
	}
}


- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* t = [touches anyObject];
	if ( t ) 
	{
		CGPoint p = [t locationInView:self];
        CGRect r = screenImageRect;
		int x = (1.0 - ((p.y - r.origin.y) / r.size.height)) * mHeight;
		int y = ((p.x - r.origin.x) / r.size.width) * mWidth;
		mScreenManager->PenDown(x, y);
	}
}


- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	mScreenManager->PenUp();
}


@end