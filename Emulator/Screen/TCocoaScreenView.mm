// ==============================
// File:			TCocoaScreenView.mm
// Project:			Einstein
//
// Copyright 2004-2007 by Paul Guyot (pguyot@kallisys.net).
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

#import "TCocoaScreenView.h"
#include <Carbon/Carbon.h>

#include "TCocoaScreenGlue.h"
#include "TCocoaScreenManager.h"

#if !(defined kCGBitmapByteOrder32Host) && TARGET_RT_BIG_ENDIAN
#define kAlphaNoneSkipFirstPlusHostByteOrder (kCGImageAlphaNoneSkipFirst)
#else
#define kAlphaNoneSkipFirstPlusHostByteOrder (kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Host)
#endif

@implementation TCocoaScreenView

// -------------------------------------------------------------------------- //
//  * (id)initWithFrame:(NSRect) screenManager:(TCocoaScreenManager*)
// -------------------------------------------------------------------------- //
- (id)initWithFrame:(NSRect)frameRect screenManager:(TCocoaScreenManager*)inScreenManager
{
	if ((self = [super initWithFrame:frameRect]) != nil) {
		mScreenManager = inScreenManager;
		mWidth = inScreenManager->GetScreenWidth();
		mHeight = inScreenManager->GetScreenHeight();
		mOrientation = kNormal;
		mScreenImage = NULL;
		mPreviousMods = 0;
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];
	}
	return self;
}

// -------------------------------------------------------------------------- //
//  * (void)drawRect:(NSRect)
// -------------------------------------------------------------------------- //
- (void)drawRect:(NSRect)rect
{
	if (mScreenManager != NULL)
	{
		CGContextRef theContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(theContext);
		if(mScreenImage == NULL)
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
								mScreenManager->GetDataProvider(),
								NULL,
								false,
								kCGRenderingIntentAbsoluteColorimetric );
			CGColorSpaceRelease(theColorSpace);
			
		}
		
		switch (mOrientation)
		{
			case kNormal:
				break;

			case k90Clockwise:
				CGContextTranslateCTM(theContext, 0, mWidth);
				CGContextRotateCTM(theContext, -M_PI/2.0);
				break;

			case k180Clockwise:
				CGContextTranslateCTM(theContext, mWidth, mHeight);
				CGContextRotateCTM(theContext, M_PI);
				break;

			case k270Clockwise:
				CGContextTranslateCTM(theContext, mHeight, 0);
				CGContextRotateCTM(theContext, M_PI/2.0);
				break;
		}
		
		CGContextSetInterpolationQuality(theContext, kCGInterpolationNone);

		CGContextDrawImage(theContext,
			CGRectMake(
				0.0,
				0.0,
				mWidth,
				mHeight),
				mScreenImage);

		// Matt: it turns out that CGContextDrawImage uses a cached image under
		// certain conditions. Under Mountain Lion, this seems to be true at all
		// times. Apart from using OpenGL, the only way I know to invalidate the
		// cache is to destroy the CGImage. I assume we are tripple and
		// quadruple buffering the Newton screen now. A better solution should
		// be found.
		// FIXME: use CGLayer or CGBitmapContext or glCopy?
		CGImageRelease(mScreenImage);
		mScreenImage = NULL;

		CGContextRestoreGState(theContext);
	}
}

// -------------------------------------------------------------------------- //
//  * (void) setScreenWidth:(int) height:(int) angle:(float)
// -------------------------------------------------------------------------- //
- (void)setScreenWidth:(int) inWidth height:(int) inHeight orientation:(EOrientation) inOrientation
{
	// Save the new values.
	mWidth = inWidth;
	mHeight = inHeight;
	mOrientation = inOrientation;
	
	// clear the image.
	if (mScreenImage)
	{
		CGImageRelease(mScreenImage);
		mScreenImage = NULL;
	}
}

// -------------------------------------------------------------------------- //
//  * (BOOL)mouseDownCanMoveWindow
// -------------------------------------------------------------------------- //
- (BOOL)mouseDownCanMoveWindow
{
	// Filter drag events.
	return NO;
}

