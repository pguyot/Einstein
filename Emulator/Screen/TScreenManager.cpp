// ==============================
// File:			TScreenManager.cp
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

//
// The original Newton MP2x00 is an LCD screen with 480x320 resolution, 16
// shades of gray, and a backlight. The resistive touchscreen is glued to the
// front of the display. The viewing area of the display is 129mm x 87mm.
// The screen diagonal is about 6.1 inches.
//
// Markings on the display are 4C112522AH52, LCDFAT650MC, an M650-LOA, 93 11 18
// LM48019K STN480X C, 97H00928 a, 5.4"
//
// The display data is sent via a AWM 2896 80C VW 1 flatband cable (20pin).
// Backlight and touchscreen use additional wires.
//
// ??? Sharp LM5H40TA LQ057V3DG02, similar (same?) pinout
// SHARP 5.5 inch display panel recommendation:
// LS055R1SX04 LS055R1SX03 LS055T1SX01 LS055R3SX01 LQ055K3SX02 LQ055T3SX02 LS055R1SC01 LQ055T3SX03 LQ055T3SX02Z LM32C041 LS055T3SX05 LS055D1SX02 LS055R3SX01 (G) LM6Q32 LS055R1SX08 LS055T1SX01A LS055D1SX05(G) LM6Q401 LM32C04P LM6Q40A LQ055W1GC01 LS055T1SX10 LQ055T3SX02Z(G) LS055D1SX04 LS055T3SX13 LS055T3SX09 LQ055T3SX05 LS055T1SX13 LQ055W1GC01Z LM5H40TA LM5H40TB LM6Q31 LM6Q33 LM6Q35 LM6Q40 LQ055T1SX05 LQ055T3SX02(G) LS055T3SX01(G) LS055T1SX09
//

#include "TScreenManager.h"

// ANSI C
#include <stdlib.h>

// K
#include <K/Streams/TStream.h>
#include <K/Defines/UByteSex.h>

// Einstein
#include "Emulator/Log/TLog.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "Emulator/Platform/TPlatformManager.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// 256*2 samples should really be enough.
#define	kTabletBufferSize		512

#define kPenDownSample			0x0000000D
#define kPenUpSample			0x0000000E
#define kInvalidSample			0x0000000F

/// ABCDEFGH -> HGFEDCBA
inline KUInt32 Swap_Pixels_FromBigEndian( const KUInt32 inWord )
{
	// ABCDEFGH -> GHEFCDAB
#if TARGET_RT_LITTLE_ENDIAN
	return (inWord >> 4 & 0x0F0F0F0F)
		| (inWord << 4 & 0xF0F0F0F0);
#else
	KUInt32 Swapped1 = UByteSex::Swap(inWord);
	return (Swapped1 >> 4 & 0x0F0F0F0F)
		| (Swapped1 << 4 & 0xF0F0F0F0);
#endif
}

// -------------------------------------------------------------------------- //
//  * TScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TScreenManager::TScreenManager(
			TLog* inLog /* = nil */,
			KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
			KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
			bool inFullScreen /* = false */,
			bool inScreenIsLandscape /* = true */)
	:
		mLog( inLog ),
		mInterruptManager( nil ),
		mMemory( nil ),
		mPortraitWidth( inPortraitWidth ),
		mPortraitHeight( inPortraitHeight ),
		mPhysicalWidth( 0 ),
		mPhysicalHeight( 0 ),
		mFullScreen( inFullScreen ),
		mScreenIsLandscape( inScreenIsLandscape ),
		mBypassTablet( false ),
		mTabletIsDown( false ),
		mPenIsDown( false ),
		mTabletSampleRate( kDefaultSampleRate ),
		mTabletBuffer( NULL ),
		mTabletBufCCrsr( 0 ),
		mTabletBufPCrsr( 0 ),
		mTabletOrientation( kOrientation_Default ),
		mScreenOrientation( kOrientation_Default ),
		mContrast( kDefaultContrast ),
		mBacklight( false ),
		mKbdIsConnected( true ),
		mScreenBuffer( NULL ),
		mOverlayIsOn( false )
{
	mTabletBuffer = (KUInt32*) ::calloc( 1, sizeof(KUInt32) * kTabletBufferSize );
	mScreenBuffer = NULL;
	memset(mOverlayIsDirty, 0, sizeof(mOverlayIsDirty));
	memset(mOverlay, 0, sizeof(mOverlay));
	ChangeScreenSize(inPortraitWidth, inPortraitHeight);
}


// -------------------------------------------------------------------------- //
//  * ChangeScreenSize( int, int )
//  Calling this requires a reboot.
// -------------------------------------------------------------------------- //
void TScreenManager::ChangeScreenSize(int inPortraitWidth, int inPortraitHeight)
{
	mPortraitWidth = inPortraitWidth;
	mPortraitHeight = inPortraitHeight;
	
	if (mScreenBuffer)
		::free(mScreenBuffer);
	
	mScreenBuffer = (KUInt8*)::calloc(1, inPortraitWidth * inPortraitHeight * kBitsPerPixel / 8);
	
	if (mFullScreen)
	{
		if (mScreenIsLandscape)
		{
			if (inPortraitWidth > inPortraitHeight)
			{
				mPhysicalWidth = inPortraitWidth;
				mPhysicalHeight = inPortraitHeight;
			} else {
				mPhysicalWidth = inPortraitHeight;
				mPhysicalHeight = inPortraitWidth;
			}
		} else {
			if (inPortraitWidth > inPortraitHeight)
			{
				mPhysicalWidth = inPortraitHeight;
				mPhysicalHeight = inPortraitWidth;
			} else {
				mPhysicalWidth = inPortraitWidth;
				mPhysicalHeight = inPortraitHeight;
			}
		}
	}
	// FIXME: calculate the actual overlay dimensions
	mOverlayRect.fTop = 0;
	mOverlayRect.fLeft = 0;
	mOverlayRect.fRight = static_cast<KUInt16>(GetScreenWidth());
	mOverlayRect.fBottom = static_cast<KUInt16>(GetScreenHeight());
}

// -------------------------------------------------------------------------- //
//  * ~TScreenManager( void )
// -------------------------------------------------------------------------- //
TScreenManager::~TScreenManager( void )
{
	if (mTabletBuffer)
	{
		::free( mTabletBuffer );
	}
	if (mScreenBuffer)
	{
		::free( mScreenBuffer );
	}
}

