// =========== Header ===========
// File:                TSerialChipEinstein.impl.s
// Project:             Einstein
//
// Copyright 2020 by Eckhart Koeppen (eck@40hz.org).
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
    .global _ZN19TSerialChipEinstein9ClassInfoEv

    .global main

main:
local___ZN19TSerialChipEinstein9ClassInfoEv:
@ (reserved for now)
    .word   0
    .word   __implname__ - .
    .word   __protname__ - .
    .word   __cap__ - .
    .word   __btbl__ - .
    .word   __monent__ - .
@ branch to sizeof glue
    b       _ZN19TSerialChipEinstein6SizeofEv
@ zero, or branch to Alloc method
    .word   0
@ zero, or branch to Free method
    .word   0
@ return, or branch to New(void)
    b       _ZN19TSerialChipEinstein3NewEv
@ return, or branch to Delete
    b       _ZN19TSerialChipEinstein6DeleteEv
@ version number
    .word   0x00010000
@ flags
    .word   0x00000000
@ (reserved)
    .word   0x00000000
@ (reserved)
    b       return__nil__19TSerialChipEinstein

_ZN19TSerialChipEinstein9ClassInfoEv:
    adr     r0,local___ZN19TSerialChipEinstein9ClassInfoEv
    mov     pc,r14

return__nil__19TSerialChipEinstein:
    mov     r0,#0
    mov     pc,r14

__implname__:
    .string "TSerialChipEinstein"
__protname__:
    .string "TSerialChip"
__cap__:
    .string "v2.0"
    .balign 4
__btbl__:
    .word   0
    b   _ZN19TSerialChipEinstein9ClassInfoEv
    b   _ZN19TSerialChipEinstein3NewEv
    b   _ZN19TSerialChipEinstein6DeleteEv
    b   _ZN19TSerialChipEinstein18InstallChipHandlerEPvP14SCCChannelInts
    b   _ZN19TSerialChipEinstein17RemoveChipHandlerEPv
    b   PutByte__19TSerialChipEinsteinFUc
    b   ResetTxBEmpty__19TSerialChipEinsteinFv
    b   GetByte__19TSerialChipEinsteinFv
    b   TxBufEmpty__19TSerialChipEinsteinFv
    b   RxBufFull__19TSerialChipEinsteinFv
    b   GetRxErrorStatus__19TSerialChipEinsteinFv
    b   GetSerialStatus__19TSerialChipEinsteinFv
    b   ResetSerialStatus__19TSerialChipEinsteinFv
    b   SetSerialOutputs__19TSerialChipEinsteinFUl
    b   ClearSerialOutputs__19TSerialChipEinsteinFUl
    b   GetSerialOutputs__19TSerialChipEinsteinFv
    b   PowerOff__19TSerialChipEinsteinFv
    b   PowerOn__19TSerialChipEinsteinFv
    b   PowerIsOn__19TSerialChipEinsteinFv
    b   SetInterruptEnable__19TSerialChipEinsteinFUc
    b   Reset__19TSerialChipEinsteinFv
    b   SetBreak__19TSerialChipEinsteinFUc
    b   SetSpeed__19TSerialChipEinsteinFUl
    b   SetIOParms__19TSerialChipEinsteinFP17TCMOSerialIOParms
    b   Reconfigure__19TSerialChipEinsteinFv
    b   Init__19TSerialChipEinsteinFP11TCardSocketP12TCardHandlerPUc
    b   CardRemoved__19TSerialChipEinsteinFv
    b   GetFeatures__19TSerialChipEinsteinFv
    b   InitByOption__19TSerialChipEinsteinFP7TOption
    b   ProcessOption__19TSerialChipEinsteinFP7TOption
    b   SetSerialMode__19TSerialChipEinsteinFUl
    b   SysEventNotify__19TSerialChipEinsteinFUl
    b   SetTxDTransceiverEnable__19TSerialChipEinsteinFUc
    b   GetByteAndStatus__19TSerialChipEinsteinFPUc
    b   SetIntSourceEnable__19TSerialChipEinsteinFUlUc
    b   AllSent__19TSerialChipEinsteinFv
    b   ConfigureForOutput__19TSerialChipEinsteinFUc
    b   InitTxDMA__19TSerialChipEinsteinFP10TCircleBufPFPv_v
    b   InitRxDMA__19TSerialChipEinsteinFP10TCircleBufUlPFPvUl_v
    b   TxDMAControl__19TSerialChipEinsteinFUc
    b   RxDMAControl__19TSerialChipEinsteinFUc
    b   SetSDLCAddress__19TSerialChipEinsteinFUc
    b   ReEnableReceiver__19TSerialChipEinsteinFUc
    b   LinkIsFree__19TSerialChipEinsteinFUc
    b   SendControlPacket__19TSerialChipEinsteinFUcN21
    b   WaitForPacket__19TSerialChipEinsteinFUl
    b   WaitForAllSent__19TSerialChipEinsteinFv

__monent__:
    .end

// =========================================================================== //
// The primary purpose of the DATA statement is to give names to constants;    //
// instead of referring to pi as 3.141592653589793 at every appearance, the    //
// variable PI can be given that value with a DATA statement and used instead  //
// of the longer form of the constant.  This also simplifies modifying the     //
// program, should the value of pi change.                                     //
//                 -- FORTRAN manual for Xerox Computers                       //
// =========================================================================== //