// -------------------------------------------------------------------------- //
//  * (BOOL)isOpaque
// -------------------------------------------------------------------------- //
- (BOOL)isOpaque
{
	// The view is opaque.
	return YES;
}

// -------------------------------------------------------------------------- //
//  * (NSPoint)convertPenPoint:(NSPoint)inMousePoint
// -------------------------------------------------------------------------- //
- (NSPoint)convertPenPoint:(NSPoint)inMousePoint
{
	NSPoint result;
	
	switch (mOrientation)
	{
		case kNormal:
			result.x = inMousePoint.x;
			result.y = mHeight - inMousePoint.y;
			break;

		case k90Clockwise:
			result.x = mWidth - inMousePoint.y;
			result.y = mHeight - inMousePoint.x;
			break;

		case k180Clockwise:
			result.x = mWidth - inMousePoint.x;
			result.y = inMousePoint.y;
			break;

		case k270Clockwise:
			result.x = inMousePoint.y;
			result.y = inMousePoint.x;
			break;
	}
	
	return result;
}

// -------------------------------------------------------------------------- //
//  * (void)mouseDown:(NSEvent *)
// -------------------------------------------------------------------------- //
- (void)mouseDown:(NSEvent *)theEvent
{
    BOOL keepOn = YES;

	NSPoint mouseLoc = [self convertPoint: [theEvent locationInWindow] fromView: nil];
	NSPoint penLoc = [self convertPenPoint: mouseLoc];
	mScreenManager->PenDown(
						(KUInt16) penLoc.x,
						(KUInt16) penLoc.y);

    while (keepOn)
	{
		NSDate* nextDate =
			[NSDate dateWithTimeIntervalSinceNow:
				((double) mScreenManager->GetTabletSampleRate()) / 4000000.0];
        theEvent = [[self window] nextEventMatchingMask:
						NSLeftMouseUpMask | NSLeftMouseDraggedMask
						untilDate: nextDate
						inMode: NSDefaultRunLoopMode /* NSEventTrackingRunLoopMode */
						dequeue: YES];
		if ([self mouse:mouseLoc inRect:[self bounds]])
		{
			if (theEvent)
			{
				switch ([theEvent type]) {
					case NSLeftMouseDragged:
						penLoc = [self convertPenPoint:[theEvent locationInWindow]];
						mScreenManager->PenDown(
							(KUInt16) penLoc.x,
							(KUInt16) penLoc.y);
						break;
					case NSLeftMouseUp:
						mScreenManager->PenUp();
						keepOn = NO;
						break;
					default:
						mScreenManager->PenDown(
							(KUInt16) penLoc.x,
							(KUInt16) penLoc.y);
						break;
				}
			} else {
				mScreenManager->PenDown(
					(KUInt16) penLoc.x,
					(KUInt16) penLoc.y);
			}
		} else {
			mScreenManager->PenUp();
			break;
		}
    }
}

// ------------------------------------------------------------------------- //
//  * (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
// ------------------------------------------------------------------------- //
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
	NSPasteboard *pboard;
	NSDragOperation sourceDragMask;
	NSDragOperation theResult = NSDragOperationNone;

	sourceDragMask = [sender draggingSourceOperationMask];
	pboard = [sender draggingPasteboard];

	if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
		if (sourceDragMask & NSDragOperationLink) {
			theResult = NSDragOperationLink;
		} else if (sourceDragMask & NSDragOperationCopy) {
			theResult = NSDragOperationCopy;
		}
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * (BOOL)performDragOperation:(id <NSDraggingInfo>)
// -------------------------------------------------------------------------- //
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pboard = [sender draggingPasteboard];
    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
        NSUInteger numberOfFiles = [files count];
        // Perform operation using the list of files
		int indexFiles;
		for(indexFiles = 0 ; indexFiles < numberOfFiles ; indexFiles++)
		{
			mScreenManager->DraggedFile([[files objectAtIndex:indexFiles] fileSystemRepresentation]);
		}
    }
    return YES;
}

