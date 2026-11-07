// ==============================
// File:			TSerialHostPortPTY.cpp
// Project:			Einstein
//
// Copyright 2020 Eckhart Koeppen
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
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
}

#include <sstream>

#include "TSerialHostPortPTY.h"
#include <Emulator/TEmulator.h>
#include <Emulator/TInterruptManager.h>

/// Create the PTY file, using the port location as part of the file name.
TSerialHostPortPTY::TSerialHostPortPTY(
	TLog* inLog,
	KUInt32 inLocation,
	const std::string& inPath,
	TEmulator* inEmulator,
	bool inStartWorker) :
		TSerialHostPort(inLog, inLocation, inEmulator)
{
	if (inPath.length() > 0)
	{
		mPath = inPath;
	} else
	{
		std::stringstream s;
		s << "/tmp/einstein-" << std::hex << inLocation << ".pty";
		mPath = s.str();
	}
	AssignInterruptID();
	CreatePTY();
	mReadMutex = new TMutex();
	mReadBuffer = new TCircleBuffer(8 * 1024);
	mSerialStatus = kSerialTxBufferEmpty;
	if (inStartWorker)
	{
		StartWorker();
	}
}

void
TSerialHostPortPTY::StartWorker()
{
	if (!mIOThread.joinable())
	{
		mIOThread = std::thread(&TSerialHostPortPTY::Run, this);
	}
}

TSerialHostPortPTY::~TSerialHostPortPTY()
{
	mStopRequested = true;
	if (mIOThread.joinable())
		mIOThread.join();
	unlink(mPath.c_str());
	if (mMain != -1)
	{
		close(mMain);
	}
	if (mRemote != -1)
	{
		close(mRemote);
	}
	delete mReadMutex;
	delete mReadBuffer;
}

void
TSerialHostPortPTY::ConfigureMaster()
{
	// Base PTY runs in standard POSIX raw mode; no special packet mode.
}

void
TSerialHostPortPTY::CreatePTY()
{
	unlink(mPath.c_str());
	mMain = posix_openpt(O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mMain == -1)
	{
		if (mLog) mLog->FLogLine("TSerialHostPortPTY: posix_openpt failed for %s", mPath.c_str());
		return;
	}
	if (grantpt(mMain) == -1 || unlockpt(mMain) == -1)
	{
		if (mLog) mLog->FLogLine("TSerialHostPortPTY: grantpt/unlockpt failed for %s", mPath.c_str());
		close(mMain);
		mMain = -1;
		return;
	}
	const char* remoteName = ptsname(mMain);
	if (remoteName != nullptr)
		mRemote = open(remoteName, O_RDWR | O_NOCTTY);
	if (mRemote == -1 || symlink(remoteName, mPath.c_str()) == -1)
	{
		if (mLog) mLog->FLogLine("TSerialHostPortPTY: slave open/link failed for %s", mPath.c_str());
		if (mRemote != -1) close(mRemote);
		close(mMain);
		mMain = mRemote = -1;
		return;
	}

	struct termios options;
	tcgetattr(mMain, &options);
	cfmakeraw(&options);
	options.c_cflag |= HUPCL | CLOCAL;
	tcsetattr(mMain, TCSANOW, &options);

	tcgetattr(mRemote, &options);
	cfmakeraw(&options);
	options.c_cflag |= HUPCL | CLOCAL;
	tcsetattr(mRemote, TCSANOW, &options);

	ConfigureMaster();

	if (mLog)
	{
		mLog->FLogLine("TSerialHostPortPTY: PTY opened %s -> %s",
			mPath.c_str(), remoteName);
	}
}

void
TSerialHostPortPTY::TeardownAndRecreatePTY()
{
	if (mLog)
		mLog->FLogLine("TSerialHostPortPTY: resetting %s", mPath.c_str());
	{
		std::lock_guard<std::mutex> lock(mPortMutex);
		unlink(mPath.c_str());
		close(mMain);
		close(mRemote);
		mMain = mRemote = -1;
	}
	{
		mReadMutex->Lock();
		if (mReadBuffer)
		{
			mReadBuffer->Discard(mReadBuffer->AvailableBytes());
		}
		mSerialStatus &= ~kSerialRxCharAvailable;
		mReadMutex->Unlock();
	}
	usleep(500000);
	if (mStopRequested)
		return;
	{
		std::lock_guard<std::mutex> lock(mPortMutex);
		CreatePTY();
	}
}

