// ==============================
// File:            TUsermodeNetwork.cp
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

/*
 USERMODE NETWORK HANDLER

 The user mode Network handler works by emulating an ethernet card on a
 pretty low level for the Newton. To use the network, the NIE2.0 and the
 Einstein NE2000 driver must be installed on the Newton. By inserting
 the NE2000 card, the driver will be activated and all network calls
 will be forwarded to the Network Handler that was loaded at startup.

 All packets from the Newton are at the lowest network level. It is up to
 the handler to simulate or forward packets to the host or host network.
 The Newton expects to receive packets on the same low level, so we must
 generate the network header, IPv4 header, and TCP or UDP header ourselves.

 CURRENT STATUS

 At the current stage the driver is loaded correctly and all call forwarding
 works. We can send and receive packets.

 - the initial ARP packet is faked
 - there is currently only one DNS request that is
 faked (http://borg.org , for no particular reason).
 - TCP connections are created and can send and receive (the code is a mess)

 NEXT ACTION

 Next we need to implement handlers for all types of packages that we want
 to support. Forwarding packages that the Newton sends should be relatively
 easy. Generating the correct reply is a bit more complex. A working TCP
 protocol will allow web browsing and possibly reading mail.

 The goal is to set up a connection to NCX, Newtsync or Escale running locally.

 - TCP disconnect action
 - TCP cleanup
 - DNS request
 - UDP protocol
 - ARP protocol
 - DHCP protocol
 - socket handling in threads
 - testing:
 * SimpleMail
 * Courier
 * Newt's Cape
 * NewtFTP
 * nBlog
 * Raissa
 * NPDS HTTP Server
 * NPDS Tracker Client
 * Dock TCP/IP (w/NCX)
 * IC/VC
 * LPR Driver

 done:
 - handler hierarchy
 - packet class
 - protocol class
 - TCP connect
 - TCP send
 - TCP receive

 Newton Synchronization happens on port: TCP 3679 (works mostly well with NCX 2.0)

 */

#define LOG_BIT_ERROR 0x01
#define LOG_BIT_WARN 0x02
#define LOG_BIT_PROTOCOL 0x04
#define LOG_BIT_HEADER 0x08
#define LOG_BIT_PACKAGE 0x10
#define LOG_BIT_CONTENT 0x20
#define LOG_BIT_CHATTY 0x40

#ifdef _DEBUG
#define LOG_LEVEL (LOG_BIT_ERROR | LOG_BIT_WARN | LOG_BIT_PROTOCOL | LOG_BIT_HEADER | LOG_BIT_PACKAGE | LOG_BIT_CONTENT)
#else
#define LOG_LEVEL 0
#endif

#define LOG_LINE(STR, ...) \
	if (mLog)              \
	mLog->FLogLine("Net: %4d: " STR, __LINE__, __VA_ARGS__)

#if (LOG_LEVEL & LOG_BIT_ERROR)
#define LOG_ERROR(...) LOG_LINE("ERROR: " __VA_ARGS__, 0)
#else
#define LOG_ERROR(...)
#endif

#if (LOG_LEVEL & LOG_BIT_WARN)
#define LOG_WARN(...) LOG_LINE("WARNING: " __VA_ARGS__, 0)
#else
#define LOG_WARN(...)
#endif

#if (LOG_LEVEL & LOG_BIT_PROTOCOL)
#define LOG_PROTOCOL(...) LOG_LINE(__VA_ARGS__, 0)
#else
#define LOG_PROTOCOL(...)
#endif

#if (LOG_LEVEL & LOG_BIT_HEADER)
#define LOG_HEADER(...) LOG_LINE(__VA_ARGS__, 0)
#define LOG_HEADER_DO(a) a
#else
#define LOG_HEADER(...)
#define LOG_HEADER_DO(a)
#endif

#if (LOG_LEVEL & LOG_BIT_PACKAGE)
#define LOG_PACKAGE(...) LOG_LINE(__VA_ARGS__, 0)
#define LOG_PACKAGE_DO(a) a
#else
#define LOG_PACKAGE(...)
#define LOG_PACKAGE_DO(a)
#endif

#if (LOG_LEVEL & LOG_BIT_CONTENT)
#define LOG_CONTENT(...) LOG_LINE(__VA_ARGS__, 0)
#define LOG_CONTENT_DO(a) a
#else
#define LOG_CONTENT(...)
#define LOG_CONTENT_DO(a)
#endif

#if (LOG_LEVEL & LOG_BIT_CHATTY)
#define LOG_CHATTY(...) LOG_LINE(__VA_ARGS__, 0)
#else
#define LOG_CHATTY(...)
#endif

//
// Handle all kinds of network packages
//

#include "TUsermodeNetwork.h"
#include "Emulator/Log/TLog.h"

#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if TARGET_OS_WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
typedef int socklen_t;
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __ANDROID__
#include <malloc.h>
#else
#if !RASPBERRY_PI && !TARGET_OS_LINUX
#include <ifaddrs.h>
#include <sys/sockio.h>
#endif
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#endif //! TARGET_OS_WIN32

/**
 This is the time in timer() calls until an unused UDP connection is closed
 and removed.
 */
const KUInt32 kUDPExpirationTime = 1000;

static KUInt32
MakeIP(KUInt8 a, KUInt8 b, KUInt8 c, KUInt8 d)
{
	KUInt32 ip = (a << 24) | (b << 16) | (c << 8) | d;
	return ip;
}

static const char* kNetworkName = "Einstein.local";
static const KUInt64 kGatewayMac = 0x9bf1ad740800ULL;
#if 0
static const KUInt32    kClientIP       = MakeIP(192, 168, 1, 42);
static const KUInt32    kClientIPMask   = MakeIP(255, 255, 255, 0);
static const KUInt32    kGatewayIP      = MakeIP(192, 168, 1, 1);
static const KUInt32    kNameServerIP   = MakeIP(192, 168, 1, 1);
#else
static const KUInt32 kClientIP = MakeIP(192, 168, 1, 42);
static const KUInt32 kClientIPMask = MakeIP(255, 255, 255, 0);
static const KUInt32 kGatewayIP = MakeIP(192, 168, 1, 1);
static const KUInt32 kNameServerIP = MakeIP(127, 0, 0, 53);
#endif

/**
 * This class is used to build and interprete TCP/IP packages.
 */
class Packet
{
public:
	/** Used in GetTCPFlags() */
	static const KUInt16 kTCPFlagURG = 0x0020;
	static const KUInt16 kTCPFlagACK = 0x0010;
	static const KUInt16 kTCPFlagPSH = 0x0008;
	static const KUInt16 kTCPFlagRST = 0x0004;
	static const KUInt16 kTCPFlagSYN = 0x0002;
	static const KUInt16 kTCPFlagFIN = 0x0001;

	/** Used in GetType(). */
	static const KUInt16 kNetTypeIP = 0x0800;
	static const KUInt16 kNetTypeARP = 0x0806;

	/** Used in GetIPProtocol() */
	static const KUInt8 kIPProtocolICMP = 1; // unhandled
	static const KUInt8 kIPProtocolTCP = 6;
	static const KUInt8 kIPProtocolUDP = 17;

	/**
	 * Create a new packet.
	 * \param data pointer to some place in memory that contains the packet data
	 * \param size number of bytes in this packet
	 * \param copy if set, the packet data will be copied, otherwise this class
	 *        will only point at the buffer
	 */
	Packet(KUInt8* data, ssize_t size, Boolean copy = 1) :
			mIndex(mIndexCount++)
	{
		if (copy)
		{
			mData = (KUInt8*) malloc(size);
			if (data)
			{
				memcpy(mData, data, size);
			} else
			{
				memset(mData, 0, size);
			}
			mCopy = 1;
		} else
		{
			if (data)
			{
				mData = data;
				mCopy = 0;
			} else
			{
				mData = (KUInt8*) calloc(size, 1);
				mCopy = 1;
			}
		}
		mSize = size;
	}

	/**
	 * Remove the packet.
	 * If the data was copied, we remove the copy as well.
	 */
	~Packet()
	{
		if (mData && mCopy)
			::free(mData);
	}

	/**
	 * Get a pointer to the raw packet data.
	 * \return pointer to the data
	 */
	KUInt8*
	Data()
	{
		return mData;
	}

	/**
	 * Get the number of bytes in the buffer.
	 * \return number of bytes.
	 */
	ssize_t
	Size()
	{
		return mSize;
	}

	/**
	 * Change the size of the packet.
	 * Does not update checksums or length fiels within the packege.
	 * Must not be larger than the initial package.
	 * Nothing is reallocated.
	 */
	void
	SetSize(ssize_t inSize)
	{
		mSize = inSize;
	}

	/**
	 * Return the unique number given to this packet.
	 */
	KUInt32
	Index()
	{
		return mIndex;
	}

	/**
	 * Read a six-byte word in network order.
	 * This is used to quickly handle MAC adresses.
	 * \return a six-byte sequence in the buffer as a 64 bit number.
	 */
	KUInt64
	Get48(KUInt32 i)
	{
		KUInt64 a0 = (KUInt64(mData[i + 0])) << 40;
		KUInt64 a1 = (KUInt64(mData[i + 1])) << 32;
		KUInt64 a2 = (KUInt64(mData[i + 2])) << 24;
		KUInt64 a3 = (KUInt64(mData[i + 3])) << 16;
		KUInt64 a4 = (KUInt64(mData[i + 4])) << 8;
		KUInt64 a5 = (KUInt64(mData[i + 5]));
		return a0 | a1 | a2 | a3 | a4 | a5;
	}

