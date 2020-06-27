// ==============================
// File:            TMainPlatformDriverPrims.s
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

//  NATIVE_PRIM 0x1, 0x01, _ZN19TMainPlatformDriver3NewEv
//  NATIVE_PRIM 0x1, 0x02, _ZN19TMainPlatformDriver6DeleteEv
    NATIVE_PRIM 0x1, 0x03, Init__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x04, BacklightTrigger__19TMainPlatformDriverFv
//  NATIVE_PRIM 0x1, 0x05, RegisterPowerSwitchInterrupt__19TMainPlatformDriverFv
//  NATIVE_PRIM 0x1, 0x06, EnableSysPowerInterrupt__19TMainPlatformDriverFv
//  NATIVE_PRIM 0x1, 0x07, InterruptHandler__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x08, TimerInterruptHandler__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x09, ResetZAPStoreCheck__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x0A, PowerOnSubsystem__19TMainPlatformDriverFUl
    NATIVE_PRIM 0x1, 0x0B, PowerOffSubsystem__19TMainPlatformDriverFUl
    NATIVE_PRIM 0x1, 0x0C, PowerOffAllSubsystems__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x0D, PauseSystem__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x0E, PowerOffSystem__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x0F, PowerOnSystem__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x10, TranslatePowerEvent__19TMainPlatformDriverFUl
    NATIVE_PRIM 0x1, 0x11, GetPCMCIAPowerSpec__19TMainPlatformDriverFUlPUl
    NATIVE_PRIM 0x1, 0x12, PowerOnDeviceCheck__19TMainPlatformDriverFUc
    NATIVE_PRIM 0x1, 0x13, SetSubsystemPower__19TMainPlatformDriverFUlT1
    NATIVE_PRIM 0x1, 0x14, GetSubsystemPower__19TMainPlatformDriverFUlPUl

    NATIVE_PRIM 0x1, 0x15, _ZN19TMainPlatformDriver12GetNextEventEP6SEvent
    NATIVE_PRIM 0x1, 0x16, BreakInMonitor__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x17, _ZN19TMainPlatformDriver23FillGestaltEmulatorInfoEP28SGestaltEinsteinEmulatorInfo
    NATIVE_PRIM 0x1, 0x18, _ZN19TMainPlatformDriver14LockEventQueueEv
    NATIVE_PRIM 0x1, 0x19, _ZN19TMainPlatformDriver16UnlockEventQueueEv
    NATIVE_PRIM 0x1, 0x1A, Log__19TMainPlatformDriverFPCc
    NATIVE_PRIM 0x1, 0x1B, GetUserInfo__19TMainPlatformDriverFUlT1Pv
    NATIVE_PRIM 0x1, 0x1C, GetHostTimeZone__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x1D, CalibrateTablet__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x1E, QuitEinstein__19TMainPlatformDriverFv
    NATIVE_PRIM 0x1, 0x1F, DisposeBuffer__19TMainPlatformDriverFUl
    NATIVE_PRIM 0x1, 0x20, CopyBufferData__19TMainPlatformDriverFUlPvN21

// ================================================================== //
// X windows:                                                         //
//         It's not how slow you make it.  It's how you make it slow. //
//         The windowing system preferred by masochists 3 to 1.       //
//         Built to take on the world... and lose!                    //
//         Don't try it 'til you've knocked it.                       //
//         Power tools for Power Fools.                               //
//         Putting new limits on productivity.                        //
//         The closer you look, the cruftier we look.                 //
//         Design by counterexample.                                  //
//         A new level of software disintegration.                    //
//         No hardware is safe.                                       //
//         Do your time.                                              //
//         Rationalization, not realization.                          //
//         Old-world software cruftsmanship at its finest.            //
//         Gratuitous incompatibility.                                //
//         Your mother.                                               //
//         THE user interference management system.                   //
//         You can't argue with failure.                              //
//         You haven't died 'til you've used it.                      //
//                                                                    //
// The environment of today... tomorrow!                              //
//         X windows.                                                 //
// ================================================================== //
