// ==============================
// File:			TCocoaScreenManager.cp
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
#include "TCocoaScreenManager.h"
#include "TCocoaScreenGlue.h"

#include "Emulator/Platform/TPlatformManager.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TCocoaScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TCocoaScreenManager::TCocoaScreenManager(
			id inProxy,
			id inEmulatorApp,
			TLog* inLog /* = nil */,
			KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
			KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
			Boolean inFullScreen /* = false */,
			Boolean inScreenIsLandscape /* = false */)
	:
		TScreenManager( inLog, inPortraitWidth, inPortraitHeight, inFullScreen, inScreenIsLandscape ),
		mProxy( inProxy ),
		mEmulatorApp( inEmulatorApp ),
		mEmulatorWindow( NULL ),
		mEmulatorScreenView( NULL ),
		mEmulatorScreenText( NULL ),
		mPlatformManager( nil ),
		mPowerIsOn( false ),
		mImageBuffer( NULL )
{
	// Capture the main display.
	if (inFullScreen)
	{
		(void) CGDisplayCapture(kCGDirectMainDisplay);
	}

	// Create the image.
	size_t theBufferSize = GetScreenWidth()
					* GetScreenHeight()
					* sizeof(KUInt32);
	mImageBuffer = (KUInt32*) ::malloc( theBufferSize );
	
	// Create the direct access data provider.
	mDataProviderRef = CGDataProviderCreateWithData(
							NULL,
							mImageBuffer,
							theBufferSize,
							NULL );
	
	int theWindowWidth = GetActualScreenWidth();
	int theWindowHeight = GetActualScreenHeight();

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
}

// -------------------------------------------------------------------------- //
//  * ~TCocoaScreenManager( void )
// -------------------------------------------------------------------------- //
TCocoaScreenManager::~TCocoaScreenManager( void )
{
	if (mPowerIsOn)
	{
		PowerOffScreen();
	}
	if (mDataProviderRef)
	{
		::CGDataProviderRelease(mDataProviderRef);
	}
	if (mImageBuffer)
	{
		::free( mImageBuffer );
	}
	// Release the main display.
	if (IsFullScreen())
	{
		(void) CGDisplayRelease(kCGDirectMainDisplay);
	}
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::PowerOn( void )
{
	// Set the text.
	EmulatorText_SetScreenOffString(mProxy, mEmulatorScreenText);

	// Report the new state.
	EmulatorApp_PowerChange(mProxy, mEmulatorApp, true);
}

// -------------------------------------------------------------------------- //
//  * PowerOff( void )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::PowerOff( void )
{
	// Set the text.
	EmulatorText_SetEinsteinOffString(mProxy, mEmulatorScreenText);

	// Report the new state.
	EmulatorApp_PowerChange(mProxy, mEmulatorApp, false);
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::PowerOnScreen( void )
{
	// Show our view.
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenView, false);
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenText, true);
	
	// Make our view the first responder.
	EmulatorWindow_SetFirstResponder(mEmulatorWindow, mEmulatorScreenView);
	mPowerIsOn = true;
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::PowerOffScreen( void )
{
	// Set the text.
	EmulatorText_SetScreenOffString(mProxy, mEmulatorScreenText);

	// Hide our view
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenView, true);
	TCocoaScreenView_SetHidden(mProxy, mEmulatorScreenText, false);
	mPowerIsOn = false;
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( Boolean )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::BacklightChanged( Boolean inState )
{
	// Redraw the screen.
	SRect wholeScreen;
	wholeScreen.fTop = 0;
	wholeScreen.fLeft = 0;
	wholeScreen.fBottom = GetScreenHeight();
	wholeScreen.fRight = GetScreenWidth();
	UpdateScreenRect( &wholeScreen );
	EmulatorApp_BacklightChange(mProxy, mEmulatorApp, inState);
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::ContrastChanged( KUInt32 )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::ScreenOrientationChanged( EOrientation inScreenOrientation )
{
	if (IsFullScreen())
	{
		// Rotate the view.
		RotateView(
			mEmulatorScreenView,
			inScreenOrientation);
	} else {
		// Resize the window.
		ResizeForRotation(
			mEmulatorWindow,
			mEmulatorScreenView,
			GetScreenWidth(),
			GetScreenHeight());
	}
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::TabletOrientationChanged( EOrientation )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::UpdateScreenRect( SRect* inUpdateRect )
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
		+ (top * dstRowBytes)
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
			dstRowPtr += dstRowBytes;
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
			dstRowPtr += dstRowBytes;
		}
	}
	
	// Update the overlay plane
	// TODO: we can save some time if we do this only for an overlapping area
	if (mOverlayIsOn) {
		// TODO: this calculation should only be done during screen sizing and orientation changes
		mOverlayRect.fLeft = GetScreenWidth()/2 - 20*8;
		mOverlayRect.fRight = mOverlayRect.fLeft+40*8;
		mOverlayRect.fTop = GetScreenHeight() - 16*5;
		mOverlayRect.fBottom = mOverlayRect.fTop + 16*4;
		
		KUInt8* dstRowPtr =
			((KUInt8*) mImageBuffer)
			+ (mOverlayRect.fTop * dstRowBytes)
			+ (mOverlayRect.fLeft * sizeof(KUInt32))
		;
		
		KUInt32 line;
		for (line=0; line<4; line++) {
			if (mOverlayIsDirty[line]) {
				KUInt32 row ;
				for (row=0; row<13; row++) {
					KUInt32 cx;
					KUInt32 *dstCursor = ((KUInt32*)(dstRowPtr + (line*16 + row)*dstRowBytes));
					for (cx=0; cx<40; cx++) {
						char c = mOverlay[line][cx] & 0x7f;
						if (c!=0 && c!=32) {
							KUInt32 x;
							KUInt16 pattern = mFontData[c][row];
							for (x=8; x>0; x--) {
								if (pattern&0x180) {
									*dstCursor++ = 0x00FF8888;
								} else {
									dstCursor++;
								}
								pattern <<= 1;
							}
						} else {
							dstCursor += 8;
						}
					}
				}
			}
		}
		
	}
	
	// Update.
	TCocoaScreenView_SetNeedsDisplay(mProxy, mEmulatorScreenView);
}

// -------------------------------------------------------------------------- //
//  * PowerSwitch( void )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::PowerSwitch( void )
{
	mPlatformManager->SendPowerSwitchEvent();
}

// -------------------------------------------------------------------------- //
//  * DraggedFile( const char* )
// -------------------------------------------------------------------------- //
void
TCocoaScreenManager::DraggedFile( const char* inPath )
{
	mPlatformManager->InstallPackage( inPath );
}

// ============================================================================== //
// panic: kernel segmentation violation. core dumped               (only kidding) //
// ============================================================================== //
