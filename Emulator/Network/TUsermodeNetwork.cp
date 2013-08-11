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

 Newton Synchronization happens on port: TCP 3679 (use NCX to test)
 
 
 */

#include <K/Defines/KDefinitions.h>
#include "TUsermodeNetwork.h"
#include "Emulator/Log/TLog.h"

//
// Handle all kinds of network packages
//

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#ifdef __ANDROID__
# include <stdio.h>
# include <fcntl.h>
#else
# include <sys/sockio.h>
# include <ifaddrs.h>
#endif

#if TARGET_IOS
#include <fcntl.h>
#endif


const KUInt32 kUDPExpirationTime = 100;


/**
 * This class is used to build and interprete TCP/IP packages.
 */
class Packet
{
public:
	/** Used in GetTCPFlags() */
	static const KUInt16 TCPFlagURG = 0x0020;
	static const KUInt16 TCPFlagACK = 0x0010;
	static const KUInt16 TCPFlagPSH = 0x0008;
	static const KUInt16 TCPFlagRST = 0x0004;
	static const KUInt16 TCPFlagSYN = 0x0002;
	static const KUInt16 TCPFlagFIN = 0x0001;
	
	/** Used in GetType(). */
	static const KUInt16 NetTypeIP	= 0x0800;
	static const KUInt16 NetTypeARP = 0x0806;
	
	/** Used in GetIPProtocol() */
	static const KUInt8 IPProtocolTCP =  6;
	static const KUInt8 IPProtocolUDP = 17;
	
	/** 
	 * Create a new packet.
	 * \param data pointer to some place in memory that contains the packet data
	 * \param size number of bytes in this packet
	 * \param copy if set, the packet data will be copied, otherwise this class
	 *        will only point at the buffer
	 */
	Packet(KUInt8 *data, KUInt32 size, bool copy=1) {
		if (copy) {
			mData = (KUInt8*)malloc(size);
			if (data) {
				memcpy(mData, data, size);
			} else {
				memset(mData, 0, size);
			}
			mCopy = 1;
		} else {
			if (data) {
				mData = data;
				mCopy = 0;
			} else {
				mData = (KUInt8*)calloc(size, 1);
				mCopy = 1;
			}
		}
		mSize = size;
	}
	
	/**
	 * Remove the packet.
	 * If the data was copied, we remove the copy as well.
	 */
	~Packet() {
		if (mData && mCopy) 
			free(mData);
	}
	
	/**
	 * Get a pointer to the raw packet data.
	 * \return pointer to the data
	 */
	KUInt8 *Data() { return mData; }
	
	/**
	 * Get the number of bytes in the buffer.
	 * \return number of bytes.
	 */
	KUInt32 Size() { return mSize; }

	/**
	 * Read a six-byte word in network order.
	 * This is used to quickly handle MAC adresses.
	 * \return a six-byte sequence in the buffer as a 64 bit number.
	 */
	KUInt64 Get48(KUInt32 i) { 
		KUInt64 a0 = (KUInt64(mData[i+0]))<<40;
		KUInt64 a1 = (KUInt64(mData[i+1]))<<32;
		KUInt64 a2 = (KUInt64(mData[i+2]))<<24;
		KUInt64 a3 = (KUInt64(mData[i+3]))<<16;
		KUInt64 a4 = (KUInt64(mData[i+4]))<<8;
		KUInt64 a5 = (KUInt64(mData[i+5]));
		return a0|a1|a2|a3|a4|a5; }
	void Set48(KUInt32 i, KUInt64 v) {
		mData[i+0] = v>>40;
		mData[i+1] = v>>32;
		mData[i+2] = v>>24;
		mData[i+3] = v>>16;
		mData[i+4] = v>>8;
		mData[i+5] = v; }
	
	/** Get a 32-bit word in network order. */
	KUInt32 Get32(KUInt32 i) { return (mData[i]<<24)|(mData[i+1]<<16)|(mData[i+2]<<8)|(mData[i+3]); }
	void Set32(KUInt32 i, KUInt32 v) { 
		mData[i+0] = v>>24;
		mData[i+1] = v>>16;
		mData[i+2] = v>>8;
		mData[i+3] = v; }
	
	/** Get a 16-bit word in network order. */
	KUInt16 Get16(KUInt32 i) { return (mData[i]<<8)|(mData[i+1]); }
	void Set16(KUInt32 i, KUInt16 v) { 
		mData[i+0] = v>>8;
		mData[i+1] = v; }
	
	/** Get a 8-bit byte in network order. */
	KUInt8 Get8(KUInt32 i) { return mData[i]; }
	void Set8(KUInt32 i, KUInt8 v) { mData[i] = v; }
	
	/**
	 * All the functions below get and set members of raw TCP packets including network header.
	 * Details can be found all over the web.
	 */
	KUInt64 GetDstMAC()			{ return Get48(0); }
	KUInt64 GetSrcMAC()			{ return Get48(6); }
	KUInt16 GetType()			{ return Get16(12); }
	
	KUInt8	GetIPVersion()		{ return mData[14]>>4; }
	KUInt8	GetIPHeaderLength() { return (mData[14]&0x0f)*4; } // in bytes!
	KUInt8	GetIPTOS()			{ return mData[15]; }
	KUInt16 GetIPTotalLength()	{ return Get16(16); }
	KUInt16 GetIPID()			{ return Get16(18); }
	KUInt8	GetIPFlags()		{ return mData[20]>>5; }
	KUInt16 GetIPFrag()			{ return Get16(20)&0x1fff; }
	KUInt8	GetIPTTL()			{ return mData[22]; }
	KUInt8	GetIPProtocol()		{ return mData[23]; }
	KUInt16 GetIPChecksum()		{ return Get16(24); }
	KUInt32	GetIPSrcIP()		{ return Get32(26); }
	KUInt32	GetIPDstIP()		{ return Get32(30); }
	
	KUInt16 GetTCPSrcPort()		{ return Get16(34); }
	KUInt16 GetTCPDstPort()		{ return Get16(36); }
	KUInt32	GetTCPSeq()			{ return Get32(38); }
	KUInt32	GetTCPAck()			{ return Get32(42); }
	KUInt8	GetTCPHeaderLength(){ return (mData[46]>>4)<<2; } // in bytes!
	KUInt16 GetTCPFlags()		{ return Get16(46)&0x0fff; }
	KUInt16 GetTCPWindow()		{ return Get16(48); }
	KUInt16 GetTCPChecksum()	{ return Get16(50); }
	KUInt16 GetTCPUrgent()		{ return Get16(52); }
	KUInt8 *GetTCPPayloadStart(){ return mData + GetTCPHeaderLength() + 34; }
	KUInt32	GetTCPPayloadSize()	{ return mSize - GetTCPHeaderLength() - 34; }

	KUInt16 GetUDPSrcPort()		{ return Get16(34); }
	KUInt16 GetUDPDstPort()		{ return Get16(36); }
	KUInt16 GetUDPLength()		{ return Get16(38); }
	KUInt16 GetUDPChecksum()	{ return Get16(40); }
	KUInt8 *GetUDPPayloadStart(){ return mData + 42; }
	KUInt32	GetUDPPayloadSize()	{ return mSize - 42; }

