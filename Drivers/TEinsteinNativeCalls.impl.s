// =========== Header ===========
// File:                TEinsteinNativeCalls.impl.s
// Project:             Einstein
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
// ===========

    .arch armv3
    .section .interp, "ax"
    . = 0
    .balign 4
    .global _ZN20TEinsteinNativeCalls9ClassInfoEv

    .global main

main:
local___ZN20TEinsteinNativeCalls9ClassInfoEv:
@ (reserved for now)
    .word   0
    .word   __implname__ - .
    .word   __protname__ - .
    .word   __cap__ - .
    .word   __btbl__ - .
    .word   __monent__ - .
@ branch to sizeof glue
    b       _ZN20TEinsteinNativeCalls6SizeofEv
@ zero, or branch to Alloc method
    .word   0
@ zero, or branch to Free method
    .word   0
@ return, or branch to New(void)
    b       _ZN20TEinsteinNativeCalls3NewEv
@ return, or branch to Delete
    b       _ZN20TEinsteinNativeCalls6DeleteEv
@ version number
    .word   0x00010000
@ flags
    .word   0x00000000
@ (reserved)
    .word   0x00000000
@ (reserved)
    b       return__nil__20TEinsteinNativeCalls

_ZN20TEinsteinNativeCalls9ClassInfoEv:
    adr     r0,local___ZN20TEinsteinNativeCalls9ClassInfoEv
    mov     pc,r14

return__nil__20TEinsteinNativeCalls:
    mov     r0,#0
    mov     pc,r14

__implname__:
    .string "TEinsteinNativeCalls"
__protname__:
    .string "TNativeCalls"
__cap__:
    .string "version"
    .string "1.0"
    .string "copyright"
    .string "Paul Guyot"
    .balign 4
__btbl__:
    .word   0
    b   _ZN20TEinsteinNativeCalls9ClassInfoEv
    b   _ZN20TEinsteinNativeCalls3NewEv
    b   _ZN20TEinsteinNativeCalls6DeleteEv
    b   OpenLib__20TEinsteinNativeCallsFPCc
    b   CloseLib__20TEinsteinNativeCallsFUl
    b   PrepareFFIStructure__20TEinsteinNativeCallsFUlPCcT1
    b   SetArgValue_uint8__20TEinsteinNativeCallsFUlN21
    b   SetArgValue_sint8__20TEinsteinNativeCallsFUlN21
    b   SetArgValue_uint16__20TEinsteinNativeCallsFUlN21
    b   SetArgValue_sint16__20TEinsteinNativeCallsFUlN21
    b   SetArgValue_uint32__20TEinsteinNativeCallsFUlN21
    b   SetArgValue_sint32__20TEinsteinNativeCallsFUlN21
    b   SetArgValue_float__20TEinsteinNativeCallsFUlT1d
    b   SetArgValue_double__20TEinsteinNativeCallsFUlT1d
    b   SetArgValue_longdouble__20TEinsteinNativeCallsFUlT1d
    b   SetArgValue_string__20TEinsteinNativeCallsFUlT1PCcT1
    b   SetArgValue_binary__20TEinsteinNativeCallsFUlT1PCvT1
    b   SetArgValue_pointer__20TEinsteinNativeCallsFUlN21
    b   SetResultType__20TEinsteinNativeCallsFUlT1
    b   Call_void__20TEinsteinNativeCallsFUl
    b   Call_int__20TEinsteinNativeCallsFUl
    b   Call_real__20TEinsteinNativeCallsFUl
    b   Call_string__20TEinsteinNativeCallsFUlPcT1
    b   Call_pointer__20TEinsteinNativeCallsFUl
    b   GetOutArgValue_string__20TEinsteinNativeCallsFUlT1PCcT1
    b   GetOutArgValue_binary__20TEinsteinNativeCallsFUlT1PCvT1
    b   DisposeFFIStructure__20TEinsteinNativeCallsFUl
    b   GetErrorMessage__20TEinsteinNativeCallsFPcUl
    b   GetErrno__20TEinsteinNativeCallsFv

__monent__:
    .end

// ========================================================================= //
// Real computer scientists only write specs for languages that might run on //
// future hardware.  Nobody trusts them to write specs for anything homo     //
// sapiens will ever be able to fit on a single planet.                      //
// ========================================================================= //
