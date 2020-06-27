// ==============================
// File:            TEinsteinNativeCalls.cp
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

#include "TEinsteinNativeCalls.impl.h"

PROTOCOL_IMPL_SOURCE_MACRO(TEinsteinNativeCalls)    // Magic stuff, do not touch

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

//-------------------------------------------------------------------------- //
//  * New( void )
// -------------------------------------------------------------------------- //
TNativeCalls*
TEinsteinNativeCalls::New( void )
{
    return this;
}

// -------------------------------------------------------------------------- //
//  * Delete( void )
// -------------------------------------------------------------------------- //
void
TEinsteinNativeCalls::Delete( void )
{
}

// ============================================================================= //
// The proof that IBM didn't invent the car is that it has a steering wheel      //
// and an accelerator instead of spurs and ropes, to be compatible with a horse. //
//                 -- Jac Goudsmit                                               //
// ============================================================================= //
