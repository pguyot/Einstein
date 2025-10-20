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
	TEmulator* inEmulator) :
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
}

TSerialHostPortPTY::~TSerialHostPortPTY()
{
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
	delete mIOThread;
}

void
TSerialHostPortPTY::CreatePTY()
{
	unlink(mPath.c_str());
	mMain = posix_openpt(O_RDWR | O_NOCTTY | O_NONBLOCK);
	grantpt(mMain);
	unlockpt(mMain);
	const char* remoteName = ptsname(mMain);
	mRemote = open(remoteName, O_RDWR | O_NOCTTY);
	symlink(remoteName, mPath.c_str());

	struct termios options;
	tcgetattr(mMain, &options);
	cfmakeraw(&options);
	options.c_cflag |= HUPCL | CLOCAL;
	tcsetattr(mMain, TCSANOW, &options);

	tcgetattr(mRemote, &options);
	cfmakeraw(&options);
	options.c_cflag |= HUPCL | CLOCAL;
	tcsetattr(mRemote, TCSANOW, &options);

	if (mLog)
	{
		mLog->FLogLine("TSerialHostPortPTY: port opened: %s", mPath.c_str());
	}
	mReadMutex = new TMutex();
	mReadBuffer = new TCircleBuffer(8 * 1024);
	mSerialStatus = kSerialTxBufferEmpty;
	mIOThread = new TThread(this);
}

void
TSerialHostPortPTY::PutByte(KUInt8 nextChar)
{
	write(mMain, &nextChar, 1);
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
TSerialHostPortPTY::SetSerialOutputs(SerialOutputControl)
{
}

void
TSerialHostPortPTY::ClearSerialOutputs(SerialOutputControl)
{
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
	while (true)
	{
		if (mLog)
		{
			mLog->LogLine("TSerialHostPortPTY: Waiting for data...");
		}
		FD_ZERO(&fdSet);
		FD_SET(mMain, &fdSet);
		if (select(FD_SETSIZE, &fdSet, 0, 0, 0) != -1 && FD_ISSET(mMain, &fdSet))
		{
			KUInt8 data[128];
			int n;
			mReadMutex->Lock();
			while ((n = (int) read(mMain, data, sizeof(data))) > 0)
			{
				mReadBuffer->Produce(data, n);
			}
			if (mLog)
			{
				mLog->LogLine("TSerialHostPortPTY: Sending data...");
			}
			mReadMutex->Unlock();
			mSerialStatus |= kSerialRxCharAvailable;
			mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
		}
	}
}
