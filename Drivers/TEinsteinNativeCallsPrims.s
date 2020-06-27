// ==============================
// File:            TEinsteinNativeCallsPrims.s
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

//  NATIVE_PRIM 0x9, 0x01, New__20TEinsteinNativeCallsFv
//  NATIVE_PRIM 0x9, 0x02, Delete__20TEinsteinNativeCallsFv
    NATIVE_PRIM 0x9, 0x03, OpenLib__20TEinsteinNativeCallsFPCc
    NATIVE_PRIM 0x9, 0x04, CloseLib__20TEinsteinNativeCallsFUl
    NATIVE_PRIM 0x9, 0x05, PrepareFFIStructure__20TEinsteinNativeCallsFUlPCcT1
    NATIVE_PRIM 0x9, 0x06, DisposeFFIStructure__20TEinsteinNativeCallsFUl
    NATIVE_PRIM 0x9, 0x07, GetErrorMessage__20TEinsteinNativeCallsFPcUl
    NATIVE_PRIM 0x9, 0x10, SetArgValue_uint8__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x11, SetArgValue_sint8__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x12, SetArgValue_uint16__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x13, SetArgValue_sint16__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x14, SetArgValue_uint32__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x15, SetArgValue_sint32__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x16, SetArgValue_uint64__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x17, SetArgValue_sint64__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x18, SetArgValue_float__20TEinsteinNativeCallsFUlT1d
    NATIVE_PRIM 0x9, 0x19, SetArgValue_double__20TEinsteinNativeCallsFUlT1d
    NATIVE_PRIM 0x9, 0x1A, SetArgValue_longdouble__20TEinsteinNativeCallsFUlT1d
    NATIVE_PRIM 0x9, 0x1B, SetArgValue_string__20TEinsteinNativeCallsFUlT1PCcT1
    NATIVE_PRIM 0x9, 0x1C, SetArgValue_binary__20TEinsteinNativeCallsFUlT1PCvT1
    NATIVE_PRIM 0x9, 0x1D, SetArgValue_pointer__20TEinsteinNativeCallsFUlN21
    NATIVE_PRIM 0x9, 0x20, SetResultType__20TEinsteinNativeCallsFUlT1
    NATIVE_PRIM 0x9, 0x21, GetOutArgValue_string__20TEinsteinNativeCallsFUlT1PCcT1
    NATIVE_PRIM 0x9, 0x22, GetOutArgValue_binary__20TEinsteinNativeCallsFUlT1PCvT1
    NATIVE_PRIM 0x9, 0x30, Call_void__20TEinsteinNativeCallsFUl
    NATIVE_PRIM 0x9, 0x31, Call_int__20TEinsteinNativeCallsFUl
    NATIVE_PRIM 0x9, 0x32, Call_real__20TEinsteinNativeCallsFUl
    NATIVE_PRIM 0x9, 0x33, Call_string__20TEinsteinNativeCallsFUlPcT1
    NATIVE_PRIM 0x9, 0x34, Call_pointer__20TEinsteinNativeCallsFUl
    NATIVE_PRIM 0x9, 0x40, GetErrno__20TEinsteinNativeCallsFv

// =========================================================================== //
// THEGODDESSOFTHENETHASTWISTINGFINGERSANDHERVOICEISLIKEAJAVELININTHENIGHTDUDE //
// =========================================================================== //
