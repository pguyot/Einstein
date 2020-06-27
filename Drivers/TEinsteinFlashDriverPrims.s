// ==============================
// File:            TEinsteinFlashDriverPrims.s
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

    NATIVE_PRIM 0x0, 0x01, Identify__20TEinsteinFlashDriverFUlT1R21SFlashChipInformation
    NATIVE_PRIM 0x0, 0x02, CleanUp__20TEinsteinFlashDriverFR16TMemoryAllocator
    NATIVE_PRIM 0x0, 0x03, Init__20TEinsteinFlashDriverFR16TMemoryAllocator
    NATIVE_PRIM 0x0, 0x04, InitializeDriverData__20TEinsteinFlashDriverFR11TFlashRangeR16TMemoryAllocator
    NATIVE_PRIM 0x0, 0x05, CleanUpDriverData__20TEinsteinFlashDriverFR11TFlashRangeR16TMemoryAllocator
    NATIVE_PRIM 0x0, 0x06, StartReadingArray__20TEinsteinFlashDriverFR11TFlashRange
    NATIVE_PRIM 0x0, 0x07, DoneReadingArray__20TEinsteinFlashDriverFR11TFlashRange
    NATIVE_PRIM 0x0, 0x08, Write__20TEinsteinFlashDriverFUlN21R11TFlashRange
//  NATIVE_PRIM 0x0, 0x09, StartErase__20TEinsteinFlashDriverFR11TFlashRangeUl
    NATIVE_PRIM 0x0, 0x0A, ResetBlockStatus__20TEinsteinFlashDriverFR11TFlashRangeUl
    NATIVE_PRIM 0x0, 0x0B, IsEraseComplete__20TEinsteinFlashDriverFR11TFlashRangeUlRl
    NATIVE_PRIM 0x0, 0x0C, LockBlock__20TEinsteinFlashDriverFR11TFlashRangeUl
    NATIVE_PRIM 0x0, 0x0D, BeginWrite__20TEinsteinFlashDriverFR11TFlashRangeUlT2
    NATIVE_PRIM 0x0, 0x0E, ReportWriteResult__20TEinsteinFlashDriverFR11TFlashRangeUl

//  NATIVE_PRIM 0x0, 0x0F, DoWrite__20TEinsteinFlashDriverFUlN31
    NATIVE_PRIM 0x0, 0x10, _ZN20TEinsteinFlashDriver7DoEraseEmm

// =========================================================================== //
// THEGODDESSOFTHENETHASTWISTINGFINGERSANDHERVOICEISLIKEAJAVELININTHENIGHTDUDE //
// =========================================================================== //
