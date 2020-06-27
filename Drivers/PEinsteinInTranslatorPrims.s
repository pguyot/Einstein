// ==============================
// File:            PEinsteinInTranslatorPrims.s
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

    NATIVE_PRIM 0x7, 0x01, _ZN21PEinsteinInTranslator3NewEv
    NATIVE_PRIM 0x7, 0x02, _ZN21PEinsteinInTranslator6DeleteEv
    NATIVE_PRIM 0x7, 0x03, Init__21PEinsteinInTranslatorFPv
    NATIVE_PRIM 0x7, 0x04, Idle__21PEinsteinInTranslatorFv
    NATIVE_PRIM 0x7, 0x05, FrameAvailable__21PEinsteinInTranslatorFv
    NATIVE_PRIM 0x7, 0x06, ProduceFrame__21PEinsteinInTranslatorFi

// =========================================================================== //
// THEGODDESSOFTHENETHASTWISTINGFINGERSANDHERVOICEISLIKEAJAVELININTHENIGHTDUDE //
// =========================================================================== //
