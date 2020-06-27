// =========== Header ===========
// File:                PMainBatteryDriver.impl.s
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
    .global _ZN18PMainBatteryDriver9ClassInfoEv

    .global main

main:
local___ZN18PMainBatteryDriver9ClassInfoEv:
@ (reserved for now)
    .word   0
    .word   __implname__ - .
    .word   __protname__ - .
    .word   __cap__ - .
    .word   __btbl__ - .
    .word   __monent__ - .
@ branch to sizeof glue
    b       _ZN18PMainBatteryDriver6SizeofEv
@ zero, or branch to Alloc method
    .word   0
@ zero, or branch to Free method
    .word   0
@ return, or branch to New(void)
    b       _ZN18PMainBatteryDriver3NewEv
@ return, or branch to Delete
    b       _ZN18PMainBatteryDriver6DeleteEv
@ version number
    .word   0x00010000
@ flags
    .word   0x00000000
@ (reserved)
    .word   0x00000000
@ (reserved)
    b       return__nil__18PMainBatteryDriver

_ZN18PMainBatteryDriver9ClassInfoEv:
    adr     r0,local___ZN18PMainBatteryDriver9ClassInfoEv
    mov     pc,r14

return__nil__18PMainBatteryDriver:
    mov     r0,#0
    mov     pc,r14

__implname__:
    .string "PMainBatteryDriver"
__protname__:
    .string "PBatteryDriver"
__cap__:
    .string "version"
    .string "1.0"
    .string "copyright"
    .string "Paul Guyot"
    .balign 4
__btbl__:
    .word   0
    b   _ZN18PMainBatteryDriver9ClassInfoEv
    b   _ZN18PMainBatteryDriver3NewEv
    b   _ZN18PMainBatteryDriver6DeleteEv
    b   Init__18PMainBatteryDriverFv
    b   WakeUp__18PMainBatteryDriverFv
    b   ShutDown__18PMainBatteryDriverFv
    b   Count__18PMainBatteryDriverFv
    b   Status__18PMainBatteryDriverFUlP16PowerPlantStatus
    b   RawStatus__18PMainBatteryDriverFUlP16PowerPlantStatus
    b   StartSleepCharge__18PMainBatteryDriverFv
    b   SetType__18PMainBatteryDriverFUlT1
    b   ReadADCVoltage__18PMainBatteryDriverFUl
    b   ConvertVoltage__18PMainBatteryDriverFUlT1

__monent__:
    .end

// ========================================================================= //
// Real computer scientists only write specs for languages that might run on //
// future hardware.  Nobody trusts them to write specs for anything homo     //
// sapiens will ever be able to fit on a single planet.                      //
// ========================================================================= //
