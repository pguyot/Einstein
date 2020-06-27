// ==============================
// File:            TMainDisplayDriverPrims.s
// Project:         Einstein
//
// Copyright 2003-2013, 2020 by Paul Guyot (pguyot@kallisys.net).
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

    .arch armv3
    .balign 4

    .include "NativePrimitives.s"

    NATIVE_PRIM 0x4, 0x01, Delete__18TMainDisplayDriverFv
//  NATIVE_PRIM 0x4, 0x02, ScreenSetup__18TMainDisplayDriverFv
    NATIVE_PRIM 0x4, 0x03, GetScreenInfo__18TMainDisplayDriverFP10ScreenInfo
    NATIVE_PRIM 0x4, 0x04, PowerInit__18TMainDisplayDriverFv
    NATIVE_PRIM 0x4, 0x05, PowerOn__18TMainDisplayDriverFv
    NATIVE_PRIM 0x4, 0x06, PowerOff__18TMainDisplayDriverFv
    NATIVE_PRIM 0x4, 0x07, Blit__18TMainDisplayDriverFP8PixelMapP4RectT2l
    NATIVE_PRIM 0x4, 0x08, GetFeature__18TMainDisplayDriverFl
    NATIVE_PRIM 0x4, 0x09, SetFeature__18TMainDisplayDriverFlT1
    NATIVE_PRIM 0x4, 0x0A, AutoAdjustFeatures__18TMainDisplayDriverFv
    NATIVE_PRIM 0x4, 0x0B, DoubleBlit__18TMainDisplayDriverFP8PixelMapT1P4RectT3l
    NATIVE_PRIM 0x4, 0x0C, EnterIdleMode__18TMainDisplayDriverFv
    NATIVE_PRIM 0x4, 0x0D, ExitIdleMode__18TMainDisplayDriverFv

// =========================================================================== //
// THEGODDESSOFTHENETHASTWISTINGFINGERSANDHERVOICEISLIKEAJAVELININTHENIGHTDUDE //
// =========================================================================== //
