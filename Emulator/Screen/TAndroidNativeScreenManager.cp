// ==============================
// File:			TAndroidNativeScreenManager.cp
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
#include "TAndroidNativeScreenManager.h"
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
//  * TAndroidNativeScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TAndroidNativeScreenManager::TAndroidNativeScreenManager(
        TLog* inLog /* = nil */,
        KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
        KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
        Boolean inFullScreen /* = true */,
        Boolean inScreenIsLandscape /* = true */)
        :
        TScreenManager(
                inLog,
                inPortraitWidth,
                inPortraitHeight,
                inFullScreen,
                inScreenIsLandscape )
{
    if (GetLog()) GetLog()->FLogLine("********** TAndroidNativeScreenManager **********");
    // Not sure yet how we solve this.
    // The view is create in Java.
    // Here we can lock the associated RAM and copy everything that changed over.
    // We will also need a way to tell the Java host that we need a redraw!
}

// -------------------------------------------------------------------------- //
//  * ~TX11ScreenManager( void )
// -------------------------------------------------------------------------- //
TAndroidNativeScreenManager::~TAndroidNativeScreenManager( void )
{
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::PowerOn( void )
{
    if (GetLog()) GetLog()->FLogLine("********** TAndroidNativeScreenManager Power **********");
    // This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::PowerOff( void )
{
    // This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::PowerOnScreen( void )
{
    if (GetLog()) GetLog()->FLogLine("Power on Screen\n");
}


// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::PowerOffScreen( void )
{
    if (GetLog()) GetLog()->FLogLine("Power off Screen\n");
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( Boolean )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::BacklightChanged( Boolean )
{
    if (GetLog()) GetLog()->FLogLine("UpdateScreenRect(0L)");
    changed = 1;
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::ContrastChanged( KUInt32 )
{
    // Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::ScreenOrientationChanged( EOrientation inNewOrientation )
{
    if (GetLog()) GetLog()->FLogLine("New orientation %d is %dx%d\n", inNewOrientation, GetScreenWidth(), GetScreenHeight());
    changed = 1;
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TAndroidNativeScreenManager::TabletOrientationChanged( EOrientation )
{
    // Just ignore it.
}


int TAndroidNativeScreenManager::update(unsigned short *buffer)
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
    int i, j, wdt = (int)GetScreenWidth(), hgt = (int)GetScreenHeight();

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


int TAndroidNativeScreenManager::updateOverlay(unsigned short *buffer)
{
    // RGB16 = [FEDCB.A98765.43210]
    //   red =  11111 011111 01111 = 0xFBEF
    // Update the overlay plane
    // TODO: we can save some time if we do this only for an overlapping area
    if (mOverlayIsOn)
    {
        mOverlayRect.fLeft = (KUInt16)(GetScreenWidth()/2 - 20*8);
        mOverlayRect.fRight = (KUInt16)(mOverlayRect.fLeft+40*8);
        mOverlayRect.fTop = (KUInt16)(GetScreenHeight() - 16*5);
        mOverlayRect.fBottom = (KUInt16)(mOverlayRect.fTop + 16*4);

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
                        char c = (char)(mOverlay[line][cx] & 0x7f);
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
TAndroidNativeScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{
    changed = 1;
    // Just tell AndroidNativeActivity that the screen changed
    // TODO: add support for bounding rectangles, so that we have to copy less screen content around
}


// ========================================================================= //
// The most likely way for the world to be destroyed, most experts agree, is
// by accident. That's where we come in; we're computer professionals. We
// cause accidents.
// ========================================================================= //
