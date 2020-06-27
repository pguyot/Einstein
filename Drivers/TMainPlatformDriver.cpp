// ==============================
// File:            TMainPlatformDriver.cp
// Project:         Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
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

#include "TMainPlatformDriver.impl.h"
#include <NameServer.h>
#include <NewtonMemory.h>
#include <UserPorts.h>
#include <UserTasks.h>

#include "K/Misc/RelocHack.h"
#include "UGlue.h"

PROTOCOL_IMPL_SOURCE_MACRO(TMainPlatformDriver) // Magic stuff, do not touch

#include "Emulator/Platform/PlatformEvents.h"
#include "Emulator/Platform/PlatformGestalt.h"

typedef long (*InterruptHandlerProcPtr)(void*);

// All these functions are in Public JT.
struct InterruptObject;
InterruptObject* RegisterInterrupt(
            ULong inInterruptMask,
            void* inCookie,
            InterruptHandlerProcPtr inHandler,
            ULong inFlags);
void DeregisterInterrupt(InterruptObject*);
extern "C" long EnableInterrupt(InterruptObject*, ULong);
extern "C" long DisableInterrupt(InterruptObject*);
extern "C" long ClearInterrupt(InterruptObject*);
class TGPIOInterface;
TGPIOInterface* GetGPIOInterfaceObject( void );
TUPort* GetNewtTaskPort( void );

// Functions not in public JT
TUPort* GetPowerPort( void );

struct GPIOIntObject;

typedef void (*GPIOInterruptHandlerProcPtr)(void*);

class TGPIOInterface
{
public:
    GPIOIntObject* RegisterInterrupt(
                        UChar inFunction,
                        void* inUserCon,
                        GPIOInterruptHandlerProcPtr inProcPtr,
                        ULong inFlags );

    void            EnableInterrupt( GPIOIntObject* inInterrupt );
};

#define kPowerSwitchGPIOFunc    0

struct SEinsteinPlatformGlobals
{
    TMainPlatformDriver*    fDriver;        ///< Instance of the driver.
};

#define kGlobalsName "Globals"
#define kGlobalsType "Einstein:Kallisys"

//-------------------------------------------------------------------------- //
//  * New( void )
// -------------------------------------------------------------------------- //
TPlatformDriver*
TMainPlatformDriver::New( void )
{
    // Make chip registry fail.
    TUNameServer theServer;
    (void) theServer.RegisterName( "extr", "TSerialChip", 0, 0 );
    (void) theServer.RegisterName( "infr", "TSerialChip", 0, 0 );
    (void) theServer.RegisterName( "tblt", "TSerialChip", 0, 0 );
    (void) theServer.RegisterName( "mdem", "TSerialChip", 0, 0 );

    mAsyncMessage = new TUAsyncMessage();
    mAsyncMessage->Init( true /* forSendRpc */ );
    mEvent = (SEvent*) NewWiredPtr( sizeof(SEvent) );

    mPowerPortId = 0;
    mNewtPortId = 0;

    // Create Gestalt entry for the version.
    TUGestalt theGestaltIntf;
    SGestaltEinsteinEmulatorInfo* theInfo = new SGestaltEinsteinEmulatorInfo;
    FillGestaltEmulatorInfo( theInfo );
    theGestaltIntf.RegisterGestalt( kGestalt_Einstein_EmulatorInfo, theInfo, sizeof(*theInfo) );

    // Create Gestalt entry for the globals.
    TUNameServer theNameServerIntf;
    SEinsteinPlatformGlobals* theGlobals = new SEinsteinPlatformGlobals;
    theGlobals->fDriver = this;
    (void) theNameServerIntf.RegisterName(
                kGlobalsName,
                kGlobalsType,
                (ULong) theGlobals,
                0 );

    // Grab the interface for native calls.
    mNativeCalls = (TNativeCalls*) NewByName("TNativeCalls", "TEinsteinNativeCalls");

    // Create the port (that will unlock the event queue).
    mPort = new TUPort();
    mPort->Init();
    (void) mAsyncMessage->SetCollectorPort(mPort->fId);

    // Create the task.
    TPlatformDriver* thisPtr = this;
    mTask = new TUTask();
    mTask->Init(
#if __GNUC__
// We need RelocFuncPtr if the package is not properly relocated in the Rex
// which is the case with MPW's Rex tool/Packer/NCT toolchain
            (TaskProcPtr) &STaskProc,
#else
            (TaskProcPtr) RelocFuncPtr(&STaskProc),
#endif
            kSpawnedTaskStackSize,
            sizeof(thisPtr),
            &thisPtr,
            kKernelTaskPriority,
            'drvr');
    mTask->Start();

    return this;
}

// -------------------------------------------------------------------------- //
//  * RegisterPowerSwitchInterrupt( void )
// -------------------------------------------------------------------------- //
NewtonErr
TMainPlatformDriver::RegisterPowerSwitchInterrupt( void )
{
    mInterrupt = GetGPIOInterfaceObject()->RegisterInterrupt(
            kPowerSwitchGPIOFunc,
            this,
            (GPIOInterruptHandlerProcPtr)
#if __GNUC__
// Cf above
                &SInterruptHandler,
#else
                RelocFuncPtr(&SInterruptHandler),
#endif
            0 );

    return noErr;
}


