// ==============================
// File:			TTapNetwork.h
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

#ifndef _TTAPNETWORK_H
#define _TTAPNETWORK_H

#include "TNetworkManager.h"
#include <K/Defines/KDefinitions.h>


class TLog;
class TInterruptManager;
class TMemoryManager;
class TThread;

///
/// Networking with /dev/tap device.
/// MacOS X how-to (until we provide a nicer interface).
/// Let's suppose your Mac is connected over Airport (en1) and has IP 192.168.1.16.
///
/// 1. install tun & tap drivers from http://tuntaposx.sourceforge.net/
/// 2. fix the permissions on /dev/tap0 so Einstein can access it:
/// $ sudo chmod ugo+rw /dev/tap0
/// 3. setup the NAT on the Mac with:
/// 3a. $ sudo /usr/sbin/sysctl -w net.inet.ip.forwarding=1
/// 3b. setup the firewall rule for NAT:
/// $ sudo /sbin/ipfw -f flush
/// $ sudo ipfw add divert natd ip from any to any via en1
/// 3c. start natd:
/// $ sudo natd -alias_address 192.168.1.16 -interface en1 -dynamic
/// If you want to do NPDS sharing on the Newton on port 8080, run this instead :
/// $ sudo natd -alias_address 192.168.1.16 -interface en1 -dynamic -redirect_port tcp 10.0.1.2:8080 8080
/// If you started natd and want to change the option (e.g. alias_address, redirect port, etc.), kill natd, wait a little bit and restart natd.
/// 4. start Einstein with TTapNetwork as the network manager.
/// 5. once Einstein is started (i.e. you clicked the start button), configure the tap0 interface (on the Mac side) with:
/// $ sudo ifconfig tap0 10.0.1.1
/// (a totally different IP than your Mac's main IP).
/// 6. insert the NE2K card from the Platform menu.
/// 7. configure the Newton as follows:
/// Configuration: manual
/// Local IP Address: 10.0.1.2
/// Gateway/Router Address: 10.0.1.1
/// Primary DNS: pick your ISP's or Google's: 8.8.8.8
///
/// And you're done.
/// Everytime you restart Einstein, you need to reconfigure tap0 with
/// $ sudo ifconfig tap0 10.0.1.1
///
/// Everytime you restart, you need to fix the permissions on /dev/tap0 again.
/// $ sudo chmod ugo+rw /dev/tap0
///
/// This probably is the first thing we should fix.
/// If you just want to connect your Mac with Einstein, you can skip the firewall & NAT stuff.
/// Your Mac will see the Newton as 10.0.1.2 and the Newton will see the Mac as 10.0.1.1.
///
/// \author Paul Guyot <pguyot@kallisys.net>
///
class TTapNetwork : public TNetworkManager
{
public:
	TTapNetwork(TLog* inLog);
	
	///
	/// Destructor.
	///
	virtual ~TTapNetwork();

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
	/// Define the select fd set (in the thread).
	/// Return the max fd + 1. If the result is 0, the thread exits.
	///
	virtual int			SetReadFDSet(fd_set* ioFDSet);
private:
    int             mTapFileDescriptor;
    char            mBuffer[1518];
    KUInt32         mBufferSize;
};

#endif
		// _TTAPNETWORK_H

// ============================================ //
// The first time, it's a KLUDGE!               //
// The second, a trick.                         //
// Later, it's a well-established technique!    //
//                 -- Mike Broido, Intermetrics //
// ============================================ //
