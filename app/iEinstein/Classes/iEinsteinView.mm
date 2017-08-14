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
#import "TScreenManager.h"

#include "TInterruptManager.h"
#include "TPlatformManager.h"
#include "TEmulator.h"

@implementation iEinsteinView

#if !(defined kCGBitmapByteOrder32Host) && TARGET_RT_BIG_ENDIAN
#define kAlphaNoneSkipFirstPlusHostByteOrder (kCGImageAlphaNoneSkipFirst)
#else
#define kAlphaNoneSkipFirstPlusHostByteOrder (kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Host)
#endif

#define SWAP(x,y) {x=x+y;y=x-y;x=x-y;}

- (void)awakeFromNib
{
	[super awakeFromNib];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(didRotate:)
												 name:UIApplicationDidChangeStatusBarOrientationNotification
											   object:nil];

	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(defaultsChanged:)
												 name:NSUserDefaultsDidChangeNotification
											   object:nil];

	//Get initial state of preference
	[self defaultsChanged:nil];
}

- (void) defaultsChanged:(NSNotification *)notification {
	NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	applePencilMode = [(NSNumber*)[prefs objectForKey:@"apple_pencil"] boolValue];
}

- (void) didRotate:(NSNotification *)notification {
	//UIInterfaceOrientation orientation = [notification.userInfo[UIApplicationStatusBarOrientationUserInfoKey] integerValue];

	// Clear the screen image.  It will be recreated on the next draw.
	CGImageRelease(mScreenImage);
	mScreenImage = NULL;
	
	// Force the next draw.
	[self setNeedsDisplay];
}

- (void)setScreenManager:(TScreenManager*)sm
{
	mScreenManager = sm;
	[self setNeedsDisplay];
}

- (void)setEmulator:(TEmulator*)em
{
    mEmulator = em;
}

// Called by the screen manager when the Newton changes its orientation
-(void) newtonOrientationChanged {
	// Clear the screen image.  It will be recreated on the next draw.
	CGImageRelease(mScreenImage);
	mScreenImage = NULL;
	
	// Force the next draw.
	[self setNeedsDisplay];
}

// Called, among other times, when Split View multi-tasking divider changes.
-(void) layoutSubviews {
	// Clear the screen image.  It will be recreated on the next draw.
	CGImageRelease(mScreenImage);
	mScreenImage = NULL;
	
	// Force the next draw.
	[self setNeedsDisplay];
}

- (void)drawRect:(CGRect)rect
{
	CGContextRef theContext = UIGraphicsGetCurrentContext(); 
	
	if ( mScreenManager == NULL )
	{
		// Just fill black
		CGFloat black[] = { 0.0, 0.0, 0.0, 1.0 };
		CGRect frame = [self frame];
		CGContextSetFillColor(theContext, black);
		CGContextFillRect(theContext, frame);
	}
	else
	{
		if ( mScreenImage == NULL )
		{
			CGColorSpaceRef theColorSpace = CGColorSpaceCreateDeviceRGB();
			
			newtonScreenWidth = mScreenManager->GetScreenWidth();
			newtonScreenHeight = mScreenManager->GetScreenHeight();
			
			mScreenImage = CGImageCreate(
					newtonScreenWidth, newtonScreenHeight,
					8, 32, newtonScreenWidth * sizeof(KUInt32),
					theColorSpace,
					kAlphaNoneSkipFirstPlusHostByteOrder,
					((TIOSScreenManager*)mScreenManager)->GetDataProvider(),
					NULL, false,
					kCGRenderingIntentDefault);
						
			CGColorSpaceRelease(theColorSpace);

			//CGRect screenBounds = [[UIScreen mainScreen] bounds]; //OLD
			CGRect screenBounds = [self bounds];
			CGRect r = [self frame];

			if ( screenBounds.size.width > newtonScreenWidth && screenBounds.size.height > newtonScreenHeight )
			{
				if ( newtonScreenWidth == newtonScreenHeight )
				{
					// Newton screen resolution is square (like 320x320)
					
					int mod = (int)screenBounds.size.width % newtonScreenWidth;
					r.size.width -= mod;
					r.size.height = r.size.width;
				}
				else
				{
					// Newton screen resolution is rectangular (like 320x480)

					int wmod = (int)r.size.width % newtonScreenWidth;
					int hmod = (int)r.size.height % newtonScreenHeight;
					
					if ( wmod > hmod )
					{
						r.size.width -= wmod;
						
						int scale = (int)r.size.width / newtonScreenWidth;
						r.size.height = newtonScreenHeight * scale;
					}
					else
					{
						r.size.height -= hmod;
						
						int scale = (int)r.size.height / newtonScreenHeight;
						r.size.width = newtonScreenWidth * scale;
					}

				}

				// Center image on screen
				
				r.origin.x += (screenBounds.size.width - r.size.width) / 2;
				r.origin.y += (screenBounds.size.height - r.size.height) / 2;
			}

			screenImageRect = r;
			screenImageRect.origin.x = floor(screenImageRect.origin.x);
			screenImageRect.origin.y = floor(screenImageRect.origin.y);
			screenImageRect.size.width = floor(screenImageRect.size.width);
			screenImageRect.size.height = floor(screenImageRect.size.height);
		}
				
		CGContextSetInterpolationQuality(theContext, kCGInterpolationNone);
		CGContextDrawImage(theContext, screenImageRect, mScreenImage);
	}
}


