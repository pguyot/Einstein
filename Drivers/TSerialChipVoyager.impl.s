// =========== Header ===========
// File:                TSerialChipVoyager.impl.s
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
    .global _ZN18TSerialChipVoyager9ClassInfoEv

    .global main

main:
local___ZN18TSerialChipVoyager9ClassInfoEv:
@ (reserved for now)
    .word   0
    .word   __implname__ - .
    .word   __protname__ - .
    .word   __cap__ - .
    .word   __btbl__ - .
    .word   __monent__ - .
@ branch to sizeof glue
    b       _ZN18TSerialChipVoyager6SizeofEv
@ zero, or branch to Alloc method
    .word   0
@ zero, or branch to Free method
    .word   0
@ return, or branch to New(void)
    b       _ZN18TSerialChipVoyager3NewEv
@ return, or branch to Delete
    b       _ZN18TSerialChipVoyager6DeleteEv
@ version number
    .word   0x00010000
@ flags
    .word   0x00000000
@ (reserved)
    .word   0x00000000
@ (reserved)
    b       return__nil__18TSerialChipVoyager

_ZN18TSerialChipVoyager9ClassInfoEv:
    adr     r0,local___ZN18TSerialChipVoyager9ClassInfoEv
    mov     pc,r14

return__nil__18TSerialChipVoyager:
    mov     r0,#0
    mov     pc,r14

__implname__:
    .string "TSerialChipVoyager"
__protname__:
    .string "TSerialChip"
__cap__:
    .string "version"
    .string "1.0"
    .string "copyright"
    .string "Paul Guyot"
    .balign 4
__btbl__:
    .word   0
    b   _ZN18TSerialChipVoyager9ClassInfoEv
    b   _ZN18TSerialChipVoyager3NewEv
    b   _ZN18TSerialChipVoyager6DeleteEv
    b   InstallChipHandler__18TSerialChipVoyagerFPvP14SCCChannelInts
    b   RemoveChipHandler__18TSerialChipVoyagerFPv
    b   PutByte__18TSerialChipVoyagerFUc
    b   ResetTxBEmpty__18TSerialChipVoyagerFv
    b   GetByte__18TSerialChipVoyagerFv
    b   TxBufEmpty__18TSerialChipVoyagerFv
    b   RxBufFull__18TSerialChipVoyagerFv
    b   GetRxErrorStatus__18TSerialChipVoyagerFv
    b   GetSerialStatus__18TSerialChipVoyagerFv
    b   ResetSerialStatus__18TSerialChipVoyagerFv
    b   SetSerialOutputs__18TSerialChipVoyagerFUl
    b   ClearSerialOutputs__18TSerialChipVoyagerFUl
    b   GetSerialOutputs__18TSerialChipVoyagerFv
    b   PowerOff__18TSerialChipVoyagerFv
    b   PowerOn__18TSerialChipVoyagerFv
    b   PowerIsOn__18TSerialChipVoyagerFv
    b   SetInterruptEnable__18TSerialChipVoyagerFUc
    b   Reset__18TSerialChipVoyagerFv
    b   SetBreak__18TSerialChipVoyagerFUc
    b   SetSpeed__18TSerialChipVoyagerFUl
    b   SetIOParms__18TSerialChipVoyagerFP17TCMOSerialIOParms
    b   Reconfigure__18TSerialChipVoyagerFv
    b   Init__18TSerialChipVoyagerFP11TCardSocketP12TCardHandlerPUc
    b   CardRemoved__18TSerialChipVoyagerFv
    b   GetFeatures__18TSerialChipVoyagerFv
    b   _ZN18TSerialChipVoyager12InitByOptionEP7TOption
    b   ProcessOption__18TSerialChipVoyagerFP7TOption
    b   SetSerialMode__18TSerialChipVoyagerFUl
    b   SysEventNotify__18TSerialChipVoyagerFUl
    b   SetTxDTransceiverEnable__18TSerialChipVoyagerFUc
    b   GetByteAndStatus__18TSerialChipVoyagerFPUc
    b   SetIntSourceEnable__18TSerialChipVoyagerFUlUc
    b   AllSent__18TSerialChipVoyagerFv
    b   ConfigureForOutput__18TSerialChipVoyagerFUc
    b   InitTxDMA__18TSerialChipVoyagerFP10TCircleBufPFPv_v
    b   InitRxDMA__18TSerialChipVoyagerFP10TCircleBufUlPFPvUl_v
    b   TxDMAControl__18TSerialChipVoyagerFUc
    b   RxDMAControl__18TSerialChipVoyagerFUc
    b   SetSDLCAddress__18TSerialChipVoyagerFUc
    b   ReEnableReceiver__18TSerialChipVoyagerFUc
    b   LinkIsFree__18TSerialChipVoyagerFUc
    b   SendControlPacket__18TSerialChipVoyagerFUcN21
    b   WaitForPacket__18TSerialChipVoyagerFUl
    b   WaitForAllSent__18TSerialChipVoyagerFv

__monent__:
    .end

// ========================================================================= //
// Real computer scientists only write specs for languages that might run on //
// future hardware.  Nobody trusts them to write specs for anything homo     //
// sapiens will ever be able to fit on a single planet.                      //
// ========================================================================= //
