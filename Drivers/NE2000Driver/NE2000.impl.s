// =========== Header ===========
// File:                NE2000.impl.s
// Project:             Einstein
//
// Copyright 2020 by Paul Guyot (pguyot@kallisys.net), 2026 by Matthias Melcher.
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
// ===========

// The class info and branch table of TNE2000Sample, the implementation of the
// TLanternDriver protocol (made by ProtocolGen for the MPW tools). The order
// of the branch table is the order of the methods in TLanternDriver.

    .arch armv3
    .section .interp, "ax"
    . = 0
    .balign 4
    .global _ZN13TNE2000Sample9ClassInfoEv

    .global main

main:
local___ZN13TNE2000Sample9ClassInfoEv:
@ (reserved for now)
    .word   0
    .word   __implname__ - .
    .word   __protname__ - .
    .word   __cap__ - .
    .word   __btbl__ - .
    .word   __monent__ - .
@ branch to sizeof glue
    b       _ZN13TNE2000Sample6SizeofEv
@ zero, or branch to Alloc method
    .word   0
@ zero, or branch to Free method
    .word   0
@ return, or branch to New(void)
    b       _ZN13TNE2000Sample3NewEv
@ return, or branch to Delete
    b       _ZN13TNE2000Sample6DeleteEv
@ version number
    .word   0x00010000
@ flags
    .word   0x00000000
@ (reserved)
    .word   0x00000000
@ (reserved)
    B       return__nil__13TNE2000Sample

_ZN13TNE2000Sample9ClassInfoEv:
    adr     r0,local___ZN13TNE2000Sample9ClassInfoEv
    mov     pc,r14

return__nil__13TNE2000Sample:
    mov     r0,#0
    mov     pc,r14

__implname__:
    .string "TNE2000Sample"
__protname__:
    .string "TLanternDriver"
__cap__:
    .string ""
    .balign 4
__btbl__:
    .word   0
    b   _ZN13TNE2000Sample9ClassInfoEv
    b   _ZN13TNE2000Sample3NewEv
    b   _ZN13TNE2000Sample6DeleteEv
    b   _ZN13TNE2000Sample4InitEv
    b   _ZN13TNE2000Sample6EnableEv
    b   _ZN13TNE2000Sample7DisableEv
    b   _ZN13TNE2000Sample16InterruptHandlerEv

__monent__:
    .end
