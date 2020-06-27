// ==============================
// File:            PMainSoundDriverPrims.s
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

//  NATIVE_PRIM 0x2, 0x01, _ZN16PMainSoundDriver3NewEv
//  NATIVE_PRIM 0x2, 0x02, _ZN16PMainSoundDriver6DeleteEv
    NATIVE_PRIM 0x2, 0x03, SetSoundHardwareInfo__16PMainSoundDriverFPC16TSoundDriverInfo
    NATIVE_PRIM 0x2, 0x04, GetSoundHardwareInfo__16PMainSoundDriverFP16TSoundDriverInfo
    NATIVE_PRIM 0x2, 0x05, SetOutputBuffers__16PMainSoundDriverFUlN31
    NATIVE_PRIM 0x2, 0x06, SetInputBuffers__16PMainSoundDriverFUlN31
    NATIVE_PRIM 0x2, 0x07, ScheduleOutputBuffer__16PMainSoundDriverFUlT1
    NATIVE_PRIM 0x2, 0x08, ScheduleInputBuffer__16PMainSoundDriverFUlT1
    NATIVE_PRIM 0x2, 0x09, PowerOutputOn__16PMainSoundDriverFl
    NATIVE_PRIM 0x2, 0x0A, PowerOutputOff__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x0B, PowerInputOn__16PMainSoundDriverFl
    NATIVE_PRIM 0x2, 0x0C, PowerInputOff__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x0D, StartOutput__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x0E, StartInput__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x0F, StopOutput__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x10, StopInput__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x11, OutputIsEnabled__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x12, InputIsEnabled__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x13, OutputIsRunning__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x14, InputIsRunning__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x15, CurrentOutputPtr__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x16, CurrentInputPtr__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x17, OutputVolume__16PMainSoundDriverFl
    NATIVE_PRIM 0x2, 0x18, OutputVolume__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x19, InputVolume__16PMainSoundDriverFl
    NATIVE_PRIM 0x2, 0x1A, InputVolume__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x1B, EnableExtSoundSource__16PMainSoundDriverFl
    NATIVE_PRIM 0x2, 0x1C, DisableExtSoundSource__16PMainSoundDriverFl
    NATIVE_PRIM 0x2, 0x1D, OutputIntHandler__16PMainSoundDriverFv
    NATIVE_PRIM 0x2, 0x1E, InputIntHandler__16PMainSoundDriverFv

    NATIVE_PRIM 0x2, 0x1F, _ZN16PMainSoundDriver22NativeSetInterruptMaskEmm

// ======================================================================= //
// Programmers used to batch environments may find it hard to live without //
// giant listings; we would find it hard to use them.                      //
//                 -- D.M. Ritchie                                         //
// ======================================================================= //
