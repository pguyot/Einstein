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
//  * TAndroidScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TAndroidScreenManager::TAndroidScreenManager(
											 TLog* inLog /* = nil */,
											 KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
											 KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
											 bool inFullScreen /* = false */,
											 bool inScreenIsLandscape /* = true */)
:
TScreenManager(
			   inLog,
			   inPortraitWidth,
			   inPortraitHeight,
			   inFullScreen,
			   inScreenIsLandscape )
{
	if (GetLog()) GetLog()->FLogLine("********** TAndroidScreenManager **********");
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
	if (GetLog()) GetLog()->FLogLine("********** TAndroidScreenManager Power **********");
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
	if (GetLog()) GetLog()->FLogLine("Power on Screen\n");
}


// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::PowerOffScreen( void )
{
	if (GetLog()) GetLog()->FLogLine("Power off Screen\n");
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( bool )
// -------------------------------------------------------------------------- //
void
TAndroidScreenManager::BacklightChanged( bool )
{
	if (GetLog()) GetLog()->FLogLine("UpdateScreenRect(0L)");
	changed = 1;
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
	if (GetLog()) GetLog()->FLogLine("New orientation %d is %dx%d\n", inNewOrientation, GetScreenWidth(), GetScreenHeight());
	changed = 1;
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
	static unsigned short lut_wt[16] = {
		ROT(0), ROT(1), ROT(2), ROT(3), 
		ROT(4), ROT(5), ROT(6), ROT(7), 
		ROT(8), ROT(9), ROT(10), ROT(11), 
		ROT(12), ROT(13), ROT(14), ROT(15), 
	};
#undef ROT
#define ROT(a) (a<<11) | (a<<7) | (a<<0)
	static unsigned short lut_gn[16] = {
		ROT(0), ROT(1), ROT(2), ROT(3), 
		ROT(4), ROT(5), ROT(6), ROT(7), 
		ROT(8), ROT(9), ROT(10), ROT(11), 
		ROT(12), ROT(13), ROT(14), ROT(15), 
	};
	if (!changed) 
		return 0;
	unsigned short *lut = GetBacklight() ? lut_gn : lut_wt;
	KUInt8* src = GetScreenBuffer();
	unsigned short *dst = buffer;
	int i, j, wdt = GetScreenWidth(), hgt = GetScreenHeight();
	
	switch (GetScreenOrientation()) {
		case kOrientation_AppleRight:
			for (i=0; i<hgt; i++) {
				dst = buffer + hgt*(wdt-1) + i;
				for (j=wdt; j>0; j-=2) {
					KUInt8 theByte = *src++;
					*dst = lut[theByte>>4];   dst -= hgt;
					*dst = lut[theByte&0x0F]; dst -= hgt;
				}
			}
			break;
		case kOrientation_AppleLeft:
			for (i=hgt; i>0; i--) {
				dst = buffer + i - 1;
				for (j=wdt; j>0; j-=2) {
					KUInt8 theByte = *src++;
					*dst = lut[theByte>>4];   dst += hgt;
					*dst = lut[theByte&0x0F]; dst += hgt;
				}
			}
			break;
		case kOrientation_AppleTop:
			dst = buffer + wdt*hgt - 1;
			for (i=hgt; i>0; i--) {
				for (j=wdt; j>0; j-=2) {
					KUInt8 theByte = *src++;
					*dst-- = lut[theByte>>4];
					*dst-- = lut[theByte&0x0F];
				}
			}
			break;
		case kOrientation_AppleBottom:
			dst = buffer;
			for (i=hgt; i>0; i--) {
				for (j=wdt; j>0; j-=2) {
					KUInt8 theByte = *src++;
					*dst++ = lut[theByte>>4];
					*dst++ = lut[theByte&0x0F];
				}
			}
			break;
	}
	
	updateOverlay(buffer);
	changed = 0;
	return 1;
}


int TAndroidScreenManager::updateOverlay(unsigned short *buffer)
{
	// RGB16 = [FEDCB.A98765.43210]
	//   red =  11111 011111 01111 = 0xFBEF
	// Update the overlay plane
	// TODO: we can save some time if we do this only for an overlapping area
	if (mOverlayIsOn)
	{
		// TODO: this calculation should only be done during screen sizing and orientation changes
		mOverlayRect.fLeft = GetScreenWidth()/2 - 20*8;
		mOverlayRect.fRight = mOverlayRect.fLeft+40*8;
		mOverlayRect.fTop = GetScreenHeight() - 16*5;
		mOverlayRect.fBottom = mOverlayRect.fTop + 16*4;
		
		KUInt32 dstRowBytes = GetScreenWidth() * 2;
		KUInt8* dstRowPtr =
		((KUInt8*)buffer)
		+ (mOverlayRect.fTop * dstRowBytes)
		+ (mOverlayRect.fLeft * sizeof(KUInt16))
		;
		
		KUInt32 line;
		for (line=0; line<4; line++) {
			if (mOverlayIsDirty[line]) {
				KUInt32 row ;
				for (row=0; row<13; row++) {
					KUInt32 cx;
					KUInt16 *dstCursor = ((KUInt16*)(dstRowPtr + (line*16 + row)*dstRowBytes));
					for (cx=0; cx<40; cx++) {
						char c = mOverlay[line][cx] & 0x7f;
						if (c!=0 && c!=32) {
							KUInt32 x;
							KUInt16 pattern = mFontData[c][row];
							for (x=8; x>0; x--) {
								if (pattern&0x180) {
									*dstCursor++ = 0xFBEF;
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
		return 1;
	}
	return 0;
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
