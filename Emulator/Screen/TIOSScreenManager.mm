// ==============================
// File:			TIOSScreenManager.cp
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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

#include <K/Defines/KDefinitions.h>
#include "TIOSScreenManager.h"
//#include "TCocoaScreenGlue.h"

#include "Emulator/Platform/TPlatformManager.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TIOSScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TIOSScreenManager::TIOSScreenManager(
                                     iEinsteinView *inView,
                                     iEinsteinViewController *inCtrl,
			TLog* inLog /* = nil */,
			KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
			KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
			Boolean inFullScreen /* = false */,
			Boolean inScreenIsLandscape /* = false */)
	:
		TScreenManager( inLog, inPortraitWidth, inPortraitHeight, inFullScreen, inScreenIsLandscape ),
		mView( inView ),
		mController( inCtrl ),
		//mEmulatorWindow( NULL ),
		//mEmulatorScreenView( NULL ),
		//mEmulatorScreenText( NULL ),
		mPlatformManager( nil ),
		//mPowerIsOn( false ),
		mImageBuffer( NULL )
{
	// Create the image.
	size_t theBufferSize = inPortraitWidth * inPortraitHeight * sizeof(KUInt32);
	mImageBuffer = (KUInt32*) ::malloc( theBufferSize );
	bzero(mImageBuffer, theBufferSize);
	
	// Create the direct access data provider.
	mDataProviderRef = CGDataProviderCreateWithData(
							NULL,
							mImageBuffer,
							theBufferSize,
							NULL );
	
	//int theWindowWidth = 320;
	//int theWindowHeight = 480;
/*
	// Create the text widget.
	mEmulatorScreenText =
		EmulatorText_Create(theWindowWidth, theWindowHeight);
	
	// Create the screen view.
	mEmulatorScreenView =
		TCocoaScreenView_Create(this, theWindowWidth, theWindowHeight);
	
	// Create the window.
	if (inFullScreen)
	{
		mEmulatorWindow =
			EmulatorWindow_CreateFullScreen(
				this,
				inEmulatorApp,
				mEmulatorScreenView,
				mEmulatorScreenText);
	} else {
		mEmulatorWindow =
			EmulatorWindow_Create(
				inEmulatorApp,
				GetScreenWidth(),
				GetScreenHeight(),
				mEmulatorScreenView,
				mEmulatorScreenText);
	}

	// Hide our view (until the OS powers the screen).
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenView, true);
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenText, false);
	
	// Open the window.
	EmulatorWindow_MakeFront(mEmulatorWindow);
   */
}

