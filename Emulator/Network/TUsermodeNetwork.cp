// ==============================
// File:			TUsermodeNetwork.cp
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

/*
 USERMODE NETWORK HANDLER
 
 The user mode Network handler works by emulating an ethernet card on a 
 pretty low level for the Newton. To use the network, the NIE2.0 and the 
 Einstein NE2000 driver must be installed on the Newton. By inserting
 the NE2000 card, the driver will be activated and all network calls
 will be forwarded to the Network Handler that was loaded at startup.
 
 All packats from the Newton are at the lowest network level. It is up to
 the handler to simulate or forward packets to the host or host network.
 The Newton expects to receive packets on the same low level, so we must
 generate the network header, IPv4 header, and TCP or UDP header ourselves.
 
 CURRENT STATUS
 
 At the current stage the driver is loaded correctly and all call forwarding 
 works. We can send and receive packets. A simuated ARP packet and a DNS
 request via UDP are accepted by the Newton.
 
 NEXT ACTION
 
 Next we need to implement handlers for all types of packages that we want
 to support. Forwarding packages that the Newton sends should be relatively
 easy. Generating the correct reply is a bit more complex.
 
 A working TCP protocol will allow web browsing and possibly reading mail.
 
 The goal is to set up a connection to NCX, Newtsync or Escale running locally. 
 
 */


#include <K/Defines/KDefinitions.h>
#include "TUsermodeNetwork.h"
#include "Emulator/Log/TLog.h"

//
// Handle all kinds of network packages
//

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>


// TODO: ARP
// TODO:  +--- send
// TODO:  +--- receive
// TODO: IPv4
// TODO:  +--- TCP
// TODO:  |		+--- send
// TODO:  |		+--- receive
// TODO:  +--- UDP
// TODO:		+--- send
// TODO:		+--- receive

class Packet
{
public:
	Packet(KUInt8 *data, KUInt32 size) {
		mData = (KUInt8*)malloc(size);
		memcpy(mData, data, size);
		mSize = size;
	}
	~Packet() {
		if (mData) 
			free(mData);
	}
	KUInt8 *Data() { return mData; }
	KUInt32 Size() { return mSize; }
	Packet *prev, *next;
private:
	KUInt8 *mData;
	KUInt32 mSize;
};


class PacketHandler 
{
public:
	PacketHandler() {
	}
	virtual ~PacketHandler() {
	}
	virtual int send(KUInt8 *data, KUInt32 size) = 0;
	virtual int timer() { return 0; }
	static int canHandle(KUInt8 *data, KUInt32 size) { return 0; }
	PacketHandler *prev, *next;
};


class UDPPacketHandler : public PacketHandler 
{
public:
	UDPPacketHandler(KUInt8 *data, KUInt32 size) {
	}
	~UDPPacketHandler() {
	}
	virtual int send(KUInt8 *data, KUInt32 size) {
		return -1;
	}
	virtual int timer() {
		return -1;
	}
	static int canHandle(KUInt8 *data, KUInt32 size) {
		return 0;
	}
};


TUsermodeNetwork::TUsermodeNetwork(TLog* inLog) :
	TNetworkManager( inLog ),
	mFirstPacketHandler( 0L ),
	mFirstPacket( 0L ),
	mLastPacket( 0L )
{
	// create the package pipe for the Newton package receiver
	// create the active handler list
}


TUsermodeNetwork::~TUsermodeNetwork()
{
	// release the package pipe
	while (mFirstPacket)
		DropPacket();
	// release all package handlers
	// TODO: delete handlers
	// release all other resources
}


/*
 * This function handles packet that are sent by the Newton to the outside 
 * world. In user mode, these packages are emulated for information that is 
 * already available on the host, or forwarded to a host socket. A possible
 * reply must be interpreted and a raw package must be generated for the Newton.
 */