// -------------------------------------------------------------------------- //
//  * EnableSysPowerInterrupt( void )
// -------------------------------------------------------------------------- //
NewtonErr
TMainPlatformDriver::EnableSysPowerInterrupt( void )
{
    GetGPIOInterfaceObject()->EnableInterrupt(mInterrupt);

    return noErr;
}

// -------------------------------------------------------------------------- //
//  * Delete( void )
// -------------------------------------------------------------------------- //
void
TMainPlatformDriver::Delete( void )
{
//  DeregisterInterrupt(mInterrupt);

    if (mEvent)
    {
        DisposeWiredPtr( (Ptr) mEvent );
    }

    if (mTask)
    {
        mTask->Suspend();
        delete mTask;
    }
}

// ------------------------------------------------------------------------ //
// SInterruptHandler( void* )
// ------------------------------------------------------------------------ //
NewtonErr
TMainPlatformDriver::SInterruptHandler( void* inUserData )
{
    return ((TMainPlatformDriver*) inUserData)->InterruptHandler();
}

// ------------------------------------------------------------------------ //
// InterruptHandler( void )
// ------------------------------------------------------------------------ //
NewtonErr
TMainPlatformDriver::InterruptHandler( void )
{
    LockEventQueue();

    if (GetNextEvent(mEvent))
    {
        switch (mEvent->fType)
        {
            case kAEvent:
                {
                    void* theEvent = mEvent->fData.aevent.fData;
                    ULong theSize  = mEvent->fData.aevent.fSize;
                    (void) SendForInterrupt(
                        GetPortIdFromName(mEvent->fData.aevent.fPort),
                        mAsyncMessage->GetMsgId(),
                        mAsyncMessage->GetReplyMemId(),
                        theEvent,
                        theSize);
                }
                break;

            default:
                UnlockEventQueue();
                break;
        }
    } else {
        UnlockEventQueue();
    }

    return noErr;
}

// ------------------------------------------------------------------------ //
// GetPortIdFromName( ULong )
// ------------------------------------------------------------------------ //
ULong
TMainPlatformDriver::GetPortIdFromName( ULong inName )
{
    switch (inName)
    {
        case kPowerPort:
            if (mPowerPortId == 0)
            {
                mPowerPortId = GetPowerPort()->fId;
            }
            return mPowerPortId;

        case kNewtPort:
            if (mNewtPortId == 0)
            {
                mNewtPortId = GetNewtTaskPort()->fId;
            }
            return mNewtPortId;
    }

    return 0;
}

// -------------------------------------------------------------------------- //
//  * STaskProc( void* inObject, ULong inObjectSize, TObjectId inTaskID )
// -------------------------------------------------------------------------- //
void
TMainPlatformDriver::STaskProc(
                        void* inObject,
                        ULong inObjectSize,
                        TObjectId inTaskID )
{
    TMainPlatformDriver** theDriverHandle = (TMainPlatformDriver**) inObject;
    (*theDriverHandle)->TaskProc();
}

// -------------------------------------------------------------------------- //
//  * TaskProc( void )
// -------------------------------------------------------------------------- //
void
TMainPlatformDriver::TaskProc( void )
{
    while (true)
    {
        ULong returnSize;
        TUMsgToken theToken;
        (void) mPort->Receive(&returnSize, nil, 0, &theToken);
        UnlockEventQueue();
    }
}

// -------------------------------------------------------------------------- //
//  * GetPlatformDriver( void )
// -------------------------------------------------------------------------- //
TMainPlatformDriver*
TMainPlatformDriver::GetPlatformDriver( void )
{
    TUNameServer theNameServerIntf;
    SEinsteinPlatformGlobals* theGlobals;
    ULong theSpec;
    (void) theNameServerIntf.Lookup(
                kGlobalsName,
                kGlobalsType,
                (ULong*) &theGlobals,
                &theSpec );

    return theGlobals->fDriver;
}

// -------------------------------------------------------------------------- //
//  * GetNativeCalls( void )
// -------------------------------------------------------------------------- //
TNativeCalls*
TMainPlatformDriver::GetNativeCalls( void )
{
    return mNativeCalls;
}

// =========================================================================== //
// A programmer is a person who passes as an exacting expert on the basis of   //
// being able to turn out, after innumerable punching, an infinite series of   //
// incomprehensible answers calculated with micrometric precisions from vague  //
// assumptions based on debatable figures taken from inconclusive documents    //
// and carried out on instruments of problematical accuracy by persons of      //
// dubious reliability and questionable mentality for the avowed purpose of    //
// annoying and confounding a hopelessly defenseless department that was       //
// unfortunate enough to ask for the information in the first place.           //
//                 -- IEEE Grid newsmagazine                                   //
// =========================================================================== //