// -------------------------------------------------------------------------- //
//  * TIOSScreenManager( void )
// -------------------------------------------------------------------------- //
TIOSScreenManager::~TIOSScreenManager( void )
{
  /*
	if (mPowerIsOn)
	{
		PowerOffScreen();
	}
   */
	if (mDataProviderRef)
	{
		::CGDataProviderRelease(mDataProviderRef);
	}
	if (mImageBuffer)
	{
		::free( mImageBuffer );
	}
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::PowerOn( void )
{
  /*
	// Set the text.
	EmulatorText_SetScreenOffString(mProxy, mEmulatorScreenText);

	// Report the new state.
	EmulatorApp_PowerChange(mProxy, mEmulatorApp, true);
   */
}

// -------------------------------------------------------------------------- //
//  * PowerOff( void )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::PowerOff( void )
{
  /*
	// Set the text.
	EmulatorText_SetEinsteinOffString(mProxy, mEmulatorScreenText);

	// Report the new state.
	EmulatorApp_PowerChange(mProxy, mEmulatorApp, false);
   */
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::PowerOnScreen( void )
{
  /*
	// Show our view.
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenView, false);
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenText, true);
	
	// Make our view the first responder.
	EmulatorWindow_SetFirstResponder(mEmulatorWindow, mEmulatorScreenView);
	mPowerIsOn = true;
   */
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::PowerOffScreen( void )
{
  /*
	// Set the text.
	EmulatorText_SetScreenOffString(mProxy, mEmulatorScreenText);

	// Hide our view
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenView, true);
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenText, false);
	mPowerIsOn = false;
   */
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( Boolean )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::BacklightChanged( Boolean inState )
{
	// Redraw the screen.
	SRect wholeScreen;
	wholeScreen.fTop = 0;
	wholeScreen.fLeft = 0;
	wholeScreen.fBottom = GetScreenHeight();
	wholeScreen.fRight = GetScreenWidth();
	UpdateScreenRect( &wholeScreen );
/*
	EmulatorApp_BacklightChange(mProxy, mEmulatorApp, inState);
   */
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::ContrastChanged( KUInt32 )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::ScreenOrientationChanged( EOrientation inScreenOrientation )
{
  // RotateView(mEmulatorScreenView, inScreenOrientation);
	[mView performSelectorOnMainThread:@selector(newtonOrientationChanged) withObject:nil waitUntilDone:YES];
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::TabletOrientationChanged( EOrientation )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{
	KUInt16 top = inUpdateRect->fTop;
	KUInt16 left = inUpdateRect->fLeft;
	KUInt16 height = inUpdateRect->fBottom - top;
	KUInt16 width = inUpdateRect->fRight - left;

	// Update the buffer.
	// We copy more pixels than what we should.
	if (left & 0x1)
	{
		left -= 1;
		width += 1;
	}
	if (width & 0x1)
	{
		width += 1;
	}
	
	KUInt8* theScreenBuffer = GetScreenBuffer();
	KUInt32 theScreenWidth = GetScreenWidth();
	KUInt32 dstRowBytes = theScreenWidth * sizeof(KUInt32);
	KUInt32 srcRowBytes = theScreenWidth * kBitsPerPixel / 8;
	KUInt32 srcWidthInBytes = width * kBitsPerPixel / 8;

	KUInt8* srcRowPtr =
		theScreenBuffer
		+ (top * srcRowBytes)
		+ (left * kBitsPerPixel / 8);
	KUInt8* dstRowPtr =
		((KUInt8*) mImageBuffer)
                + (dstRowBytes*(GetScreenHeight()-1))
		- (top * dstRowBytes)
		+ (left * sizeof(KUInt32));

	int indexRows;
	if (GetBacklight())
	{
		for (indexRows = height; indexRows != 0; indexRows--)
		{
			KUInt8* srcCursor = srcRowPtr;
			KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
			KUInt32* dstCursor = (KUInt32*) dstRowPtr;
			do {
				KUInt8 theByte = *srcCursor++;
				// First pixel
				KUInt32 thePixel = (theByte & 0xF0) >> 4;
				thePixel |= thePixel << 4;	// blue
				thePixel <<= 8;	// shift to green.
				*dstCursor++ = thePixel;
				// Second pixel
				thePixel = (theByte & 0x0F);
				thePixel |= thePixel << 4;	// blue
				thePixel <<= 8;	// shift to green.
				*dstCursor++ = thePixel;
			} while (srcCursor < srcEnd);
			srcRowPtr += srcRowBytes;
			dstRowPtr -= dstRowBytes;
		}
	} else {
		for (indexRows = height; indexRows != 0; indexRows--)
		{
			KUInt8* srcCursor = srcRowPtr;
			KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
			KUInt32* dstCursor = (KUInt32*) dstRowPtr;
			do {
				KUInt8 theByte = *srcCursor++;
				// First pixel
				KUInt32 thePixel = (theByte & 0xF0) >> 4;
				thePixel |= thePixel << 4;	// blue
				thePixel |= thePixel << 8 | thePixel << 16;	// copy to green & red
				*dstCursor++ = thePixel;
				// Second pixel
				thePixel = (theByte & 0x0F);
				thePixel |= thePixel << 4;	// blue
				thePixel |= thePixel << 8 | thePixel << 16;	// copy to green & red
				*dstCursor++ = thePixel;
			} while (srcCursor < srcEnd);
			srcRowPtr += srcRowBytes;
			dstRowPtr -= dstRowBytes;
		}
	}

	// Update.
	
	CGRect newtonRect = CGRectMake(left, top, width, height);
	[mView performSelectorOnMainThread:@selector(setNeedsDisplayInNewtonRect:) withObject:[NSValue valueWithCGRect:newtonRect] waitUntilDone:YES];
}

// -------------------------------------------------------------------------- //
//  * PowerSwitch( void )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::PowerSwitch( void )
{
	mPlatformManager->SendPowerSwitchEvent();
}

// -------------------------------------------------------------------------- //
//  * DraggedFile( const char* )
// -------------------------------------------------------------------------- //
void
TIOSScreenManager::DraggedFile( const char* inPath )
{
	mPlatformManager->InstallPackage( inPath );
}

// -------------------------------------------------------------------------- //
//  * OpenEinsteinMenu()
// -------------------------------------------------------------------------- //
void 
TIOSScreenManager::OpenEinsteinMenu() 
{
    static int once = 0;
    CGRect newtonRect = CGRectMake(0,0,0,0);
    if (!once)
	[mController performSelectorOnMainThread:@selector(openEinsteinMenu:) withObject:[NSValue valueWithCGRect:newtonRect] waitUntilDone:YES];
    once = 1;
    
    //openEinsteinMenu((iEinsteinViewController*)mController);

    //((iEinsteinViewController*)mController)->openEinsteinMenu();
}


// ============================================================================== //
// panic: kernel segmentation violation. core dumped               (only kidding) //
// ============================================================================== //
