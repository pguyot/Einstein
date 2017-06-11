// ==============================
// File:			TTapNetwork.cp
// Project:			Einstein
//
// Copyright 2010 by Paul Guyot (pguyot@kallisys.net).
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

#include <K/Defines/KDefinitions.h>
#include "TTapNetwork.h"
#include "Emulator/Log/TLog.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sockio.h>
#include <sys/select.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Emulator/TMemory.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"

TTapNetwork::TTapNetwork(TLog* inLog) :
	TNetworkManager( inLog )
{
    mBufferSize = 0;
    mTapFileDescriptor = open("/dev/tap0", O_RDWR);
    if (mTapFileDescriptor < 0) {
        if (inLog) {
            inLog->FLogLine("Could not open tap device ! (%i)", errno);
        }
    } else {
        fcntl(mTapFileDescriptor, F_SETFL, O_NONBLOCK);
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(mTapFileDescriptor, &readSet);
        AsyncWaitForReadyToRead(mTapFileDescriptor + 1, &readSet);
    }
}


TTapNetwork::~TTapNetwork()
{
}

/*
 * This function handles packet that are sent by the Newton to the outside 
 * world.
 */
int TTapNetwork::SendPacket(KUInt8 *data, KUInt32 size)
{
    ssize_t nbWritten = write(mTapFileDescriptor, data, size);
    if (nbWritten != size && mLog) {
		mLog->FLogLine("Sending packet failed. (%i)", errno);
    }
    
    // Raise an interrupt if it sends a packet
    // (actually, we should call AsyncWaitForReadyToRead only when the card
    // is inserted).
    if (mBufferSize > 0) {
        mMemory->GetPCMCIAController(0)->RaiseInterrupt();
    }

	return 0;
}


int TTapNetwork::GetDeviceAddress(KUInt8 *data, KUInt32 size)
{
	static KUInt8 gLocalMAC[]   = { 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23 };
	assert(size==6);
	memcpy(data, gLocalMAC, 6);
	return 0;
}

KUInt32 TTapNetwork::DataAvailable()
{
    if (mBufferSize == 0) {
        ssize_t nbRead = read(mTapFileDescriptor, mBuffer, sizeof(mBuffer));
        if (nbRead > 0) {
            mBufferSize = (KUInt32)nbRead;
        }
    }
    return mBufferSize;
}


int TTapNetwork::ReceiveData(KUInt8 *data, KUInt32 size)
{
    int result;
	if (mBufferSize == size) {
		memcpy(data, mBuffer, size);
		mBufferSize = 0;
        result = 0;
	} else {
	    if (mLog) {
    		mLog->FLogLine("Tried to read %ui bytes, while %ui are available", size, mBufferSize);
	    }
		result = -1;
	}
    // Asynchronously select for next packet.
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(mTapFileDescriptor, &readSet);
    AsyncWaitForReadyToRead(mTapFileDescriptor + 1, &readSet);
    
    return result;
}

// -------------------------------------------------------------------------- //
//  * SetReadFDSet(fd_set* ioFDSet)
// -------------------------------------------------------------------------- //
int
TTapNetwork::SetReadFDSet(fd_set* ioFDSet) {
    FD_SET(mTapFileDescriptor, ioFDSet);
    return mTapFileDescriptor + 1;
}

// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