// -------------------------------------------------------------------------- //
//  * (void)flagsChanged:(NSEvent*)
// -------------------------------------------------------------------------- //
- (void)flagsChanged:(NSEvent*) theEvent
{
	// Cocoa doesn't distinguish left & right keys.
	// So I ask Carbon.
	// FIXME: Carbon does not neccessarily handle the same event as Cocoa!
	UInt32 theMods = GetCurrentKeyModifiers();

	if (( theMods & cmdKey ) != ( mPreviousMods & cmdKey ))
	{
		if (theMods & cmdKey)
		{
			mScreenManager->KeyDown(0x37);	// Apple key (any).
		} else {
			mScreenManager->KeyUp(0x37);	// Apple key (any).
		}
	}

	if (( theMods & shiftKey ) != ( mPreviousMods & shiftKey ))
	{
		if (theMods & shiftKey)
		{
			mScreenManager->KeyDown(0x38);	// Shift left.
		} else {
			mScreenManager->KeyUp(0x38);	// Shift left.
		}
	}

	if (( theMods & alphaLock ) != ( mPreviousMods & alphaLock ))
	{
		// Temporary hack.
		mScreenManager->KeyDown(0x39);	// caps lock.
		mScreenManager->KeyUp(0x39);	// caps lock.
//		if (theMods & alphaLock)
//		{
//			mScreenManager->KeyDown(0x39);	// caps lock.
//		} else {
//			mScreenManager->KeyUp(0x39);	// caps lock.
//		}
	}
	
	if (( theMods & optionKey ) != ( mPreviousMods & optionKey ))
	{
		if (theMods & optionKey)
		{
			mScreenManager->KeyDown(0x3A);	// Option left.
		} else {
			mScreenManager->KeyUp(0x3A);	// Option left.
		}
	}

	if (( theMods & controlKey ) != ( mPreviousMods & controlKey ))
	{
		if (theMods & controlKey)
		{
			mScreenManager->KeyDown(0x3B);	// Control left.
		} else {
			mScreenManager->KeyUp(0x3B);	// Control left.
		}
	}

	if (( theMods & rightShiftKey ) != ( mPreviousMods & rightShiftKey ))
	{
		if (theMods & rightShiftKey)
		{
			mScreenManager->KeyDown(0x3C);	// Shift right.
		} else {
			mScreenManager->KeyUp(0x3C);	// Shift right.
		}
	}

	if (( theMods & rightOptionKey ) != ( mPreviousMods & rightOptionKey ))
	{
		if (theMods & rightOptionKey)
		{
			mScreenManager->KeyDown(0x3D);	// Option right.
		} else {
			mScreenManager->KeyUp(0x3D);	// Option right.
		}
	}

	mPreviousMods = theMods;
}

// -------------------------------------------------------------------------- //
//  * (void)keyDown:(NSEvent*)
// -------------------------------------------------------------------------- //
- (void)keyDown:(NSEvent*) theEvent
{
	int theKeyCode = [theEvent keyCode];
	if ([theEvent isARepeat])
	{
		mScreenManager->KeyRepeat(theKeyCode);
	} else {
		mScreenManager->KeyDown(theKeyCode);
	}
}

// -------------------------------------------------------------------------- //
//  * (void)keyUp:(NSEvent*)
// -------------------------------------------------------------------------- //
- (void)keyUp:(NSEvent*) theEvent
{
	int theKeyCode = [theEvent keyCode];		
	mScreenManager->KeyUp(theKeyCode);	
}

// -------------------------------------------------------------------------- //
//  * (BOOL)mouseInView
// -------------------------------------------------------------------------- //
- (BOOL)mouseInView
{
	NSRect box = [self frame];
	NSPoint loc = [[self window] mouseLocationOutsideOfEventStream];
	return [self mouse: loc inRect: box];
}

@end

// ========================================================================== //
// DOS Air:                                                                   //
// All the passengers go out onto the runway, grab hold of the plane, push it //
// until it gets in the air, hop on, jump off when it hits the ground again.  //
// Then they grab the plane again, push it back into the air, hop on, et      //
// cetera.                                                                    //
// ========================================================================== //
