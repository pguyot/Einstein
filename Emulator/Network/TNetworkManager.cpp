// ==============================
// File:			TNetworkManager.cp
// Project:			Einstein
//
// Copyright 2010-2020 by Matthias Melcher (mm@matthiasm.com).
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

#include <K/Threads/TThread.h>
#include <K/Threads/TCondVar.h>
#include <K/Threads/TMutex.h>
#include "TNetworkManager.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/TMemory.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"
#if TARGET_OS_WIN32
#else
#include <sys/select.h>
#endif

// -------------------------------------------------------------------------- //
//  * TNetworkManager(TLog* inLog)
// -------------------------------------------------------------------------- //
TNetworkManager::TNetworkManager(TLog* inLog) :
	mLog( inLog ),
	mInterruptManager( 0L ),
	mMemory( 0L ),
	mThread( NULL ),
	mSelectNFDS( 0 )
{
	mSelectCondVar = new TCondVar();
	mSelectMutex = new TMutex();
	mSelectMutex->Lock();
	mThread = new TThread(this);
	mSelectCondVar->Wait(mSelectMutex);
	mSelectMutex->Unlock();
}

void TNetworkManager::LogBuffer(KUInt8 *data, KUInt32 size)
{
	if (mLog) {
		mLog->FLogLine("%d bytes at 0x%08x", size, data);
		char t[72];
		int i;
		for (i=0; i<size; i++) {
			if ((i&15)==0) {
				memset(t, ' ', 71);
				t[71] = 0;
				sprintf(t, "0x%04x:  ", i);
				t[9] = ' ';
			}
			unsigned char v = data[i];
			int p = 9 + (i&15)*5/2;
			sprintf(t+p, "%02x", v); t[p+2] = ' ';
			p = 50 + (i&15);
			t[p] = ((v>=32)&&(v<127)) ? v : '.';
			if ((i&15)==15)
				mLog->LogLine( t );
		}
		if (((i-1)&15)!=15)
			mLog->LogLine( t );
	}
}
	

void TNetworkManager::LogARPPacket(KUInt8 *data, KUInt32 size) {
	if (mLog) {
		mLog->FLogLine("**** ARP Logging not yet supported ****");
	}
}

void TNetworkManager::LogIPv4Packet(KUInt8 *d, KUInt32 n) {
	if (mLog) {
		mLog->FLogLine("  > IPv4 Internet protocol"); 
		mLog->FLogLine("    [14] Version: %d (4)", d[14]>>4);
		mLog->FLogLine("    [14] Header Length: %d (5)", d[14]&0x0f);
		mLog->FLogLine("    [15] Type Of Service: 0x%02x", d[15]);
		mLog->FLogLine("    [16] Total Length: %d (%d)", (d[16]<<8)|d[17], n-14); // minus 14 bytes for Ethernet header
		mLog->FLogLine("    [18] ID: %d", (d[18]<<8)|d[19]);
		mLog->FLogLine("    [20] Flags: %d", d[20]>>5);
		mLog->FLogLine("    [20] Fragment: %d", ((d[20]<<8)|d[21])&0x1fff);
		mLog->FLogLine("    [22] Time To Live: %d", d[22]);
		mLog->FLogLine("    [23] Protocol: %d", d[23]); // TCP, UDP, etc.
		mLog->FLogLine("    [24] Header Checksum: 0x%04x (0x%04x==0)", (d[24]<<8)|d[25], GetIPv4Checksum(d, n));
		mLog->FLogLine("    [26] Src IP: %d.%d.%d.%d", d[26], d[27], d[28], d[29]);
		mLog->FLogLine("    [30] Dst IP: %d.%d.%d.%d", d[30], d[31], d[32], d[33]);
		
		switch (d[23]) {
			case  6: LogTCPPacket(d, n); break;
			case 17: LogUDPPacket(d, n); break;
            case  1:
                mLog->FLogLine("    > **** Unsupported Protocol: 'ICMP' ****\n");
                break;
			default:
				mLog->FLogLine("    > **** Unknown Protocol: %d ****\n", d[23]);
                break;
		}
	}
}

