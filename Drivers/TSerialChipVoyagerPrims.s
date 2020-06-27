// ==============================
// File:            TSerialChipVoyagerPrims.s
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

//  NATIVE_PRIM 0x6, 0x01, New__18TSerialChipVoyagerFv
//  NATIVE_PRIM 0x6, 0x02, Delete__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x03, InstallChipHandler__18TSerialChipVoyagerFPvP14SCCChannelInts
    NATIVE_PRIM 0x6, 0x04, RemoveChipHandler__18TSerialChipVoyagerFPv
    NATIVE_PRIM 0x6, 0x05, PutByte__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x06, ResetTxBEmpty__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x07, GetByte__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x08, TxBufEmpty__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x09, RxBufFull__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x0A, GetRxErrorStatus__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x0B, GetSerialStatus__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x0C, ResetSerialStatus__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x0D, SetSerialOutputs__18TSerialChipVoyagerFUl
    NATIVE_PRIM 0x6, 0x0E, ClearSerialOutputs__18TSerialChipVoyagerFUl
    NATIVE_PRIM 0x6, 0x0F, GetSerialOutputs__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x10, PowerOff__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x11, PowerOn__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x12, PowerIsOn__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x13, SetInterruptEnable__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x14, Reset__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x15, SetBreak__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x16, SetSpeed__18TSerialChipVoyagerFUl
    NATIVE_PRIM 0x6, 0x17, SetIOParms__18TSerialChipVoyagerFP17TCMOSerialIOParms
    NATIVE_PRIM 0x6, 0x18, Reconfigure__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x19, Init__18TSerialChipVoyagerFP11TCardSocketP12TCardHandlerPUc
    NATIVE_PRIM 0x6, 0x1A, CardRemoved__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x1B, GetFeatures__18TSerialChipVoyagerFv
//  NATIVE_PRIM 0x6, 0x1C, InitByOption__18TSerialChipVoyagerFP7TOption
    NATIVE_PRIM 0x6, 0x1D, ProcessOption__18TSerialChipVoyagerFP7TOption
    NATIVE_PRIM 0x6, 0x1E, SetSerialMode__18TSerialChipVoyagerFUl
    NATIVE_PRIM 0x6, 0x1F, SysEventNotify__18TSerialChipVoyagerFUl
    NATIVE_PRIM 0x6, 0x20, SetTxDTransceiverEnable__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x21, GetByteAndStatus__18TSerialChipVoyagerFPUc
    NATIVE_PRIM 0x6, 0x22, SetIntSourceEnable__18TSerialChipVoyagerFUlUc
    NATIVE_PRIM 0x6, 0x23, AllSent__18TSerialChipVoyagerFv
    NATIVE_PRIM 0x6, 0x24, ConfigureForOutput__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x25, InitTxDMA__18TSerialChipVoyagerFP10TCircleBufPFPv_v
    NATIVE_PRIM 0x6, 0x26, InitRxDMA__18TSerialChipVoyagerFP10TCircleBufUlPFPvUl_v
    NATIVE_PRIM 0x6, 0x27, TxDMAControl__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x28, RxDMAControl__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x29, SetSDLCAddress__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x2A, ReEnableReceiver__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x2B, LinkIsFree__18TSerialChipVoyagerFUc
    NATIVE_PRIM 0x6, 0x2C, SendControlPacket__18TSerialChipVoyagerFUcN21
    NATIVE_PRIM 0x6, 0x2D, WaitForPacket__18TSerialChipVoyagerFUl
    NATIVE_PRIM 0x6, 0x2E, WaitForAllSent__18TSerialChipVoyagerFv

// ============================================================== //
// Computers are unreliable, but humans are even more unreliable. //
// Any system which depends on human reliability is unreliable.   //
//                 -- Gilb                                        //
// ============================================================== //
