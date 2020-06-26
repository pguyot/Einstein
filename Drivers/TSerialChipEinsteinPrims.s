// ==============================
// File:            TSerialChipEinsteinPrims.s
// Project:         Einstein
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
// ==============================

    .arch armv3
    .balign 4

    .include "NativePrimitives.s"

//  NATIVE_PRIM 0x6, 0x31, New__19TSerialChipEinsteinFv
//  NATIVE_PRIM 0x6, 0x32, Delete__19TSerialChipEinsteinFv
//  NATIVE_PRIM 0x6, 0x33, InstallChipHandler__19TSerialChipEinsteinFPvP14SCCChannelInts
//  NATIVE_PRIM 0x6, 0x34, RemoveChipHandler__19TSerialChipEinsteinFPv
    NATIVE_PRIM 0x6, 0x35, PutByte__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x36, ResetTxBEmpty__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x37, GetByte__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x38, TxBufEmpty__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x39, RxBufFull__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x3A, GetRxErrorStatus__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x3B, GetSerialStatus__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x3B, _ZN19TSerialChipEinstein15GetSerialStatusEv
    NATIVE_PRIM 0x6, 0x3C, ResetSerialStatus__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x3D, SetSerialOutputs__19TSerialChipEinsteinFUl
    NATIVE_PRIM 0x6, 0x3E, ClearSerialOutputs__19TSerialChipEinsteinFUl
    NATIVE_PRIM 0x6, 0x3F, GetSerialOutputs__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x40, PowerOff__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x41, PowerOn__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x42, PowerIsOn__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x43, SetInterruptEnable__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x44, Reset__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x45, SetBreak__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x46, SetSpeed__19TSerialChipEinsteinFUl
    NATIVE_PRIM 0x6, 0x47, SetIOParms__19TSerialChipEinsteinFP17TCMOSerialIOParms
    NATIVE_PRIM 0x6, 0x48, Reconfigure__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x49, Init__19TSerialChipEinsteinFP11TCardSocketP12TCardHandlerPUc
    NATIVE_PRIM 0x6, 0x4A, CardRemoved__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x4B, GetFeatures__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x4C, InitByOption__19TSerialChipEinsteinFP7TOption
    NATIVE_PRIM 0x6, 0x4D, ProcessOption__19TSerialChipEinsteinFP7TOption
    NATIVE_PRIM 0x6, 0x4E, SetSerialMode__19TSerialChipEinsteinFUl
    NATIVE_PRIM 0x6, 0x4F, SysEventNotify__19TSerialChipEinsteinFUl
    NATIVE_PRIM 0x6, 0x50, SetTxDTransceiverEnable__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x51, GetByteAndStatus__19TSerialChipEinsteinFPUc
    NATIVE_PRIM 0x6, 0x52, SetIntSourceEnable__19TSerialChipEinsteinFUlUc
    NATIVE_PRIM 0x6, 0x53, AllSent__19TSerialChipEinsteinFv
    NATIVE_PRIM 0x6, 0x54, ConfigureForOutput__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x55, InitTxDMA__19TSerialChipEinsteinFP10TCircleBufPFPv_v
    NATIVE_PRIM 0x6, 0x56, InitRxDMA__19TSerialChipEinsteinFP10TCircleBufUlPFPvUl_v
    NATIVE_PRIM 0x6, 0x57, TxDMAControl__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x58, RxDMAControl__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x59, SetSDLCAddress__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x5A, ReEnableReceiver__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x5B, LinkIsFree__19TSerialChipEinsteinFUc
    NATIVE_PRIM 0x6, 0x5C, SendControlPacket__19TSerialChipEinsteinFUcN21
    NATIVE_PRIM 0x6, 0x5D, WaitForPacket__19TSerialChipEinsteinFUl
    NATIVE_PRIM 0x6, 0x5E, WaitForAllSent__19TSerialChipEinsteinFv

// ============================================================== //
// Computers are unreliable, but humans are even more unreliable. //
// Any system which depends on human reliability is unreliable.   //
//                 -- Gilb                                        //
// ============================================================== //