// -------------------------------------------------------------------------- //
//  * Blit( KUInt32, SRect*, SRect* )
// -------------------------------------------------------------------------- //
void
TScreenManager::Blit(
					KUInt32 inPixelMapAddr,
					SRect* inSrcRect,
					SRect* inDstRect,
					KUInt32 inMode )
{    
	// Update the local screen buffer.
	KUInt32 baseAddy;
	(void) mMemory->Read( inPixelMapAddr, baseAddy );
	KUInt32 srcRowBytes;
	(void) mMemory->Read( inPixelMapAddr + 4, srcRowBytes );
	srcRowBytes = srcRowBytes >> 16;
	KUInt32 pixmapTopLeft;
	(void) mMemory->Read( inPixelMapAddr + 8, pixmapTopLeft );

	KUInt16 pixmapTop = (KUInt16) (pixmapTopLeft >> 16);
	KUInt16 pixmapLeft = (KUInt16) (pixmapTopLeft & 0xFFFF);

	// Call the proper function depending on the orientation.
	if (mFullScreen)
	{
		switch (mScreenOrientation)
		{
			case kOrientation_AppleTop:
				Blit_270(
					baseAddy,
					srcRowBytes,
					pixmapTop,
					pixmapLeft,
					inSrcRect,
					inDstRect,
					inMode);
			break;
			case kOrientation_AppleLeft:
			case kOrientation_AppleRight:
				Blit_0(
					baseAddy,
					srcRowBytes,
					pixmapTop,
					pixmapLeft,
					inSrcRect,
					inDstRect,
					inMode);
			break;
			case kOrientation_AppleBottom:
				Blit_90(
					baseAddy,
					srcRowBytes,
					pixmapTop,
					pixmapLeft,
					inSrcRect,
					inDstRect,
					inMode);
			break;
				/* FIXME: Matt sais: Blit_180 seems broken.
			case kOrientation_AppleLeft:
				Blit_180(
					baseAddy,
					srcRowBytes,
					pixmapTop,
					pixmapLeft,
					inSrcRect,
					inDstRect,
					inMode);
			break;
				 */
		}
	} else {
		Blit_0(
			baseAddy,
			srcRowBytes,
			pixmapTop,
			pixmapLeft,
			inSrcRect,
			inDstRect,
			inMode);
	}
}

// -------------------------------------------------------------------------- //
//  * Blit_0( KUInt32, SRect*, SRect* )
// -------------------------------------------------------------------------- //
inline void
TScreenManager::Blit_0(
					KUInt32 inBaseAddy,
					KUInt32 inSrcRowBytes,
					KUInt16 inPixmapTop,
					KUInt16 inPixmapLeft,
					SRect* inSrcRect,
					SRect* inDstRect,
					KUInt32 inMode )
{
	// We copy more pixels than what we should.
	KUInt16 srcLeft = inSrcRect->fLeft - inPixmapLeft;
	KUInt16 dstLeft = inDstRect->fLeft;

	KUInt16 additionalLeftPixels = srcLeft & 0x07;
	dstLeft -= additionalLeftPixels;
	srcLeft -= additionalLeftPixels;
	KUInt32 leftMask = (KUInt32) (0xFFFFFFFF >> (additionalLeftPixels * 4));
	KUInt16 srcRight = inSrcRect->fRight - inPixmapLeft;
	KUInt16 additionalRightPixels = srcRight & 0x07;
	KUInt32 rightMask;
	if (additionalRightPixels)
	{
		additionalRightPixels = 8 - additionalRightPixels;
	}
	rightMask = (KUInt32) (0xFFFFFFFF << (additionalRightPixels * 4));
	srcRight += additionalRightPixels;
	
	KUInt32 dstRowBytes = GetScreenWidth() * kBitsPerPixel / 8;

	KUInt16 srcTop = inSrcRect->fTop - inPixmapTop;
	KUInt16 srcBottom = inSrcRect->fBottom - inPixmapTop;
	KUInt32 nbRows = srcBottom - srcTop;
	KUInt32 indexRows;
	KUInt32 srcRowAddy =
		inBaseAddy
		+ (srcTop * inSrcRowBytes)
		+ (srcLeft * kBitsPerPixel / 8);
	KUInt8* dstPixelsRow =
		mScreenBuffer
		+ (inDstRect->fTop * dstRowBytes)
		+ (dstLeft * kBitsPerPixel / 8);
	KUInt32 srcWidthInBytes =
		(srcRight - srcLeft)
		* kBitsPerPixel / 8;
	KUInt32 chunk;
	
	if (srcWidthInBytes <= 4)
	{
		// Single column
		KUInt32 theMask = leftMask & rightMask;
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			(void) mMemory->Read( srcRowAddy, chunk );
			KUInt32 originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & ~theMask)
					| ((~chunk) & theMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord) | (chunk & theMask));
			}
			*rowPixels = UByteSex_ToBigEndian( originalWord );

			srcRowAddy += inSrcRowBytes;
			dstPixelsRow += dstRowBytes;
		}
	} else {	
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			// First column.
			(void) mMemory->Read( srcRowAddy, chunk );
			KUInt32 originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~leftMask))
					| ((~chunk) & leftMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord)
					| (chunk & leftMask));
			}
			*rowPixels++ = UByteSex_ToBigEndian( originalWord );

			// Middle columns.			
			KUInt32 srcCursor = srcRowAddy + 4;
			KUInt32 lastPixAddy = srcRowAddy + srcWidthInBytes - 4;
			while (srcCursor < lastPixAddy) {
				(void) mMemory->Read( srcCursor, chunk );				
				srcCursor += 4;
				if (inMode == 0)
				{
					// srcCopy
					*rowPixels++ = UByteSex_ToBigEndian( ~chunk );
				} else {
					originalWord = UByteSex_FromBigEndian( *rowPixels );
					*rowPixels++ = UByteSex_ToBigEndian( ~(~originalWord | chunk) );
				}
			}
			
			// Last column.
			(void) mMemory->Read( srcCursor, chunk );
			originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~rightMask))
					| ((~chunk) & rightMask);
			} else /* if (inMode == 1) */ {
				originalWord =
					~((~originalWord)
						| (chunk & rightMask));
			}
			*rowPixels = UByteSex_ToBigEndian( originalWord );
			
			srcRowAddy += inSrcRowBytes;
			dstPixelsRow += dstRowBytes;
		}
	}
	
	// Tell the implementation that the screen was updated.
	UpdateScreenRect( inDstRect );
}