	KUInt16 GetARPHType()		{ return Get16(14); }
	KUInt16 GetARPPType()		{ return Get16(16); }
	KUInt8	GetARPHLen()		{ return mData[18]; }
	KUInt8	GetARPPLen()		{ return mData[19]; }
	KUInt16 GetARPOp()			{ return Get16(20); }
	KUInt64 GetARPSHA()			{ return Get48(22); }
	KUInt32	GetARPSPA()			{ return Get32(28); }
	KUInt64 GetARPTHA()			{ return Get48(32); }
	KUInt32	GetARPTPA()			{ return Get32(38); }

	
	void	SetDstMAC(KUInt64 v)		{ Set48(0, v); }
	void	SetSrcMAC(KUInt64 v)		{ Set48(6, v); }
	void	SetType(KUInt16 v)			{ Set16(12, v); }
	
	void	SetIPVersion(KUInt8 v)		{ mData[14] = (mData[14]&0x0f) | (v<<4); }
	void	SetIPHeaderLength(KUInt8 v) { mData[14] = (mData[14]&0xf0) | ((v/4)&0x0f); } // in bytes!
	void	SetIPTOS(KUInt8 v)			{ mData[15] = v; }
	void	SetIPTotalLength(KUInt16 v)	{ Set16(16, v); }
	void	SetIPID(KUInt16 v)			{ Set16(18, v); }
	void	SetIPFlags(KUInt8 v)		{ mData[20] = (mData[20]&0x1f) | (v<<5); }
	void	SetIPFrag(KUInt16 v)		{ mData[20] = (mData[20]&0xe0) | ((v>>8)&0x1f); mData[21] = v; }
	void	SetIPTTL(KUInt8 v)			{ mData[22] = v; }
	void	SetIPProtocol(KUInt8 v)		{ mData[23] = v; }
	void	SetIPChecksum(KUInt16 v)	{ Set16(24, v); }
	void	SetIPSrcIP(KUInt32 v)		{ Set32(26, v); }
	void	SetIPDstIP(KUInt32 v)		{ Set32(30, v); }
	
	void	SetTCPSrcPort(KUInt16 v)	{ Set16(34, v); }
	void	SetTCPDstPort(KUInt16 v)	{ Set16(36, v); }
	void	SetTCPSeq(KUInt32 v)		{ Set32(38, v); }
	void	SetTCPAck(KUInt32 v)		{ Set32(42, v); }
	void	SetTCPHeaderLength(KUInt8 v){ mData[46] = (mData[46]&0x0f) | ((v>>2)<<4); } // in bytes!
	void	SetTCPFlags(KUInt16 v)		{ mData[46] = (mData[46]&0xf0) | ((v>>8)&0x0f); mData[47] = v; }
	void	SetTCPWindow(KUInt16 v)		{ Set16(48, v); }
	void	SetTCPChecksum(KUInt16 v)	{ Set16(50, v); }
	void	SetTCPUrgent(KUInt16 v)		{ Set16(52, v); }
	void	SetTCPPayload(KUInt8 *, KUInt32);

	void	SetUDPSrcPort(KUInt16 v)	{ Set16(34, v); }
	void	SetUDPDstPort(KUInt16 v)	{ Set16(36, v); }
	void	SetUDPLength(KUInt16 v)		{ Set16(38, v); }
	void	SetUDPChecksum(KUInt16 v)	{ Set16(40, v); }
	void	SetUDPPayload(KUInt8 *, KUInt32);

	void	SetARPHType(KUInt16 v)		{ Set16(14, v); }
	void	SetARPPType(KUInt16 v)		{ Set16(16, v); }
	void	SetARPHLen(KUInt8 v)		{ mData[18] = v; }
	void	SetARPPLen(KUInt8 v)		{ mData[19] = v; }
	void	SetARPOp(KUInt16 v)			{ Set16(20, v); }
	void	SetARPSHA(KUInt64 v)		{ Set48(22, v); }
	void	SetARPSPA(KUInt32 v)		{ Set32(28, v); }
	void	SetARPTHA(KUInt64 v)		{ Set48(32, v); }
	void	SetARPTPA(KUInt32 v)		{ Set32(38, v); }
	
	void LogPayload(TLog *mLog, const char *label) {
		if (mLog) {
			char buf[200];
			strcpy(buf, "Net: ");
			if (label) { strcat(buf, label); strcat(buf, " "); }
			if ( GetType()==NetTypeIP ) {
				if ( GetIPProtocol()==IPProtocolTCP ) {
					KUInt32 o = strlen(buf);
					sprintf(buf+o, "TCP %c%c%c%c%c %4u bytes payload (Seq:%4u, Ack:%4u) [",
							(unsigned int)(GetTCPFlags()&0x10?'A':'.'),
							(unsigned int)(GetTCPFlags()&0x08?'P':'.'),
							(unsigned int)(GetTCPFlags()&0x04?'R':'.'),
							(unsigned int)(GetTCPFlags()&0x02?'S':'.'),
							(unsigned int)(GetTCPFlags()&0x01?'F':'.'),
							(unsigned int)(GetTCPPayloadSize()), 
							(unsigned int)(GetTCPSeq()%9999), 
							(unsigned int)(GetTCPAck()%9999));
					KUInt32 i = 0, s = GetTCPPayloadStart()-mData;
					o = strlen(buf);
					while (s<mSize && i<128) {
						KUInt8 c = mData[s];
						if (c<32 || c>126) c = '.';
						buf[i+o] = (char)c;
						s++; i++;
					}
					if (s==mSize)
						buf[i+o] = ']';
					else
						buf[i+o] = '+';
					buf[i+o+1] = 0;
				} else {
					strcat(buf, "UDP");
				}
			} else {
				KUInt32 o = strlen(buf);
				sprintf(buf+o, "%u bytes", (unsigned int)mSize);
			}
			mLog->LogLine(buf);
		}
	}
	
	Packet *prev, *next;
	
private:
	KUInt8 *mData;
	KUInt32 mSize;
	KUInt8	mCopy;
};



/**
 * This is a generic handler for network packets.
 *
 * To handle new types of packats, a new class should be derived.
 */
class PacketHandler 
{
public:
	
	/**
	 * Create a new packet handler.
	 *
	 * \param h link handler to this network interface
	 */
	PacketHandler(TUsermodeNetwork *h) :
		net(h)
	{
	}
	
	/**
	 * Remove a packet handler, freeing all resources.
	 */
	virtual ~PacketHandler() {
	}
	
	/**
	 * Send a Newton packet to the outside world.
	 * \param p send this packet
	 * \return 1 if the packet was handled ok and it should not be sent by any other handler
	 * \return 0 if we don't know how to handle the packet and another handler should have a go
	 * \return -1 if an error occured and the packet should not be sent by any other handler
	 */
	virtual int send(Packet &p) { return 0; }
	
	/**
	 * Every 10th of a second or so handle outstanding tasks.
	 */
	virtual void timer() { }
	
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
	static int canHandle(Packet &p, TUsermodeNetwork *n) { return 0; }
	
	PacketHandler *prev, *next;
	TUsermodeNetwork *net;
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
		kStateDisconnected = 0,		// handler just created, no socket
									// we expect a SYN package from the Newt and will reply with SYN ACK (or timeout)
		kStateConnected,			// socket was opened successfully
		kStatePeerDiscWaitForACK,	// Peer disconnected, send a FIN and wait for an ACK
		kStatePeerDiscWaitForFIN,	// Perr disconnected, now wait for the FIN from the Newt and ACK it.
									// Newt requests a disconnect
									// Peer request a disconnect
		kStateError,
		