	void
	Set48(KUInt32 i, KUInt64 v)
	{
		mData[i + 0] = v >> 40;
		mData[i + 1] = v >> 32;
		mData[i + 2] = v >> 24;
		mData[i + 3] = v >> 16;
		mData[i + 4] = v >> 8;
		mData[i + 5] = v;
	}

	/** Get a 32-bit word in network order. */
	KUInt32
	Get32(KUInt32 i)
	{
		return (mData[i] << 24) | (mData[i + 1] << 16) | (mData[i + 2] << 8) | (mData[i + 3]);
	}

	void
	Set32(KUInt32 i, KUInt32 v)
	{
		mData[i + 0] = v >> 24;
		mData[i + 1] = v >> 16;
		mData[i + 2] = v >> 8;
		mData[i + 3] = v;
	}

	/** Get a 16-bit word in network order. */
	KUInt16
	Get16(KUInt32 i)
	{
		return (mData[i] << 8) | (mData[i + 1]);
	}
	void
	Set16(KUInt32 i, KUInt16 v)
	{
		mData[i + 0] = v >> 8;
		mData[i + 1] = v;
	}

	/** Get a 8-bit byte in network order. */
	KUInt8
	Get8(KUInt32 i)
	{
		return mData[i];
	}

	void
	Set8(KUInt32 i, KUInt8 v)
	{
		mData[i] = v;
	}

	/**
	 * All the functions below get and set members of raw TCP packets including network header.
	 * Details can be found all over the web.
	 */
	KUInt64
	GetDstMAC()
	{
		return Get48(0);
	}
	KUInt64
	GetSrcMAC()
	{
		return Get48(6);
	}
	KUInt16
	GetType()
	{
		return Get16(12);
	}

	KUInt8
	GetIPVersion()
	{
		return mData[14] >> 4;
	}
	KUInt8
	GetIPHeaderLength()
	{
		return (mData[14] & 0x0f) * 4;
	} // in bytes!
	KUInt8
	GetIPTOS()
	{
		return mData[15];
	}
	KUInt16
	GetIPTotalLength()
	{
		return Get16(16);
	}
	KUInt16
	GetIPID()
	{
		return Get16(18);
	}
	KUInt8
	GetIPFlags()
	{
		return mData[20] >> 5;
	}
	KUInt16
	GetIPFrag()
	{
		return Get16(20) & 0x1fff;
	}
	KUInt8
	GetIPTTL()
	{
		return mData[22];
	}
	KUInt8
	GetIPProtocol()
	{
		return mData[23];
	}
	KUInt16
	GetIPChecksum()
	{
		return Get16(24);
	}
	KUInt32
	GetIPSrcIP()
	{
		return Get32(26);
	}
	KUInt32
	GetIPDstIP()
	{
		return Get32(30);
	}

	KUInt16
	GetTCPSrcPort()
	{
		return Get16(34);
	}
	KUInt16
	GetTCPDstPort()
	{
		return Get16(36);
	}
	KUInt32
	GetTCPSeq()
	{
		return Get32(38);
	}
	KUInt32
	GetTCPAck()
	{
		return Get32(42);
	}
	KUInt8
	GetTCPHeaderLength()
	{
		return (mData[46] >> 4) << 2;
	} // in bytes!
	KUInt16
	GetTCPFlags()
	{
		return Get16(46) & 0x0fff;
	}
	KUInt16
	GetTCPWindow()
	{
		return Get16(48);
	}
	KUInt16
	GetTCPChecksum()
	{
		return Get16(50);
	}
	KUInt16
	GetTCPUrgent()
	{
		return Get16(52);
	}
	KUInt8*
	GetTCPPayloadStart()
	{
		return mData + GetTCPHeaderLength() + 34;
	}
	ssize_t
	GetTCPPayloadSize()
	{
		return mSize - GetTCPHeaderLength() - 34;
	}

	KUInt16
	GetUDPSrcPort()
	{
		return Get16(34);
	}
	KUInt16
	GetUDPDstPort()
	{
		return Get16(36);
	}
	KUInt16
	GetUDPLength()
	{
		return Get16(38);
	}
	KUInt16
	GetUDPChecksum()
	{
		return Get16(40);
	}
	KUInt8*
	GetUDPPayloadStart()
	{
		return mData + 42;
	}
	ssize_t
	GetUDPPayloadSize()
	{
		return mSize - 42;
	}

	KUInt16
	GetARPHType()
	{
		return Get16(14);
	}
	KUInt16
	GetARPPType()
	{
		return Get16(16);
	}
	KUInt8
	GetARPHLen()
	{
		return mData[18];
	}
	KUInt8
	GetARPPLen()
	{
		return mData[19];
	}
	KUInt16
	GetARPOp()
	{
		return Get16(20);
	}
	KUInt64
	GetARPSHA()
	{
		return Get48(22);
	}
	KUInt32
	GetARPSPA()
	{
		return Get32(28);
	}
	KUInt64
	GetARPTHA()
	{
		return Get48(32);
	}
	KUInt32
	GetARPTPA()
	{
		return Get32(38);
	}

	void
	SetDstMAC(KUInt64 v)
	{
		Set48(0, v);
	}
	void
	SetSrcMAC(KUInt64 v)
	{
		Set48(6, v);
	}
	void
	SetType(KUInt16 v)
	{
		Set16(12, v);
	}

	void
	SetIPVersion(KUInt8 v)
	{
		mData[14] = (mData[14] & 0x0f) | (v << 4);
	}
	void
	SetIPHeaderLength(KUInt8 v)
	{
		mData[14] = (mData[14] & 0xf0) | ((v / 4) & 0x0f);
	} // in bytes!
	void
	SetIPTOS(KUInt8 v)
	{
		mData[15] = v;
	}
	void
	SetIPTotalLength(KUInt16 v)
	{
		Set16(16, v);
	}
	void
	SetIPID(KUInt16 v)
	{
		Set16(18, v);
	}
	void
	SetIPFlags(KUInt8 v)
	{
		mData[20] = (mData[20] & 0x1f) | (v << 5);
	}
	void
	SetIPFrag(KUInt16 v)
	{
		mData[20] = (mData[20] & 0xe0) | ((v >> 8) & 0x1f);
		mData[21] = v;
	}
	void
	SetIPTTL(KUInt8 v)
	{
		mData[22] = v;
	}
	void
	SetIPProtocol(KUInt8 v)
	{
		mData[23] = v;
	}
	void
	SetIPChecksum(KUInt16 v)
	{
		Set16(24, v);
	}
	void
	SetIPSrcIP(KUInt32 v)
	{
		Set32(26, v);
	}
	void
	SetIPDstIP(KUInt32 v)
	{
		Set32(30, v);
	}

	void
	SetTCPSrcPort(KUInt16 v)
	{
		Set16(34, v);
	}
	void
	SetTCPDstPort(KUInt16 v)
	{
		Set16(36, v);
	}
	void
	SetTCPSeq(KUInt32 v)
	{
		Set32(38, v);
	}
	void
	SetTCPAck(KUInt32 v)
	{
		Set32(42, v);
	}
	void
	SetTCPHeaderLength(KUInt8 v)
	{
		mData[46] = (mData[46] & 0x0f) | ((v >> 2) << 4);
	} // in bytes!
	void
	SetTCPFlags(KUInt16 v)
	{
		mData[46] = (mData[46] & 0xf0) | ((v >> 8) & 0x0f);
		mData[47] = v;
	}
	void
	SetTCPWindow(KUInt16 v)
	{
		Set16(48, v);
	}
	void
	SetTCPChecksum(KUInt16 v)
	{
		Set16(50, v);
	}
	void
	SetTCPUrgent(KUInt16 v)
	{
		Set16(52, v);
	}
	void SetTCPPayload(KUInt8*, KUInt32);

	void
	SetUDPSrcPort(KUInt16 v)
	{
		Set16(34, v);
	}
	void
	SetUDPDstPort(KUInt16 v)
	{
		Set16(36, v);
	}
	void
	SetUDPLength(KUInt16 v)
	{
		Set16(38, v);
	}
	void
	SetUDPChecksum(KUInt16 v)
	{
		Set16(40, v);
	}
	void SetUDPPayload(KUInt8*, KUInt32);

	void
	SetARPHType(KUInt16 v)
	{
		Set16(14, v);
	}
	void
	SetARPPType(KUInt16 v)
	{
		Set16(16, v);
	}
	void
	SetARPHLen(KUInt8 v)
	{
		mData[18] = v;
	}
	void
	SetARPPLen(KUInt8 v)
	{
		mData[19] = v;
	}
	void
	SetARPOp(KUInt16 v)
	{
		Set16(20, v);
	}
	void
	SetARPSHA(KUInt64 v)
	{
		Set48(22, v);
	}
	void
	SetARPSPA(KUInt32 v)
	{
		Set32(28, v);
	}
	void
	SetARPTHA(KUInt64 v)
	{
		Set48(32, v);
	}
	void
	SetARPTPA(KUInt32 v)
	{
		Set32(38, v);
	}

	Packet* mPrev = nullptr;
	Packet* mNext = nullptr;

private:
	KUInt8* mData = nullptr;
	ssize_t mSize = 0;
	KUInt8 mCopy = 0;
	KUInt32 mIndex = 0;
	static KUInt32 mIndexCount;
};

/**
 * Give newly created packages a unique index.
 */
KUInt32 Packet::mIndexCount = 1000;

/**
 * This is a generic handler for network packets.
 *
 * To handle new types of packets, a new class should be derived.
 */
class PacketHandler
{
public:
	/**
	 * Create a new packet handler.
	 *
	 * \param h link handler to this network interface
	 */
	PacketHandler(TUsermodeNetwork* h) :
			mNet(h),
			mLog(h->GetLog())
	{
	}

