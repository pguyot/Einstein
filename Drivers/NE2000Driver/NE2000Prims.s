// ==============================
// File:            NE2000Prims.s
// Project:         Einstein
//
// Copyright 2010, 2026 by Matthias Melcher.
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

// The calls of the NE2000 driver into the emulator, see
// TNativePrimitives::ExecuteNetworkManagerNative() (0x0a00 to 0x0a15).

    .arch armv3
    .balign 4

    .include "NativePrimitives.s"

// Debugging helpers
    NATIVE_PRIM 0x0a, 0x00, EinsteinNetUnknown
    NATIVE_PRIM 0x0a, 0x01, EinsteinNetLog
// Protocol class interface
    NATIVE_PRIM 0x0a, 0x02, EinsteinNetNew
    NATIVE_PRIM 0x0a, 0x03, EinsteinNetDelete
// Task services
    NATIVE_PRIM 0x0a, 0x04, EinsteinNetInit
    NATIVE_PRIM 0x0a, 0x05, EinsteinNetEnable
    NATIVE_PRIM 0x0a, 0x06, EinsteinNetDisable
    NATIVE_PRIM 0x0a, 0x07, EinsteinNetInterruptHandler
// Client services (from the event handlers of TLanternDriverAPI)
    NATIVE_PRIM 0x0a, 0x08, EinsteinNetSendBuffer
    NATIVE_PRIM 0x0a, 0x09, EinsteinNetSendCBufferList
    NATIVE_PRIM 0x0a, 0x0a, EinsteinNetSendPacket
    NATIVE_PRIM 0x0a, 0x0b, EinsteinNetGetDeviceAddress
    NATIVE_PRIM 0x0a, 0x0c, EinsteinNetAddMulticastAddress
    NATIVE_PRIM 0x0a, 0x0d, EinsteinNetDelMulticastAddress
    NATIVE_PRIM 0x0a, 0x0e, EinsteinNetGetLinkIntegrity
// Optional services
    NATIVE_PRIM 0x0a, 0x0f, EinsteinNetSetPromiscuous
    NATIVE_PRIM 0x0a, 0x10, EinsteinNetGetThroughput
// Private template services
    NATIVE_PRIM 0x0a, 0x11, EinsteinNetTimerExpired
// NE2000 template driver specific
    NATIVE_PRIM 0x0a, 0x12, EinsteinNetInitCard
    NATIVE_PRIM 0x0a, 0x13, EinsteinNetSetCardInfo
// Einstein/Newton communication
    NATIVE_PRIM 0x0a, 0x14, EinsteinNetDataAvailable
    NATIVE_PRIM 0x0a, 0x15, EinsteinNetReceiveData