void TNetworkManager::LogTCPPacket(KUInt8 *d, KUInt32 n) {
	if (mLog) {
		mLog->FLogLine("    > Protocol is TCP");
		mLog->FLogLine("      [34] Src Port: %d", (d[34]<<8)|d[35]);
		mLog->FLogLine("      [36] Dst Port: %d", (d[36]<<8)|d[37]);
		mLog->FLogLine("      [38] Seq#: %d", (d[38]<<24)|(d[39]<<16)|(d[40]<<8)|d[41]);
		mLog->FLogLine("      [42] Ack#: %d", (d[42]<<24)|(d[43]<<16)|(d[44]<<8)|d[45]);
		mLog->FLogLine("      [46] Header Length: %d (5)", d[46]>>4);
		mLog->FLogLine("      [47] Flags: %s %s %s %s %s %s", 
					   d[47]&0x20?"URG":"urg",
					   d[47]&0x10?"ACK":"ack",
					   d[47]&0x08?"PSH":"psh",
					   d[47]&0x04?"RST":"rst",
					   d[47]&0x02?"SYN":"syn",
					   d[47]&0x01?"FIN":"fin"
					   );
		mLog->FLogLine("      [48] Window:    0x%04x", (d[48]<<8)|d[49]);
		mLog->FLogLine("      [50] Checksum:  0x%04x (0x%04x==0?)", (d[50]<<8)|d[51], GetTCPChecksum(d, n));
		mLog->FLogLine("      [52] UrgentPtr: 0x%04x", (d[52]<<8)|d[53]);
		mLog->FLogLine("      [54] ... %d bytes of payload\n", n-54);
	}
}

void TNetworkManager::LogUDPPacket(KUInt8 *d, KUInt32 n) {
	if (mLog) {
		mLog->FLogLine("    > Protocol is UDP");
		mLog->FLogLine("      [34] Src Port: %d", (d[34]<<8)|d[35]);
		mLog->FLogLine("      [36] Dst Port: %d", (d[36]<<8)|d[37]);
		mLog->FLogLine("      [38] Length: %d", (d[38]<<8)|d[39]);
		mLog->FLogLine("      [40] Checksum:  0x%04x (0x%04x==0?)", (d[40]<<8)|d[41], GetUDPChecksum(d, n));
		mLog->FLogLine("      [42] ... %d bytes of payload\n", n-42);
	}
}

void TNetworkManager::LogPacket(KUInt8 *d, KUInt32 n) {
	if (mLog) {
		mLog->FLogLine("TNetworkManager: Log Packet of %d bytes:", n);
		mLog->FLogLine("  [0] Dst MAC: %02x:%02x:%02x:%02x:%02x:%02x", d[0], d[1], d[2], d[3], d[4], d[5]);
		mLog->FLogLine("  [6] Src MAC: %02x:%02x:%02x:%02x:%02x:%02x", d[6], d[7], d[8], d[9], d[10], d[11]);
		KUInt32 t = (d[12]<<8) | d[13];
		mLog->FLogLine("  [12] Type:   0x%04x", t);
		switch (t) {
			case 0x0800: LogIPv4Packet(d, n); break;
			case 0x0806: LogARPPacket(d, n); break;
			case 0x809b: mLog->LogLine("  **** unsupported: Apple Talk ****"); break;
			case 0x86dd: mLog->LogLine("  **** unsupported: IPv6 ****"); break; 
			default: mLog->FLogLine("  **** unsupported type: 0x%04x ****", t); break; 
		}
	}
}