	/**
	 * Remove a packet handler, freeing all resources.
	 */
	virtual ~PacketHandler() = default;

	/**
	 * Send a Newton packet to the outside world.
	 * \param p send this packet
	 * \return 1 if the packet was handled ok and it should not be sent by any other handler
	 * \return 0 if we don't know how to handle the packet and another handler should have a go
	 * \return -1 if an error occured and the packet should not be sent by any other handler
	 */
	virtual int
	send(Packet& p)
	{
		(void) p;
		return 0;
	}

	/**
	 * Every 10th of a second or so handle outstanding tasks.
	 */
	virtual void
	timer()
	{
	}

	/**
	 * Find out if this packet can be handled.
	 * If it can not be handled, the packet will be offered to the next handler.
	 * If we can handle it, we will either handle it right here, or instantiate
	 * a new handler, link it into the list of handlers, and then call send().
	 * \param p the Newton packet that could be sent
	 * \param n the network interface
	 * \return 0 if we can not handle this packet
	 * \return 1 if we can handled it and it need not to be propagated any further
	 */
	static int
	canHandle(Packet& p, TUsermodeNetwork* n)
	{
		(void) p;
		(void) n;
		return 0;
	}

	PacketHandler* mPrev = nullptr;
	PacketHandler* mNext = nullptr;
	TUsermodeNetwork* mNet = nullptr;
	TLog* mLog = nullptr;
};

/**
 * This class handles TCP packets.
 *
 * TCP is a connection that provides a nicely formed stream of data using all
 * kinds of handshake and tricks.
 *
 * In the emulation, we have a 100% connection
 * between Einstein and the Newton, so we only emulate a perfectly working
 * connection. The outside communication is done by the host, so no need for
 * any trickery here.
 */
class TCPPacketHandler : public PacketHandler
{
public:
	enum State {
		kStateDisconnected = 0, // handler just created, no socket
								// we expect a SYN package from the Newt and will reply with SYN ACK (or timeout)
		kStateConnectionWaitACK, // we expect the ACK connection packet
		kStateConnected, // socket was opened successfully
		kStateConnectedWaitACK, // package was forewarded to Newt, now I want an ACK
		kStatePeerDiscWaitForACK, // Peer disconnected, we sent a FIN, wait for an ACK from Newton
		kStatePeerDiscWaitForFIN, // Peer disconnected, we got the ACK, now we wait for a FIN from Newton
		kStateNewwtonDiscWaitForACK, // Newton requested a disconnect, we sent a FIN+ACK, waiting for final ACK
		kStateError,

		kStateClosed, // States according to TCP State Transition Diagram
		kStateListen,
		kStateSynRcvd,
		kStateSynSent,
		kStateEstablished,
		kStateCloseWait,
		kStateLastAck,
		kStateFinWait1,
		kStateFinWait2,
		kStateClosing,
		kStateTimeWait
	};
	static const char* kStateNameList[];

	/**
	 * Return the current state as a C-string.
	 */
	const char*
	StateName()
	{
		auto tState = state > kStateError ? kStateError : state;
		return kStateNameList[tState];
	}

	/**
	 * Create a TCP packet handler.
	 * A TCP connection can be uniquely identified by the destinatin MAC, IP, and
	 * port number. Let's remember these now so we can later identify the packets
	 * that we need to handle.
	 */
	TCPPacketHandler(TUsermodeNetwork* h, Packet& packet) :
			PacketHandler(h)
	{
		myMAC = packet.GetSrcMAC();
		myIP = packet.GetIPSrcIP();
		myPort = packet.GetTCPSrcPort();

		theirMAC = packet.GetDstMAC();
		theirIP = packet.GetIPDstIP();
		theirPort = packet.GetTCPDstPort();
		theirID = 1000;

		// SYN-ACK packet should have ACK set to client SEQ + 1
		mNewtonPacketsSeq = packet.GetTCPSeq() + 1;
		LOG_PROTOCOL("| Adding TCP handler for port %u to %u.%u.%u.%u.",
			theirPort,
			(unsigned int) ((theirIP >> 24) & 0xff),
			(unsigned int) ((theirIP >> 16) & 0xff),
			(unsigned int) ((theirIP >> 8) & 0xff),
			(unsigned int) (theirIP & 0xff));
	}

	/**
	 * Delete this handler.
	 */
	~TCPPacketHandler()
	{
		close_socket();
	}

	void
	close_socket()
	{
#if TARGET_OS_WIN32
		if (mSocket != INVALID_SOCKET)
		{
			if (state != kStateDisconnected)
			{
				shutdown(mSocket, SD_BOTH);
			}
			::closesocket(mSocket);
			mSocket = INVALID_SOCKET;
		}
#else
		if (mSocket != INVALID_SOCKET)
		{
			if (state != kStateDisconnected)
			{
				shutdown(mSocket, SHUT_RDWR);
			}
			::close(mSocket);
			mSocket = INVALID_SOCKET;
		}
#endif
	}

	/**
	 * Create a generic TCP packet.
	 * This is a working TCP packet for this particular connection. Space is
	 * allocated for the payload. The payload must be copied into this
	 * packet and the checksums must be updated.
	 * \param size this is the desired size of the payload.
	 * \see UpdateChecksums(Packet *p)
	 * \see Packet::SetTCPPayload(KUInt8 *, KUInt32)
	 */
	Packet*
	NewPacket(ssize_t size)
	{
		Packet* p = new Packet(0L, size + 54);
		p->SetDstMAC(myMAC);
		p->SetSrcMAC(theirMAC);
		p->SetType(Packet::kNetTypeIP);

		p->SetIPVersion(4);
		p->SetIPHeaderLength(20);
		p->SetIPTOS(0);
		p->SetIPTotalLength(size + 54 - 14); // not counting the network header
		p->SetIPID(theirID);
		theirID++;
		p->SetIPFlags(0);
		p->SetIPFrag(0);
		p->SetIPTTL(64);
		p->SetIPProtocol(Packet::kIPProtocolTCP);
		p->SetIPSrcIP(theirIP);
		p->SetIPDstIP(myIP);

		p->SetTCPSrcPort(theirPort);
		p->SetTCPDstPort(myPort);
		p->SetTCPSeq(mEinsteinPacketsSeq);
		p->SetTCPAck(mNewtonPacketsSeq);
		p->SetTCPHeaderLength(20);
		p->SetTCPFlags(Packet::kTCPFlagACK);
		p->SetTCPWindow(0x1000);

		return p;
	}

	/**
	 * Update the IP and TCP checksums in the packet.
	 */
	void
	UpdateChecksums(Packet* p)
	{
		mNet->SetIPv4Checksum(p->Data(), p->Size());
		mNet->SetTCPChecksum(p->Data(), p->Size());
	}

	/**
	 * Connect to a client.
	 * The class was just created. The first package that we expect
	 * is the SYN package that opens the socket and connects to the
	 * remote system.
	 * \return  1 if the connection was established
	 * \return -1 if no connection could be established
	 */
	int
	connect(Packet& packet)
	{
		mSeqBase = mNewtonPacketsSeq = packet.GetTCPSeq();
		// create a socket
		mSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mSocket == INVALID_SOCKET)
			return -1;
		// FIXME: should we remove the handler? Should we remove some kind of not-ACK package?
		// TODO: send a NACK package to the Newton

		// tell the socket who to connect to and connect
		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(theirPort);
		sa.sin_addr.s_addr = htonl(theirIP);

		// perform some presets for the socket
		// - connect() will block. Put this socket into non-blocking first
		// - get the current settings and add the flag to avoid blocking
		int err;
#if TARGET_OS_WIN32
		u_long arg = 1;
		ioctlsocket(mSocket, FIONBIO, &arg);
#else
		int fl = fcntl(mSocket, F_GETFL);
		err = fcntl(mSocket, F_SETFL, fl | O_NONBLOCK);
		if (err == -1)
			return -1;
#endif

		// Since we are in non-blocking mode, ::connect will return with an error
		err = ::connect(mSocket, (struct sockaddr*) &sa, sizeof(sa));
		// We expect an error
		if (err == -1)
		{
			// ::connect is initiated, but we have no reply yet
#if TARGET_OS_WIN32
			if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
			if (errno == EINPROGRESS)
#endif
			{
				struct timeval timeout = {};
				timeout.tv_sec = 10;
				timeout.tv_usec = 0;
				fd_set set;
				FD_ZERO(&set);
				FD_SET(mSocket, &set);
				// wait for a few seconds for a reply
				err = ::select(mSocket + 1, NULL, &set, NULL, &timeout);
				// if ::select failed, it return -1
				// if there was a timout, ::select returns 0
				// if the connection was established, ::select returns 1
			} else
			{
				LOG_PROTOCOL("| TTCPPacketHandler::connect: Can't connect (%d.%d.%d.%d:%d): %s.",
					theirIP >> 24, (theirIP >> 16) & 255, (theirIP >> 8) & 255, theirIP & 255, theirPort,
					strerror(errno));
				return -1;
			}
			// No connection could be established
			if (err == -1 || err == 0)
			{ // error or timeout
				LOG_PROTOCOL("| TTCPPacketHandler::connect: Can't connect (::select) (%d.%d.%d.%d:%d): %s.",
					theirIP >> 24, (theirIP >> 16) & 255, (theirIP >> 8) & 255, theirIP & 255, theirPort,
					strerror(errno));
				return -1;
			}
			// no error, we are connected. Fall through.
		}

