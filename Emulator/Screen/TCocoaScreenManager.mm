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
			CocoaScreenProxy* inProxy,
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
	mImageBuffer = (KUInt32*) ::calloc( 1, theBufferSize );
	
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
	// inUpdateRect is in NewtonOS screen coordinates, origin is top left

	// Set some variables for fast buffer access.
	KUInt8* theScreenBuffer = GetScreenBuffer();
	KUInt32 theScreenWidth = GetScreenWidth();
	KUInt32 theScreenHeight = GetScreenHeight();
	KUInt32 dstRowBytes = theScreenWidth * sizeof(KUInt32);
	KUInt32 srcRowBytes = theScreenWidth * kBitsPerPixel / 8;

	// Find the size of the rectangle that needs to be updated.
	KUInt16 top = inUpdateRect->fTop;
	KUInt16 left = inUpdateRect->fLeft;
	KUInt16 bottom = inUpdateRect->fBottom;
	KUInt16 right = inUpdateRect->fRight;

	// Make sure that we clip to the actual screen size
	if (top<0) top = 0;
	if (top>=theScreenHeight) top = theScreenHeight-1;
	if (bottom<0) bottom = 0;
	if (bottom>=theScreenHeight) bottom = theScreenHeight-1;
	if (top>bottom) return;
	if (left<0) left = 0;
	if (left>=theScreenWidth) left = theScreenWidth-1;
	if (right<0) right = 0;
	if (right>=theScreenWidth) right = theScreenWidth-1;
	if (left>right) return;

	// Ok, now here is our clipped rectangle.
	KUInt16 height = bottom - top;
	KUInt16 width = right - left;

	// A single byte holds two pixels. Make sure that the horizontal coordinates
	// are on even addresses so that we can access full byte.
	if (left & 0x1) // the start position must be on an even address
	{
		left -= 1;
		width += 1;
	}
	if (width & 0x1) // and the value for width must also be even
	{
		width += 1;
	}

	KUInt8* srcRowPtr =
		theScreenBuffer
		+ (top * srcRowBytes)
		+ (left * kBitsPerPixel / 8);
	KUInt8* dstRowPtr =
		((KUInt8*) mImageBuffer)
		+ (top * dstRowBytes)
		+ (left * sizeof(KUInt32));

	// Use a lookup table for quick conversion from nibbles to words.
	static const KUInt32 grayLUT[] = {
		0x000000, 0x111111, 0x222222, 0x333333, 0x444444, 0x555555, 0x666666, 0x777777,
		0x888888, 0x999999, 0xaaaaaa, 0xbbbbbb, 0xcccccc, 0xdddddd, 0xeeeeee, 0xffffff };
	// A second table to simulate the backlight.
	static const KUInt32 greenLUT[] = {
		0x000000, 0x041104, 0x082208, 0x0c330c, 0x104410, 0x145514, 0x186618, 0x1c771c,
		0x208820, 0x249924, 0x28aa28, 0x2cbb2c, 0x40cc40, 0x44dd44, 0x58ee58, 0x4cff4c };
	KUInt32 const *lut = GetBacklight() ? greenLUT : grayLUT;

	int indexRows;
	for (indexRows = height; indexRows != 0; indexRows--)
	{
		KUInt8* srcCursor = srcRowPtr;
		KUInt8* srcEnd = srcRowPtr + width/2;
		KUInt32* dstCursor = (KUInt32*) dstRowPtr;
		do {
			KUInt8 theByte = *srcCursor++;
			*dstCursor++ = lut[theByte>>4];
			*dstCursor++ = lut[theByte&0x0f];
		} while (srcCursor < srcEnd);
		srcRowPtr += srcRowBytes;
		dstRowPtr += dstRowBytes;
	}

	// Update the overlay plane
	// TODO: we can save some time if we do this only for an overlapping area
	if (mOverlayIsOn) {
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
				// TODO: to be perfect, we need to add changed areas to the update rectangle
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
	
	// Tell macOS to add this rectangle to the area that needs a refresh.
	// Rectangle in macOS coordinates.
	TCocoaScreenView_SetNeedsDisplayInRect(mProxy, mEmulatorScreenView,
										   CGRectMake(left,
													  theScreenHeight-top-height,
													  width, height));
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
