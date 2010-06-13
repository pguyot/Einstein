// ==============================
// File:			TUsermodeNetwork.h
// Project:			Einstein
//
// Copyright 2010 by Matthias Melcher (mm@matthiasm.com).
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

#ifndef _TUSERMODENETWORK_H
#define _TUSERMODENETWORK_H

#include "TNetworkManager.h"
#include <K/Defines/KDefinitions.h>


class TLog;
class TInterruptManager;
class TMemoryManager;

class PacketHandler;
class Packet;


///
/// Emulation of a PCMCIA network card on PCMCIA driver level (see Lantern)
///
/// \author Matthias Melcher <mm@matthiasm.com>
///
class TUsermodeNetwork : public TNetworkManager
{
public:
	TUsermodeNetwork(TLog* inLog);
	
	///
	/// Destructor.
	///
	virtual ~TUsermodeNetwork();

	///
	/// Newton sends a block of data.
	/// This is a raw ethernet network datagram. The network manager is expected
	/// to somehow send this package to the outside world.
	///
	/// \param data send this block of data from the Newton to the world
	/// \param size of the block
	///
	virtual int SendPacket(KUInt8 *data, KUInt32 size);
	
	///
	/// Fill the buffer with the MAC address of the network card.
	///
	/// \param data pointer to a six byte buffer
	/// \param size ethernet MAC addresses are always 6 bytes
	///
	virtual int GetDeviceAddress(KUInt8 *data, KUInt32 size);
	
	///
	/// Number of bytes available for Newton.
	/// This number is polled on a regular base. If no block is available,
	/// return 0. If a block of data is waiting, return the size of the raw
	/// ethernet datagram. Do not split blocks of data unless you create a
	/// complete rwa ethernet datagramm for each of them.	
	///
	/// \return the number of bytes in the first block that is available for the Newton
	///
	virtual KUInt32 DataAvailable();
	
	///
	/// Newton receives a block of data.
	/// Copy the block that was received from the outside world int this buffer.
	/// The Newton expects a raw ethernet datagramm. The size will always be
	/// whatever the menager returned in a previous call to DataAvailable().
	///
	/// \param data fill this buffer with the next available block of data
	/// \param size the number of bytes that we expect in the buffer
	///
	virtual int ReceiveData(KUInt8 *data, KUInt32 size);
	
	///
	/// Newton device timer expired.
	/// Out of lazyness and for testing only, we use this timer to poll the open
	/// sockets of all active protocols.
	///
	int TimerExpired();
	
	void AddPacketHandler(PacketHandler*);
	
	void RemovePacketHandler(PacketHandler*);
	
	void Enqueue(Packet*);
	
	void DropPacket();
	
private:
	PacketHandler	*mFirstPacketHandler, *mLastPacketHandler;
	Packet			*mFirstPacket, *mLastPacket;
};

#endif
		// _TUSERMODENETWORK_H

// ============================================ //
// The first time, it's a KLUDGE!               //
// The second, a trick.                         //
// Later, it's a well-established technique!    //
//                 -- Mike Broido, Intermetrics //
// ============================================ //