		// succesful connection, return a SYN ACK to the Newton.
		Packet* reply = NewPacket(4); // 4 bytes are reserved for the extended TCP header
		reply->SetTCPFlags(Packet::kTCPFlagACK | Packet::kTCPFlagSYN);
		reply->SetTCPHeaderLength(24);
		reply->SetTCPSeq(mEinsteinPacketsSeq);
		reply->SetTCPAck(++mNewtonPacketsSeq);
		reply->Set8(54, 2); // Kind 2
		reply->Set8(55, 4); // Length 4
		reply->Set16(56, 1460); // Maximum segment size
		UpdateChecksums(reply);
		LOG_HEADER_DO(mNet->Log(reply, "| W E>N", __LINE__, 0, mSeqBase);)
		mNet->Enqueue(reply);
		state = kStateConnectionWaitACK;
		return 1;
	}

	/**
	 * Send a Newton packet to the outside world.
	 * \param packet send this packet
	 * \return 1 if the packet was handled ok
	 * \return 0 if we don't know how to handle the packet
	 * \return -1 if an error occured and no other handler should handle this packet
	 */
	int
	send(Packet& packet) override
	{
		if ((packet.GetType() != Packet::kNetTypeIP)
			|| (packet.GetIPProtocol() != Packet::kIPProtocolTCP)
			|| (myPort != packet.GetTCPSrcPort())
			|| (theirIP != packet.GetIPDstIP())
			|| (theirPort != packet.GetTCPDstPort()))
			return 0;
		LOG_PROTOCOL("|       Send: state=%s.", StateName());
		LOG_HEADER_DO(mNet->Log(&packet, "| W E<N", __LINE__, mSeqBase, 0);)
		switch (state)
		{
			case kStateDisconnected:
				// FIXME: we assume that the original packet is a correct SYN packet
				return connect(packet);
			case kStateConnectionWaitACK:
				mEinsteinPacketsSeq = packet.GetTCPAck();
				state = kStateConnected;
				return 1;
			case kStateConnected:
			case kStateConnectedWaitACK:
				// Make sure that we don;t forward packes until the last one was acknowledged
				if ((state == kStateConnectedWaitACK) && (packet.GetTCPFlags() & Packet::kTCPFlagACK))
				{
					// mNewtonPacketsSeq = packet.GetTCPAck();
					state = kStateConnected;
				}
				// The socket is connected. Traffic can come from either side
				if (packet.GetTCPPayloadSize() > 0)
				{
					// this is a data packet: send data to the socket
					LOG_HEADER_DO(mNet->Log(&packet, "| W<E N", __LINE__, mSeqBase, 0);)
					::send(mSocket, (char*) packet.GetTCPPayloadStart(), packet.GetTCPPayloadSize(), 0);
					// FIXME: use return value
					mNewtonPacketsSeq = packet.GetTCPSeq() + (KUInt32) packet.GetTCPPayloadSize();
					// return an ACK to Newton
					Packet* reply = NewPacket(0);
					reply->SetTCPFlags(Packet::kTCPFlagACK);
					reply->SetTCPSeq(mEinsteinPacketsSeq);
					reply->SetTCPAck(mNewtonPacketsSeq);
					UpdateChecksums(reply);
					LOG_HEADER_DO(mNet->Log(reply, "| W E>N", __LINE__, 0, mSeqBase);)
					mNet->Enqueue(reply);
				}
				if (packet.GetTCPFlags() & Packet::kTCPFlagFIN)
				{
					// Newton initiates a disconnection.
					// We don't support half-duplex
					close_socket();
					// Peer has closed connection.
					mNewtonPacketsSeq = packet.GetTCPSeq() + 1;
					Packet* reply = NewPacket(0);
					// Send a FIN request to the Newton
					reply->SetTCPFlags(Packet::kTCPFlagFIN | Packet::kTCPFlagACK);
					UpdateChecksums(reply);
					LOG_HEADER_DO(mNet->Log(reply, "| W E>N", __LINE__, 0, mSeqBase);)
					mNet->Enqueue(reply);
					// Wait for the newton to acknowledge the FIN and close on its side.
					state = kStateNewwtonDiscWaitForACK;
					break;
				}
				return 1;
			case kStateNewwtonDiscWaitForACK:
				// Just close down.
				// FIXME: we assume that this is an ACK packet
				LOG_PROTOCOL("| Newton closed. Removing TCP handler for port %u to %u.%u.%u.%u.",
					theirPort,
					(unsigned int) ((theirIP >> 24) & 0xff),
					(unsigned int) ((theirIP >> 16) & 0xff),
					(unsigned int) ((theirIP >> 8) & 0xff),
					(unsigned int) (theirIP & 0xff));
				mNet->RemovePacketHandler(this);
				return 1;
			case kStatePeerDiscWaitForACK:
				// Peer sent a FIN, we forewarded if to Newton.
				// Newton should send an ACK.
				if ((packet.GetTCPFlags() & Packet::kTCPFlagACK))
				{
					mNewtonPacketsSeq = packet.GetTCPSeq() + 1;
				}
				state = kStatePeerDiscWaitForFIN;
				return 1;
			case kStatePeerDiscWaitForFIN:
				// Peer sent a FIN, we forewarded if to Newton.
				// Newton did send an ACK.
				// Newton should now send a FIN.
				if ((packet.GetTCPFlags() & Packet::kTCPFlagFIN))
				{
					mNewtonPacketsSeq = packet.GetTCPSeq() + 1;
					// Acknowldge
					Packet* reply = NewPacket(0);
					//                    reply->SetTCPFlags(Packet::kTCPFlagACK);
					reply->SetTCPFlags(Packet::kTCPFlagRST);
					UpdateChecksums(reply);
					LOG_HEADER_DO(mNet->Log(reply, "| W E>N", __LINE__, 0, mSeqBase);)
					mNet->Enqueue(reply);
					// And now all communication on this socket should be done
					close_socket();
					LOG_PROTOCOL("| Peer closing. Removing TCP handler for port %u to %u.%u.%u.%u.",
						theirPort,
						(unsigned int) ((theirIP >> 24) & 0xff),
						(unsigned int) ((theirIP >> 16) & 0xff),
						(unsigned int) ((theirIP >> 8) & 0xff),
						(unsigned int) (theirIP & 0xff));
					mNet->RemovePacketHandler(this);
				}
				return 1;
			default:
				LOG_ERROR("---> TCP send: unsupported state in 'send()'.");
				break;
		}
		return -1;
	}

	/**
	 * Handle all reoccuring events.
	 * We currently use polling to read from the sockets. This is easy on the
	 * host and does not delay package delivery too much. In a future version
	 * it would be better to used threads to read from the sockets and
	 * interrupts to send data to the Newton.
	 */
	void
	timer() override
	{
		// Don't do anything until Newton picked up all packages.
		if (mNet->PacketAvailable()) // DataAvailable())
			return;
		switch (state)
		{
			case kStateNewwtonDiscWaitForACK:
				// TODO: count down the timer and remove myself if expired
				return;
			case kStateDisconnected:
				// TODO: this should not happen
				return;
			case kStateConnected: {
				KUInt8 buf[TUsermodeNetwork::kMaxRxBuffer];
				ssize_t avail = ::recv(mSocket, (char*) buf, sizeof(buf), 0);
				if (avail == -1)
				{
					// FIXME: do stuff depending on the error that occured
				} else if (avail == 0)
				{
					// Peer has closed connection.
					LOG_PROTOCOL("  ,---- Einstein > Newton -- TCP ----------");
					LOG_PROTOCOL("  | Peer is closing the connection. Send FIN.");
					// Foreward a FIN request to the Newton
					Packet* reply = NewPacket(0);
					reply->SetTCPFlags(Packet::kTCPFlagFIN | Packet::kTCPFlagACK);
					reply->SetTCPAck(mNewtonPacketsSeq);
					UpdateChecksums(reply);
					LOG_HEADER_DO(mNet->Log(reply, "  | W E>N", __LINE__, 0, mSeqBase);)
					mNet->Enqueue(reply);
					// Increment sequence number for last packet.
					mEinsteinPacketsSeq++;
					LOG_PROTOCOL("  `---- Einstein > Newton -- done ---------");
					// Wait for the newton to acknowledge the FIN and and send a FIN to peer.
					state = kStatePeerDiscWaitForACK;
					break;
				} else if (avail > 0)
				{
					LOG_PROTOCOL("  ,---- Einstein > Newton -- TCP ----------");
					LOG_PROTOCOL("  | W>E N Received %d bytes payload.", (int) avail);
					Packet* reply = NewPacket(avail);
					memcpy(reply->GetTCPPayloadStart(), buf, avail);
					reply->SetTCPFlags(Packet::kTCPFlagACK /*|Packet::kTCPFlagPSH*/);
					reply->SetTCPAck(mNewtonPacketsSeq);
					UpdateChecksums(reply);
					LOG_PROTOCOL("  | W E N Forwarding %d bytes total, %d bytes payload.",
						34 + reply->GetTCPHeaderLength() + reply->GetTCPPayloadSize(),
						reply->GetTCPPayloadSize());
					LOG_HEADER_DO(mNet->Log(reply, "  | W E>N", __LINE__, 0, mSeqBase);)
					mNet->Enqueue(reply);
					mEinsteinPacketsSeq += avail;
					LOG_PROTOCOL("  `---- Einstein > Newton -- done ---------");
					// The following line blocks the handler from sending new
					// packages until the last sent package was acknowledged
					// bei NewtonOS.
					state = kStateConnectedWaitACK;
				}
				break;
			}
			default:
				break;
		}
	}

	/**
	 * Can we handle the given package?
	 * This function is static and can be called before this class is instantiated.
	 * At this point, no active handler was able to handle the packet. If this
	 * is a generic TCP/IP packet request to open a socket, we will handle it.
	 * \param packet the Newton packet that could be sent
	 * \param n the network interface
	 * \return 0 if we can not handle this packet
	 * \return 1 if we can handled it and it need not to be propagated any further
	 * \return -1 if an error occurred an no other handler should handle this packet
	 */
	static int
	canHandle(Packet& packet, TUsermodeNetwork* net)
	{
		if (packet.GetType() != Packet::kNetTypeIP)
			return 0;
		if (packet.GetIPProtocol() != Packet::kIPProtocolTCP)
			return 0;
		// if ( packet.GetTCPFlags() != Packet::TCPFlagSYN && packet.GetTCPFlags() != Packet::TCPFlagRST)
		//     return 0; // only SYN is set
		PacketHandler* ph = new TCPPacketHandler(net, packet);
		net->AddPacketHandler(ph);
		return ph->send(packet);
	}

	KUInt64 myMAC = 0;
	KUInt64 theirMAC = 0;
	KUInt32 myIP = 0;
	KUInt32 theirIP = 0;
	KUInt16 myPort = 0;
	KUInt16 theirPort = 0;
	KUInt32 mEinsteinPacketsSeq = 0; ///<! sequence number of our packets, incremented after each send
	KUInt32 mSeqBase = 0;
	KUInt32 mNewtonPacketsSeq = 0; ///<! sequence number of Newton packets for our ACKs
	KUInt16 theirID = 0;
	enum State state = kStateDisconnected;
	int mSocket = INVALID_SOCKET;
};

