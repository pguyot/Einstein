// ==============================
// File:			TNetworkManager.h
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

#ifndef _TNETWORKMANAGER_H
#define _TNETWORKMANAGER_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TMemory;
class TThread;
class TMutex;
class TCondVar;

///
/// Emulation of a PCMCIA network card on PCMCIA driver level (see Lantern)
///
/// \author Matthias Melcher <mm@matthiasm.com>
///
class TNetworkManager
{
public:
	TNetworkManager(TLog* inLog);
	
	///
	/// Destructor.
	///
	virtual ~TNetworkManager()
	{
	}

	///
	/// Set the interrupt manager.
	/// This method is called once the interrupt manager is created.
	///
	/// \param inManager	reference to the interrupt manager
	///
	void	SetInterruptManager( TInterruptManager* inManager )
	{
		mInterruptManager = inManager;
	}
	
	///
	/// Set the memory interface.
	/// This method is called once the memory interface is created.
	///
	/// \param inManager	reference to the memory interface
	///
	void	SetMemory( TMemory* inMemory )
	{
		mMemory = inMemory;
	}
	
	///
	/// Newton sends a block of data.
	/// This is a raw ethernet network datagram. The network manager is expected
	/// to somehow send this package to the outside world.
	///
	/// \param data send this block of data from the Newton to the world
	/// \param size of the block
	///
	virtual int SendPacket(KUInt8 *data, KUInt32 size) = 0;
	
	///
	/// Fill the buffer with the MAC address of the network card.
	///
	/// \param data pointer to a six byte buffer
	/// \param size ethernet MAC addresses are always 6 bytes
	///
	virtual int GetDeviceAddress(KUInt8 *data, KUInt32 size) = 0;
	
	///
	/// Number of bytes available for Newton.
	/// This number is polled on a regular base. If no block is available,
	/// return 0. If a block of data is waiting, return the size of the raw
	/// ethernet datagram. Do not split blocks of data unless you create a
	/// complete rwa ethernet datagramm for each of them.	
	///
	/// \return the number of bytes in the first block that is available for the Newton
	///
	virtual KUInt32 DataAvailable() = 0;
	
	///
	/// Newton receives a block of data.
	/// Copy the block that was received from the outside world int this buffer.
	/// The Newton expects a raw ethernet datagramm. The size will always be
	/// whatever the menager returned in a previous call to DataAvailable().
	///
	/// \param data fill this buffer with the next available block of data
	/// \param size the number of bytes that we expect in the buffer
	///
	virtual int ReceiveData(KUInt8 *data, KUInt32 size) = 0;
	
	///
	/// Newton device driver timer expired.
	///
	virtual int TimerExpired() { return 0; }

	TLog *GetLog() { return mLog; }
	
	void LogBuffer(KUInt8 *data, KUInt32 size);	
	void LogPacket(KUInt8 *data, KUInt32 size);
	void LogARPPacket(KUInt8 *data, KUInt32 size);
	void LogIPv4Packet(KUInt8 *data, KUInt32 size);
	void LogTCPPacket(KUInt8 *data, KUInt32 size);
	void LogUDPPacket(KUInt8 *data, KUInt32 size);
	//void LogPayload(KUInt8 *data, KUInt32 size, const char *d="");
	
	KUInt16 GetIPv4Checksum(KUInt8 *data, KUInt32 size, bool set=0);
	void SetIPv4Checksum(KUInt8 *data, KUInt32 size) { GetIPv4Checksum(data, size, true); }
	
	KUInt16 GetTCPChecksum(KUInt8 *data, KUInt32 size, bool set=0);
	void SetTCPChecksum(KUInt8 *data, KUInt32 size) { GetTCPChecksum(data, size, true); }
	
	KUInt16 GetUDPChecksum(KUInt8 *data, KUInt32 size, bool set=0);
	void SetUDPChecksum(KUInt8 *data, KUInt32 size) { GetUDPChecksum(data, size, true); }
	
	///
	/// Asynchronously wait for sockets to be readable and call IsReadyToRead in the async thread.
	/// This function returns immediatly. The fd_set is copied.
	///
	void				AsyncWaitForReadyToRead(int nfds, const fd_set* inFDSet);
	
	///
	/// Thread loop entry point.
	/// Select on fds.
	///
	void				Run( void );
	
protected:
	///
	/// Method called (from the thread) when a file descriptor of the set is read for reading.
	/// Default implementation raises an interrupt on PCMCIA card 0 (FIXME: provide the socket number somehow).
	///
	virtual void		IsReadyToRead(fd_set* inFDSet);
	
	///
	/// Define the select fd set (in the thread).
	/// Return the max fd + 1. If the result is 0, the thread exits.
	///
	virtual int			SetReadFDSet(fd_set* ioFDSet);
	
	TLog*				mLog;				///< Reference to the log.
	TInterruptManager*	mInterruptManager;	///< Reference to the interrupt mgr.
	TMemory*			mMemory;			///< Interface to the memory.	

private:
    TThread*			mThread;
	TMutex*				mSelectMutex;
	TCondVar*			mSelectCondVar;
	int					mSelectNFDS;
	fd_set				mSelectSet;
};

class TNullNetwork : public TNetworkManager
{
public:
	TNullNetwork(TLog* inLog) : TNetworkManager(inLog) { }
	virtual ~TNullNetwork() { }
	void Run() { }
	virtual int SendPacket(KUInt8 *data, KUInt32 size) { return -1; }
	virtual int GetDeviceAddress(KUInt8 *data, KUInt32 size) { return -1; }
	virtual KUInt32 DataAvailable() { return 0; }
	virtual int ReceiveData(KUInt8 *data, KUInt32 size) { return -1; }
};

#endif
		// _TNETWORKMANAGER_H

// ============================================ //
// The first time, it's a KLUDGE!               //
// The second, a trick.                         //
// Later, it's a well-established technique!    //
//                 -- Mike Broido, Intermetrics //
// ============================================ //