// -------------------------------------------------------------------------- //
//  * Blit_90( KUInt32, SRect*, SRect* )
// -------------------------------------------------------------------------- //
inline void
TScreenManager::Blit_90(
					KUInt32 inBaseAddy,
					KUInt32 inSrcRowBytes,
					KUInt16 inPixmapTop,
					KUInt16 inPixmapLeft,
					SRect* inSrcRect,
					SRect* inDstRect,
					KUInt32 inMode )
{
	// We copy more pixels than what we should.
	KUInt16 srcLeft = inSrcRect->fLeft - inPixmapLeft;
	KUInt16 dstLeft = inDstRect->fLeft;

	KUInt16 additionalLeftPixels = srcLeft & 0x07;
	dstLeft -= additionalLeftPixels;
	srcLeft -= additionalLeftPixels;
	KUInt32 leftMask = (KUInt32) (0xFFFFFFFF >> (additionalLeftPixels * 4));
	KUInt16 srcRight = inSrcRect->fRight - inPixmapLeft;
	KUInt16 additionalRightPixels = srcRight & 0x07;
	KUInt32 rightMask;
	if (additionalRightPixels)
	{
		additionalRightPixels = 8 - additionalRightPixels;
	}
	rightMask = (KUInt32) (0xFFFFFFFF << (additionalRightPixels * 4));
	srcRight += additionalRightPixels;
	
	KUInt32 dstRowBytes = mPhysicalWidth * kBitsPerPixel / 8;

	KUInt16 srcTop = inSrcRect->fTop - inPixmapTop;
	KUInt16 srcBottom = inSrcRect->fBottom - inPixmapTop;
	KUInt32 nbRows = srcBottom - srcTop;
	KUInt32 indexRows;
	KUInt32 srcRowAddy =
		inBaseAddy
		+ (srcTop * inSrcRowBytes)
		+ (srcLeft * kBitsPerPixel / 8);
	KUInt8* dstPixelsRow =
		mScreenBuffer
		+ (inDstRect->fTop * dstRowBytes)
		+ (dstLeft * kBitsPerPixel / 8);
	KUInt32 srcWidthInBytes =
		(srcRight - srcLeft)
		* kBitsPerPixel / 8;
	KUInt32 chunk;
	
	if (srcWidthInBytes <= 4)
	{
		// Single column
		KUInt32 theMask = leftMask & rightMask;
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			(void) mMemory->Read( srcRowAddy, chunk );
			KUInt32 originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & ~theMask)
					| ((~chunk) & theMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord) | (chunk & theMask));
			}
			*rowPixels = UByteSex_ToBigEndian( originalWord );

			srcRowAddy += inSrcRowBytes;
			dstPixelsRow += dstRowBytes;
		}
	} else {	
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			// First column.
			(void) mMemory->Read( srcRowAddy, chunk );
			KUInt32 originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~leftMask))
					| ((~chunk) & leftMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord)
					| (chunk & leftMask));
			}
			*rowPixels++ = UByteSex_ToBigEndian( originalWord );

			// Middle columns.			
			KUInt32 srcCursor = srcRowAddy + 4;
			KUInt32 lastPixAddy = srcRowAddy + srcWidthInBytes - 4;
			while (srcCursor < lastPixAddy) {
				(void) mMemory->Read( srcCursor, chunk );				
				srcCursor += 4;
				if (inMode == 0)
				{
					// srcCopy
					*rowPixels++ = UByteSex_ToBigEndian( ~chunk );
				} else {
					originalWord = UByteSex_FromBigEndian( *rowPixels );
					*rowPixels++ = UByteSex_ToBigEndian( ~(~originalWord | chunk) );
				}
			}
			
			// Last column.
			(void) mMemory->Read( srcCursor, chunk );
			originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~rightMask))
					| ((~chunk) & rightMask);
			} else /* if (inMode == 1) */ {
				originalWord =
					~((~originalWord)
						| (chunk & rightMask));
			}
			*rowPixels = UByteSex_ToBigEndian( originalWord );
			
			srcRowAddy += inSrcRowBytes;
			dstPixelsRow += dstRowBytes;
		}
	}
	
	// Tell the implementation that the screen was updated.
	UpdateScreenRect( inDstRect );
}

