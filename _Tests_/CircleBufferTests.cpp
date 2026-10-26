// ==============================
// File:			CircleBufferTests.cpp
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

#include <K/Misc/TCircleBuffer.h>
#include <gtest/gtest.h>

#include <vector>

// The sound managers queue the samples of NewtonOS in a TCircleBuffer. These
// tests produce a numbered stream of bytes and check that it comes out
// complete and in order, also when the buffer wraps around, fills up exactly,
// or must grow.

namespace {

// Produce inAmount bytes of the stream, counting from ioNext.
void
ProduceStream(TCircleBuffer& ioBuffer, KUIntPtr inAmount, unsigned& ioNext)
{
	std::vector<KUInt8> theData(inAmount);
	for (auto& b : theData)
		b = (KUInt8) (ioNext++);
	ioBuffer.Produce(theData.data(), inAmount);
}

// Consume up to inAmount bytes and check that they continue the stream.
// Returns the number of bytes consumed.
KUIntPtr
ConsumeStream(TCircleBuffer& ioBuffer, KUIntPtr inAmount, unsigned& ioNext)
{
	std::vector<KUInt8> theData(inAmount);
	KUIntPtr theAmount = ioBuffer.Consume(theData.data(), inAmount);
	for (KUIntPtr i = 0; i < theAmount; i++)
	{
		EXPECT_EQ(theData[i], (KUInt8) ioNext) << "at byte " << i;
		ioNext++;
	}
	return theAmount;
}

} // namespace

TEST(CircleBuffer, StartsEmpty)
{
	TCircleBuffer theBuffer(16);
	EXPECT_TRUE(theBuffer.IsEmpty());
	EXPECT_EQ(theBuffer.AvailableBytes(), 0u);
	KUInt8 theByte;
	EXPECT_EQ(theBuffer.Consume(&theByte, 1), 0u);
}

TEST(CircleBuffer, FillToCapacity)
{
	// Filling the whole capacity must not make the buffer look empty.
	TCircleBuffer theBuffer(16);
	unsigned produced = 0, consumed = 0;
	ProduceStream(theBuffer, 16, produced);
	EXPECT_FALSE(theBuffer.IsEmpty());
	EXPECT_EQ(theBuffer.AvailableBytes(), 16u);
	EXPECT_EQ(ConsumeStream(theBuffer, 16, consumed), 16u);
	EXPECT_TRUE(theBuffer.IsEmpty());
}

TEST(CircleBuffer, FillToCapacityAfterWrap)
{
	// The same with the cursors in the middle of the buffer.
	TCircleBuffer theBuffer(16);
	unsigned produced = 0, consumed = 0;
	ProduceStream(theBuffer, 10, produced);
	EXPECT_EQ(ConsumeStream(theBuffer, 10, consumed), 10u);
	ProduceStream(theBuffer, 16, produced);
	EXPECT_EQ(theBuffer.AvailableBytes(), 16u);
	EXPECT_EQ(ConsumeStream(theBuffer, 16, consumed), 16u);
	EXPECT_TRUE(theBuffer.IsEmpty());
}

TEST(CircleBuffer, ProduceIntoWrappedBuffer)
{
	// The producer is behind the consumer: new data must not overwrite the
	// data that was not consumed yet.
	TCircleBuffer theBuffer(16);
	unsigned produced = 0, consumed = 0;
	ProduceStream(theBuffer, 12, produced);
	EXPECT_EQ(ConsumeStream(theBuffer, 8, consumed), 8u);
	ProduceStream(theBuffer, 8, produced); // wraps around
	ProduceStream(theBuffer, 6, produced); // producer behind the consumer
	EXPECT_EQ(theBuffer.AvailableBytes(), 18u);
	EXPECT_EQ(ConsumeStream(theBuffer, 100, consumed), 18u);
	EXPECT_EQ(consumed, produced);
}

TEST(CircleBuffer, ConsumeMoreThanAvailableAcrossWrap)
{
	TCircleBuffer theBuffer(16);
	unsigned produced = 0, consumed = 0;
	ProduceStream(theBuffer, 14, produced);
	EXPECT_EQ(ConsumeStream(theBuffer, 12, consumed), 12u);
	ProduceStream(theBuffer, 6, produced); // wraps around
	EXPECT_EQ(ConsumeStream(theBuffer, 50, consumed), 8u);
	EXPECT_TRUE(theBuffer.IsEmpty());
	ProduceStream(theBuffer, 5, produced);
	EXPECT_EQ(ConsumeStream(theBuffer, 50, consumed), 5u);
}

TEST(CircleBuffer, SoundLikeTraffic)
{
	// NewtonOS produces whole buffers of 0xEA0 bytes, CoreAudio consumes
	// smaller pieces of changing size, sometimes far behind.
	TCircleBuffer theBuffer(0xEA0 * 4);
	unsigned produced = 0, consumed = 0;
	for (int round = 0; round < 500; round++)
	{
		int buffers = 1 + (round * 7) % 5;
		for (int i = 0; i < buffers; i++)
			ProduceStream(theBuffer, 0xEA0, produced);
		KUIntPtr piece = 512 + (round * 131) % 1500;
		while (theBuffer.AvailableBytes() > (KUIntPtr) (round % 3) * 0xEA0)
			ConsumeStream(theBuffer, piece, consumed);
		ASSERT_EQ(theBuffer.AvailableBytes(), (KUIntPtr) (produced - consumed));
	}
	ConsumeStream(theBuffer, produced - consumed, consumed);
	EXPECT_TRUE(theBuffer.IsEmpty());
	EXPECT_EQ(consumed, produced);
}
