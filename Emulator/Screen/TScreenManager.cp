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

#include <K/Defines/KDefinitions.h>
#include "TScreenManager.h"

// ANSI C
#include <stdlib.h>

// K
#include <K/Streams/TStream.h>
#include <K/Defines/UByteSex.h>

// Einstein
#include "Emulator/Log/TLog.h"
#include "TInterruptManager.h"
#include "TMemory.h"
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
			Boolean inFullScreen /* = false */,
			Boolean inScreenIsLandscape /* = true */)
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
		mScreenBuffer( NULL )
{
	mTabletBuffer = (KUInt32*) ::malloc( sizeof(KUInt32) * kTabletBufferSize );
	mScreenBuffer = 0L;
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
	
	mScreenBuffer = (KUInt8*)
	::malloc(
			 inPortraitWidth *
			 inPortraitHeight *
			 kBitsPerPixel / 8);
	
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
Boolean
TScreenManager::GetSample(
					KUInt32* outPackedSample,
					KUInt32* outTimeInTicks )
{
	Boolean theResult = false;
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
		KUInt32 theTimeInTicks = inTimeInTicks;
		if (theTimeInTicks == 0)
		{
			theTimeInTicks = mInterruptManager->GetTimer();
		}
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
	inStream->TransferByte( mFullScreen );
	inStream->TransferByte( mScreenIsLandscape );
	inStream->TransferByte( mBypassTablet );
	inStream->TransferByte( mTabletIsDown );
	inStream->TransferByte( mPenIsDown );
	inStream->TransferInt32BE( mTabletSampleRate );
	t = mTabletOrientation; inStream->TransferInt32BE( t ); mTabletOrientation = (EOrientation)t;
	t = mScreenOrientation; inStream->TransferInt32BE( t ); mScreenOrientation = (EOrientation)t;
	inStream->TransferInt32BE( mContrast );
	inStream->TransferByte( mBacklight );
	inStream->TransferByte( mKbdIsConnected );
	
	KUInt32 count = mPortraitWidth * mPortraitHeight * kBitsPerPixel / 8;
	inStream->Transfer(mScreenBuffer, &count);
	
	if (inStream->IsReading())
		PowerOnScreen();
}


// ======================================================================= //
// "Our attitude with TCP/IP is, `Hey, we'll do it, but don't make a big   //
// system, because we can't fix it if it breaks -- nobody can.'"           //
//                                                                         //
// "TCP/IP is OK if you've got a little informal club, and it doesn't make //
// any difference if it takes a while to fix it."                          //
//                 -- Ken Olson, in Digital News, 1988                     //
// ======================================================================= //