KUInt16 TNetworkManager::GetUDPChecksum(KUInt8 *d, ssize_t n, Boolean set) {
	KUInt32 s = 0;
	KUInt16 i, UDPLength = n-34;
	// UDP Pseudo Header:
	s = s + (d[26]<<8) + d[27] + (d[28]<<8) + d[29]; // src IP
	s = s + (d[30]<<8) + d[31] + (d[32]<<8) + d[33]; // dst IP
	s = s + d[23] + UDPLength; 
	if (set) {
		d[40] = d[41] = 0;
	}
	// UDP Data
	for (i=34; i<n; i+=2) {
		s += (d[i]<<8);
		if (i+1<n) s += d[i+1];
	}
	// 1's complement
	while (s>>16)
		s = (s & 0xFFFF) + (s >> 16);
	s = ~s;
	// set it
	if (set) {
		if (s==0) s=0xffff;
		d[40] = s>>8;
		d[41] = s;
	}
	return s;	
}

KUInt16 TNetworkManager::GetTCPChecksum(KUInt8 *d, ssize_t n, Boolean set) {
	KUInt32 s = 0;
	KUInt16 i, TCPLength = n-34;
	// TCP Pseudo Header:
	s = s + (d[26]<<8) + d[27] + (d[28]<<8) + d[29]; // src IP
	s = s + (d[30]<<8) + d[31] + (d[32]<<8) + d[33]; // dst IP
	s = s + d[23] + TCPLength; 
	if (set) {
		d[50] = d[51] = 0;
	}
	// TCP Data
	for (i=34; i<n; i+=2) {
		s += (d[i]<<8);
		if (i+1<n) s += d[i+1];
	}
	// 1's complement
	while (s>>16)
		s = (s & 0xFFFF) + (s >> 16);
	s = ~s;
	// set it
	if (set) {
		if (s==0) s=0xffff;
		d[50] = s>>8;
		d[51] = s;
	}
	return s;	
}

KUInt16
TNetworkManager::GetIPv4Checksum(KUInt8 *d, ssize_t n, Boolean set) {
	KUInt32 s = 0;
	KUInt16 v, i;
	if (set) {
		d[24] = d[25] = 0;
	}
	for (i=14; i<34; i+=2) {
		v = (d[i]<<8)|d[i+1];
		s += v;
	}
	while (s>>16)
		s = (s & 0xFFFF) + (s >> 16);
	s = ~s;
	if (set) {
		if (s==0) s=0xffff;
		d[24] = s>>8;
		d[25] = s;
	}
	return s;
}

// -------------------------------------------------------------------------- //
//  * AsyncWaitForReadyToRead(int, const fd_set*)
// -------------------------------------------------------------------------- //
void
TNetworkManager::AsyncWaitForReadyToRead(int nfds, const fd_set* inFDSet) {
	mSelectMutex->Lock();
	mSelectNFDS = nfds;
	mSelectSet = *inFDSet;
	mSelectCondVar->Signal();
	mSelectMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * Run( void )
// -------------------------------------------------------------------------- //
void
TNetworkManager::Run() {
	mSelectMutex->Lock();
	mSelectCondVar->Signal();	// Synchronization with constructor.
	while (true) {
		mSelectCondVar->Wait(mSelectMutex);
		
		int nbReady = select(mSelectNFDS, &mSelectSet, NULL, NULL, NULL);
		if (nbReady > 0) {
			IsReadyToRead(&mSelectSet);
		} else if (nbReady < 0) {
			break;
		}
	}
	mSelectMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * IsReadyToRead(fd_set* inFDSet)
// -------------------------------------------------------------------------- //
void
TNetworkManager::IsReadyToRead(fd_set* inFDSet) {
	mMemory->GetPCMCIAController(0)->RaiseInterrupt(TPCMCIAController::kSocketCardIREQIntVector);
}

// -------------------------------------------------------------------------- //
//  * SetReadFDSet(fd_set* ioFDSet)
// -------------------------------------------------------------------------- //
int
TNetworkManager::SetReadFDSet(fd_set* ioFDSet) {
	// We don't have fds yet (those would be connected sockets once the Newton
	// can establish connections with this interface).
    return 0;
}

// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