const char* TCPPacketHandler::kStateNameList[] {
	"kStateDisconnected",
	"kStateConnectionWaitACK",
	"kStateConnected",
	"kStateConnectedWaitACK",
	"kStatePeerDiscWaitForACK",
	"kStatePeerDiscWaitForFIN",
	"kStateNewtonDiscWaitForACK",
	"kStateError"
};

/**
 * This class handles UDP packets.
 */
class UDPPacketHandler : public PacketHandler
{
public:
	/**
	 * Create a UDP packet handler.
	 */
	UDPPacketHandler(TUsermodeNetwork* h, Packet& packet) :
			PacketHandler(h)
	{
		myMAC = packet.GetSrcMAC();
		myIP = packet.GetIPSrcIP();
		myPort = packet.GetUDPSrcPort();
		theirMAC = packet.GetDstMAC();
		theirIP = packet.GetIPDstIP();
		theirPort = packet.GetUDPDstPort();
		LOG_PROTOCOL("| Adding UDP handler for port %d to %u.%u.%u.%u.",
			theirPort,
			(unsigned int) ((theirIP >> 24) & 0xff),
			(unsigned int) ((theirIP >> 16) & 0xff),
			(unsigned int) ((theirIP >> 8) & 0xff),
			(unsigned int) (theirIP & 0xff));
	}

	/**
	 * Delete this handler.
	 */
	~UDPPacketHandler() override
	{
		close_socket();
	}

	void
	close_socket()
	{
#if TARGET_OS_WIN32
		if (mSocket != INVALID_SOCKET)
		{
			::closesocket(mSocket);
			mSocket = INVALID_SOCKET;
		}
#else
		if (mSocket != INVALID_SOCKET)
		{
			::close(mSocket);
			mSocket = INVALID_SOCKET;
		}
#endif
	}

	/**
	 * Create a generic UDP packet.
	 * \param size this is the desired size of the payload.
	 * \see UpdateChecksums(Packet *p)
	 * \see Packet::SetUDPPayload(KUInt8 *, KUInt32)
	 */
	Packet*
	NewPacket(ssize_t size)
	{
		Packet* p = new Packet(0L, size + 42);
		p->SetDstMAC(myMAC);
		p->SetSrcMAC(theirMAC);
		p->SetType(Packet::kNetTypeIP);

		p->SetIPVersion(4);
		p->SetIPHeaderLength(20);
		p->SetIPTOS(0);
		p->SetIPTotalLength(size + 42 - 14); // not counting the network header
		p->SetIPID(theirID);
		theirID++;
		p->SetIPFlags(0);
		p->SetIPFrag(0);
		p->SetIPTTL(64);
		p->SetIPProtocol(Packet::kIPProtocolUDP);
		p->SetIPSrcIP(theirIP);
		p->SetIPDstIP(myIP);

		p->SetUDPSrcPort(theirPort);
		p->SetUDPDstPort(myPort);
		p->SetUDPLength(size + 8);

		return p;
	}

	/**
	 * Update IP and UDP cheksums.
	 */
	void
	UpdateChecksums(Packet* p)
	{
		mNet->SetIPv4Checksum(p->Data(), p->Size());
		mNet->SetUDPChecksum(p->Data(), p->Size());
	}

	/**
	 * Send a Newton packet to the outside world.
	 * \param packet send this packet
	 * \return 1 if the packet was handled ok
	 * \return 0 if we don't know how to handle the packet
	 * \return -1 if an error occured and no other handler should handle this packet
	 */
	int
	send(Packet& packet) override
	{
		if ((packet.GetType() != Packet::kNetTypeIP)
			|| (packet.GetIPProtocol() != Packet::kIPProtocolUDP)
			|| (myPort != packet.GetUDPSrcPort())
			|| (theirIP != packet.GetIPDstIP())
			|| (theirPort != packet.GetUDPDstPort()))
			return 0;
		LOG_HEADER_DO(mNet->Log(&packet, "| W E<N", __LINE__);)
		if (mSocket == INVALID_SOCKET)
		{
			mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (mSocket == INVALID_SOCKET)
				return -1;
#if TARGET_OS_WIN32
			u_long arg = 1;
			ioctlsocket(mSocket, FIONBIO, &arg);
#else
			int fl = fcntl(mSocket, F_GETFL);
			int err = fcntl(mSocket, F_SETFL, fl | O_NONBLOCK);
			if (err == -1)
				return -1;
#endif
			memset(&theirSockAddr, 0, sizeof(theirSockAddr));
			theirSockAddr.sin_family = AF_INET;
			theirSockAddr.sin_addr.s_addr = htonl(theirIP);
			theirSockAddr.sin_port = htons(theirPort);
		}
		mExpire = kUDPExpirationTime;
		LOG_HEADER_DO(mNet->Log(&packet, "| W<E N", __LINE__);)

		mNet->LogPacket(packet.Data(), packet.Size());
		mNet->LogBuffer(packet.Data(), packet.Size());

		ssize_t ret = sendto(mSocket, (char*) packet.GetUDPPayloadStart(), packet.GetUDPPayloadSize(),
			0, (struct sockaddr*) &theirSockAddr, sizeof(theirSockAddr));
		if (ret == -1)
			return -1;
		return 1;
	}

	/**
	 * Handle all reoccuring events.
	 * We currently use polling to read from the sockets. This is easy on the
	 * host and does not delay package delivery too much. In a future version
	 * it would be better to used threads to read from the sockets and
	 * interrupts to send data to the Newton.
	 */
	void
	timer() override
	{
		// Don't do anything until Newton picked up all packages.
		if (mNet->PacketAvailable()) // DataAvailable())
			return;
		KUInt8 buf[TUsermodeNetwork::kMaxRxBuffer];
		socklen_t addrLen = sizeof(theirSockAddr);
		if (mSocket == INVALID_SOCKET)
			return;
		int maxTry = 5;
		for (; maxTry > 0; maxTry--)
		{
			ssize_t avail = recvfrom(mSocket, (char*) buf, sizeof(buf), 0, (struct sockaddr*) &theirSockAddr, &addrLen);
			if (avail < 1)
			{
				if (--mExpire == 0)
				{
					LOG_PROTOCOL("[ Timer expired. Removing UDP handler for port %u to %u.%u.%u.%u.",
						theirPort,
						(unsigned int) ((theirIP >> 24) & 0xff),
						(unsigned int) ((theirIP >> 16) & 0xff),
						(unsigned int) ((theirIP >> 8) & 0xff),
						(unsigned int) (theirIP & 0xff));
					mNet->RemovePacketHandler(this);
				}
				return;
			}
			LOG_PROTOCOL("  ,---- Einstein > Newton -- UDP ----------");
			LOG_PROTOCOL("  | W>E N Received %d bytes (UDP).", (int) avail);
			Packet* reply = NewPacket(avail);
			memcpy(reply->GetUDPPayloadStart(), buf, avail);
			UpdateChecksums(reply);
			LOG_HEADER_DO(mNet->Log(reply, "  | W E>N", __LINE__);)
			mNet->LogPacket(reply->Data(), reply->Size());
			mNet->LogBuffer(reply->Data(), reply->Size());

			mNet->Enqueue(reply);
			LOG_PROTOCOL("  `---- Einstein > Newton -- done ---------");
			mExpire = kUDPExpirationTime;
		}
	}

	/**
	 * Can we handle the given package?
	 *
	 * UDP packets usually expect a single reply through the same port. We send
	 * the given packet out right away and add a handler for receiving replies
	 * on the same port.
	 *
	 * \param packet the Newton packet that could be sent
	 * \param n the network interface
	 * \return 0 if we can not handle this packet
	 * \return 1 if we can handled it and it need not to be propagated any further
	 * \return -1 if an error occurred an no other handler should handle this packet
	 */
	static int
	canHandle(Packet& packet, TUsermodeNetwork* net)
	{
		if (packet.GetType() != Packet::kNetTypeIP)
			return 0;
		if (packet.GetIPProtocol() != Packet::kIPProtocolUDP)
			return 0;
		PacketHandler* ph = new UDPPacketHandler(net, packet);
		net->AddPacketHandler(ph);
		return ph->send(packet);
	}

	KUInt64 myMAC = 0;
	KUInt64 theirMAC = 0;
	KUInt32 myIP = 0;
	KUInt32 theirIP = 0;
	KUInt16 myPort = 0;
	KUInt16 theirPort = 0;
	KUInt16 theirID = 2000;
	int mSocket = INVALID_SOCKET;
	KUInt32 mExpire = 0;
	struct sockaddr_in theirSockAddr = {};
};