		kStateClosed,				// States according to TCP State Transition Diagram
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
	
	/**
	 * Create a TCP packet handler.
	 * A TCP connection can be uniquely identified by the destinatin MAC, IP, and 
	 * port number. Let's remember these now so we can later identify the packets
	 * that we need to handle.
	 */
	TCPPacketHandler(TUsermodeNetwork *h, Packet &packet) :
	PacketHandler(h),
	myMAC(0),	theirMAC(0),
	myIP(0),	theirIP(0),
	myPort(0),	theirPort(0),
	mySeqNr(0),	theirSeqNr(0),
	theirID(0),
	state(kStateDisconnected),
	mSocket(-1)
	{
		myMAC = packet.GetSrcMAC();
		myIP = packet.GetIPSrcIP();
		myPort = packet.GetTCPSrcPort();
		mySeqNr = packet.GetTCPSeq();
		
		theirMAC = packet.GetDstMAC();
		theirIP = packet.GetIPDstIP();
		theirPort = packet.GetTCPDstPort();
		theirSeqNr = packet.GetTCPAck();
		theirID = 1000;
		printf("Net: Adding TCP handler for port %u to %u.%u.%u.%u\n", 
			   theirPort,
			   (unsigned int)((theirIP>>24)&0xff), 
			   (unsigned int)((theirIP>>16)&0xff), 
			   (unsigned int)((theirIP>>8)&0xff), 
			   (unsigned int)(theirIP&0xff));
	}
	
	/**
	 * Delete this handler.
	 */
	~TCPPacketHandler() 
	{
		if ( mSocket != -1 ) {
			if ( state!=kStateDisconnected ) {
				shutdown( mSocket, SHUT_RDWR);
			}
			close(mSocket);
			mSocket = -1;
		}
	}
	
	/**
	 * Create a generic TCP packet.
	 * This is a working TCP packet for this particular connection. Space is 
	 * allocated for the payload. The payload must be copied into this 
	 * packet an the checksums must be updated.
	 * \param size this is the desired size of the payload.
	 * \see UpdateChecksums(Packet *p)
	 * \see Packet::SetTCPPayload(KUInt8 *, KUInt32)
	 */
	Packet *NewPacket(KUInt32 size) {
		Packet *p = new Packet(0L, size+54);
		p->SetDstMAC(myMAC);
		p->SetSrcMAC(theirMAC);
		p->SetType(Packet::NetTypeIP);
		
		p->SetIPVersion(4);
		p->SetIPHeaderLength(20);
		p->SetIPTOS(0);
		p->SetIPTotalLength(size+54-14);	// not counting the network header
		p->SetIPID(theirID); theirID++;
		p->SetIPFlags(0);
		p->SetIPFrag(0);
		p->SetIPTTL(64);
		p->SetIPProtocol(Packet::IPProtocolTCP);
		p->SetIPSrcIP(theirIP);
		p->SetIPDstIP(myIP);
		
		p->SetTCPSrcPort(theirPort);
		p->SetTCPDstPort(myPort);
		p->SetTCPSeq(theirSeqNr);
		p->SetTCPAck(mySeqNr);
		p->SetTCPHeaderLength(20);
		p->SetTCPFlags(Packet::TCPFlagACK);
		p->SetTCPWindow(0x1000);		
		
		return p;
	}
	
	void UpdateChecksums(Packet *p) {
		net->SetIPv4Checksum(p->Data(), p->Size());
		net->SetTCPChecksum(p->Data(), p->Size());
	}
	
	/**
	 * Connect to a client.
	 * The class was just created. The first package that we expect 
	 * is the SYN package that opens the socket and connects to the 
	 * remote system.
	 * \return  1 if the connection was established
	 * \return -1 if no connection could be established
	 */
	int connect(Packet &packet) {
		// create a socket
		mSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mSocket==-1) 
			return -1;
		// FIXME: should we remove the handler? Should we remove some kind of not-ACK package?
		
		// tell the socket who to connect to and connect
		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(theirPort);
		sa.sin_addr.s_addr = htonl(theirIP);
		// TODO: connect() will block. We can also put this socket into non-blocking first
		int err = ::connect(mSocket, (struct sockaddr*)&sa, sizeof(sa));
		if (err==-1) {
			fprintf(stderr, "Can't connect: %s\n", strerror(errno));
			return -1;
		}
		
		// perform some presets for the socket
		int fl = fcntl(mSocket, F_GETFL);
		err = fcntl(mSocket, F_SETFL, fl|O_NONBLOCK);
		if (err==-1)
			return -1;
		
