// ==============================
// File:			TAndroidScreenManager.cp
// Project:			Einstein
//
// Copyright 2011 by Matthias Melcher (einstein@matthiasm.com).
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
#include "TAndroidScreenManager.h"
#include "app/AndroidGlue.h"

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Threads/TThread.h>

// Einstein
#include "Emulator/Log/TLog.h"


// -------------------------------------------------------------------------- //
//  * GetDisplaySize( void )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::GetScreenSize(
									 KUInt32* outWidth,
									 KUInt32* outHeight)
{
	*outWidth = 320;
	*outHeight = 480;
}


// -------------------------------------------------------------------------- //
//  * TAndroidScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TAndroidScreenManager::TAndroidScreenManager(
											 TLog* inLog /* = nil */,
											 KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
											 KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
											 Boolean inFullScreen /* = false */,
											 Boolean inScreenIsLandscape /* = true */)
:
TScreenManager(
			   inLog,
			   inPortraitWidth,
			   inPortraitHeight,
			   inFullScreen,
			   inScreenIsLandscape )
{
	LOGE("********** TAndroidScreenManager **********");
	// Not sure yet how we solve this. 
	// The view is create in Java.
	// Here we can lock the associated RAM and copy everything that changed over.
	// We will also need a way to tell the Java host that we need a redraw!
}

// -------------------------------------------------------------------------- //
//  * ~TX11ScreenManager( void )
// -------------------------------------------------------------------------- //
TAndroidScreenManager::~TAndroidScreenManager( void )
{
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::PowerOn( void )
{
	LOGE("********** TAndroidScreenManager Power **********");
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::PowerOff( void )
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::PowerOnScreen( void )
{
	LOGI("Power on Screen\n");
}


// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::PowerOffScreen( void )
{
	LOGI("Power off Screen\n");
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( Boolean )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::BacklightChanged( Boolean )
{
	LOGW("UpdateScreenRect(0L)");
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::ContrastChanged( KUInt32 )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::ScreenOrientationChanged( EOrientation inNewOrientation )
{
	LOGW("New orientation %d is %dx%d\n", inNewOrientation, GetScreenWidth(), GetScreenHeight());
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::TabletOrientationChanged( EOrientation )
{
	// Just ignore it.
}


int TAndroidScreenManager::update(unsigned short *buffer)
{
#define ROT(a) (a<<12) | (a<<7) | (a<<1)
	static unsigned short lut[16] = {
		ROT(0), ROT(1), ROT(2), ROT(3), 
		ROT(4), ROT(5), ROT(6), ROT(7), 
		ROT(8), ROT(9), ROT(10), ROT(11), 
		ROT(12), ROT(13), ROT(14), ROT(15), 
	};
	if (!changed) 
		return 0;
	KUInt8* src = GetScreenBuffer();
	unsigned short *dst = buffer;
	int i;
	
	for (i=320*480; i>0; i-=2) {
		KUInt8 theByte = *src++;
#if 0
		KUInt8 thePixel = (theByte & 0xF0) | (theByte>>4);
		*dst++ = lut[thePixel&0x0F];
		thePixel = (theByte<<4) | (theByte & 0x0f);
		*dst++ = lut[thePixel&0x0F];
#else
		*dst++ = lut[theByte>>4];
		*dst++ = lut[theByte&0x0F];
#endif
	}
	changed = 0;
	return 1;
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{

	changed = 1;
#if 0
	static bool firstTime = true;
	if (firstTime) {
		mWidget->label("");
		firstTime = false;
	}
	int mBitsPerPixel = 24;
	
	KUInt16 top, left, height, width;
	if (inUpdateRect) {
		top = inUpdateRect->fTop;
		left = inUpdateRect->fLeft;
		height = inUpdateRect->fBottom - top;
		width = inUpdateRect->fRight - left;
	} else {
		top = 0; 
		left = 0;
		height = GetScreenHeight();
		width = GetScreenWidth();
	}
	
	KUInt8 rs, gs, bs; 
	if (GetBacklight()) {
		rs = 1; gs = 0; bs = 1;
	} else {
		rs = 0; gs = 0; bs = 0;
	}
	
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
	KUInt32 dstRowBytes = theScreenWidth * mBitsPerPixel / 8;
	KUInt32 srcRowBytes = theScreenWidth * kBitsPerPixel / 8;
	KUInt32 srcWidthInBytes = width * kBitsPerPixel / 8;
	
	KUInt8* srcRowPtr =
	theScreenBuffer
	+ (top * srcRowBytes)
	+ (left * kBitsPerPixel / 8);
	KUInt8* dstRowPtr =
	((KUInt8*)mWidget->getRGBData())
	+ (top * dstRowBytes)
	+ (left * mBitsPerPixel / 8);
	
	int indexRows;
	
	for (indexRows = height; indexRows != 0; indexRows--)
	{
		KUInt8* srcCursor = srcRowPtr;
		KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
		KUInt8* dstCursor = dstRowPtr;
		do {
			KUInt8 theByte = *srcCursor++;
			// First pixel
			KUInt8 thePixel = (theByte & 0xF0) | (theByte>>4);
			*dstCursor++ = thePixel>>rs;
			*dstCursor++ = thePixel>>gs;
			*dstCursor++ = thePixel>>bs;
			// Second pixel
			thePixel = (theByte<<4) | (theByte & 0x0f);
			*dstCursor++ = thePixel>>rs;
			*dstCursor++ = thePixel>>gs;
			*dstCursor++ = thePixel>>bs;
		} while (srcCursor < srcEnd);
		srcRowPtr += srcRowBytes;
		dstRowPtr += dstRowBytes;
	}
	
	mWidget->redraw();
#endif
}


// ========================================================================= //
// The most likely way for the world to be destroyed, most experts agree, is 
// by accident. That's where we come in; we're computer professionals. We 
// cause accidents.
// ========================================================================= //
