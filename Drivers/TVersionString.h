// ==============================
// File:            TVersionString.h
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

#ifndef _TVERSIONSTRING_H
#define _TVERSIONSTRING_H

#include <Newton.h>
#include <Protocols.h>

///
/// Protocol for the (optional) version string.
/// This string is only used if the system version is not the ROM version (currently 2.2).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test   aucun test défini.
///
PROTOCOL TVersionString : public TProtocol
{
public:
    static  TVersionString*     New( char* inImplementationName );  // PROTOCOL ctor
            void                Delete( void );                     // PROTOCOL dtor
            void                VersionString( UShort* outVersionString );
};

#endif
        // _TVERSIONSTRING_H

// ============================================================== //
// Unix will self-destruct in five seconds... 4... 3... 2... 1... //
// ============================================================== //
