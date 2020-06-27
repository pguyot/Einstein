// =========== Header ===========
// File:                PMainSoundDriver.impl.s
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
    .global _ZN16PMainSoundDriver9ClassInfoEv

    .global main

main:
local___ZN16PMainSoundDriver9ClassInfoEv:
@ (reserved for now)
    .word   0
    .word   __implname__ - .
    .word   __protname__ - .
    .word   __cap__ - .
    .word   __btbl__ - .
    .word   __monent__ - .
@ branch to sizeof glue
    b       _ZN16PMainSoundDriver6SizeofEv
@ zero, or branch to Alloc method
    .word   0
@ zero, or branch to Free method
    .word   0
@ return, or branch to New(void)
    b       _ZN16PMainSoundDriver3NewEv
@ return, or branch to Delete
    b       _ZN16PMainSoundDriver6DeleteEv
@ version number
    .word   0x00010000
@ flags
    .word   0x00000000
@ (reserved)
    .word   0x00000000
@ (reserved)
    B       return__nil__16PMainSoundDriver

_ZN16PMainSoundDriver9ClassInfoEv:
    adr     r0,local___ZN16PMainSoundDriver9ClassInfoEv
    mov     pc,r14

return__nil__16PMainSoundDriver:
    mov     r0,#0
    mov     pc,r14

__implname__:
    .string "PMainSoundDriver"
__protname__:
    .string "PSoundDriver"
__cap__:
    .string "SoundInput"
    .string ""
    .string "SoundOutput"
    .string ""
    .string "version"
    .string "1.0"
    .string "copyright"
    .string "Paul Guyot"
    .balign 4
__btbl__:
    .word   0
    b   _ZN16PMainSoundDriver9ClassInfoEv
    b   _ZN16PMainSoundDriver3NewEv
    b   _ZN16PMainSoundDriver6DeleteEv
    b   SetSoundHardwareInfo__16PMainSoundDriverFPC16TSoundDriverInfo
    b   GetSoundHardwareInfo__16PMainSoundDriverFP16TSoundDriverInfo
    b   SetOutputBuffers__16PMainSoundDriverFUlN31
    b   SetInputBuffers__16PMainSoundDriverFUlN31
    b   ScheduleOutputBuffer__16PMainSoundDriverFUlT1
    b   ScheduleInputBuffer__16PMainSoundDriverFUlT1
    b   PowerOutputOn__16PMainSoundDriverFl
    b   PowerOutputOff__16PMainSoundDriverFv
    b   PowerInputOn__16PMainSoundDriverFl
    b   PowerInputOff__16PMainSoundDriverFv
    b   StartOutput__16PMainSoundDriverFv
    b   StartInput__16PMainSoundDriverFv
    b   StopOutput__16PMainSoundDriverFv
    b   StopInput__16PMainSoundDriverFv
    b   OutputIsEnabled__16PMainSoundDriverFv
    b   InputIsEnabled__16PMainSoundDriverFv
    b   OutputIsRunning__16PMainSoundDriverFv
    b   InputIsRunning__16PMainSoundDriverFv
    b   CurrentOutputPtr__16PMainSoundDriverFv
    b   CurrentInputPtr__16PMainSoundDriverFv
    b   OutputVolume__16PMainSoundDriverFl
    b   OutputVolume__16PMainSoundDriverFv
    b   InputVolume__16PMainSoundDriverFl
    b   InputVolume__16PMainSoundDriverFv
    b   EnableExtSoundSource__16PMainSoundDriverFl
    b   DisableExtSoundSource__16PMainSoundDriverFl
    b   OutputIntHandler__16PMainSoundDriverFv
    b   InputIntHandler__16PMainSoundDriverFv

__monent__:
    .end

// ========================================================================= //
// Real computer scientists only write specs for languages that might run on //
// future hardware.  Nobody trusts them to write specs for anything homo     //
// sapiens will ever be able to fit on a single planet.                      //
// ========================================================================= //