/**
 * This class handles ARP packets.
 */
class ARPPacketHandler : public PacketHandler
{
public:
	/**
	 * Can we handle the given package?
	 *
	 * ARP packets are sent to find the MAC address for a specific IP. The MAC
	 * address is needed to send IP packets to peers directly. In an emulated
	 * environment however, a MAC has little meaning, which is why we make up
	 * MACs as we go by using the IP for the lower 4 bytes and adding 00:fa:... .
	 *
	 * This is a very simple and quick operation. canHandle() does not add
	 * itself to the handler list, but answers the request immediatly.
	 *
	 * \param packet the Newton packet that could be sent
	 * \param n the network interface
	 * \return 0 if we can not handle this packet
	 * \return 1 if we can handled it and it need not to be propagated any further
	 * \return -1 if an error occurred an no other handler should handle this packet
	 * \see man 3 getaddrinfo()
	 */
	static int
	canHandle(Packet& packet, TUsermodeNetwork* net)
	{
		auto mLog = net->GetLog();
		(void) mLog;
		if (packet.GetType() != Packet::kNetTypeARP)
			return 0;
		if (packet.GetARPOp() != 1) // is this a request?
			return 0;
		if (packet.GetARPHType() != 1) // is this an Ethernet MAC address?
			return 0;
		if (packet.GetARPPType() != 0x0800) // Which upper layer protocol? IP?
			return 0;
		if (packet.GetARPHLen() != 6) // Length of MAC Address?
			return 0;
		if (packet.GetARPPLen() != 4) // Length of IP Address?
			return 0;
		Packet* reply = new Packet(0L, 42);
		reply->SetSrcMAC(packet.GetDstMAC());
		reply->SetDstMAC(packet.GetSrcMAC());
		reply->SetType(Packet::kNetTypeARP);
		reply->SetARPHType(1);
		reply->SetARPPType(0x0800);
		reply->SetARPHLen(6);
		reply->SetARPPLen(4);
		reply->SetARPOp(2); // reply

		KUInt64 a = packet.GetARPTPA();
		KUInt64 b = 0x000000fa00000000ULL;
		KUInt64 c = a | b;
		reply->SetARPSHA(c); // faking a MAC address

		reply->SetARPSPA(packet.GetARPTPA());
		reply->SetARPTHA(packet.GetARPSHA());
		reply->SetARPTPA(packet.GetARPSPA());
		LOG_HEADER_DO(net->Log(reply, "| W E>N", __LINE__);)
		net->Enqueue(reply);
		KUInt32 theirIP = packet.GetARPTPA();
		(void) theirIP;
		LOG_PROTOCOL("| ARP request for IP %u.%u.%u.%u.",
			(unsigned int) ((theirIP >> 24) & 0xff),
			(unsigned int) ((theirIP >> 16) & 0xff),
			(unsigned int) ((theirIP >> 8) & 0xff),
			(unsigned int) (theirIP & 0xff));
		return 1;
	}
};

#if 0
// Code to get the IP that connects to the outside world
void GetPrimaryIp(char* buffer, size_t buflen)
{
    assert(buflen >= 16);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sock != INVALID_SOCKET);

    const char* kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
    assert(err != -1);

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    assert(err != -1);

    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, buflen);
    assert(p);

    close(sock);
}
#endif

/**
 * This class handles DNS requests.
 *
 * Check if this UDP request is a name lookup for our non-existing
 * Nameserver and use the Host network interface to synthesize a reply.
 */
class DNSPacketHandler : public UDPPacketHandler
{
public:
	/**
	 * DNS requests look up the IP address for a given client name.
	 * \param packet the Newton packet that could be sent
	 * \param net the network interface
	 * \return 0 if we can not handle this packet
	 * \return 1 if we can handled it and it need not to be propagated any further
	 * \return -1 if an error occurred an no other handler should handle this packet
	 */
	static int
	canHandle(Packet& packet, TUsermodeNetwork* net)
	{
		// DNS: Transaction ID: 0x241a
		if (packet.GetType() != Packet::kNetTypeIP)
			return 0;
		if (packet.GetIPProtocol() != Packet::kIPProtocolUDP)
			return 0;
		if (packet.GetUDPDstPort() != 53) // DNS requests are sent to port 53
			return 0;
		if (packet.GetIPDstIP() != kNameServerIP) // DNS server as set by DHCP
			return 0;

		auto mLog = net->GetLog();
		(void) mLog;
		LOG_PROTOCOL("| DNS request.");
		LOG_HEADER_DO(net->Log(&packet, "| W E<N", __LINE__);)

		//      KUInt16 queryID = packet.Get16(0x0028);
		KUInt16 flags = packet.Get16(0x002C);
		if ((flags & 0x8b50) != 0x0100)
		{
			LOG_PROTOCOL("| Flags not recognised: 0x%04x.", flags);
			return 0;
		}
		KUInt16 qdCount = packet.Get16(0x002e);
		if (qdCount == 0)
		{
			LOG_PROTOCOL("| No queries found.");
			return 0;
		}
		KUInt8 name[128]; // Size limit is 64...
		KUInt8 space = 126;
		KUInt8 *d = name, *s = packet.Data() + 0x0036;
		for (;;)
		{
			KUInt8 n = *s++;
			if (n == 0)
				break;
			for (KUInt8 i = 0; i < n; i++)
			{
				*d++ = *s++;
				if (space-- == 0)
					break;
			}
			if (*s)
			{
				*d++ = '.';
				if (space-- == 0)
					break;
			}
		}
		*d++ = 0;
		ssize_t ix = s - packet.Data();
		KUInt16 qType = packet.Get16((KUInt32) ix);
		ix += 2;
		KUInt16 qClass = packet.Get16((KUInt32) ix);
		ix += 2;
		if (qType != 1 && qClass != 1)
		{
			LOG_PROTOCOL("| Unsupported query type %d %d.", qType, qClass);
			return 0;
		}

		KUInt32 ip = MakeIP(127, 0, 0, 1);
		struct hostent* host = gethostbyname((char*) name);
		int err = 0;
		if (host && (host->h_addrtype == AF_INET))
		{
			// dwError = WSAGetLastError(), h_errno
			KUInt8* ipp = (KUInt8*) host->h_addr_list[0];
			ip = (ipp[0] << 24) | (ipp[1] << 16) | (ipp[2] << 8) | (ipp[3]);
			LOG_PROTOCOL("| Host \"%s\" ip at %d.%d.%d.%d.", name, ipp[0], ipp[1], ipp[2], ipp[3]);
		} else
		{
			LOG_PROTOCOL("| Host \"%s\" not found!", name);
			err = 1;
		}

		KUInt32 replySize = (KUInt32) packet.Size();
		if (!err)
			replySize += 16;
		Packet* reply = new Packet(nullptr, replySize);
		memcpy(reply->Data(), packet.Data(), packet.Size());
		KUInt32 size = replySize - 42;

		/* --- Sample request and reply.
		 0x0000:  00fa c0a8 0101 58b0 3577 d722 0800 4500  ......X.5w."..E.
		 0x0010:  003c 0002 0000 4011 f733 c0a8 012a c0a8  .<....@..3...*..
		 0x0020:  0101 0800 0035 0028 7d6a:0001 0100 0001  .....5.(}j......
		 0x0030:  0000 0000 0000 0a6d 6573 7361 6765 7061  .......messagepa
		 0x0040:  6403 6f72 6700 0001 0001                 d.org.....

		 0x0000:  58b0 3577 d722 00fa c0a8 0101 0800 4500  X.5w."........E.
		 0x0010:  004c 07d0 0000 4011 ef55 c0a8 0101 c0a8  .L....@..U......
		 0x0020:  012a 0035 0800 0038 5440:0001 8180 0001  .*.5...8T@......
		 0x0030:  0001 0000 0000 0a6d 6573 7361 6765 7061  .......messagepa
		 0x0040:  6403 6f72 6700 0001 0001 c00c 0001 0001  d.org...........
		 0x0050:  0000 0e10 0004 d9a0 00c5                 ..........
		 */

		// Network II
		reply->SetDstMAC(packet.GetSrcMAC());
		reply->SetSrcMAC(packet.GetDstMAC());
		reply->SetType(0x0800);
		// IP
		reply->SetIPVersion(4);
		reply->SetIPHeaderLength(20);
		reply->SetIPTOS(0);
		reply->SetIPTotalLength(size + 42 - 14); // not counting the network header
		reply->SetIPID(packet.GetIPID() + 1);
		reply->SetIPFlags(0);
		reply->SetIPFrag(0);
		reply->SetIPTTL(64);
		reply->SetIPProtocol(Packet::kIPProtocolUDP);
		reply->SetIPSrcIP(packet.GetIPDstIP());
		reply->SetIPDstIP(packet.GetIPSrcIP());
		// UDP
		reply->SetUDPSrcPort(packet.GetUDPDstPort());
		reply->SetUDPDstPort(packet.GetUDPSrcPort());
		reply->SetUDPLength(size + 8);

		if (err)
		{
			// 3 = no such name
			reply->Set16(0x002c, 0x8183);
			reply->Set16(0x0030, 0x0000); // no reply
		} else
		{
			reply->Set16(0x002c, 0x8180);
			reply->Set16(0x0030, 0x0001); // one reply
			// ix was calculated when we read the incomming package
			reply->Set16((KUInt32) ix, 0xc00c);
			ix += 2; // Name
			reply->Set16((KUInt32) ix, 0x0001);
			ix += 2; // Type
			reply->Set16((KUInt32) ix, 0x0001);
			ix += 2; // Class
			reply->Set32((KUInt32) ix, 0x00000e10);
			ix += 4; // TTL
			reply->Set16((KUInt32) ix, 0x0004);
			ix += 2; // Record Length
			reply->Set32((KUInt32) ix, ip);
			ix += 4; // IP
		}
		net->SetIPv4Checksum(reply->Data(), reply->Size());
		net->SetUDPChecksum(reply->Data(), reply->Size());

		//        net->LogBuffer(reply->Data(), reply->Size());

		LOG_PROTOCOL("| DNS reply.");
		LOG_HEADER_DO(net->Log(reply, "| W E>N", __LINE__);)

		net->Enqueue(reply);

		return 1;
	}
};

