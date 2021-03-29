// ==============================
// File:			TDiag.cpp
// Project:			Einstein
//
// Copyright 2021, Matthias Melcher, einstein@messagepad.org.
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
// $Id$
// ==============================

#include "TDiag.h"

// POSIX & ANSI C
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Streams/TStream.h>

// Einstein
#include "Log/TLog.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"

#if TARGET_OS_WIN32
//---
#include "Emulator/Screen/TNullScreenManager.h"
#include "Emulator/Screen/TFLScreenManager.h"
//---
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Sound/TWaveSoundManager.h"
//---
#include "Emulator/Network/TUsermodeNetwork.h"
//---
#include "Emulator/JIT/Generic/TJITGeneric.h"
//---
#include "Emulator/Serial/TSerialPortManager.h"
#include "Emulator/Serial/TBasicSerialPortManager.h"
#include "Emulator/Serial/TTcpClientSerialPortManager.h"
#endif

#if TARGET_OS_ANDROID
// ...
#endif

#if TARGET_OS_MAC
// ...
#endif

#if TARGET_OS_IOS
// ...
#endif

#if TARGET_OS_LINUX
// ...
#endif


// -------------------------------------------------------------------------- //
// * void KPrintPlugins()
// -------------------------------------------------------------------------- //
void TDiag::PrintDrivers(TEmulator* inEmulator)
{
    KPrintf("\n Einstein Drivers List"
            "\n=======================\n");

#if TARGET_OS_WIN32
    // --- Screen
    TScreenManager* screen = inEmulator->GetScreenManager();
    if (dynamic_cast<TNullScreenManager*>(screen)) KPrintf("Screen: TNullScreenManager\n");
    else if (dynamic_cast<TFLScreenManager*>(screen)) KPrintf("Screen: TFLScreenManager\n");
    else KPrintf("Screen: unknown\n");
    // --- Sound
    TSoundManager* snd = inEmulator->GetSoundManager();
    if (dynamic_cast<TNullSoundManager*>(snd)) KPrintf("Sound: TNullSoundManager\n");
    else if (dynamic_cast<TWaveSoundManager*>(snd)) KPrintf("Sound: TWaveSoundManager\n"); 
    else KPrintf("Sound: unknown\n");
    // --- Network
    TNetworkManager* net = inEmulator->GetNetworkManager();
    if (dynamic_cast<TUsermodeNetwork*>(net)) KPrintf("Network: TUsermodeNetwork\n");
    else KPrintf("Network: unknown\n");
    // --- Serial Ports
    TSerialPortManager* ser0 = inEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
    if (dynamic_cast<TTcpClientSerialPortManager*>(ser0)) KPrintf("Serial Extr: TTcpClientSerialPortManager\n");
    else if (dynamic_cast<TBasicSerialPortManager*>(ser0)) KPrintf("Serial Extr: TBasicSerialPortManager\n");
    else KPrintf("Serial Extr: unknown\n");
    TSerialPortManager* ser1 = inEmulator->SerialPorts.GetDriverFor(TSerialPorts::kInfr);
    if (dynamic_cast<TTcpClientSerialPortManager*>(ser1)) KPrintf("Serial Infr: TTcpClientSerialPortManager\n");
    else if (dynamic_cast<TBasicSerialPortManager*>(ser1)) KPrintf("Serial Infr: TBasicSerialPortManager\n");
    else KPrintf("Serial Infr: unknown\n");
    TSerialPortManager* ser2 = inEmulator->SerialPorts.GetDriverFor(TSerialPorts::kTblt);
    if (dynamic_cast<TTcpClientSerialPortManager*>(ser2)) KPrintf("Serial Tblt: TTcpClientSerialPortManager\n");
    else if (dynamic_cast<TBasicSerialPortManager*>(ser2)) KPrintf("Serial Tblt: TBasicSerialPortManager\n");
    else KPrintf("Serial Tblt: unknown\n");
    TSerialPortManager* ser3 = inEmulator->SerialPorts.GetDriverFor(TSerialPorts::kMdem);
    if (dynamic_cast<TTcpClientSerialPortManager*>(ser3)) KPrintf("Serial Mdem: TTcpClientSerialPortManager\n");
    else if (dynamic_cast<TBasicSerialPortManager*>(ser3)) KPrintf("Serial Mdem: TBasicSerialPortManager\n");
    else KPrintf("Serial Mdem: unknown\n");
    // --- JIT Compiler
    TJITGeneric* jit = inEmulator->GetMemory()->GetJITObject();
    if (dynamic_cast<TJITGeneric*>(jit)) KPrintf("JIT Compiler: TJITGeneric\n");
    else KPrintf("JIT Compiler: unknown\n");
#else
    // TODO: implement other platforms here: Android, macOS, iOS, Linux
    KPrintf("(not supported on this platform)\n");
#endif
    KPrintf("--- end\n");
}

