// ==============================
// File:            PEinsteinOutTranslator.cp
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

#include "PEinsteinOutTranslator.impl.h"
#include <stdio.h>
#include <stdarg.h>

PROTOCOL_IMPL_SOURCE_MACRO(PEinsteinOutTranslator)  // Magic stuff, do not touch

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * Print( const char* inFormat, ... )
// -------------------------------------------------------------------------- //
ULong
PEinsteinOutTranslator::Print( const char* inFormat, ... )
{
    va_list ap;
    va_start(ap, inFormat);
    char theBuffer[1024];
    ULong count = ::vsprintf(theBuffer, inFormat, ap);
    va_end(ap);
    DoPrint(theBuffer);
    return count;
}


// =============================================================================== //
// ===  ALL USERS PLEASE NOTE  ========================                            //
//                                                                                 //
// The garbage collector now works.  In addition a new, experimental garbage       //
// collection algorithm has been installed.  With SI:%DSK-GC-QLX-BITS set to 17,   //
// (NOT the default) the old garbage collection algorithm remains in force; when   //
// virtual storage is filled, the machine cold boots itself.  With SI:%DSK-GC-     //
// QLX-BITS set to 23, the new garbage collector is enabled.  Unlike most garbage  //
// collectors, the new gc starts its mark phase from the mind of the user, rather  //
// than from the obarray.  This allows the garbage collection of significantly     //
// more Qs.  As the garbage collector runs, it may ask you something like "Do you  //
// remember what SI:RDTBL-TRANS does?", and if you can't give a reasonable answer  //
// in thirty seconds, the symbol becomes a candidate for GCing.  The variable      //
// SI:%GC-QLX-LUSER-TM governs how long the GC waits before timing out the user.   //
// =============================================================================== //
