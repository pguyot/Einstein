// ==============================
// File:            TInetService.cp
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

#include "TInetService.h"

PROTOCOL_IMPL_SOURCE_MACRO(TInetService);

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * New( void )
// -------------------------------------------------------------------------- //
TCMService*
TInetService::New( void )
{
    return this;
}

// -------------------------------------------------------------------------- //
//  * Delete( void )
// -------------------------------------------------------------------------- //
TCMService*
TInetService::Delete( void )
{
    // This space for rent.
}

// -------------------------------------------------------------------------- //
//  * Start( TOptionArray*, ULong, TServiceInfo* )
// -------------------------------------------------------------------------- //
NewtonErr
TInetService::Start(
                TOptionArray* options,
                ULong serviceId,
                TServiceInfo* serviceInfo )
{
    TUPort myPort;
    NewtonErr theErr = ServiceToPort( serviceId, &myPort );

    if (theErr == kError_Not_Registered)
    {
        TInetTool* theTool = new TInetTool( serviceId );
        theErr = StartCommTool( theTool, serviceId, serviceInfo );
        if (theErr == noErr)
        {
            theErr = OpenCommTool( serviceInfo->GetPortId(), options, this );
        }
    } else if (theErr == noErr) {
        serviceInfo->SetPortId( port.fId );
        serviceInfo->SetServiceId( serviceId );
    }

    return theErr;
}

// -------------------------------------------------------------------------- //
//  * DoneStarting( TAEvent*, ULong, TServiceInfo* )
// -------------------------------------------------------------------------- //
NewtonErr
TInetService::DoneStarting(
                TAEvent* event,
                ULong size,
                TServiceInfo* serviceInfo)
{
    return noErr;
}

// ========================== //
// Trap full -- please empty. //
// ========================== //