// -------------------------------------------------------------------------- //
// * void TDiag::PrintThreadStates(TEmulator*)
// -------------------------------------------------------------------------- //

void TDiag::PrintThreadStates(TEmulator* inEmulator)
{
    KPrintf("\n Einstein Thread States"
            "\n========================\n");

    // Threads:     TFLApp::emulatorThread -> TFLApp::EmulatorThreadEntry
    //              TNetworkManager::mThread -> TNetworkManager::Run()
    //              TTcpClientSerialPortManager::mWorkerThread -> TTcpClientSerialPortManager::HandleDMA()
    //          OK: TInterruptManager::mThread -> TInterruptManager::Run()
    //              MainThread

    // Handle the conditions of TInterruptManager::mThread, launching TInterruptManager::Run()
    TInterruptManager* intMgr = inEmulator->GetInterruptManager();
    switch (intMgr->mThreadState) {
    case TInterruptManager::TThreadState::kNull:
        KPrintf("TInterruptManager: kNull, not yet created\n"); break;
    case TInterruptManager::TThreadState::kInstatiating:
        KPrintf("TInterruptManager: kInstatiating, task instantiated\n"); break;
    case TInterruptManager::TThreadState::kRunning:
        KPrintf("TInterruptManager: kRunning, task running\n"); break;
    case TInterruptManager::TThreadState::kMutexLock1:
        KPrintf("TInterruptManager: kMutexLock1, waiting to enter main loop\n"); break;
    case TInterruptManager::TThreadState::kMutexLock1Done:
        KPrintf("TInterruptManager: kMutexLock1Done, entering main loop\n"); break;
    case TInterruptManager::TThreadState::kMutexLock2:
        KPrintf("TInterruptManager: kMutexLock2, waiting for cond var, no timer set\n"); break;
    case TInterruptManager::TThreadState::kMutexLock2Done:
        KPrintf("TInterruptManager: kMutexLock2Done, running after cond var was set\n"); break;
    case TInterruptManager::TThreadState::kMutexLock3:
        KPrintf("TInterruptManager: kMutexLock3, waiting for a wakeup call\n"); break;
    case TInterruptManager::TThreadState::kMutexLock3Done:
        KPrintf("TInterruptManager: kMutexLock3Done, running after wakeup call\n"); break;
    case TInterruptManager::TThreadState::kMutexLock4:
        KPrintf("TInterruptManager: kMutexLock4, waiting for cond var or timer to expire\n"); break;
    case TInterruptManager::TThreadState::kMutexLock4Done:
        KPrintf("TInterruptManager: kMutexLock4Done, running after cond var set or timer expired\n"); break;
    case TInterruptManager::TThreadState::kExiting:
        KPrintf("TInterruptManager: kExiting, quit the loop, exiting the thread\n"); break;
    case TInterruptManager::TThreadState::kDeleting:
        KPrintf("TInterruptManager: kDeleting, deleteing thread\n"); break;
    }
#if TARGET_OS_WIN32
    // Insert Windows specific drivers here
#else
    // TODO: implement other platforms here: Android, macOS, iOS, Linux
    KPrintf("(not supported on this platform)\n");
#endif
    KPrintf("--- end\n");
}

// =================================== //
// The world is not octal despite DEC. //
// =================================== //