// -------------------------------------------------------------------------- //
//  * Blit_180( KUInt32, SRect*, SRect* )
// FIXME: this function is quite different to Blit_0. Blit_0 is working for me.
// -------------------------------------------------------------------------- //
inline void
TScreenManager::Blit_180(
					KUInt32 inBaseAddy,
					KUInt32 inSrcRowBytes,
					KUInt16 inPixmapTop,
					KUInt16 inPixmapLeft,
					SRect* inSrcRect,
					SRect* inDstRect,
					KUInt32 inMode )
{
	// We copy more pixels than what we should.
	KUInt16 srcLeft = inSrcRect->fLeft - inPixmapLeft;
	KUInt16 dstLeft = inDstRect->fLeft;

	KUInt16 additionalLeftPixels = srcLeft & 0x07;
	dstLeft -= additionalLeftPixels;
	srcLeft -= additionalLeftPixels;
	KUInt32 leftMask = (KUInt32) (0xFFFFFFFF >> (additionalLeftPixels * 4));
	KUInt16 srcRight = inSrcRect->fRight - inPixmapLeft;
	KUInt16 additionalRightPixels = srcRight & 0x07;
	KUInt32 rightMask;
	if (additionalRightPixels)
	{
		additionalRightPixels = 8 - additionalRightPixels;
	}
	rightMask = (KUInt32) (0xFFFFFFFF << (additionalRightPixels * 4));
	srcRight += additionalRightPixels;
	
	KUInt32 dstRowBytes = mPhysicalWidth * kBitsPerPixel / 8;

	KUInt16 srcTop = inSrcRect->fTop - inPixmapTop;
	KUInt16 srcBottom = inSrcRect->fBottom - inPixmapTop;
	KUInt32 nbRows = srcBottom - srcTop;
	KUInt32 indexRows;
	KUInt32 srcRowAddy =
		inBaseAddy
		+ (srcTop * inSrcRowBytes)
		+ (srcLeft * kBitsPerPixel / 8);
	KUInt8* dstPixelsRow =
		mScreenBuffer
		+ ((mPhysicalHeight - inDstRect->fTop - 1) * dstRowBytes)
		+ ((mPhysicalWidth - dstLeft) * kBitsPerPixel / 8);
	KUInt32 srcWidthInBytes =
		(srcRight - srcLeft)
		* kBitsPerPixel / 8;
	KUInt32 chunk;

	if (srcWidthInBytes <= 4)
	{
		// Single column
		KUInt32 theMask = leftMask & rightMask;
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			(void) mMemory->Read( srcRowAddy, chunk );
			// Swap the word.
			--rowPixels;
			KUInt32 originalWord = Swap_Pixels_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & ~theMask)
					| ((~chunk) & theMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord) | (chunk & theMask));
			}
			// Swap the word again.
			*rowPixels = Swap_Pixels_FromBigEndian( originalWord );

			srcRowAddy += inSrcRowBytes;
			dstPixelsRow -= dstRowBytes;
		}
	} else {	
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			// First column.
			(void) mMemory->Read( srcRowAddy, chunk );
			// Swap the word.
			--rowPixels;
			KUInt32 originalWord = Swap_Pixels_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~leftMask))
					| ((~chunk) & leftMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord)
					| (chunk & leftMask));
			}
			// Swap the word again.
			*rowPixels-- = Swap_Pixels_FromBigEndian( originalWord );

			// Middle columns.			
			KUInt32 srcCursor = srcRowAddy + 4;
			KUInt32 lastPixAddy = srcRowAddy + srcWidthInBytes - 4;
			while (srcCursor < lastPixAddy) {
				(void) mMemory->Read( srcCursor, chunk );				
				srcCursor += 4;
				if (inMode == 0)
				{
					// srcCopy
					*rowPixels-- = Swap_Pixels_FromBigEndian( ~chunk );
				} else {
					originalWord = Swap_Pixels_FromBigEndian( *rowPixels );
					*rowPixels-- = Swap_Pixels_FromBigEndian( ~(~originalWord | chunk) );
				}
			}
			
			// Last column.
			(void) mMemory->Read( srcCursor, chunk );
			// Swap.
			originalWord = Swap_Pixels_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~rightMask))
					| ((~chunk) & rightMask);
			} else /* if (inMode == 1) */ {
				originalWord =
					~((~originalWord)
						| (chunk & rightMask));
			}
			// Swap again.
			*rowPixels = Swap_Pixels_FromBigEndian( originalWord );
			
			srcRowAddy += inSrcRowBytes;
			dstPixelsRow -= dstRowBytes;
		}
	}
	
	// Tell the implementation that the screen was updated.
	SRect theDstRect;
	theDstRect.fTop = mPhysicalHeight - inDstRect->fBottom;
	theDstRect.fBottom = mPhysicalHeight - inDstRect->fTop;
	theDstRect.fLeft = mPhysicalWidth - inDstRect->fRight;
	theDstRect.fRight = mPhysicalWidth - inDstRect->fLeft;
	UpdateScreenRect( &theDstRect );
}

// -------------------------------------------------------------------------- //
//  * Blit_270( KUInt32, SRect*, SRect* )
// -------------------------------------------------------------------------- //
inline void
TScreenManager::Blit_270(
					KUInt32 inBaseAddy,
					KUInt32 inSrcRowBytes,
					KUInt16 inPixmapTop,
					KUInt16 inPixmapLeft,
					SRect* inSrcRect,
					SRect* inDstRect,
					KUInt32 inMode )
{
	// We copy more pixels than what we should.
	KUInt16 srcLeft = inSrcRect->fLeft - inPixmapLeft;
	KUInt16 dstLeft = inDstRect->fLeft;

	KUInt16 additionalLeftPixels = srcLeft & 0x07;
	dstLeft -= additionalLeftPixels;
	srcLeft -= additionalLeftPixels;
	KUInt32 leftMask = (KUInt32) (0xFFFFFFFF >> (additionalLeftPixels * 4));
	KUInt16 srcRight = inSrcRect->fRight - inPixmapLeft;
	KUInt16 additionalRightPixels = srcRight & 0x07;
	KUInt32 rightMask;
	if (additionalRightPixels)
	{
		additionalRightPixels = 8 - additionalRightPixels;
	}
	rightMask = (KUInt32) (0xFFFFFFFF << (additionalRightPixels * 4));
	srcRight += additionalRightPixels;
	
	KUInt32 dstRowBytes = mPhysicalWidth * kBitsPerPixel / 8;

	KUInt16 srcTop = inSrcRect->fTop - inPixmapTop;
	KUInt16 srcBottom = inSrcRect->fBottom - inPixmapTop;
	KUInt32 nbRows = srcBottom - srcTop;
	KUInt32 indexRows;
	KUInt32 srcRowAddy =
		inBaseAddy
		+ (srcTop * inSrcRowBytes)
		+ (srcLeft * kBitsPerPixel / 8);
	KUInt8* dstPixelsRow =
		mScreenBuffer
		+ (inDstRect->fTop * dstRowBytes)
		+ (dstLeft * kBitsPerPixel / 8);
	KUInt32 srcWidthInBytes =
		(srcRight - srcLeft)
		* kBitsPerPixel / 8;
	KUInt32 chunk;
	
	if (srcWidthInBytes <= 4)
	{
		// Single column
		KUInt32 theMask = leftMask & rightMask;
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			(void) mMemory->Read( srcRowAddy, chunk );
			KUInt32 originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & ~theMask)
					| ((~chunk) & theMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord) | (chunk & theMask));
			}
			*rowPixels = UByteSex_ToBigEndian( originalWord );

			srcRowAddy += inSrcRowBytes;
			dstPixelsRow += dstRowBytes;
		}
	} else {	
		for (indexRows = nbRows; indexRows != 0; indexRows--)
		{
			KUInt32* rowPixels = (KUInt32*) dstPixelsRow;
			
			// First column.
			(void) mMemory->Read( srcRowAddy, chunk );
			KUInt32 originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~leftMask))
					| ((~chunk) & leftMask);
			} else /* if (inMode == 1) */ {
				originalWord = ~((~originalWord)
					| (chunk & leftMask));
			}
			*rowPixels++ = UByteSex_ToBigEndian( originalWord );

			// Middle columns.			
			KUInt32 srcCursor = srcRowAddy + 4;
			KUInt32 lastPixAddy = srcRowAddy + srcWidthInBytes - 4;
			while (srcCursor < lastPixAddy) {
				(void) mMemory->Read( srcCursor, chunk );				
				srcCursor += 4;
				if (inMode == 0)
				{
					// srcCopy
					*rowPixels++ = UByteSex_ToBigEndian( ~chunk );
				} else {
					originalWord = UByteSex_FromBigEndian( *rowPixels );
					*rowPixels++ = UByteSex_ToBigEndian( ~(~originalWord | chunk) );
				}
			}
			
			// Last column.
			(void) mMemory->Read( srcCursor, chunk );
			originalWord = UByteSex_FromBigEndian( *rowPixels );
			if (inMode == 0)
			{
				// srcCopy
				originalWord = (originalWord & (~rightMask))
					| ((~chunk) & rightMask);
			} else /* if (inMode == 1) */ {
				originalWord =
					~((~originalWord)
						| (chunk & rightMask));
			}
			*rowPixels = UByteSex_ToBigEndian( originalWord );
			
			srcRowAddy += inSrcRowBytes;
			dstPixelsRow += dstRowBytes;
		}
	}
	
	// Tell the implementation that the screen was updated.
	UpdateScreenRect( inDstRect );
}

