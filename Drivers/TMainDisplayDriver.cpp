// ==============================
// File:            TMainDisplayDriver.cp
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

#include "TMainDisplayDriver.impl.h"

PROTOCOL_IMPL_SOURCE_MACRO(TMainDisplayDriver)  // Magic stuff, do not touch

// ------------------------------------------------------------------------ //
// ScreenSetup( void )
// ------------------------------------------------------------------------ //
ULong
TMainDisplayDriver::ScreenSetup( void )
{
    // Fill & register Gestalt records.
    TUGestalt theGestalt;
    mGestaltSoftContrast.fHasSoftContrast = false;
    mGestaltSoftContrast.fMinContrast = 0;
    mGestaltSoftContrast.fMaxContrast = 0;
    (void) theGestalt.RegisterGestalt(
                kGestalt_SoftContrast,
                &mGestaltSoftContrast,
                sizeof(mGestaltSoftContrast));

    mGestaltBacklight.fHasBacklight = true;
    (void) theGestalt.RegisterGestalt(
                kGestalt_Ext_Backlight,
                &mGestaltBacklight,
                sizeof(mGestaltBacklight));

    return 0;
}

// ====================================================================== //
// The IBM purchase of ROLM gives new meaning to the term "twisted pair". //
//                 -- Howard Anderson, "Yankee Group"                     //
// ====================================================================== //