/**
 * This class handles DHCP requests.
 *
 * Einstein does not forward DHCP requests but instead returns a prefabricated
 * setup. Using fixed IPs will help us later to catch DNS and other requests
 * and map them onto the host network.
 */
class DHCPPacketHandler : public PacketHandler
{
public:
	/**
	 * Can we handle the given package?
	 *
	 * DHCP Packages are special UDP broadcasts that are needed to assign an IP
	 * to a machine that newly joins a network. We reply with a fixed set of
	 * IP adresses for local, mask, gateway, and DNS. We also return
	 * "Einstein.local" as a network name.
	 *
	 * \param packet the Newton packet that could be sent
	 * \param n the network interface
	 * \return 0 if we can not handle this packet
	 * \return 1 if we can handled it and it need not to be propagated any further
	 * \return -1 if an error occurred an no other handler should handle this packet
	 */
	static int
	canHandle(Packet& packet, TUsermodeNetwork* net)
	{
		if (packet.GetType() != Packet::kNetTypeIP)
			return 0;
		if (packet.GetIPProtocol() != Packet::kIPProtocolUDP)
			return 0;
		if (packet.GetUDPDstPort() != 67) // DHCP requests are sent to port 67
			return 0;
		if (packet.Get8(42) != 1)
			return 0;
		//      if (packet.GetIPDstIP()!=0xffffffff) // DHCP are broadcast to 255.255.255.255
		//          return 0;
		if (packet.Get32(0x0116) != 0x63825363) // check for DHCP magic number
			return 0;

		auto mLog = net->GetLog();
		(void) mLog;
		//        LOG_PROTOCOL(",---- Einstein < Newton -----------------");
		LOG_PROTOCOL("| DHCP request %d:", packet.Get8(0x011c));
		LOG_HEADER_DO(net->Log(&packet, "| W E<N", __LINE__);)

		// For Newton packages, Option 53 seems to be locked at this offset
		KUInt8 packetType = packet.Get8(0x011c);
		static constexpr KUInt8 kDHCPDiscover = 1;
		static constexpr KUInt8 kDHCPOffer = 2;
		static constexpr KUInt8 kDHCPRequest = 3;
		static constexpr KUInt8 kDHCPDecline = 4;
		static constexpr KUInt8 kDHCPAcknowledge = 5;
		//      static constexpr KUInt8 kDHCPNACK        = 6;
		static constexpr KUInt8 kDHCPRelease = 7;
		//      static constexpr KUInt8 kDHCPInform      = 8;

		// Release packages are handled by just accepting them
		if (packetType == kDHCPRelease)
		{
			LOG_PROTOCOL("| OK: Release event.");
			return 1;
		}

		// Everything but Discover and Request is unsupported
		if ((packetType != kDHCPDiscover) && (packetType != kDHCPRequest))
		{
			// NOTE: Add handlers for NACK and Inform packet types.
			LOG_PROTOCOL("| Unsupported: Event %d.", packetType);
			return 0;
		}

		// If this is a Discover or Request packet, we reply with a
		// preconfigured DHCP setup. 590 bytes seems to be a magic size.
		Packet* reply = new Packet(0L, 590);
		memset(reply->Data(), 0, 590);

		// Ethernet II
		reply->Set48(0x0000, packet.GetSrcMAC()); // dest MAC: who called?
		reply->Set48(0x0006, kGatewayMac); // src MAC: DHCP's MAC adress
		reply->Set16(0x000c, 0x0800); // type: IP
		// IP4
		reply->Set8(0x000e, 0x45); // Version/length
		reply->Set8(0x000f, 0); // service field
		reply->Set16(0x0010, reply->Size() - 14);
		reply->Set16(0x0012, packet.GetIPID()); // identification (FIXME?)
		reply->Set16(0x0014, 0); // Flags
		reply->Set8(0x0016, 0xff); // TTL
		reply->Set8(0x0017, 17); // Protocol: UDP
		reply->Set16(0x0018, 0); // Checksum (ignore)
		reply->Set32(0x001a, kGatewayIP); // src IP: 10.0.1.1
		reply->Set32(0x001e, kClientIP); // dst IP: 10.0.1.8
		// UDP
		reply->Set16(0x0022, 67); // src port (bootps)
		reply->Set16(0x0024, 68); // dst port (bootpc)
		reply->Set16(0x0026, reply->Size() - 34); // size
		reply->Set16(0x0028, 0); // checksum
		// BOOTP
		reply->Set8(0x002a, 2); // msg type: boot reply
		reply->Set8(0x002b, 1); // hardware type
		reply->Set8(0x002c, 6); // hardware address length
		reply->Set8(0x002d, 0); // hops 0
		reply->Set32(0x002e, packet.Get32(0x002e)); // transaction ID
		reply->Set16(0x0032, 0); // seconds elapsed
		reply->Set16(0x0034, 0); // flags
		reply->Set32(0x0036, 0); // client IP
		reply->Set32(0x003a, kClientIP); // your IP
		reply->Set32(0x003e, kGatewayIP); // server IP
		reply->Set32(0x0042, 0); // relay agent IP
		reply->Set48(0x0046, packet.Get48(6)); // client MAC (padded with 0'a
		// Gap.
		strncpy((char*) (reply->Data() + 0x56), kNetworkName, 32);
		// Gap.
		reply->Set32(0x0116, 0x63825363); // DHCP magic cookie
		// Options
		KUInt8* d = reply->Data() + 0x011a;
		// DHCP option 53: DHCP reply type
		*d++ = 53;
		*d++ = 1;
		if (packetType == kDHCPDiscover)
			*d++ = kDHCPOffer;
		else if (packetType == kDHCPRequest)
			*d++ = kDHCPAcknowledge;
		else
			*d++ = kDHCPDecline;
		// DHCP option 1: 255.255.255.0 subnet mask
		WriteIPOption(d, 1, kClientIPMask);
		// DHCP option 3: 192.168.1.1 router
		WriteIPOption(d, 3, kGatewayIP);
		// DHCP option 6: DNS servers 9.7.10.15, 9.7.10.16, 9.7.10.18
		WriteIPOption(d, 6, kNameServerIP);
		// DHCP option 51: 86400s (1 day) IP lease time
		*d++ = 51;
		*d++ = 4;
		*d++ = 0x00;
		*d++ = 0x01;
		*d++ = 0x51;
		*d++ = 0x80;
		// DHCP option 54: 192.168.1.1 DHCP server
		WriteIPOption(d, 54, kGatewayIP);
		// DHCP end option:
		*d++ = 0xff;

		auto packageSize = 590; // d-reply->Data();
		// reply->SetSize(packageSize);
		reply->SetIPTotalLength(packageSize - 14);
		reply->SetUDPLength(packageSize - 34);
		net->SetIPv4Checksum(reply->Data(), packageSize);
		net->SetUDPChecksum(reply->Data(), packageSize);

		LOG_PROTOCOL("| DHCP reply %d:", reply->Get8(0x011c));
		LOG_HEADER_DO(net->Log(reply, "| W E>N", __LINE__);)

		net->Enqueue(reply);

		return 1;
	}

	static void
	WriteIPOption(KUInt8*& d, KUInt8 id, KUInt32 ip)
	{
		*d++ = id;
		*d++ = 4;
		*d++ = (KUInt8) (ip >> 24);
		*d++ = (KUInt8) (ip >> 16);
		*d++ = (KUInt8) (ip >> 8);
		*d++ = (KUInt8) (ip);
	}
};

// MARK: - TUsermodeNetwork -

/**
 * Create a network driver that connects Newton to the world.
 */
TUsermodeNetwork::TUsermodeNetwork(TLog* inLog) :
		TNetworkManager(inLog)
{
#if TARGET_OS_WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
}

/**
 * Free all resources.
 */
TUsermodeNetwork::~TUsermodeNetwork()
{
	// release the package pipe
	while (mFirstPacket)
		DropPacket();
	// release all package handlers
	while (mFirstPacketHandler)
		RemovePacketHandler(mFirstPacketHandler);
		// release all other resources
#if TARGET_OS_WIN32
	WSACleanup();
#endif
}

/**
 * This function handles packet that are sent by the Newton to the outside
 * world. In user mode, these packages are emulated for information that is
 * already available on the host, or forwarded to a host socket. A possible
 * reply must be interpreted and a raw package must be generated for the Newton.
 */
