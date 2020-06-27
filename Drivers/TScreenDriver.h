// ==============================
// File:            TScreenDriver.h
// Project:         Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TSCREENDRIVER_H
#define _TSCREENDRIVER_H

#include <Newton.h>
#include <Protocols.h>
#include <NewtQD.h>

struct ScreenInfo {
    ULong   fScreenHeight;  // 00000140 (320) height
    ULong   fScreenWidth;   // 000001E0 (480) width
    ULong   fBitsPerPixel;  // 00000004 (depth?)
    ULong   fUnknown_0C;    // 00000037 55 (?)
    UShort  fResolutionX;   // 00640064 100/100 (bpi?)
    UShort  fResolutionY;   //
    ULong   fUnknown_14;    // 00000020 32 (?)
    ULong   fUnknown_18;    // 00000020 32 (?)
};

///
/// Protocol for the screen driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
PROTOCOL TScreenDriver : public TProtocol
{
public:
    void    Delete();
    ULong   ScreenSetup();
    ULong   GetScreenInfo(ScreenInfo*);
    ULong   PowerInit();
    ULong   PowerOn();
    ULong   PowerOff();
    ULong   Blit(PixelMap*, Rect*, Rect*, long);
    long    GetFeature(long);
    void    SetFeature(long, long);
    ULong   AutoAdjustFeatures();
    ULong   DoubleBlit(PixelMap*, PixelMap*, Rect*, Rect*, long);
    ULong   EnterIdleMode();
    ULong   ExitIdleMode();
};

#endif
        // _TSCREENDRIVER_H

// =========================================================================== //
// Some programming languages manage to absorb change, but withstand progress. //
//                 -- Epigrams in Programming, ACM SIGPLAN Sept. 1982          //
// =========================================================================== //
