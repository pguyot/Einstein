// =========== Header ===========
// File:                TMainTabletDriver.impl.s
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
    .global _ZN17TMainTabletDriver9ClassInfoEv

    .global main

main:
local___ZN17TMainTabletDriver9ClassInfoEv:
@ (reserved for now)
    .word   0
    .word   __implname__ - .
    .word   __protname__ - .
    .word   __cap__ - .
    .word   __btbl__ - .
    .word   __monent__ - .
@ branch to sizeof glue
    b       _ZN17TMainTabletDriver6SizeofEv
@ zero, or branch to Alloc method
    .word   0
@ zero, or branch to Free method
    .word   0
@ return, or branch to New(void)
    b       _ZN17TMainTabletDriver3NewEv
@ return, or branch to Delete
    b       _ZN17TMainTabletDriver6DeleteEv
@ version number
    .word   0x00010000
@ flags
    .word   0x00000000
@ (reserved)
    .word   0x00000000
@ (reserved)
    b       return__nil__17TMainTabletDriver

_ZN17TMainTabletDriver9ClassInfoEv:
    adr     r0,local___ZN17TMainTabletDriver9ClassInfoEv
    mov     pc,r14

return__nil__17TMainTabletDriver:
    mov     r0,#0
    mov     pc,r14

__implname__:
    .string "TMainTabletDriver"
__protname__:
    .string "TTabletDriver"
__cap__:
    .string "version"
    .string "1.0"
    .string "copyright"
    .string "Paul Guyot"
    .balign 4
__btbl__:
    .word   0
    b   _ZN17TMainTabletDriver9ClassInfoEv
    b   _ZN17TMainTabletDriver3NewEv
    b   _ZN17TMainTabletDriver6DeleteEv
    b   Init__17TMainTabletDriverFRC4Rect
    b   WakeUp__17TMainTabletDriverFv
    b   ShutDown__17TMainTabletDriverFv
    b   TabletIdle__17TMainTabletDriverFv
    b   GetSampleRate__17TMainTabletDriverFv
    b   SetSampleRate__17TMainTabletDriverFUl
    b   GetTabletCalibration__17TMainTabletDriverFP11Calibration
    b   SetTabletCalibration__17TMainTabletDriverFRC11Calibration
    b   SetDoingCalibration__17TMainTabletDriverFUcPUl
    b   GetTabletResolution__17TMainTabletDriverFPlT1
    b   TabSetOrientation__17TMainTabletDriverFl
    b   GetTabletState__17TMainTabletDriverFv
    b   GetFingerInputState__17TMainTabletDriverFPUc
    b   SetFingerInputState__17TMainTabletDriverFUc
    b   RecalibrateTabletAfterRotate__17TMainTabletDriverFv
    b   TabletNeedsRecalibration__17TMainTabletDriverFv
    b   StartBypassTablet__17TMainTabletDriverFv
    b   StopBypassTablet__17TMainTabletDriverFv
    b   ReturnTabletToConsciousness__17TMainTabletDriverFUlN21

__monent__:
    .end

// ========================================================================= //
// Real computer scientists only write specs for languages that might run on //
// future hardware.  Nobody trusts them to write specs for anything homo     //
// sapiens will ever be able to fit on a single planet.                      //
// ========================================================================= //
