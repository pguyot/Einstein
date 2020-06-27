// ==============================
// File:            TMainVersionString.cp
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

#include "TMainVersionString.impl.h"
#include <Unicode.h>

PROTOCOL_IMPL_SOURCE_MACRO(TMainVersionString)  // Magic stuff, do not touch

// -------------------------------------------------------------------------- //
//  * New( void )
// -------------------------------------------------------------------------- //
TVersionString*
TMainVersionString::New( void )
{
    return this;
}

// -------------------------------------------------------------------------- //
//  * Delete( void )
// -------------------------------------------------------------------------- //
void
TMainVersionString::Delete( void )
{
}

// -------------------------------------------------------------------------- //
//  * VersionString( UShort* )
// -------------------------------------------------------------------------- //
void
TMainVersionString::VersionString( UShort* outVersionString )
{
    ConvertToUnicode( "Einstein", outVersionString );
}

// ============================================================================= //
// Brace yourselves.  We're about to try something that borders on the unique:   //
// an actually rather serious technical book which is not only (gasp) vehemently //
// anti-Solemn, but also (shudder) takes sides.  I tend to think of it as        //
// `Constructive Snottiness.'                                                    //
//                 -- Mike Padlipsky, "Elements of Networking Style"             //
// ============================================================================= //