// -------------------------------------------------------------------------- //
//  * WakeUpTablet( void )
// -------------------------------------------------------------------------- //
void
TScreenManager::WakeUpTablet( void )
{
	mBypassTablet = false;
	mTabletIsDown = false;
}

// -------------------------------------------------------------------------- //
//  * ShutDownTablet( void )
// -------------------------------------------------------------------------- //
void
TScreenManager::ShutDownTablet( void )
{
	mBypassTablet = true;
	mTabletIsDown = true;
}

// -------------------------------------------------------------------------- //
//  * StartBypassTablet( void )
// -------------------------------------------------------------------------- //
void
TScreenManager::StartBypassTablet( void )
{
	mBypassTablet = true;
}

// -------------------------------------------------------------------------- //
//  * StopBypassTablet( void )
// -------------------------------------------------------------------------- //
void
TScreenManager::StopBypassTablet( void )
{
	mBypassTablet = false;

	// Reset pen is down.
	mPenIsDown = false;
}

// -------------------------------------------------------------------------- //
//  * GetTabletState( void ) const
// -------------------------------------------------------------------------- //
TScreenManager::ETabletState
TScreenManager::GetTabletState( void ) const
{
	ETabletState theState;
	if (mTabletIsDown)
	{
		theState = kTabletIsOff;
	} else if (mBypassTablet) {
		theState = kTabletIsBypassed;
	} else if (mPenIsDown) {
		theState = kPenIsDown;
	} else {
		theState = kPenIsUp;
	}
	
	return theState;
}

// -------------------------------------------------------------------------- //
//  * PenDown( KUInt16, KUInt16, KUInt8, KUInt32 )
// -------------------------------------------------------------------------- //
void
TScreenManager::PenDown(
				KUInt16 inXCoord,
				KUInt16 inYCoord,
				KUInt8 inPressure /* = 4 */,
				KUInt32 inTimeInTicks /* = 0 */ )
{
	KUInt16 theXCoord = inXCoord;
	KUInt16 theYCoord = inYCoord;

	if (mFullScreen)
	{
		switch (mTabletOrientation)
		{
			case kOrientation_AppleTop:
				theXCoord = mPhysicalWidth - inYCoord - 1;
				theYCoord = inXCoord;
			break;
			case kOrientation_AppleRight:
//				theXCoord = inXCoord;
//				theYCoord = inYCoord;
			break;
			case kOrientation_AppleBottom:
				theXCoord = inYCoord;
				theYCoord = mPhysicalHeight - inXCoord - 1;
			break;
			case kOrientation_AppleLeft:
				theXCoord = mPhysicalHeight - inXCoord - 1;
				theYCoord = mPhysicalWidth - inYCoord - 1;
			break;
		}
	}

	// Limite: 2048x2048.
	// Il faut modifier Convert__6TInkerFv pour augmenter.
	if (!mPenIsDown)
	{
		InsertSample( kPenDownSample, inTimeInTicks );
		mPenIsDown = true;
	}
	KUInt32 theSampleRecord =
		(theXCoord & 0x7FF) << 21
		| (theYCoord & 0x7FF) << 7
		| (inPressure & 0x0F);
	InsertSample( theSampleRecord, inTimeInTicks );
}

// -------------------------------------------------------------------------- //
//  * PenUp( KUInt32 )
// -------------------------------------------------------------------------- //
void
TScreenManager::PenUp( KUInt32 inTimeInTicks /* = 0 */ )
{
	if (mPenIsDown)
	{
		InsertSample( kPenUpSample, inTimeInTicks );
		mPenIsDown = false;
	}
}

