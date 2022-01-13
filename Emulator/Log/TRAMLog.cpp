// ==============================
// File:			TRAMLog.cp
// Project:			Einstein
//
// Copyright 2014 by Paul Guyot (pguyot@kallisys.net).
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

#include "TRAMLog.h"

#include <stdlib.h>
#include <string.h>

// -------------------------------------------------------------------------- //
//  * TRAMLog()
// -------------------------------------------------------------------------- //
TRAMLog::TRAMLog()
{
    mBuffer = (char*) ::malloc(1);
    mBufferSize = 0;
}

// -------------------------------------------------------------------------- //
//  * ~TRAMLog( void )
// -------------------------------------------------------------------------- //
TRAMLog::~TRAMLog( void )
{
    free(mBuffer);
}

// -------------------------------------------------------------------------- //
//  * DoLogLine( const char* )
// -------------------------------------------------------------------------- //
void
TRAMLog::DoLogLine( const char* inLine )
{
    ssize_t len = ::strlen(inLine);
    ssize_t index = mBufferSize;
    mBufferSize += len + 1;
    mBuffer = (char*) ::realloc(mBuffer, mBufferSize + 1);
    memcpy(mBuffer + index, inLine, len);
    mBuffer[index + len] = '\n';
}
