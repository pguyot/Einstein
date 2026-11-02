// ==============================
// File:			TSerialHostPortPTYBasilisk.cpp
// Project:			Einstein
//
// Copyright 2026 Eckhart Koeppen
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

extern "C" {
#include <sys/ioctl.h>
#include <termios.h>
}

#include "TSerialHostPortPTYBasilisk.h"
#include "Emulator/Log/TLog.h"

TSerialHostPortPTYBasilisk::TSerialHostPortPTYBasilisk(
	TLog* inLog,
	KUInt32 inLocation,
	const std::string& inPath,
	TEmulator* inEmulator) :
		TSerialHostPortPTY(inLog, inLocation, inPath, inEmulator, /* inStartWorker = */ false)
{
	// ConfigureMaster() called from TSerialHostPortPTY constructor resolves to
	// base method; re-run derived configuration now that object is fully constructed.
	{
		std::lock_guard<std::mutex> lock(mPortMutex);
		if (mMain != -1)
			ConfigureMaster();
	}
	StartWorker();
}

void
TSerialHostPortPTYBasilisk::ConfigureMaster()
{
	int packetMode = 1;
	mPacketMode = (ioctl(mMain, TIOCPKT, &packetMode) == 0);
	if (mLog && !mPacketMode)
		mLog->FLogLine("TSerialHostPortPTYBasilisk: TIOCPKT unavailable on %s; Basilisk workaround disabled", mPath.c_str());
	mFlushDetectArmed = false;
	mSawSetupFlush = false;
}

bool
TSerialHostPortPTYBasilisk::ProcessIncomingBytes(KUInt8* inData, int inCount, bool& outResetRequested)
{
	outResetRequested = false;
	if (inCount <= 0)
		return false;

	if (mPacketMode)
	{
		if (inData[0] != 0)
		{
			// Basilisk's KillIO can wait forever on a blocking read.
			// As in the legacy driver, infer closure from a second
			// combined flush and disconnect the PTY to release it.
			if (!mFlushDetectArmed)
			{
				if (inData[0] & TIOCPKT_NOSTOP)
					mFlushDetectArmed = true;
			} else if (mFlushDetectArmed
				&& (inData[0] & (TIOCPKT_FLUSHREAD | TIOCPKT_FLUSHWRITE))
					== (TIOCPKT_FLUSHREAD | TIOCPKT_FLUSHWRITE))
			{
				if (!mSawSetupFlush)
				{
					mSawSetupFlush = true;
				} else
				{
					outResetRequested = true;
				}
			}
			return false;
		}
		if (inCount == 1)
			return false;
		mReadBuffer->Produce(inData + 1, inCount - 1);
		return true;
	}

	mReadBuffer->Produce(inData, inCount);
	return true;
}
