// ==============================
// File:			TMemoryStream.h
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

#ifndef _TMEMORYSTREAM_H
#define _TMEMORYSTREAM_H

#include <K/Defines/KDefinitions.h>
#include <K/Exceptions/IO/TEOFException.h>
#include <K/Streams/TStream.h>

#include <algorithm>
#include <cstring>
#include <vector>

///
/// A stream in memory. Header only, so the build files of all front ends
/// don't need to know about it.
///
/// Created without data, the stream is for writing and the data grows as
/// needed. Created with data, the stream is for reading. Like TFileStream,
/// reading past the end returns fewer bytes, and the Get... methods throw an
/// EOFException.
///
class TMemoryStream : public TStream
{
public:
	///
	/// Create an empty stream for writing.
	///
	TMemoryStream()
	{
		mIsWriting = 1;
	}

	///
	/// Create a stream that reads a copy of the given data.
	///
	TMemoryStream(const void* inData, KUInt32 inSize) :
			mData((const KUInt8*) inData, (const KUInt8*) inData + inSize)
	{
		mIsReading = 1;
	}

	///
	/// Read data, advancing the cursor. Returns fewer bytes at the end.
	///
	void
	Read(void* outBuffer, KUInt32* ioCount) override
	{
		size_t theCount = std::min((size_t) *ioCount, mData.size() - mCursor);
		if (theCount > 0)
			::memcpy(outBuffer, mData.data() + mCursor, theCount);
		mCursor += theCount;
		*ioCount = (KUInt32) theCount;
	}

	///
	/// Append data.
	///
	void
	Write(const void* inBuffer, KUInt32* ioCount) override
	{
		const KUInt8* theBytes = (const KUInt8*) inBuffer;
		mData.insert(mData.end(), theBytes, theBytes + *ioCount);
	}

	///
	/// Nothing to flush.
	///
	void
	FlushOutput(void) override
	{
	}

	///
	/// Return the next byte without advancing the cursor.
	///
	KUInt8
	PeekByte(void) override
	{
		if (mCursor >= mData.size())
		{
#if HAS_EXCEPTION_HANDLING
			throw EOFException;
#else
			return 0;
#endif
		}
		return mData[mCursor];
	}

	///
	/// The data written so far, or the data to be read.
	///
	const std::vector<KUInt8>&
	GetData() const
	{
		return mData;
	}

private:
	std::vector<KUInt8> mData;
	size_t mCursor { 0 };
};

#endif
// _TMEMORYSTREAM_H
