// ==============================
// File:            NativePrimitives.s
// Project:         Einstein
//
// Copyright 2020 by Paul Guyot (pguyot@kallisys.net).
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

    .macro NATIVE_PRIM impl, id, symbol
    .section ".text.\symbol\()", "ax"
    .global \symbol
\symbol\():
    .balign 4
    stmdb       sp!, {lr}
    mov         lr, #\id\()
.if \impl\()
    add         lr, lr, #\impl\() * 0x100
.endif
    mcr         p10, 0, lr, c0, c0
    ldmia       sp!, {pc}
    .endm

// =================================================================== //
// It is easier to make a saint out of a libertine than out of a prig. //
//                 -- George Santayana                                 //
// =================================================================== //
