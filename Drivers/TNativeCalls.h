// ==============================
// File:            TNativeCalls.h
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

#ifndef _TNATIVECALLS_H
#define _TNATIVECALLS_H

#include <Newton.h>
#include <Objects.h>
#include <Protocols.h>

///
/// Interface for native calls.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test   aucun test défini.
///
PROTOCOL TNativeCalls : public TProtocol
{
public:
    static  TNativeCalls*   New(char* inImplementationName);
            void            Delete(void);

    ULong           OpenLib(const char* inPath);
    void            CloseLib(ULong inLib);
    ULong           PrepareFFIStructure(ULong inLib, const char* inSymbol, ULong nbArgs);
    void            SetArgValue_uint8(ULong inFFIIx, ULong inArgIndex, ULong inValue);
    void            SetArgValue_sint8(ULong inFFIIx, ULong inArgIndex, ULong inValue);
    void            SetArgValue_uint16(ULong inFFIIx, ULong inArgIndex, ULong inValue);
    void            SetArgValue_sint16(ULong inFFIIx, ULong inArgIndex, ULong inValue);
    void            SetArgValue_uint32(ULong inFFIIx, ULong inArgIndex, ULong inValue);
    void            SetArgValue_sint32(ULong inFFIIx, ULong inArgIndex, ULong inValue);
//  void            SetArgValue_uint64(ULong inFFIIx, ULong inArgIndex, ULong inValue);
//  void            SetArgValue_sint64(ULong inFFIIx, ULong inArgIndex, ULong inValue);
    void            SetArgValue_float(ULong inFFIIx, ULong inArgIndex, double inValue);
    void            SetArgValue_double(ULong inFFIIx, ULong inArgIndex, double inValue);
    void            SetArgValue_longdouble(ULong inFFIIx, ULong inArgIndex, double inValue);
    void            SetArgValue_string(ULong inFFIIx, ULong inArgIndex, const char* inValue, ULong inSize);
    void            SetArgValue_binary(ULong inFFIIx, ULong inArgIndex, const void* inBinary, ULong inSize);
    void            SetArgValue_pointer(ULong inFFIIx, ULong inArgIndex, ULong inValue);
    void            SetResultType(ULong inFFIIx, ULong inType);
    void            Call_void(ULong inFFIIx);
    ULong           Call_int(ULong inFFIIx);
    double          Call_real(ULong inFFIIx);
    void            Call_string(ULong inFFIIx, char* outResult, ULong inSize);
    ULong           Call_pointer(ULong inFFIIx);
    void            GetOutArgValue_string(ULong inFFIIx, ULong inArgIndex, const char* inValue, ULong inSize);
    void            GetOutArgValue_binary(ULong inFFIIx, ULong inArgIndex, const void* inBinary, ULong inSize);
    void            DisposeFFIStructure(ULong inFFIIx);
    void            GetErrorMessage(char* outBuffer, ULong inSize);
    ULong           GetErrno();
};

#endif
        // _TNATIVECALLS_H

// ======================================================================== //
// Trying to establish voice contact ... please ____    yell into keyboard. //
// ======================================================================== //
