// ==============================
// File:			TSerialHostPortDirect.cpp
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
#include <termios.h>
#include <unistd.h>
}

#include "TSerialHostPortDirect.h"
#include <Emulator/TEmulator.h>
#include <Emulator/TInterruptManager.h>

TSerialHostPortDirect::TSerialHostPortDirect(
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
		mPath = std::string("/dev/cu.usbserial");
	}
	mFile = open(mPath.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mFile != -1)
	{
		struct termios options;
		mSerialStatus = 0;

		tcgetattr(mFile, &options);

		options.c_oflag &= ~OPOST;
		options.c_iflag &= ~INLCR & ~ICRNL & ~INPCK;
		options.c_cflag |= IXON | IXOFF;
		options.c_iflag |= IGNPAR;
		options.c_cflag &= ~PARENB & ~CSTOPB & ~CSIZE & ~CRTSCTS & ~IXANY;

		options.c_cc[VMIN] = 0;
		options.c_cc[VTIME] = 10;
		options.c_cflag |= CLOCAL | CREAD | HUPCL | CS8;
		options.c_lflag = 0;

		cfsetspeed(&options, 115200);
		tcflush(mFile, TCIFLUSH);
		tcsetattr(mFile, TCSANOW, &options);
		mLog->FLogLine("TSerialHostPortDirect: port opened: %p", this);
		mReadMutex = new TMutex();
		mReadBuffer = new TCircleBuffer(8 * 1024);
		mIOThread = new TThread(this);
	} else
	{
		mSerialStatus = kSerialChipGone;
	}
	AssignInterruptID();
}

TSerialHostPortDirect::~TSerialHostPortDirect()
{
	if (mFile != -1)
	{
		close(mFile);
	}
	delete mReadMutex;
	delete mReadBuffer;
	delete mIOThread;
}

void
TSerialHostPortDirect::PutByte(KUInt8 nextChar)
{
	write(mFile, &nextChar, 1);
	mSerialStatus &= ~kSerialTxBufferEmpty;
	mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
}

KUInt8
TSerialHostPortDirect::GetByte()
{
	KUInt8 data;
	mReadMutex->Lock();
	mReadBuffer->Consume(&data, sizeof(data));
	if (mReadBuffer->IsEmpty())
	{
		mSerialStatus &= ~kSerialRxCharAvailable;
	}
	mReadMutex->Unlock();
	return data;
}

void
TSerialHostPortDirect::SetSerialOutputs(SerialOutputControl)
{
}

void
TSerialHostPortDirect::ClearSerialOutputs(SerialOutputControl)
{
}

void
TSerialHostPortDirect::SetInterruptEnable(bool /* enable */)
{
}

void
TSerialHostPortDirect::Reset()
{
}

void
TSerialHostPortDirect::SetBreak(bool /* assert */)
{
}

InterfaceSpeed
TSerialHostPortDirect::SetSpeed(BitRate /* bitsPerSec */)
{
	return 0;
}

void
TSerialHostPortDirect::SetIOParms(TCMOSerialIOParms* /* opt */)
{
}

void
TSerialHostPortDirect::Reconfigure()
{
}

KSInt32
TSerialHostPortDirect::InitByOption(TOption* /* initOpt */)
{
	return 0;
}

KSInt32
TSerialHostPortDirect::ProcessOption(TOption* /* opt */)
{
	return 0;
}

KSInt32
TSerialHostPortDirect::SetSerialMode(SerialMode /* mode */)
{
	return 0;
}

RxErrorStatus
TSerialHostPortDirect::GetByteAndStatus(KUInt8* /* nextCharPtr */)
{
	return 0;
}

bool
TSerialHostPortDirect::AllSent()
{
	return true;
}

KSInt32
TSerialHostPortDirect::WaitForAllSent()
{
	return 0;
}

void
TSerialHostPortDirect::Run()
{
	fd_set fdSet;
	while (true)
	{
		mLog->LogLine("TSerialHostPortDirect: Waiting for data...");
		FD_ZERO(&fdSet);
		FD_SET(mFile, &fdSet);
		if (select(FD_SETSIZE, &fdSet, 0, 0, 0) != -1 && FD_ISSET(mFile, &fdSet))
		{
			KUInt8 data;
			mReadMutex->Lock();
			if (mLog)
			{
				mLog->LogLine("TSerialHostPortDirect: data available");
			}
			while (read(mFile, &data, sizeof(data)) == sizeof(data))
			{
				mReadBuffer->Produce(&data, sizeof(data));
			}
			mReadMutex->Unlock();
			mSerialStatus |= kSerialRxCharAvailable;
			mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
		}
	}
}
