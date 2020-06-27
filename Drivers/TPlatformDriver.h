// ==============================
// File:            TPlatformDriver.h
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

#ifndef _TPLATFORMDRIVER_H
#define _TPLATFORMDRIVER_H

#include <Newton.h>
#include <Protocols.h>

class TPlatformDriver;
TPlatformDriver* GetPlatformDriver( void );

///
/// Protocol for the platform driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test   aucun test défini.
///
PROTOCOL TPlatformDriver : public TProtocol
{
public:
    static  TPlatformDriver*    New( char* inImplementationName );  // PROTOCOL ctor
            void                Delete( void );                     // PROTOCOL dtor
            void                Init( void );

            // TVoyagerPlatformDriver doesn't do anything and returns 0.
            ULong               BacklightTrigger( void );

            // TVoyagerPlatformDriver returns 0.
            NewtonErr           RegisterPowerSwitchInterrupt( void );

            // TVoyagerPlatformDriver returns 0.
            NewtonErr           EnableSysPowerInterrupt( void );
            NewtonErr           InterruptHandler( void );
            ULong               TimerInterruptHandler( void );
            ULong               ResetZAPStoreCheck( void );
            ULong               PowerOnSubsystem( ULong inArg1 );
            ULong               PowerOffSubsystem( ULong inArg1 );
            ULong               PowerOffAllSubsystems( void );

            // TVoyagerPlatformDriver returns 0.
            ULong               PauseSystem( void );
            ULong               PowerOffSystem( void );

            // TVoyagerPlatformDriver returns 0.
            ULong               PowerOnSystem( void );

            // TVoyagerPlatformDriver returns 0, 1, 2 or 3.
            ULong               TranslatePowerEvent( ULong inArg1 );

            // inArg1:
            //  0 -> *outArg2 = 5
            //  1 -> *outArg2 = 7
            //  default: return -10005
            NewtonErr           GetPCMCIAPowerSpec( ULong inArg1, ULong* outArg2 );

            // Looks for keyboard.
            ULong               PowerOnDeviceCheck( unsigned char inArg1 );


            ULong               SetSubsystemPower( ULong, ULong );
            ULong               GetSubsystemPower( ULong, ULong* );
};

#endif
        // _TPLATFORMDRIVER_H

// =================================================== //
// The clothes have no emperor.                        //
//                 -- C.A.R. Hoare, commenting on ADA. //
// =================================================== //
