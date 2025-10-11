// ==============================
// File:            TUsermodeNetwork.h
// Project:            Einstein
//
// Copyright 2010-2022 by Matthias Melcher (mm@matthiasm.com).
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

#include <K/Defines/KDefinitions.h>
#include "TNetworkManager.h"

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
	static const int kMaxTxBuffer = 1448;
	static const int kMaxRxBuffer = 1448;

	///
	/// Constructor
	///
	TUsermodeNetwork(TLog* inLog);

	///
	/// Destructor.
	///
	~TUsermodeNetwork() override;

	///
	/// Newton sends a block of data.
	/// This is a raw ethernet network datagram. The network manager is expected
	/// to somehow send this package to the outside world.
	///
	/// \param data send this block of data from the Newton to the world
	/// \param size of the block
	///
	int SendPacket(KUInt8* data, KUInt32 size) override;

	///
	/// Fill the buffer with the MAC address of the network card.
	///
	/// \param data pointer to a six byte buffer
	/// \param size ethernet MAC addresses are always 6 bytes
	///
	int GetDeviceAddress(KUInt8* data, KUInt32 size) override;

	///
	/// Number of bytes available for Newton.
	/// This number is polled on a regular base. If no block is available,
	/// return 0. If a block of data is waiting, return the size of the raw
	/// ethernet datagram. Do not split blocks of data unless you create a
	/// complete raw ethernet datagramm for each of them.
	///
	/// \return the number of bytes in the first block that is available for the Newton
	///
	KUInt32 DataAvailable() override;

	///
	/// Newton receives a block of data.
	/// Copy the block that was received from the outside world into this buffer.
	/// The Newton expects a raw ethernet datagramm. The size will always(?) be
	/// whatever the menager returned in a previous call to DataAvailable().
	///
	/// \param data fill this buffer with the next available block of data
	/// \param size the number of bytes that we expect in the buffer
	///
	int ReceiveData(KUInt8* data, KUInt32 size) override;

	///
	/// Newton device timer expired.
	/// Out of lazyness and for testing only, we use this timer to poll the open
	/// sockets of all active protocols.
	///
	int TimerExpired() override;

	///
	/// Add a new packet handler to the list.
	///
	void AddPacketHandler(PacketHandler*);

	///
	/// Remove the packet handler from the list and delete it.
	void RemovePacketHandler(PacketHandler*);

	///
	/// Add a package to the Fifo so that NewtonOS can pick it up later.
	///
	void Enqueue(Packet*);

	///
	/// Remove the oldest packet from the Fifo.
	///
	void DropPacket();

	///
	/// Chek if any packets are in the Fifo.
	///
	bool
	PacketAvailable()
	{
		return (mLastPacket != nullptr);
	}

	///
	/// Print a brief summary of a packge.
	///
	void Log(class Packet* p, const char* label, int line, int adjSeq = 0, int adjAck = 0);

private:
	// Linked list of packet handlers.
	PacketHandler* mFirstPacketHandler { nullptr };
	PacketHandler* mLastPacketHandler { nullptr };

	// Double linked list of packets, used as a Fifo.
	Packet* mFirstPacket { nullptr };
	Packet* mLastPacket { nullptr };
};

#endif
// _TUSERMODENETWORK_H

// ============================================ //
// The first time, it's a KLUDGE!               //
// The second, a trick.                         //
// Later, it's a well-established technique!    //
//                 -- Mike Broido, Intermetrics //
// ============================================ //