- (void)reset
{
	mEmulator = NULL;
	mScreenManager = NULL;
	CGImageRelease(mScreenImage);
	mScreenImage = NULL;
}


- (void)setNeedsDisplayInNewtonRect:(NSValue*)v
{
	// Convert from Newton display coordinates to native
	
	CGRect inRect = [v CGRectValue];
	CGRect r = screenImageRect;
	
	float wratio = r.size.width / newtonScreenWidth;
	float hratio = r.size.height / newtonScreenHeight;

	int left = (inRect.origin.x * wratio) + r.origin.x;
	int top = (inRect.origin.y * hratio) + r.origin.y;	
	int right = left + (inRect.size.width * wratio);
	int bottom = top + (inRect.size.height * hratio);
	
	CGRect outRect = CGRectMake(left, top, right - left + 1, bottom - top + 1);
	[self setNeedsDisplayInRect:outRect];
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* t = nil;
	if (applePencilMode) {
		for (UITouch* aTouch in touches) {
			if (aTouch.type == UITouchTypeStylus) {
				t = aTouch;
				break;
			}
		}
	} else {
		t = [touches anyObject];
	}
	
	if ( t )
	{
        if (!mEmulator->GetPlatformManager()->IsPowerOn()) {
            // After five minutes, the MP switches itself off. On the iPad and
            // iPhone, the host OS should decide when to put the device to sleep.
            // Newton OS nevertheless switches itself off. To continue work,
            // we'd have to pull the power switch. Since we have no power switch 
            // on Einstein/IOS, any screen touch will power the Newton back on.
            mEmulator->GetPlatformManager()->SendPowerSwitchEvent();
        }
		CGPoint p = [t locationInView:self];
        CGRect r = screenImageRect;
		int x = (1.0 - ((p.y - r.origin.y) / r.size.height)) * newtonScreenHeight;
		int y = ((p.x - r.origin.x) / r.size.width) * newtonScreenWidth;
		
		// Translate coordinates based on orientation
		switch (mScreenManager->GetScreenOrientation()) {
			case TScreenManager::kOrientation_AppleRight:
				SWAP(x,y);
				y = newtonScreenHeight-y;
				break;
			case TScreenManager::kOrientation_AppleLeft:
				SWAP(x,y);
				x = newtonScreenWidth-x;
				break;
			case TScreenManager::kOrientation_AppleTop:
				y = newtonScreenWidth-y;
				x = newtonScreenHeight-x;
				break;
			default:
				break;
		}
		
		mScreenManager->PenDown(x, y);
	}
}


- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* t = nil;
	if (applePencilMode) {
		for (UITouch* aTouch in touches) {
			if (aTouch.type == UITouchTypeStylus) {
				t = aTouch;
				break;
			}
		}
	} else {
		t = [touches anyObject];
	}

	if ( t ) 
	{
		CGPoint p = [t locationInView:self];
        CGRect r = screenImageRect;
		int x = (1.0 - ((p.y - r.origin.y) / r.size.height)) * newtonScreenHeight;
		int y = ((p.x - r.origin.x) / r.size.width) * newtonScreenWidth;

		// Translate coordinates based on orientation
		switch (mScreenManager->GetScreenOrientation()) {
			case TScreenManager::kOrientation_AppleRight:
				SWAP(x,y);
				y = newtonScreenHeight-y;
				break;
			case TScreenManager::kOrientation_AppleLeft:
				SWAP(x,y);
				x = newtonScreenWidth-x;
				break;
			case TScreenManager::kOrientation_AppleTop:
				y = newtonScreenWidth-y;
				x = newtonScreenHeight-x;
				break;
			default:
				break;
		}
		
		mScreenManager->PenDown(x, y);
	}
}


- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	mScreenManager->PenUp();
}


@end