int TUsermodeNetwork::SendPacket(KUInt8 *data, KUInt32 size)
{
	int err = -1;
	
	// offer this package to all active handlers
	PacketHandler *ph = mFirstPacketHandler;
	while (ph && err) {
		err = ph->send(data, size);
		ph = ph->next;
	}
	
	// now offer the package to possible new handlers
	/*
	if (err) {
		if (TCPPacketHandler::canHandle(data, size)) {
			ph = new TCPPacketHandler(data, size);
			AddPacketHandler(ph);
			err = ph->send(data, size);
		}
	}
	*/
	if (err) {
		if (UDPPacketHandler::canHandle(data, size)) {
			ph = new UDPPacketHandler(data, size);
			AddPacketHandler(ph);
			err = ph->send(data, size);
		}
	}
	if (!err) 
		return err;
	
	// FIXME: simulate the ARP request for now
	// see: man 3 getaddrinfo()
	static KUInt8 b1[] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x08, 0x06, 0x00, 0x01,
		0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0xc0, 0xa8, 0x00, 0xc6,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01,
	};
	static KUInt8 r1[] = {
		0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0x08, 0x06, 0x00, 0x01,
		0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0xc0, 0xa8, 0x00, 0x01, 
		0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0xc0, 0xa8, 0x00, 0xc6,
	};
	if (size==sizeof(b1) && memcmp(b1, data, size)==0) {
		Enqueue(new Packet(r1, sizeof(r1)));
		return 0;
	}
	
	// FIXME: simulate the DNS request
	static KUInt8 b2[] = {
		0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x08, 0x00, 0x45, 0x00,
		0x00, 0x36, 0x00, 0x00, 0x00, 0x00, 0x40, 0x11, 0xf8, 0x9f, 0xc0, 0xa8, 0x00, 0xc6, 0xc0, 0xa8,
		0x00, 0x01, 0x08, 0x00, 0x00, 0x35, 0x00, 0x22, 0xc3, 0x0e, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x62, 0x6f, 0x72, 0x67, 0x03, 0x6f, 0x72, 0x67, 0x00,
		0x00, 0x01, 0x00, 0x01
	};
	static KUInt8 r2[] = {
		0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0x08, 0x00, 0x45, 0x00, 
		0x00, 0x46, 0x05, 0x97, 0x00, 0x00, 0x40, 0x11, 0xf2, 0xf6, 0xc0, 0xa8, 0x00, 0x01, 0xc0, 0xa8, 
		0x00, 0xc8, 0x00, 0x35, 0xcd, 0x0c, 0x00, 0x32, 0xaa, 0xe8, 0x00, 0x01, 0x81, 0x80, 0x00, 0x01, 
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x62, 0x6f, 0x72, 0x67, 0x03, 0x6f, 0x72, 0x67, 0x00, 
		0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x04, 0x7e, 0x00, 0x04, 
		0x40, 0x69, 0xcd, 0x7b,                   
	};
	if (size==sizeof(b2) && memcmp(b2, data, size)==0) {
		r2[34] = b2[36]; r2[35] = b2[37];
		r2[36] = b2[34]; r2[37] = b2[35]; // swap ports
		r2[33] = 198;
		SetIPv4Checksum(r2, sizeof(r2));
		SetUDPChecksum(r2, sizeof(r2));
		LogPacket(b2, sizeof(b2));
		LogPacket(r2, sizeof(r2));
		Enqueue(new Packet(r2, sizeof(r2)));
		return 0;
	}
	
	
	// if we can't interprete the package, we offer some debugging information
	if (mLog) {
		mLog->LogLine("\nTUsermodeNetwork: Newton is sending an unsupported package:");
		LogBuffer(data, size);
		LogPacket(data, size);
	}
	return 0;
}


int TUsermodeNetwork::GetDeviceAddress(KUInt8 *data, KUInt32 size)
{
	// TODO: of course we need the true MAC of this ethernet card
	// see: ioctl ? getifaddrs ? http://othermark.livejournal.com/3005.html
	static KUInt8 gLocalMAC[]   = { 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23 };
	assert(size==6);
	memcpy(data, gLocalMAC, 6);
	return 0;
}


KUInt32 TUsermodeNetwork::DataAvailable()
{	
	if (mLastPacket) {
		return mLastPacket->Size();
	} else {
		return 0;
	}
}


int TUsermodeNetwork::ReceiveData(KUInt8 *data, KUInt32 size)
{
	Packet *pkt = mLastPacket;
	if (pkt) {
		assert(pkt->Size()==size);
		// copy the data over
		memcpy(data, pkt->Data(), size);
		// remove this package from the pipe
		DropPacket();
		return 0;
	} else {
		return -1;
	}
}


/**
 * Add a new handler for a package type to the list.
 */
void TUsermodeNetwork::AddPacketHandler(PacketHandler *inPacketHandler) 
{
	PacketHandler *n = mFirstPacketHandler;
	if (n) 
		n->prev = inPacketHandler;
	else
		mLastPacketHandler = inPacketHandler;
	inPacketHandler->next = n;
	inPacketHandler->prev = 0;
	mFirstPacketHandler = inPacketHandler;
}


/**
 * Add a new package to the beginning of the pipe.
 * This makes the give block ready to be sent at the next possible occasion.
 *
 * \param inPacket the package that will be queued
 */
void TUsermodeNetwork::Enqueue(Packet *inPacket)
{
	Packet *n = mFirstPacket;
	if (n) 
		n->prev = inPacket;
	else
		mLastPacket = inPacket;
	inPacket->next = n;
	inPacket->prev = 0;
	mFirstPacket = inPacket;
}


/**
 * Drops the last packet in the pipe.
 */
void TUsermodeNetwork::DropPacket()
{
	Packet *pkt = mLastPacket;
	if (pkt) {
		Packet *prevPkt = pkt->prev;
		if (prevPkt) 
			prevPkt->next = 0L;
		else
			mFirstPacket = 0L;
		mLastPacket = prevPkt;
		delete pkt;
	}
}


// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
