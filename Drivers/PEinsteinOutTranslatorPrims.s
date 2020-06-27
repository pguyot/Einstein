// ==============================
// File:            PEinsteinOutTranslatorPrims.s
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

    .macro NATIVE_PRIM impl, id, symbol
    .section ".text.\symbol\()", "ax"
    .global \symbol
\symbol\():
    .balign 4
    stmdb       sp!, {lr}
    mov         lr, #\id\()
    add         lr, lr, #\impl\() * 0x100
    mcr         p10, 0, lr, c0, c0
    ldmia       sp!, {pc}
    .endm

    NATIVE_PRIM 0x8, 0x01, _ZN22PEinsteinOutTranslator3NewEv
    NATIVE_PRIM 0x8, 0x02, _ZN22PEinsteinOutTranslator6DeleteEv
    NATIVE_PRIM 0x8, 0x03, Init__22PEinsteinOutTranslatorFPv
    NATIVE_PRIM 0x8, 0x04, Idle__22PEinsteinOutTranslatorFv
    NATIVE_PRIM 0x8, 0x05, ConsumeFrame__22PEinsteinOutTranslatorFRC6RefVariT2
    NATIVE_PRIM 0x8, 0x06, Flush__22PEinsteinOutTranslatorFi
    NATIVE_PRIM 0x8, 0x07, Prompt__22PEinsteinOutTranslatorFi
//  NATIVE_PRIM 0x8, 0x08, _ZN22PEinsteinOutTranslator5PrintEPKcz
    NATIVE_PRIM 0x8, 0x09, Putc__22PEinsteinOutTranslatorFi
    NATIVE_PRIM 0x8, 0x0A, EnterBreakLoop__22PEinsteinOutTranslatorFi
    NATIVE_PRIM 0x8, 0x0B, ExitBreakLoop__22PEinsteinOutTranslatorFv
    NATIVE_PRIM 0x8, 0x0C, StackTrace__22PEinsteinOutTranslatorFPv
    NATIVE_PRIM 0x8, 0x0D, ExceptionNotify__22PEinsteinOutTranslatorFP9Exception

    NATIVE_PRIM 0x8, 0x0E, _ZN22PEinsteinOutTranslator7DoPrintEPKc

// =========================================================================== //
// THEGODDESSOFTHENETHASTWISTINGFINGERSANDHERVOICEISLIKEAJAVELININTHENIGHTDUDE //
// =========================================================================== //