		// succesful connection, return a SYN ACK to the Newton.
		mySeqNr++; 
		Packet *reply = NewPacket(4); // 4 bytes are reserved for the extended TCP header
		reply->SetTCPFlags(Packet::TCPFlagACK|Packet::TCPFlagSYN);
		reply->SetTCPHeaderLength(24);
		reply->Set32(54, 0x02040578); // set Maximum Segment Size
		UpdateChecksums(reply);
		reply->LogPayload(net->GetLog(), "W E>N");
		net->Enqueue(reply);
		state = kStateConnected;
		return 1;
	}
	
	/**
	 * Send a Newton packet to the outside world.
	 * \param packet send this packet
	 * \return 1 if the packet was handled ok
	 * \return 0 if we don't know how to handle the packet
	 * \return -1 if an error occured and no other handler should handle this packet
	 */
	virtual int send(Packet &packet) {
		if (   ( packet.GetType() != Packet::NetTypeIP ) 
			|| ( packet.GetIPProtocol() != Packet::IPProtocolTCP ) 
			|| ( myPort != packet.GetTCPSrcPort() )
			|| ( theirIP != packet.GetIPDstIP() )
			|| ( theirPort != packet.GetTCPDstPort() ) )
			return 0;
		switch (state) {
			case kStateDisconnected:
				// FIXME: we assume that the original packet is a correct SYN packet
				return connect(packet);
			case kStateConnected:
				// The socket is connected. Traffic can come from either side
				if (packet.GetTCPPayloadSize()==0) { 
					// this is an acknowledge package
					theirSeqNr = packet.GetTCPAck();
				} else { // this is a data package, ack needed
					// FIXME: if FIN is set, we need to start disconnecting
					packet.LogPayload(net->GetLog(), "W<E N");
					write(mSocket, packet.GetTCPPayloadStart(), packet.GetTCPPayloadSize());					
					Packet *reply = NewPacket(0);
					mySeqNr += packet.GetTCPPayloadSize(); 
					reply->SetTCPAck(mySeqNr);
					reply->SetTCPFlags(Packet::TCPFlagACK);
					UpdateChecksums(reply);
					reply->LogPayload(net->GetLog(), "W E>N");
					net->Enqueue(reply);
					// TODO: now enqueue the reply from the remote client
				}
				return 1;
			case kStatePeerDiscWaitForACK:
				// TODO: make sure that this really is the FIN ACK packege (FIN's not set!)
				theirSeqNr = packet.GetTCPAck();
				state = kStatePeerDiscWaitForFIN;
				return 1;
			case kStatePeerDiscWaitForFIN: {
				theirSeqNr = packet.GetTCPAck();
				// TODO: make sure that this really is the FIN packege from the Newton!
				// FIXME: actually, instead of a FIN we may get a second ACK which would
				// mean that the Newton does not agree to closing the connection. So we have
				// to reopen the socket (or find a way to keep the peer from closing the socket).
				// SO_LINGER, TIME_WAIT, SO_REUSEADDR, close vs. shutdown, getpeername()
				// http://www.faqs.org/faqs/unix-faq/socket/
				if ( (packet.GetTCPFlags()&Packet::TCPFlagFIN)==0 ) {
					struct sockaddr_in sa;
					memset(&sa, 0, sizeof(sa));
					sa.sin_family = AF_INET;
					sa.sin_port = htons(theirPort);
					sa.sin_addr.s_addr = htonl(theirIP);
					int err = ::connect(mSocket, (struct sockaddr*)&sa, sizeof(sa));
					// perform some presets for the socket
					int fl = fcntl(mSocket, F_GETFL);
					err = fcntl(mSocket, F_SETFL, fl|O_NONBLOCK);
					state = kStateConnected;
					return 1;
				}
				// Acknowledge the FIN request
				Packet *reply = NewPacket(0);
				reply->SetTCPAck(mySeqNr+1);
				reply->SetTCPFlags(Packet::TCPFlagACK);
				UpdateChecksums(reply);
				reply->LogPayload(net->GetLog(), "W E>N");
				net->Enqueue(reply);				
				close(mSocket);
				net->RemovePacketHandler(this);
				printf("Net: Peer closing. Removing TCP handler for port %u to %u.%u.%u.%u\n", 
					   theirPort,
					   (unsigned int)((theirIP>>24)&0xff), 
					   (unsigned int)((theirIP>>16)&0xff), 
					   (unsigned int)((theirIP>>8)&0xff), 
					   (unsigned int)(theirIP&0xff));
				delete this;
				return 1; }
			default:
				printf("---> TCP send: unsupported state in 'send()'\n");
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
	virtual void timer() {
		switch (state) {
			case kStatePeerDiscWaitForACK:
				// TODO: count down the timer and remove myself if expired
				return;
			case kStatePeerDiscWaitForFIN:
				// TODO: count down the timer and remove myself if expired
				return;
			case kStateDisconnected:
				// TODO: this should not happen
				return;
			default:
				break;
		}
		// ok, the state is kStateConnected. See if there are any icomming messages
		KUInt8 buf[TUsermodeNetwork::kMaxTxBuffer];
		int avail = recv(mSocket, buf, sizeof(buf), 0);
		if (avail==0) {
			// Peer has closed connection.
			Packet *reply = NewPacket(0);
			// Send a FIN request to the Newton
			reply->SetTCPFlags(Packet::TCPFlagFIN|Packet::TCPFlagACK);
			UpdateChecksums(reply);
			reply->LogPayload(net->GetLog(), "W E>N");
			net->Enqueue(reply);
			// Wait for the newton to acknowledge the FIN
			state = kStatePeerDiscWaitForACK;
		} else if (avail>0) {
			printf("Net: W>E N Data %d bytes\n", avail);
			//if (avail>200) avail = 200;
			Packet *reply = NewPacket(avail);
			// /*ssize_t n =*/ read(mSocket, reply->GetTCPPayloadStart(), avail);
			memcpy(reply->GetTCPPayloadStart(), buf, avail);
			reply->SetTCPFlags(Packet::TCPFlagACK|Packet::TCPFlagPSH);
			UpdateChecksums(reply);
			reply->LogPayload(net->GetLog(), "W E>N");
			net->Enqueue(reply);
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
	static int canHandle(Packet &packet, TUsermodeNetwork *net) {
		if ( packet.GetType() != Packet::NetTypeIP ) 
			return 0;
		if ( packet.GetIPProtocol() != Packet::IPProtocolTCP ) 
			return 0;
		//if ( packet.GetTCPFlags() != Packet::TCPFlagSYN && packet.GetTCPFlags() != Packet::TCPFlagRST) 
		//	return 0; // only SYN is set
		PacketHandler *ph = new TCPPacketHandler(net, packet);
		net->AddPacketHandler(ph);
		return ph->send(packet);
	}
	
	KUInt64		myMAC, theirMAC;
	KUInt32		myIP, theirIP;
	KUInt16		myPort, theirPort;
	KUInt32		mySeqNr, theirSeqNr;
	KUInt16		theirID;
	enum State	state;
	int			mSocket;
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
	UDPPacketHandler(TUsermodeNetwork *h, Packet &packet) :
		PacketHandler(h),
		myMAC(0),		theirMAC(0),
		myIP(0),		theirIP(0),
		myPort(0),		theirPort(0),
						theirID(2000),
		mSocket(-1),	
		mExpire(kUDPExpirationTime)
	{
		myMAC = packet.GetSrcMAC();
		myIP = packet.GetIPSrcIP();
		myPort = packet.GetUDPSrcPort();		
		theirMAC = packet.GetDstMAC();
		theirIP = packet.GetIPDstIP();
		theirPort = packet.GetUDPDstPort();
		printf("Net: Adding UDP handler for port %d to %u.%u.%u.%u\n", 
			   theirPort,
			   (unsigned int)((theirIP>>24)&0xff), 
			   (unsigned int)((theirIP>>16)&0xff), 
			   (unsigned int)((theirIP>>8)&0xff), 
			   (unsigned int)(theirIP&0xff));
	}
	
	/**
	 * Delete this handler.
	 */
	~UDPPacketHandler() 
	{
		if ( mSocket != -1 ) {
			close(mSocket);
			mSocket = -1;
		}
	}
	
	/**
	 * Create a generic UDP packet.
	 * \param size this is the desired size of the payload.
	 * \see UpdateChecksums(Packet *p)
	 * \see Packet::SetUDPPayload(KUInt8 *, KUInt32)
	 */
	Packet *NewPacket(KUInt32 size) {
		Packet *p = new Packet(0L, size+42);
		p->SetDstMAC(myMAC);
		p->SetSrcMAC(theirMAC);
		p->SetType(Packet::NetTypeIP);
		
		p->SetIPVersion(4);
		p->SetIPHeaderLength(20);
		p->SetIPTOS(0);
		p->SetIPTotalLength(size+42-14);	// not counting the network header
		p->SetIPID(theirID); theirID++;
		p->SetIPFlags(0);
		p->SetIPFrag(0);
		p->SetIPTTL(64);
		p->SetIPProtocol(Packet::IPProtocolUDP);
		p->SetIPSrcIP(theirIP);
		p->SetIPDstIP(myIP);
		
		p->SetUDPSrcPort(theirPort);
		p->SetUDPDstPort(myPort);
		p->SetUDPLength(size+8);
		
		return p;
	}
	
	void UpdateChecksums(Packet *p) {
		net->SetIPv4Checksum(p->Data(), p->Size());
		net->SetUDPChecksum(p->Data(), p->Size());
	}
	
	/**
	 * Send a Newton packet to the outside world.
	 * \param packet send this packet
	 * \return 1 if the packet was handled ok
	 * \return 0 if we don't know how to handle the packet
	 * \return -1 if an error occured and no other handler should handle this packet
	 */
	virtual int send(Packet &packet) {
		if (   ( packet.GetType() != Packet::NetTypeIP ) 
			|| ( packet.GetIPProtocol() != Packet::IPProtocolUDP ) 
			|| ( myPort != packet.GetUDPSrcPort() )
			|| ( theirIP != packet.GetIPDstIP() )
			|| ( theirPort != packet.GetUDPDstPort() ) )
			return 0;
		if (mSocket==-1) {
			mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (mSocket==-1)
				return -1;
			int fl = fcntl(mSocket, F_GETFL);
			int err = fcntl(mSocket, F_SETFL, fl|O_NONBLOCK);
			if (err==-1)
				return -1;
			memset(&theirSockAddr, 0, sizeof(theirSockAddr));
			theirSockAddr.sin_family = AF_INET;
			theirSockAddr.sin_addr.s_addr = htonl(theirIP);
			theirSockAddr.sin_port = htons(theirPort);
		}
		mExpire = kUDPExpirationTime;
		packet.LogPayload(net->GetLog(), "W<E N");
		int ret = 
			sendto(mSocket, packet.GetUDPPayloadStart(), packet.GetUDPPayloadSize(),
				   0, (struct sockaddr*)&theirSockAddr, sizeof(theirSockAddr));
		if (ret==-1) 
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
	virtual void timer() {
		KUInt8 buf[TUsermodeNetwork::kMaxTxBuffer];
		socklen_t addrLen = sizeof(theirSockAddr);
		if (mSocket==-1)
			return;
		int maxTry = 5;
		for (;maxTry>0; maxTry--) {
			int avail =
				recvfrom(mSocket, buf, sizeof(buf), 0, (struct sockaddr*)&theirSockAddr, &addrLen);
			if (avail<1) {
				if ( --mExpire == 0 ) {
					printf("Net: Timer expired. Removing UDP handler for port %u to %u.%u.%u.%u\n",
						   theirPort,
						   (unsigned int)((theirIP>>24)&0xff),
						   (unsigned int)((theirIP>>16)&0xff),
						   (unsigned int)((theirIP>>8)&0xff),
						   (unsigned int)(theirIP&0xff));
					net->RemovePacketHandler(this);
					delete this;
				}
				return;
			}
			printf("Net: W>E N Data %d bytes\n", avail);
			Packet *reply = NewPacket(avail);
			memcpy(reply->GetUDPPayloadStart(), buf, avail);
			UpdateChecksums(reply);
			reply->LogPayload(net->GetLog(), "W E>N");
			net->Enqueue(reply);
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
	static int canHandle(Packet &packet, TUsermodeNetwork *net) {
		if ( packet.GetType() != Packet::NetTypeIP )
			return 0;
		if ( packet.GetIPProtocol() != Packet::IPProtocolUDP ) 
			return 0;
		PacketHandler *ph = new UDPPacketHandler(net, packet);
		net->AddPacketHandler(ph);
		return ph->send(packet);
	}
	
	KUInt64		myMAC, theirMAC;
	KUInt32		myIP, theirIP;
	KUInt16		myPort, theirPort;
	KUInt16		theirID;
	int			mSocket;
	KUInt32		mExpire;
	struct sockaddr_in theirSockAddr;
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
	static int canHandle(Packet &packet, TUsermodeNetwork *net) {
		if ( packet.GetType() != Packet::NetTypeARP )
			return 0;
		if ( packet.GetARPOp() != 1 ) // is this a request?
			return 0;
		if ( packet.GetARPHType() != 1 ) // is this an Ethernet MAC address?
			return 0;
		if ( packet.GetARPPType() != 0x0800 ) // Which upper layer protocol? IP?
			return 0;
		if ( packet.GetARPHLen() != 6 ) // Length of MAC Address?
			return 0;
		if ( packet.GetARPPLen() != 4 ) // Length of IP Address?
			return 0;
		Packet *reply = new Packet(0L, 42);
		reply->SetSrcMAC( packet.GetDstMAC() );
		reply->SetDstMAC( packet.GetSrcMAC() );
		reply->SetType( Packet::NetTypeARP );
		reply->SetARPHType( 1 );
		reply->SetARPPType( 0x0800 );
		reply->SetARPHLen( 6 );
		reply->SetARPPLen( 4 );
		reply->SetARPOp( 2 ); // reply
		
		KUInt64 a = packet.GetARPTPA();
		KUInt64 b = 0x000000fa00000000ULL;
		KUInt64 c = a | b;
		reply->SetARPSHA( c ); // faking a MAC address
		
		reply->SetARPSPA( packet.GetARPTPA() );
		reply->SetARPTHA( packet.GetARPSHA() );
		reply->SetARPTPA( packet.GetARPSPA() );
		reply->LogPayload(net->GetLog(), "W E>N");
		net->Enqueue(reply);
		KUInt32 theirIP = packet.GetARPTPA();
		printf("Net: ARP request for IP %u.%u.%u.%u\n",
			   (unsigned int)((theirIP>>24)&0xff),
			   (unsigned int)((theirIP>>16)&0xff),
			   (unsigned int)((theirIP>>8)&0xff),
			   (unsigned int)(theirIP&0xff));
		return 1;
	}
};



/**
 * This class handles ARP packets.
 */
class DHCPPacketHandler : public PacketHandler
{
public:
	
	/**
	 * Can we handle the given package?
	 *
	 * DHCP Packages are special UDP broadcasts that are needed to assign an IP
	 * to a machine that newly joins a network. Since our emulator shares the IP
	 * of the host, we can reply to these requests immediatly.
	 *
	 * \param packet the Newton packet that could be sent
	 * \param n the network interface
	 * \return 0 if we can not handle this packet
	 * \return 1 if we can handled it and it need not to be propagated any further
	 * \return -1 if an error occurred an no other handler should handle this packet
	 * \see man 3 getaddrinfo()
	 */
	static int canHandle(Packet &packet, TUsermodeNetwork *net) {
		if ( packet.GetType() != Packet::NetTypeIP )
			return 0;
		if ( packet.GetIPProtocol() != Packet::IPProtocolUDP )
			return 0;
		if (packet.GetUDPDstPort()!=67) // DHCP requests come on port 57 and are answered on 68
			return 0;
		if (packet.Get8(42)==3)
			printf("PARTY! ***********************************");
		if (packet.GetIPDstIP()!=0xffffffff) // DHCP are broadcast to 255.255.255.255
			return 0;
		if (packet.Get32(0x0116)!=0x63825363) // check for DHCP magic number
			return 0;
		
		printf("Net: DHCP request (%d bytes)\n", packet.Size());
		int i = 0;
		for (i=0; i<packet.Size(); i++) {
			KUInt8 d = packet.Data()[i];
			printf("0x%02x, ", d);
			if ((i&15)==15) printf("\n");
		}
		printf("\n");
		

#if 0
		// options:
		// 22, 23, 26, 31, 32, 35, 1, 3, 6, 15
		KUInt8 prepackedReply[] = {
			0x00, 0x0b, 0x82, 0x01, 0xfc, 0x42, 0x00, 0x08, 0x74, 0xad, 0xf1, 0x9b, 0x08, 0x00, 0x45, 0x00,
			0x01, 0x48, 0x04, 0x45, 0x00, 0x00, 0x80, 0x11, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01, 0xc0, 0xa8,
			0x00, 0x0a, 0x00, 0x43, 0x00, 0x44, 0x01, 0x34, 0x22, 0x33, 0x02, 0x01, 0x06, 0x00, 0x00, 0x00,
			0x3d, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x0a, 0xc0, 0xa8,
			0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x82, 0x01, 0xfc, 0x42, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x82, 0x53, 0x63, 0x35, 0x01, 0x02, 0x01, 0x04, 0xff,
			0xff, 0xff, 0x00, 0x3a, 0x04, 0x00, 0x00, 0x07, 0x08, 0x3b, 0x04, 0x00, 0x00, 0x0c, 0x4e, 0x33,
			0x04, 0x00, 0x00, 0x0e, 0x10, 0x36, 0x04, 0xc0, 0xa8, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		Packet *reply = new Packet(prepackedReply, sizeof(prepackedReply), 1);
	
		reply->SetDstMAC(packet.GetSrcMAC());
		reply->Set32(46, packet.Get32(46));
		
		net->SetUDPChecksum(reply->Data(), reply->Size());
		net->Enqueue(reply);

		printf("Net: DHCP reply (%d bytes)\n", reply->Size());
		{
			int i = 0;
			for (i=0; i<reply->Size(); i++) {
				KUInt8 d = reply->Data()[i];
				printf("%02x %c  ", d, ((d>=32)&&(d<127))?d:'.');
				if ((i&15)==15) printf("\n");
			}
			printf("\n");
		}
#endif

#if 0
		// ok, I am pretty sure this is a DHCP package. Generate a generic DHCP reply based on the request
		Packet *reply = new Packet(0L, packet.Size());
		reply->SetDstMAC(packet.GetSrcMAC());
		reply->SetSrcMAC(0x000874adf19b);	// virtual router MAC
		reply->Set16(12, 0x0800); // Type
		reply->Set8(14, 0x45); // length
		reply->Set8(15, 0x00); // flags
		reply->Set16(16, packet.Get16(16));
		reply->Set16(18, 0x0445); // identification
		reply->Set8(22, 128); // time to live
		reply->Set8(23, 0x11); // UDP
		reply->SetIPSrcIP(0x0a000101);
		reply->SetIPDstIP(0x0a000117);
		reply->SetUDPSrcPort(67);
		reply->SetUDPDstPort(68);
		reply->SetUDPLength(packet.GetUDPLength());
		
		reply->Set8(42, 2);	// DHCPOFFER
		reply->Set8(43, 1);
		reply->Set8(44, 6);
		reply->Set8(45, 0);
		reply->Set32(46, packet.Get32(46));
		reply->Set32(0x3a, 0x0a000117); // YIP
		reply->Set32(0x3e, 0x0a000101); //
		reply->Set48(0x46, reply->GetDstMAC());
		
		// is it DHCPDISCOVER? If so, we answer with DHCPOFFER
		if (packet.Get8(42)==1) {
//			reply->Set8(42, 2);	// DHCPOFFER
//			reply->Set32(58, 0x0a000105); // FIXME: YIADDR, suggested client address
//			reply->Set32(62, 0x0a000101); // FIXME: SIADDR, server address
//			reply->Set32(66, 0x00000000); // GIADDR, Gateway address is 0 for now
			
			memset(reply->Data()+0x011a, 0, reply->Size()-0x011a); // clear the DHCP options
			reply->Set32(0x0116, 0x63825363);
			KUInt8 *d = reply->Data()+0x011a;
			
			// TODO: add options!
			// DHCP option 53: DHCP Offer
			*d++ = 53; *d++ = 1; *d++ = 2;
			// DHCP option 1: 255.255.255.0 subnet mask
			*d++ = 1; *d++ = 4; *d++ = 255; *d++ = 255; *d++ = 255; *d++ = 0;
			// DHCP option 3: 192.168.1.1 router
			*d++ = 3; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
			// DHCP option 51: 86400s (1 day) IP lease time
			*d++ = 51; *d++ = 4; *d++ = 0x00; *d++ = 0x01; *d++ = 0x51; *d++ = 0x80;
			// DHCP option 54: 192.168.1.1 DHCP server
			*d++ = 54; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
			// DHCP option 6: DNS servers 9.7.10.15, 9.7.10.16, 9.7.10.18
			*d++ = 6; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
			// DHCP end option:
			*d++ = 0xff;
			
			net->SetIPv4Checksum(reply->Data(), reply->Size());
			net->SetUDPChecksum(reply->Data(), reply->Size());
			net->Enqueue(reply);
			
			printf("Net: DHCP reply (%d bytes)\n", reply->Size());
			int i = 0;
			for (i=0; i<reply->Size(); i++) {
				KUInt8 d = reply->Data()[i];
				printf("%02x %c  ", d, ((d>=32)&&(d<127))?d:'.');
				if ((i&15)==15) printf("\n");
			}
			printf("\n");

		}
#endif
		
		Packet *reply = new Packet(0L, 342);

		// ethernet II
		reply->Set48(0x0000, packet.Get48(6));	// dest MAC
		reply->Set48(0x0006, 0x9bf1ad740800);	// src MAC
		reply->Set16(0x000c, 0x0800);	// type
		// IP4
		reply->Set8(0x000e, 0x45); // Version/length
		reply->Set8(0x000f, 0); // service field
		reply->Set16(0x0010, reply->Size()-14);
		reply->Set16(0x0012, 1093); // identification (FIXME?)
		reply->Set16(0x0014, 0); // Flags
		reply->Set8(0x0016, 128); // TTL
		reply->Set8(0x0017, 17); // Protocol: UDP
		reply->Set16(0x0018, 0); // Checksum (ignore)
		reply->Set32(0x001a, 0x0a000101); // src IP
		reply->Set32(0x001e, 0x0a000108); // dst IP
		// UDP
		reply->Set16(0x0022, 67); // src port (bootps)
		reply->Set16(0x0024, 68); // dst port (bootpc)
		reply->Set16(0x0026, reply->Size()-34); // size
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
		reply->Set32(0x003a, 0x0a000108); // your IP
		reply->Set32(0x003e, 0x0a000101); // server IP
		reply->Set32(0x0042, 0); // relay agent IP
		reply->Set48(0x0046, packet.Get48(6));	// client MAC (padded with 0'a
		// server host name, boot file name are all 0's
		reply->Set32(0x0116, 0x63825363); // DHCP magic cookie
		KUInt8 *d = reply->Data()+0x011a;
		// DHCP option 53: DHCP Offer
		*d++ = 53; *d++ = 1; *d++ = 2;
		// 22, 23, 26, 31, 32, 35, 1, 3, 6, 15
		
		/*
		// DHCP option 1: 255.255.255.0 subnet mask
		*d++ = 1; *d++ = 4; *d++ = 255; *d++ = 255; *d++ = 255; *d++ = 0;
		// DHCP option 3: 192.168.1.1 router
		*d++ = 3; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
		// DHCP option 51: 86400s (1 day) IP lease time
		*d++ = 51; *d++ = 4; *d++ = 0x00; *d++ = 0x01; *d++ = 0x51; *d++ = 0x80;
		// DHCP option 54: 192.168.1.1 DHCP server
		*d++ = 54; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
		// DHCP option 6: DNS servers 9.7.10.15, 9.7.10.16, 9.7.10.18
		*d++ = 6; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
		 */
		
		// 22: Maximum Datagram Reassembly Size
		*d++ = 22; *d++ = 2; *d++ = 2; *d++ = 64;	// 576 (minimum)
		// 23: Default IP Time-to-live
		*d++ = 23; *d++ = 1; *d++ = 80;
		// 26: Interface MTU Option
		*d++ = 26; *d++ = 2; *d++ = 0; *d++ = 68;
		// 31: Perform Router Discovery Option
		*d++ = 31; *d++ = 1; *d++ = 0;
		// 32
		// 35: ARP Cache Timeout Option
		*d++ = 35; *d++ = 4; *d++ = 0; *d++ = 0; *d++ = 16; *d++ = 16; // 3600 sec
		// 1: Subnet Mask
		*d++ = 1; *d++ = 4; *d++ = 255; *d++ = 0; *d++ = 0; *d++ = 0;
		// 3: router
		*d++ = 3; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
		// 6: DNS Server
		*d++ = 6; *d++ = 4; *d++ = 10; *d++ = 0; *d++ = 1; *d++ = 1;
		// 15
		*d++ = 15; *d++ = 4; *d++ = 'E'; *d++ = 'i'; *d++ = 'n'; *d++ = 'i';
		
		// DHCP end option:
		*d++ = 0xff;
		if ( d>=reply->Data()+reply->Size())
			printf("ERRROR: reply package too short\n");
		
		net->SetUDPChecksum(reply->Data(), reply->Size());
		net->Enqueue(reply);

		printf("Net: DHCP reply (%d bytes)\n", reply->Size());
		 i = 0;
		for (i=0; i<reply->Size(); i++) {
			KUInt8 d = reply->Data()[i];
			printf("%02x %c  ", d, ((d>=32)&&(d<127))?d:'.');
			if ((i&15)==15) printf("\n");
		}
		printf("\n");

		
		
		/* Data starts at 42  1 1 6 0 DHCPDISCOVER:   @ 0x0116: Magic 0x63825363
		
		Packet *reply = new Packet(0L, 42);
		reply->SetSrcMAC( packet.GetDstMAC() );
		reply->SetDstMAC( packet.GetSrcMAC() );
		reply->SetType( Packet::NetTypeARP );
		reply->SetARPHType( 1 );
		reply->SetARPPType( 0x0800 );
		reply->SetARPHLen( 6 );
		reply->SetARPPLen( 4 );
		reply->SetARPOp( 2 ); // reply
		
		KUInt64 a = packet.GetARPTPA();
		KUInt64 b = 0x000000fa00000000ULL;
		KUInt64 c = a | b;
		reply->SetARPSHA( c ); // faking a MAC address
		
		reply->SetARPSPA( packet.GetARPTPA() );
		reply->SetARPTHA( packet.GetARPSHA() );
		reply->SetARPTPA( packet.GetARPSPA() );
		reply->LogPayload(net->GetLog(), "W E>N");
		net->Enqueue(reply);
		KUInt32 theirIP = packet.GetARPTPA();
		  0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f 
	00	 ff .  ff .  ff .  ff .  ff .  ff .  58 X  b0 .  35 5  77 w  d7 .  22 "  08 .  00 .  45 E  00 .
	01	 02 .  40 @  00 .  01 .  00 .  00 .  3c <  11 .  7c |  ad .  00 .  00 .  00 .  00 .  ff .  ff .
	02	 ff .  ff .  00 .  44 D  00 .  43 C  02 .  2c ,  13 .  9c .  01 .  01 .  06 .  00 .  17 .  d1 .
	03	 4b K  ab .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	04	 00 .  00 .  00 .  00 .  00 .  00 .  58 X  b0 .  35 5  77 w  d7 .  22 "  00 .  00 .  00 .  00 .
	05	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	06	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	07	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	08	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	09	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	0a	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	0b	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	0c	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	0d	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	0e	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	0f	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	10	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	11	 00 .  00 .  00 .  00 .  00 .  00 .  63 c  82 .  53 S  63 c  35 5  01 .  01 .  33 3  04 .  00 .
	12	 00 .  04 .  b0 .  37 7  0a .  16 .  17 .  1a .  1f .  20    23 #  01 .  03 .  06 .  0f .  ff .
	13	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	14	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	15	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	16	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	17	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	18	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	19	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	1a	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	1b	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	1c	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	1d	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	1e	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	1f	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	20	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	21	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	22	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	23	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
	24	 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .


		 58 X  b0 .  35 5  77 w  d7 .  22 "  9b .  f1 .  ad .  74 t  08 .  00 .  08 .  00 .  45 E  00 .
		 01 .  48 H  04 .  45 E  00 .  00 .  80 .  11 .  00 .  00 .  0a .  00 .  01 .  01 .  0a .  00 .
		 01 .  08 .  00 .  43 C  00 .  44 D  01 .  34 4  f7 .  47 G  02 .  01 .  06 .  00 .  5c \  a8 .
		 a9 .  f0 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  0a .  00 .  01 .  08 .  0a .  00 .
		 01 .  01 .  00 .  00 .  00 .  00 .  58 X  b0 .  35 5  77 w  d7 .  22 "  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .  63 c  82 .  53 S  63 c  35 5  01 .  02 .  16 .  02 .  02 .
		 40 @  17 .  01 .  50 P  1a .  02 .  00 .  44 D  1f .  01 .  00 .  23 #  04 .  00 .  00 .  10 .
		 10 .  01 .  04 .  ff .  00 .  00 .  00 .  03 .  04 .  0a .  00 .  01 .  01 .  06 .  04 .  0a .
		 00 .  01 .  01 .  0f .  04 .  45 E  69 i  6e n  69 i  ff .  00 .  00 .  00 .  00 .  00 .  00 .
		 00 .  00 .  00 .  00 .  00 .  00 .
		 
		 0000  ff ff ff ff ff ff 00 0b  82 01 fc 42 08 00 45 00   ........ ...B..E.
		 0010  01 2c a8 36 00 00 fa 11  17 8b 00 00 00 00 ff ff   .,.6.... ........
		 0020  ff ff 00 44 00 43 01 18  59 1f 01 01 06 00 00 00   ...D.C.. Y.......
		 0030  3d 1d 00 00 00 00 00 00  00 00 00 00 00 00 00 00   =....... ........
		 0040  00 00 00 00 00 00 00 0b  82 01 fc 42 00 00 00 00   ........ ...B....
		 0050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0060  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0070  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0080  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0090  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00a0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00b0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00c0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00d0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00e0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00f0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0100  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0110  00 00 00 00 00 00 63 82  53 63 35 01 01 3d 07 01   ......c. Sc5..=..
		 0120  00 0b 82 01 fc 42 32 04  00 00 00 00 37 04 01 03   .....B2. ....7...
		 0130  06 2a ff 00 00 00 00 00  00 00                     .*...... ..
		 
		 
		 0000  00 0b 82 01 fc 42 00 08  74 ad f1 9b 08 00 45 00   .....B.. t.....E.
		 0010  01 48 04 45 00 00 80 11  00 00 c0 a8 00 01 c0 a8   .H.E.... ........
		 0020  00 0a 00 43 00 44 01 34  22 33 02 01 06 00 00 00   ...C.D.4 "3......
		 0030  3d 1d 00 00 00 00 00 00  00 00 c0 a8 00 0a c0 a8   =....... ........
		 0040  00 01 00 00 00 00 00 0b  82 01 fc 42 00 00 00 00   ........ ...B....
		 0050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0060  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0070  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0080  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0090  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00a0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00b0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00c0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00d0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00e0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00f0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0100  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0110  00 00 00 00 00 00 63 82  53 63 35 01 02 01 04 ff   ......c. Sc5.....
		 0120  ff ff 00 3a 04 00 00 07  08 3b 04 00 00 0c 4e 33   ...:.... .;....N3
		 0130  04 00 00 0e 10 36 04 c0  a8 00 01 ff 00 00 00 00   .....6.. ........
		 0140  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0150  00 00 00 00 00 00                                  ......
		 
		 
		 0000  ff ff ff ff ff ff 00 0b  82 01 fc 42 08 00 45 00   ........ ...B..E.
		 0010  01 2c a8 37 00 00 fa 11  17 8a 00 00 00 00 ff ff   .,.7.... ........
		 0020  ff ff 00 44 00 43 01 18  9f bd 01 01 06 00 00 00   ...D.C.. ........
		 0030  3d 1e 00 00 00 00 00 00  00 00 00 00 00 00 00 00   =....... ........
		 0040  00 00 00 00 00 00 00 0b  82 01 fc 42 00 00 00 00   ........ ...B....
		 0050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0060  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0070  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0080  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0090  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00a0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00b0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00c0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00d0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00e0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00f0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0100  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0110  00 00 00 00 00 00 63 82  53 63 35 01 03 3d 07 01   ......c. Sc5..=..
		 0120  00 0b 82 01 fc 42 32 04  c0 a8 00 0a 36 04 c0 a8   .....B2. ....6...
		 0130  00 01 37 04 01 03 06 2a  ff 00                     ..7....* ..
		 
		 
		 0000  00 0b 82 01 fc 42 00 08  74 ad f1 9b 08 00 45 00   .....B.. t.....E.
		 0010  01 48 04 46 00 00 80 11  00 00 c0 a8 00 01 c0 a8   .H.F.... ........
		 0020  00 0a 00 43 00 44 01 34  df db 02 01 06 00 00 00   ...C.D.4 ........
		 0030  3d 1e 00 00 00 00 00 00  00 00 c0 a8 00 0a 00 00   =....... ........
		 0040  00 00 00 00 00 00 00 0b  82 01 fc 42 00 00 00 00   ........ ...B....
		 0050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0060  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0070  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0080  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0090  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00a0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00b0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00c0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00d0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00e0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 00f0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0100  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0110  00 00 00 00 00 00 63 82  53 63 35 01 05 3a 04 00   ......c. Sc5..:..
		 0120  00 07 08 3b 04 00 00 0c  4e 33 04 00 00 0e 10 36   ...;.... N3.....6
		 0130  04 c0 a8 00 01 01 04 ff  ff ff 00 ff 00 00 00 00   ........ ........
		 0140  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
		 0150  00 00 00 00 00 00                                  ......

		 */
		return 1;
	}
};



/**
 * Create an interface betweenthe Newton network driver and Einstein.
 */
TUsermodeNetwork::TUsermodeNetwork(TLog* inLog) :
	TNetworkManager( inLog ),
	mFirstPacketHandler( 0L ),
	mFirstPacket( 0L ),
	mLastPacket( 0L )
{
	// to implement DHCP, look at getifaddrs()
	// http://www.developerweb.net/forum/showthread.php?t=5085
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
	// TODO: delete handlers
	// release all other resources
}


/**
 * This function handles packet that are sent by the Newton to the outside 
 * world. In user mode, these packages are emulated for information that is 
 * already available on the host, or forwarded to a host socket. A possible
 * reply must be interpreted and a raw package must be generated for the Newton.
 */
int TUsermodeNetwork::SendPacket(KUInt8 *data, KUInt32 size)
{
	int err = 0;
	Packet packet(data, size, 0); // convert data into a packet
	packet.LogPayload(GetLog(), "W E<N");
	
	// offer this package to all active handlers
	PacketHandler *ph = mFirstPacketHandler;
	while (ph) {
		switch( ph->send(packet) ) {				
			case -1:	return -1;	// an error occured, packet must not be handled
			case 1:		return 0;	// packet was handled successfuly, leave
			case 0:		break;		// another handler should deal with this packet
		}
		ph = ph->next;
	}
	
	// now offer the package to possible new handlers
	switch( DHCPPacketHandler::canHandle(packet, this) ) {
		case -1:	return -1;	// an error occured, packet must not be handled
		case 1:		return 0;	// packet was handled successfuly, leave
		case 0:		break;		// another handler should deal with this packet
	}
	switch( ARPPacketHandler::canHandle(packet, this) ) {
		case -1:	return -1;	// an error occured, packet must not be handled
		case 1:		return 0;	// packet was handled successfuly, leave
		case 0:		break;		// another handler should deal with this packet
	}
	switch( TCPPacketHandler::canHandle(packet, this) ) {
		case -1:	return -1;	// an error occured, packet must not be handled
		case 1:		return 0;	// packet was handled successfuly, leave
		case 0:		break;		// another handler should deal with this packet
	}
	switch( UDPPacketHandler::canHandle(packet, this) ) {				
		case -1:	return -1;	// an error occured, packet must not be handled
		case 1:		return 0;	// packet was handled successfuly, leave
		case 0:		break;		// another handler should deal with this packet
	}
	
	// if we can't interprete the package, we offer some debugging information
	printf("Net: Send Packet - I can't handle this packet:\n");
	if (mLog) {
		mLog->LogLine("\nTUsermodeNetwork: Newton is sending an unsupported package:");
		LogBuffer(data, size);
		LogPacket(data, size);
	}
	return err;
}


/**
 * We forward the timer of the Newton driver to all packet handlers.
 */
int TUsermodeNetwork::TimerExpired()
{
	PacketHandler *ph = mFirstPacketHandler;
	while (ph) {
		PacketHandler *pNext = ph->next;
		ph->timer();
		ph = pNext;
	}
	return 0;
}


/**
 * Return the MAC address of the card that we emulate.
 */
int TUsermodeNetwork::GetDeviceAddress(KUInt8 *data, KUInt32 size)
{
	// TODO: of course we need the true MAC of this ethernet card
	// see: ioctl ? getifaddrs ? http://othermark.livejournal.com/3005.html
	static KUInt8 gLocalMAC[]   = { 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x22 };
	//assert(size==6);
	memcpy(data, gLocalMAC, 6);
	return 0;
}


/**
 * Return the number of bytes available for the Newton driver.
 */
KUInt32 TUsermodeNetwork::DataAvailable()
{	
	if (mLastPacket) {
		return mLastPacket->Size();
	} else {
		return 0;
	}
}


/**
 * Fill the provided buffer with a raw packet.
 */
int TUsermodeNetwork::ReceiveData(KUInt8 *data, KUInt32 size)
{
	Packet *pkt = mLastPacket;
	if (pkt) {
		//assert(pkt->Size()==size);
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
 * Remove a packet handler from the list.
 */
void TUsermodeNetwork::RemovePacketHandler(PacketHandler *ph) 
{
	PacketHandler *pp = ph->prev;
	PacketHandler *pn = ph->next;
	if (pp) pp->next = pn; else mFirstPacketHandler = pn;
	if (pn) pn->prev = pp; else mLastPacketHandler = pp;
	//delete ph;
}


/**
 * Add a new packet to the beginning of the pipe.
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
