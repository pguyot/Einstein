// ==============================
// File:            PMainBatteryDriverPrims.s
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

    NATIVE_PRIM 0x3, 0x01, _ZN18PMainBatteryDriver3NewEv
    NATIVE_PRIM 0x3, 0x02, _ZN18PMainBatteryDriver6DeleteEv
    NATIVE_PRIM 0x3, 0x03, Init__18PMainBatteryDriverFv
    NATIVE_PRIM 0x3, 0x04, WakeUp__18PMainBatteryDriverFv
    NATIVE_PRIM 0x3, 0x05, ShutDown__18PMainBatteryDriverFv
    NATIVE_PRIM 0x3, 0x06, Count__18PMainBatteryDriverFv
    NATIVE_PRIM 0x3, 0x07, Status__18PMainBatteryDriverFUlP16PowerPlantStatus
    NATIVE_PRIM 0x3, 0x08, RawStatus__18PMainBatteryDriverFUlP16PowerPlantStatus
    NATIVE_PRIM 0x3, 0x09, StartSleepCharge__18PMainBatteryDriverFv
    NATIVE_PRIM 0x3, 0x0A, SetType__18PMainBatteryDriverFUlT1
    NATIVE_PRIM 0x3, 0x0B, ReadADCVoltage__18PMainBatteryDriverFUl
    NATIVE_PRIM 0x3, 0x0C, ConvertVoltage__18PMainBatteryDriverFUlT1

// =========================================================================== //
// THEGODDESSOFTHENETHASTWISTINGFINGERSANDHERVOICEISLIKEAJAVELININTHENIGHTDUDE //
// =========================================================================== //