bool
TSerialHostPortPTY::ProcessIncomingBytes(KUInt8* inData, int inCount, bool& outResetRequested)
{
	outResetRequested = false;
	if (inCount <= 0)
		return false;
	mReadBuffer->Produce(inData, inCount);
	return true;
}

void
TSerialHostPortPTY::PutByte(KUInt8 nextChar)
{
	{
		std::lock_guard<std::mutex> lock(mPortMutex);
		if (mMain != -1)
			write(mMain, &nextChar, 1);
	}
	mSerialStatus |= kSerialTxBufferEmpty;
	mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
}

KUInt8
TSerialHostPortPTY::GetByte()
{
	KUInt8 data;
	mReadMutex->Lock();
	mReadBuffer->Consume(&data, sizeof(data));
	if (mReadBuffer->IsEmpty())
	{
		mSerialStatus &= ~kSerialRxCharAvailable;
	} else
	{
		mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
	}
	mReadMutex->Unlock();
	return data;
}

void
TSerialHostPortPTY::SetInterruptEnable(bool /* enable */)
{
}

void
TSerialHostPortPTY::Reset()
{
}

void
TSerialHostPortPTY::SetBreak(bool /* assert */)
{
}

InterfaceSpeed
TSerialHostPortPTY::SetSpeed(BitRate /* bitsPerSec */)
{
	return 0;
}

void
TSerialHostPortPTY::SetIOParms(TCMOSerialIOParms* /* opt */)
{
}

void
TSerialHostPortPTY::Reconfigure()
{
}

KSInt32
TSerialHostPortPTY::InitByOption(TOption* /* initOpt */)
{
	return 0;
}

KSInt32
TSerialHostPortPTY::ProcessOption(TOption* /* opt */)
{
	return 0;
}

KSInt32
TSerialHostPortPTY::SetSerialMode(SerialMode /* mode */)
{
	return 0;
}

RxErrorStatus
TSerialHostPortPTY::GetByteAndStatus(KUInt8* /* nextCharPtr */)
{
	return 0;
}

bool
TSerialHostPortPTY::AllSent()
{
	return true;
}

KSInt32
TSerialHostPortPTY::WaitForAllSent()
{
	return 0;
}

void
TSerialHostPortPTY::Run()
{
	fd_set fdSet;
	while (!mStopRequested)
	{
		if (mMain == -1)
		{
			usleep(500000);
			if (mStopRequested) break;
			std::lock_guard<std::mutex> lock(mPortMutex);
			CreatePTY();
			continue;
		}
		// Backpressure: while the Newton holds RTS low, received bytes pile
		// up here. Stop reading once enough are waiting, so the PTY's own
		// buffer fills and the writer on the host side blocks.
		mReadMutex->Lock();
		bool bufferFull = mReadBuffer->AvailableBytes() >= kMaxReadBufferSize;
		mReadMutex->Unlock();
		if (bufferFull)
		{
			usleep(10000);
			continue;
		}

		FD_ZERO(&fdSet);
		FD_SET(mMain, &fdSet);
		struct timeval timeout { 0, 100000 }; // allow destructor to join
		if (select(mMain + 1, &fdSet, 0, 0, &timeout) > 0 && FD_ISSET(mMain, &fdSet))
		{
			KUInt8 data[129];
			int n;
			bool receivedData = false;
			bool resetPTY = false;
			mReadMutex->Lock();
			while (mReadBuffer->AvailableBytes() < kMaxReadBufferSize
				&& (n = (int) read(mMain, data, sizeof(data))) > 0)
			{
				bool resetRequested = false;
				if (ProcessIncomingBytes(data, n, resetRequested))
				{
					receivedData = true;
				}
				if (resetRequested)
				{
					resetPTY = true;
					break;
				}
			}
			// Set the flag before unlocking, so that GetByte() can't see it
			// without the data.
			if (receivedData)
			{
				mSerialStatus |= kSerialRxCharAvailable;
			}
			mReadMutex->Unlock();
			if (receivedData)
			{
				mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
			}
			if (resetPTY)
			{
				TeardownAndRecreatePTY();
			}
		}
	}
}
