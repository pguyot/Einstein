// ==============================
// File:            PInTranslator.h
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

#ifndef _PINTRANSLATOR_H
#define _PINTRANSLATOR_H

#include <Newton.h>
#include <Objects.h>
#include <Protocols.h>

///
/// Classe pour .
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 117 $
///
/// \test   aucun test défini.
///
PROTOCOL PInTranslator : public TProtocol
{
public:
    static  PInTranslator*  New( char* inImplementationName );  // PROTOCOL ctor
            void            Delete( void );                     // PROTOCOL dtor

    ULong                   Init(void*);
    ULong                   Idle( void );
    ULong                   FrameAvailable( void );
    Ref                     ProduceFrame( int );
};

#endif
        // _PINTRANSLATOR_H

// ======================================================================== //
// Trying to establish voice contact ... please ____    yell into keyboard. //
// ======================================================================== //