// -------------------------------------------------------------------------- //
//  * GetSample( KUInt32*, KUInt32* )
// -------------------------------------------------------------------------- //
bool
TScreenManager::GetSample(
					KUInt32* outPackedSample,
					KUInt32* outTimeInTicks )
{
	bool theResult = false;
	KUInt32 newCCrsr = mTabletBufCCrsr;
	if (newCCrsr != mTabletBufPCrsr)
	{
		theResult = true;
		*outPackedSample = mTabletBuffer[newCCrsr++];
		*outTimeInTicks = mTabletBuffer[newCCrsr++];
		if (newCCrsr == kTabletBufferSize)
		{
			newCCrsr = 0;
		}
		mTabletBufCCrsr = newCCrsr;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * InsertSample( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TScreenManager::InsertSample(
					KUInt32 inPackedSample,
					KUInt32 inTimeInTicks /* = 0 */ )
{
	if (!mBypassTablet)
	{
//		KUInt32 theTimeInTicks = inTimeInTicks;
//		if (theTimeInTicks == 0)
//		{
//			theTimeInTicks = mInterruptManager->GetTimer();
//		}
		
//		mPlatformManager->SendTabletSampleEvent( inPackedSample, theTimeInTicks );
		
		KUInt32 newPCrsr = mTabletBufPCrsr;
		mTabletBuffer[newPCrsr++] = inPackedSample;
		mTabletBuffer[newPCrsr++] = inTimeInTicks;
		if (newPCrsr == kTabletBufferSize)
		{
			newPCrsr = 0;
		}
		mTabletBufPCrsr = newPCrsr;
		
		// Tell the driver.
		RaiseTabletInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * RaiseTabletInterrupt( void ) const
// -------------------------------------------------------------------------- //
void
TScreenManager::RaiseTabletInterrupt( void ) const
{
	mInterruptManager->RaiseInterrupt( TInterruptManager::kTabletIntMask );
}

// -------------------------------------------------------------------------- //
//  * KeyDown( KUInt8 )
// -------------------------------------------------------------------------- //
void
TScreenManager::KeyDown( KUInt8 inKeyCode )
{
	mPlatformManager->SendKeyEvent( kKeyDownEventType, inKeyCode );
}

// -------------------------------------------------------------------------- //
//  * KeyUp( KUInt8 )
// -------------------------------------------------------------------------- //
void
TScreenManager::KeyUp( KUInt8 inKeyCode )
{
	mPlatformManager->SendKeyEvent( kKeyUpEventType, inKeyCode );
}

// -------------------------------------------------------------------------- //
//  * KeyRepeat( KUInt8 )
// -------------------------------------------------------------------------- //
void
TScreenManager::KeyRepeat( KUInt8 inKeyCode )
{
	mPlatformManager->SendKeyEvent( kKeyRepeatEventType, inKeyCode );
}


// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TScreenManager::TransferState( TStream* inStream )
{
	KUInt32 t;
	
	inStream->TransferInt32BE( mPortraitWidth );
	inStream->TransferInt32BE( mPortraitHeight );
	inStream->TransferInt32BE( mPhysicalWidth );
	inStream->TransferInt32BE( mPhysicalHeight );
	inStream->TransferBoolean( mFullScreen );
	inStream->TransferBoolean( mScreenIsLandscape );
	inStream->TransferBoolean( mBypassTablet );
	inStream->TransferBoolean( mTabletIsDown );
	inStream->TransferBoolean( mPenIsDown );
	inStream->TransferInt32BE( mTabletSampleRate );
	t = mTabletOrientation; inStream->TransferInt32BE( t ); mTabletOrientation = (EOrientation)t;
	t = mScreenOrientation; inStream->TransferInt32BE( t ); mScreenOrientation = (EOrientation)t;
	inStream->TransferInt32BE( mContrast );
	inStream->TransferBoolean( mBacklight );
	inStream->TransferBoolean( mKbdIsConnected );
	
	KUInt32 count = mPortraitWidth * mPortraitHeight * kBitsPerPixel / 8;
	inStream->Transfer(mScreenBuffer, &count);
	
	if (inStream->IsReading())
		PowerOnScreen();
}


// -------------------------------------------------------------------------- //
//  * OverlayOn()
// -------------------------------------------------------------------------- //
void
TScreenManager::OverlayOn()
{
	mOverlayIsOn = true;
	PowerOn();
	PowerOnScreen();
	OverlayFlush();
}


// -------------------------------------------------------------------------- //
//  * OverlayOff()
// -------------------------------------------------------------------------- //
void
TScreenManager::OverlayOff()
{
	mOverlayIsOn = false;
	OverlayFlush();
}


// -------------------------------------------------------------------------- //
//  * OverlayFlush()
// -------------------------------------------------------------------------- //
void
TScreenManager::OverlayFlush()
{
	UpdateScreenRect( &mOverlayRect );
}


// -------------------------------------------------------------------------- //
//  * OverlayClear()
// -------------------------------------------------------------------------- //
void
TScreenManager::OverlayClear(KSInt32 line)
{
	if (line==-1) {
		memset(mOverlayIsDirty, 0, sizeof(mOverlayIsDirty));
		memset(mOverlay, 32, sizeof(mOverlay));
	} else if (line>=0 && line<=3) {
		if (mOverlayIsDirty[line]) {
			mOverlayIsDirty[line] = 0;
			memset(mOverlay[line], 32, sizeof(mOverlay[line]));
		}
	}
}


// -------------------------------------------------------------------------- //
//  * OverlayPrintAt( int, int, const char*, char )
//  The screen overlay area ia small text screen (40x4 characters) that is
//  rendered on top of the Einstein screen. It is used to show boot progress
//  or inform the user of error conditions.
// -------------------------------------------------------------------------- //
void
TScreenManager::OverlayPrintAt(KSInt32 x, KSInt32 y, const char *text, bool centered)
{
	size_t len = strlen(text);
	if (centered) {
		if (len>40) len = 40;
		ssize_t i, margin = (40-len)/2;
		char *dst =  mOverlay[y];
		const char *src =  text;
		for (i=margin; i>0; i--) *dst++ = 32;
		for (i=len; i>0; i--) *dst++ = *src++;
		for (i=40-len-margin; i>0; i--) *dst++ = 32;
	} else {
		if (len+x>40) len = 40-x;
		memcpy(mOverlay[y]+x, text, len);
	}
	mOverlayIsDirty[y] = 1;
}


// -------------------------------------------------------------------------- //
//  * OverlayPrintProgress( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TScreenManager::OverlayPrintProgress(KSInt32 y, KUInt32 percent)
{
	if (percent>100) percent = 100;
	sprintf(mOverlay[y], "%3d%%", (unsigned int)percent);
	int i, n = percent*34/100;
	char *d = mOverlay[y]+4;
	*d++ = 1;
	for (i=n; i>0; i--) *d++=3;
	for (i=34-n; i>0; i--) *d++=2;
	*d++ = 4;
	mOverlayIsDirty[y] = 1;
}


KUInt8 TScreenManager::mFontData[128][13] = {
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x01,0x03,0x06,0x0c,0x0c,0x0c,0x0c,0x0c,0x06,0x03,0x01,0x00}, // (    Progress bar elements
	{ 0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00}, //  =
	{ 0x00,0xff,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0xff,0xff,0x00}, //   #
	{ 0x00,0x80,0xc0,0x60,0x30,0x30,0x30,0x30,0x30,0x60,0xc0,0x80,0x00}, //    )
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, //
	{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // ' '
	{ 0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x10,0x00,0x00,0x00}, // '!'
	{ 0x00,0x24,0x24,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '"'
	{ 0x00,0x00,0x24,0x24,0x7e,0x24,0x7e,0x24,0x24,0x00,0x00,0x00,0x00}, // '#'
	{ 0x00,0x10,0x3c,0x50,0x50,0x38,0x14,0x14,0x78,0x10,0x00,0x00,0x00}, // '$'
	{ 0x00,0x22,0x52,0x24,0x08,0x08,0x10,0x24,0x2a,0x44,0x00,0x00,0x00}, // '%'
	{ 0x00,0x00,0x00,0x30,0x48,0x48,0x30,0x4a,0x44,0x3a,0x00,0x00,0x00}, // '&'
	{ 0x00,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '''
	{ 0x00,0x04,0x08,0x08,0x10,0x10,0x10,0x08,0x08,0x04,0x00,0x00,0x00}, // '('
	{ 0x00,0x20,0x10,0x10,0x08,0x08,0x08,0x10,0x10,0x20,0x00,0x00,0x00}, // ')'
	{ 0x00,0x00,0x00,0x24,0x18,0x7e,0x18,0x24,0x00,0x00,0x00,0x00,0x00}, // '*'
	{ 0x00,0x00,0x00,0x10,0x10,0x7c,0x10,0x10,0x00,0x00,0x00,0x00,0x00}, // '+'
	{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x30,0x40,0x00,0x00}, // ','
	{ 0x00,0x00,0x00,0x00,0x00,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '-'
	{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x38,0x10,0x00,0x00}, // '.'
	{ 0x00,0x02,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x80,0x00,0x00,0x00}, // '/'
	{ 0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00,0x00,0x00}, // '0'
	{ 0x00,0x10,0x30,0x50,0x10,0x10,0x10,0x10,0x10,0x7c,0x00,0x00,0x00}, // '1'
	{ 0x00,0x3c,0x42,0x42,0x02,0x04,0x18,0x20,0x40,0x7e,0x00,0x00,0x00}, // '2'
	{ 0x00,0x7e,0x02,0x04,0x08,0x1c,0x02,0x02,0x42,0x3c,0x00,0x00,0x00}, // '3'
	{ 0x00,0x04,0x0c,0x14,0x24,0x44,0x44,0x7e,0x04,0x04,0x00,0x00,0x00}, // '4'
	{ 0x00,0x7e,0x40,0x40,0x5c,0x62,0x02,0x02,0x42,0x3c,0x00,0x00,0x00}, // '5'
	{ 0x00,0x1c,0x20,0x40,0x40,0x5c,0x62,0x42,0x42,0x3c,0x00,0x00,0x00}, // '6'
	{ 0x00,0x7e,0x02,0x04,0x08,0x08,0x10,0x10,0x20,0x20,0x00,0x00,0x00}, // '7'
	{ 0x00,0x3c,0x42,0x42,0x42,0x3c,0x42,0x42,0x42,0x3c,0x00,0x00,0x00}, // '8'
	{ 0x00,0x3c,0x42,0x42,0x46,0x3a,0x02,0x02,0x04,0x38,0x00,0x00,0x00}, // '9'
	{ 0x00,0x00,0x00,0x10,0x38,0x10,0x00,0x00,0x10,0x38,0x10,0x00,0x00}, // ':'
	{ 0x00,0x00,0x00,0x10,0x38,0x10,0x00,0x00,0x38,0x30,0x40,0x00,0x00}, // ';'
	{ 0x00,0x04,0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x04,0x00,0x00,0x00}, // '<'
	{ 0x00,0x00,0x00,0x00,0x7f,0x00,0x7f,0x00,0x00,0x00,0x00,0x00,0x00}, // '='
	{ 0x00,0x40,0x20,0x10,0x08,0x04,0x08,0x10,0x20,0x40,0x00,0x00,0x00}, // '>'
	{ 0x00,0x3c,0x42,0x42,0x02,0x04,0x08,0x08,0x00,0x08,0x00,0x00,0x00}, // '?'
	{ 0x00,0x3c,0x42,0x42,0x4e,0x52,0x56,0x4a,0x40,0x3c,0x00,0x00,0x00}, // '@'
	{ 0x00,0x18,0x24,0x42,0x42,0x42,0x7e,0x42,0x42,0x42,0x00,0x00,0x00}, // 'A'
	{ 0x00,0x78,0x44,0x42,0x44,0x78,0x44,0x42,0x44,0x78,0x00,0x00,0x00}, // 'B'
	{ 0x00,0x3c,0x42,0x40,0x40,0x40,0x40,0x40,0x42,0x3c,0x00,0x00,0x00}, // 'C'
	{ 0x00,0x78,0x44,0x42,0x42,0x42,0x42,0x42,0x44,0x78,0x00,0x00,0x00}, // 'D'
	{ 0x00,0x7e,0x40,0x40,0x40,0x78,0x40,0x40,0x40,0x7e,0x00,0x00,0x00}, // 'E'
	{ 0x00,0x7e,0x40,0x40,0x40,0x78,0x40,0x40,0x40,0x40,0x00,0x00,0x00}, // 'F'
	{ 0x00,0x3c,0x42,0x40,0x40,0x40,0x4e,0x42,0x46,0x3a,0x00,0x00,0x00}, // 'G'
	{ 0x00,0x42,0x42,0x42,0x42,0x7e,0x42,0x42,0x42,0x42,0x00,0x00,0x00}, // 'H'
	{ 0x00,0x7c,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7c,0x00,0x00,0x00}, // 'I'
	{ 0x00,0x1e,0x04,0x04,0x04,0x04,0x04,0x04,0x44,0x38,0x00,0x00,0x00}, // 'J'
	{ 0x00,0x42,0x44,0x48,0x50,0x60,0x50,0x48,0x44,0x42,0x00,0x00,0x00}, // 'K'
	{ 0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7e,0x00,0x00,0x00}, // 'L'
	{ 0x00,0x82,0x82,0xc6,0xaa,0x92,0x92,0x82,0x82,0x82,0x00,0x00,0x00}, // 'M'
	{ 0x00,0x42,0x42,0x62,0x52,0x4a,0x46,0x42,0x42,0x42,0x00,0x00,0x00}, // 'N'
	{ 0x00,0x3c,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00,0x00,0x00}, // 'O'
	{ 0x00,0x7c,0x42,0x42,0x42,0x7c,0x40,0x40,0x40,0x40,0x00,0x00,0x00}, // 'P'
	{ 0x00,0x3c,0x42,0x42,0x42,0x42,0x42,0x52,0x4a,0x3c,0x02,0x00,0x00}, // 'Q'
	{ 0x00,0x7c,0x42,0x42,0x42,0x7c,0x50,0x48,0x44,0x42,0x00,0x00,0x00}, // 'R'
	{ 0x00,0x3c,0x42,0x40,0x40,0x3c,0x02,0x02,0x42,0x3c,0x00,0x00,0x00}, // 'S'
	{ 0x00,0xfe,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x00}, // 'T'
	{ 0x00,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00,0x00,0x00}, // 'U'
	{ 0x00,0x82,0x82,0x44,0x44,0x44,0x28,0x28,0x28,0x10,0x00,0x00,0x00}, // 'V'
	{ 0x00,0x82,0x82,0x82,0x82,0x92,0x92,0x92,0xaa,0x44,0x00,0x00,0x00}, // 'W'
	{ 0x00,0x82,0x82,0x44,0x28,0x10,0x28,0x44,0x82,0x82,0x00,0x00,0x00}, // 'X'
	{ 0x00,0x82,0x82,0x44,0x28,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x00}, // 'Y'
	{ 0x00,0x7e,0x02,0x04,0x08,0x10,0x20,0x40,0x40,0x7e,0x00,0x00,0x00}, // 'Z'
	{ 0x00,0x3c,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x3c,0x00,0x00,0x00}, // '['
	{ 0x00,0x80,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x02,0x00,0x00,0x00}, // '\'
	{ 0x00,0x78,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x78,0x00,0x00,0x00}, // ']'
	{ 0x00,0x10,0x28,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '^'
	{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00}, // '_'
	{ 0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '`'
	{ 0x00,0x00,0x00,0x00,0x3c,0x02,0x3e,0x42,0x46,0x3a,0x00,0x00,0x00}, // 'a'
	{ 0x00,0x40,0x40,0x40,0x5c,0x62,0x42,0x42,0x62,0x5c,0x00,0x00,0x00}, // 'b'
	{ 0x00,0x00,0x00,0x00,0x3c,0x42,0x40,0x40,0x42,0x3c,0x00,0x00,0x00}, // 'c'
	{ 0x00,0x02,0x02,0x02,0x3a,0x46,0x42,0x42,0x46,0x3a,0x00,0x00,0x00}, // 'd'
	{ 0x00,0x00,0x00,0x00,0x3c,0x42,0x7e,0x40,0x42,0x3c,0x00,0x00,0x00}, // 'e'
	{ 0x00,0x1c,0x22,0x20,0x20,0x7c,0x20,0x20,0x20,0x20,0x00,0x00,0x00}, // 'f'
	{ 0x00,0x00,0x00,0x00,0x3a,0x44,0x44,0x38,0x40,0x3c,0x42,0x3c,0x00}, // 'g'
	{ 0x00,0x40,0x40,0x40,0x5c,0x62,0x42,0x42,0x42,0x42,0x00,0x00,0x00}, // 'h'
	{ 0x00,0x00,0x10,0x00,0x30,0x10,0x10,0x10,0x10,0x7c,0x00,0x00,0x00}, // 'i'
	{ 0x00,0x00,0x04,0x00,0x0c,0x04,0x04,0x04,0x04,0x44,0x44,0x38,0x00}, // 'j'
	{ 0x00,0x40,0x40,0x40,0x44,0x48,0x70,0x48,0x44,0x42,0x00,0x00,0x00}, // 'k'
	{ 0x00,0x30,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7c,0x00,0x00,0x00}, // 'l'
	{ 0x00,0x00,0x00,0x00,0xec,0x92,0x92,0x92,0x92,0x82,0x00,0x00,0x00}, // 'm'
	{ 0x00,0x00,0x00,0x00,0x5c,0x62,0x42,0x42,0x42,0x42,0x00,0x00,0x00}, // 'n'
	{ 0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x42,0x3c,0x00,0x00,0x00}, // 'o'
	{ 0x00,0x00,0x00,0x00,0x5c,0x62,0x42,0x62,0x5c,0x40,0x40,0x40,0x00}, // 'p'
	{ 0x00,0x00,0x00,0x00,0x3a,0x46,0x42,0x46,0x3a,0x02,0x02,0x02,0x00}, // 'q'
	{ 0x00,0x00,0x00,0x00,0x5c,0x22,0x20,0x20,0x20,0x20,0x00,0x00,0x00}, // 'r'
	{ 0x00,0x00,0x00,0x00,0x3c,0x42,0x30,0x0c,0x42,0x3c,0x00,0x00,0x00}, // 's'
	{ 0x00,0x00,0x20,0x20,0x7c,0x20,0x20,0x20,0x22,0x1c,0x00,0x00,0x00}, // 't'
	{ 0x00,0x00,0x00,0x00,0x44,0x44,0x44,0x44,0x44,0x3a,0x00,0x00,0x00}, // 'u'
	{ 0x00,0x00,0x00,0x00,0x44,0x44,0x44,0x28,0x28,0x10,0x00,0x00,0x00}, // 'v'
	{ 0x00,0x00,0x00,0x00,0x82,0x82,0x92,0x92,0xaa,0x44,0x00,0x00,0x00}, // 'w'
	{ 0x00,0x00,0x00,0x00,0x42,0x24,0x18,0x18,0x24,0x42,0x00,0x00,0x00}, // 'x'
	{ 0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x46,0x3a,0x02,0x42,0x3c,0x00}, // 'y'
	{ 0x00,0x00,0x00,0x00,0x7e,0x04,0x08,0x10,0x20,0x7e,0x00,0x00,0x00}, // 'z'
	{ 0x00,0x0e,0x10,0x10,0x08,0x30,0x08,0x10,0x10,0x0e,0x00,0x00,0x00}, // '{'
	{ 0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x00}, // '|'
	{ 0x00,0x70,0x08,0x08,0x10,0x0c,0x10,0x08,0x08,0x70,0x00,0x00,0x00}, // '}'
	{ 0x00,0x24,0x54,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '~'
	{ 0x00,0x7F,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00}, // DEL
};



// ======================================================================= //
// "Our attitude with TCP/IP is, `Hey, we'll do it, but don't make a big   //
// system, because we can't fix it if it breaks -- nobody can.'"           //
//                                                                         //
// "TCP/IP is OK if you've got a little informal club, and it doesn't make //
// any difference if it takes a while to fix it."                          //
//                 -- Ken Olson, in Digital News, 1988                     //
// ======================================================================= //
