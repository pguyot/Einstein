// ==============================
// File:            TMainTabletDriverPrims.s
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

//  NATIVE_PRIM 0x5, 0x01, New__17TMainTabletDriverFv
//  NATIVE_PRIM 0x5, 0x02, Delete__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x03, Init__17TMainTabletDriverFRC4Rect
    NATIVE_PRIM 0x5, 0x04, WakeUp__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x05, ShutDown__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x06, TabletIdle__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x07, GetSampleRate__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x08, SetSampleRate__17TMainTabletDriverFUl
    NATIVE_PRIM 0x5, 0x09, GetTabletCalibration__17TMainTabletDriverFP11Calibration
    NATIVE_PRIM 0x5, 0x0A, SetTabletCalibration__17TMainTabletDriverFRC11Calibration
    NATIVE_PRIM 0x5, 0x0B, SetDoingCalibration__17TMainTabletDriverFUcPUl
    NATIVE_PRIM 0x5, 0x0C, GetTabletResolution__17TMainTabletDriverFPlT1
    NATIVE_PRIM 0x5, 0x0D, TabSetOrientation__17TMainTabletDriverFl
    NATIVE_PRIM 0x5, 0x0E, GetTabletState__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x0F, GetFingerInputState__17TMainTabletDriverFPUc
    NATIVE_PRIM 0x5, 0x10, SetFingerInputState__17TMainTabletDriverFUc
    NATIVE_PRIM 0x5, 0x11, RecalibrateTabletAfterRotate__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x12, TabletNeedsRecalibration__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x13, StartBypassTablet__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x14, StopBypassTablet__17TMainTabletDriverFv
    NATIVE_PRIM 0x5, 0x15, ReturnTabletToConsciousness__17TMainTabletDriverFUlN21

    NATIVE_PRIM 0x5, 0x16, _ZN17TMainTabletDriver15NativeGetSampleEPmS0_

// =========================================================================== //
// THEGODDESSOFTHENETHASTWISTINGFINGERSANDHERVOICEISLIKEAJAVELININTHENIGHTDUDE //
// =========================================================================== //
