// ==============================
// File:            TStartupDriver.h
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

#ifndef _TSTARTUPDRIVER_H
#define _TSTARTUPDRIVER_H

#include <Newton.h>
#include <Protocols.h>

///
/// Protocol for the (optional) startup driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test   aucun test défini.
///
PROTOCOL TStartupDriver : public TProtocol
{
public:
    void    Init( void );   // Entry point
};

#endif
        // _TSTARTUPDRIVER_H

// ============================================================================= //
// The best way to accelerate a Macintoy is at 9.8 meters per second per second. //
// ============================================================================= //
