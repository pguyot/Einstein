// ==============================
// File:			TResetStream.h
// Project:			Einstein
//
// Copyright 2026 by Matthias Melcher.
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

#ifndef _TRESETSTREAM_H
#define _TRESETSTREAM_H

#include <K/Defines/KDefinitions.h>
#include <K/Streams/TStream.h>

///
/// A stream that resets values instead of reading or writing them.
///
/// Walking the same TransferState() tree with a TResetStream sets every value
/// that is transferred with a reset value (TransferInt32BE(value, reset), ...)
/// to its power-on state, and keeps every value that is transferred without
/// one. No bytes are moved. Header only, so the build files of all front ends
/// don't need to know about it.
///
class TResetStream : public TStream
{
public:
	TResetStream()
	{
		mIsResetting = 1;
	}

	/// Nothing to read.
	void
	Read(void* outBuffer, KUInt32* ioCount) override
	{
		(void) outBuffer;
		*ioCount = 0;
	}

	/// Nothing is written.
	void
	Write(const void* inBuffer, KUInt32* ioCount) override
	{
		(void) inBuffer;
		(void) ioCount;
	}

	/// Nothing to flush.
	void
	FlushOutput(void) override
	{
	}

	/// Nothing to peek.
	KUInt8
	PeekByte(void) override
	{
		return 0;
	}
};

#endif
// _TRESETSTREAM_H
