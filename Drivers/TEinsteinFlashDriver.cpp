// ==============================
// File:            TEinsteinFlashDriver.cp
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

#include "TEinsteinFlashDriver.impl.h"

PROTOCOL_IMPL_SOURCE_MACRO(TEinsteinFlashDriver)    // Magic stuff, do not touch

struct SFlashChipInformation
{
    ULong   fUnknown_00;    // 0x89 or 0xB0 (89: 28F008SA)
    ULong   fUnknown_04;    // ID (physical[4])
    ULong   fUnknown_08;    // 0x1 or 0x2 (B0 -> 01, 89 -> 02)
    ULong   fUnknown_0C;    // 0x1 or 0x2
    ULong   fUnknown_10;    // 0x200000 or 0x100000 // address size
    ULong   fUnknown_14;    // 0x10000  // block size
};

class TMemoryAllocator;

class TFlashRange
{
public:
//  ULong                   fVTable;
    TFlashDriver*           fDriver;    // 0x04
    ULong                   fUnknown_08;    // parameters to PrepareForBlockCommand(unsigned long)
    ULong                   fUnknown_0C;
    ULong                   fUnknown_10;
    ULong                   fUnknown_14;
    SFlashChipInformation   fFlashInformation;  // 0x18-2C,
    ULong                   fUnknown_30;
    ULong                   fUnknown_34;
    ULong                   fUnknown_38;
    ULong                   fBlockSize;
    ULong                   fDriverDataPtr;
    ULong                   fUnknown_44;
    ULong                   fUnknown_48;

    virtual ULong Delete(TMemoryAllocator&);
    virtual ~TFlashRange( void );
    virtual ULong StartOfBlockWriteVirtualAddress( ULong inAddress ) const;
    virtual ULong AdjustVirtualAddresses(long);
    virtual ULong DoWrite(unsigned long, unsigned long, char*);
    virtual ULong PrepareForBlockCommand(unsigned long);
};

// -------------------------------------------------------------------------- //
//  * StartErase( TFlashRange&, ULong )
// -------------------------------------------------------------------------- //
ULong
TEinsteinFlashDriver::StartErase( TFlashRange& ioRange, ULong addr )
{
    // Retrieve the size of the block.
    return DoErase( addr, ioRange.fBlockSize );
}

// =========================== //
// Error in operator: add beer //
// =========================== //
