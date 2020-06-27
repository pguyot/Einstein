// ==============================
// File:            TFlashDriver.h
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

#ifndef _TFLASHDRIVER_H
#define _TFLASHDRIVER_H

#include <Newton.h>
#include <Protocols.h>

struct SFlashChipInformation;
class TFlashRange;
class TMemoryAllocator;

///
/// Protocol for Flash Drivers.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test   aucun test défini.
///
PROTOCOL TFlashDriver : public TProtocol
{
public:
    ULong   Identify(unsigned long, unsigned long, SFlashChipInformation&);
    void    CleanUp(TMemoryAllocator&);
    ULong   Init(TMemoryAllocator&);
    ULong   InitializeDriverData(TFlashRange&, TMemoryAllocator&);
    ULong   CleanUpDriverData(TFlashRange&, TMemoryAllocator&);
    ULong   StartReadingArray(TFlashRange&);
    ULong   DoneReadingArray(TFlashRange&);
    ULong   Write(unsigned long, unsigned long, unsigned long, TFlashRange&);
    ULong   StartErase(TFlashRange&, unsigned long);
    ULong   ResetBlockStatus(TFlashRange&, unsigned long);
    ULong   IsEraseComplete(TFlashRange&, unsigned long, long&);
    ULong   LockBlock(TFlashRange&, unsigned long);
    ULong   BeginWrite(TFlashRange&, unsigned long, unsigned long);
    ULong   ReportWriteResult(TFlashRange&, unsigned long);
};

#endif
        // _TFLASHDRIVER_H

// ============================================================ //
// [We] use bad software and bad machines for the wrong things. //
//                 -- R.W. Hamming                              //
// ============================================================ //