int
TUsermodeNetwork::SendPacket(KUInt8* data, KUInt32 size)
{
	int result = 0;
	Packet packet(data, size, 0); // convert data into a packet
	LOG_PROTOCOL(",---- Einstein < Newton -----------------");

	// offer this package to all active handlers
	PacketHandler* ph = mFirstPacketHandler;
	while (ph)
	{
		result = ph->send(packet);
		// A result of 1 means handled, -1 means error, either way, we are done.
		if (result != 0)
			goto done;
		// A result of 0 means that we could not handle this package, so we will
		// try to find another handler.
		ph = ph->mNext;
	}

	LOG_PROTOCOL("| Searching for a new handler:");
	LOG_HEADER_DO(Log(&packet, "| W E<N", __LINE__);)

	// None of the existing handlers could work with the package. Now offer the
	// package to all manager classes, possibly creating a new handler.
	result = DHCPPacketHandler::canHandle(packet, this);
	if (result != 0)
		goto done;

	result = DNSPacketHandler::canHandle(packet, this);
	if (result != 0)
		goto done;

	result = ARPPacketHandler::canHandle(packet, this);
	if (result != 0)
		goto done;

	result = TCPPacketHandler::canHandle(packet, this);
	if (result != 0)
		goto done;

	result = UDPPacketHandler::canHandle(packet, this);
	if (result != 0)
		goto done;

	LOG_PROTOCOL("| Unsupported package");
	LOG_PACKAGE_DO(LogPacket(packet.Data(), packet.Size());)
	LOG_CONTENT_DO(LogBuffer(packet.Data(), packet.Size());)

done:
	int ret = -1;
	if (result == -1)
	{
		LOG_PROTOCOL("`---- Einstein < Newton -- Aborted ------");
		ret = -1;
	} else if (result == 1)
	{
		LOG_PROTOCOL("`---- Einstein < Newton -- OK -----------");
		ret = 0;
	} else
	{
		LOG_PROTOCOL("`---- Einstein < Newton -- Unsupported --");
		result = -1;
	}
	return ret;
}

/**
 * We forward the timer of the Newton driver to all packet handlers.
 */
int
TUsermodeNetwork::TimerExpired()
{
	PacketHandler* ph = mFirstPacketHandler;
	while (ph)
	{
		PacketHandler* pNext = ph->mNext;
		ph->timer();
		ph = pNext;
	}
	return 0;
}

/**
 * Return the MAC address of the card that we emulate.
 */
int
TUsermodeNetwork::GetDeviceAddress(KUInt8* data, KUInt32 size)
{
	(void) size;
	// TODO: of course we need the true MAC of this ethernet card
	// see: ioctl ? getifaddrs ? http://othermark.livejournal.com/3005.html
	static KUInt8 gLocalMAC[] = { 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x22 };
	// assert(size==6);
	memcpy(data, gLocalMAC, 6);
	return 0;
}

/**
 * Return the number of bytes available for the Newton driver.
 */
KUInt32
TUsermodeNetwork::DataAvailable()
{
	// static std::chrono::time_point<std::chrono::steady_clock> delayBase = 0;
	static auto delayBase = std::chrono::steady_clock::now();

	if (mLastPacket)
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> delayDelta = now - delayBase;
		auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(delayDelta);
		if (seconds.count() < 0.25)
			return 0;
		delayBase = now;
		return (KUInt32) mLastPacket->Size();
	} else
	{
		return 0;
	}
}

/**
 * Fill the provided buffer with a raw packet.
 */
int
TUsermodeNetwork::ReceiveData(KUInt8* data, KUInt32 size)
{
	LOG_CHATTY("    [ W E>N Newton requests packet, size %d", size);
	Packet* pkt = mLastPacket;
	if (pkt)
	{
		LOG_CHATTY("    [ W E>N Packet %d available, size %d.", pkt->Index(), size);
		if (size == pkt->Size())
		{
			memcpy(data, pkt->Data(), size);
			DropPacket();
			return 0;
		} else
		{
			// TODO: if Newton asks for less bytes than we have in the package
			// we must split the packet in two new ones. But does that ever happen?
			LOG_ERROR("    [ W E>N Newton requested %d bytes, but pending packet %d has %dbytes.", size, pkt->Index(), pkt->Size());
			return -1;
		}
	} else
	{
		LOG_ERROR("    [ W E>N Newton requested %d bytes, but no packet is available.", size, pkt->Index(), pkt->Size());
		return -1;
	}
}

/**
 * Add a new handler for a package type to the list.
 */
void
TUsermodeNetwork::AddPacketHandler(PacketHandler* inPacketHandler)
{
	PacketHandler* n = mFirstPacketHandler;
	if (n)
		n->mPrev = inPacketHandler;
	else
		mLastPacketHandler = inPacketHandler;
	inPacketHandler->mNext = n;
	inPacketHandler->mPrev = NULL;
	mFirstPacketHandler = inPacketHandler;
}

/**
 * Remove a packet handler from the list.
 */
void
TUsermodeNetwork::RemovePacketHandler(PacketHandler* ph)
{
	PacketHandler* pp = ph->mPrev;
	PacketHandler* pn = ph->mNext;
	if (pp)
		pp->mNext = pn;
	else
		mFirstPacketHandler = pn;
	if (pn)
		pn->mPrev = pp;
	else
		mLastPacketHandler = pp;
	delete ph;
}

/**
 * Add a new packet to the beginning of the pipe.
 * This makes the given block ready to be sent at the next possible occasion.
 *
 * \param inPacket the package that will be queued
 */
void
TUsermodeNetwork::Enqueue(Packet* inPacket)
{
	Packet* n = mFirstPacket;
	if (n)
		n->mPrev = inPacket;
	else
		mLastPacket = inPacket;
	inPacket->mNext = n;
	inPacket->mPrev = NULL;
	mFirstPacket = inPacket;
}

/**
 * Drops the last packet in the pipe.
 */
void
TUsermodeNetwork::DropPacket()
{
	Packet* pkt = mLastPacket;
	if (pkt)
	{
		Packet* prevPkt = pkt->mPrev;
		if (prevPkt)
			prevPkt->mNext = NULL;
		else
			mFirstPacket = NULL;
		mLastPacket = prevPkt;
		delete pkt;
	}
}

/**
 * Write a brief one-liner description of the given package.
 */
void
TUsermodeNetwork::Log(Packet* p, const char* label, int line, int adjSeq, int adjAck)
{
	if (!mLog)
		return;
	char buf[2048];
	buf[0] = 0;
	if (label)
	{
		strncat(buf, label, sizeof(buf) - 1);
		if (*label)
			strncat(buf, " ", sizeof(buf) - 1);
	}
	if (p->GetType() == Packet::kNetTypeIP
		&& p->GetIPProtocol() == Packet::kIPProtocolTCP)
	{
		ssize_t o = strlen(buf);
		char as = adjSeq ? '*' : 'q';
		char aa = adjAck ? '*' : 'k';
		snprintf(buf + o, 2047 - o, "TCP:p:%d %c%c%c%c%c%c %4u bytes payload (Se%c:%4u, Ac%c:%4u) [",
			p->Index(),
			(unsigned int) (p->GetTCPFlags() & Packet::kTCPFlagURG ? 'U' : '.'),
			(unsigned int) (p->GetTCPFlags() & Packet::kTCPFlagACK ? 'A' : '.'),
			(unsigned int) (p->GetTCPFlags() & Packet::kTCPFlagPSH ? 'P' : '.'),
			(unsigned int) (p->GetTCPFlags() & Packet::kTCPFlagRST ? 'R' : '.'),
			(unsigned int) (p->GetTCPFlags() & Packet::kTCPFlagSYN ? 'S' : '.'),
			(unsigned int) (p->GetTCPFlags() & Packet::kTCPFlagFIN ? 'F' : '.'),
			(unsigned int) (p->GetTCPPayloadSize()),
			as, (unsigned int) ((p->GetTCPSeq() - adjSeq) % 9999),
			aa, (unsigned int) ((p->GetTCPAck() - adjAck) % 9999));
		ssize_t i = 0, s = p->GetTCPPayloadStart() - p->Data();
		o = strlen(buf);
		while (s < p->Size() && i < 128)
		{
			KUInt8 c = p->Data()[s];
			if (c < 32 || c > 126)
				c = '.';
			buf[i + o] = (char) c;
			s++;
			i++;
		}
		if (s == p->Size())
			buf[i + o] = ']';
		else
			buf[i + o] = '+';
		buf[i + o + 1] = 0;
	} else if (p->GetType() == Packet::kNetTypeIP
		&& p->GetIPProtocol() == Packet::kIPProtocolUDP
		&& p->GetUDPDstPort() == 67
		&& p->Get8(42) == 1
		&& p->GetIPDstIP() == 0xffffffff
		&& p->Get32(0x0116) == 0x63825363)
	{
		ssize_t o = strlen(buf);
		snprintf(buf + o, 2047 - o, "DHCP:p:%d:%u bytes", p->Index(), (unsigned int) p->Size());
	} else if (p->GetType() == Packet::kNetTypeIP
		&& p->GetIPProtocol() == Packet::kIPProtocolUDP)
	{
		ssize_t o = strlen(buf);
		snprintf(buf + o, 2047 - o, "UDP:p:%d %4u bytes [", p->Index(), (unsigned int) (p->GetUDPPayloadSize()));
		ssize_t i = 0, s = p->GetUDPPayloadStart() - p->Data();
		o = strlen(buf);
		while (s < p->Size() && i < 128)
		{
			KUInt8 c = p->Data()[s];
			if (c < 32 || c > 126)
				c = '.';
			buf[i + o] = (char) c;
			s++;
			i++;
		}
		if (s == p->Size())
			buf[i + o] = ']';
		else
			buf[i + o] = '+';
		buf[i + o + 1] = 0;
	} else if (p->GetType() == Packet::kNetTypeARP)
	{
		ssize_t o = strlen(buf);
		snprintf(buf + o, 2047 - o, "ARP:p:%d:%u bytes", p->Index(), (unsigned int) p->GetARPHLen());
	} else
	{
		ssize_t o = strlen(buf);
		snprintf(buf + o, 2047 - o, "????:p:%d:%u bytes", p->Index(), (unsigned int) p->Size());
	}
	mLog->FLogLine("Net: %4d: %s", line, buf);
}

// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
