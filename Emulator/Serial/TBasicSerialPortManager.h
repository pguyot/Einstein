// ==============================
// File:			TBasicSerialPortManager.h
// Project:			Einstein
//
// Copyright 2017-2018 by Matthias Melcher (mm@matthiasm.com).
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

#ifndef _T_BASIC_SERIAL_PORT_MANAGER_H
#define _T_BASIC_SERIAL_PORT_MANAGER_H

#include "TSerialPortDriver.h"

#include <mutex>

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;

///
/// Virtual base class to emulate a serial port.
///
/// \author Matthias Melcher
///
class TBasicSerialPortManager : public TSerialPortDriver
{
public:
	const KUInt8 kSerCmd_TxCtrlChanged = 'c';
	const KUInt8 kSerCmd_StopThread = 'Q';

	///
	/// Constructor.
	///
	TBasicSerialPortManager(TLog* inLog,
		TSerialPorts::EPortIndex inPortIx);

	///
	/// Destructor.
	///
	~TBasicSerialPortManager() override;

	///
	/// Return the Identification of this driver
	///
	TSerialPorts::EDriverID
	GetID() override
	{
		return TSerialPorts::kNullDriver;
	}

	///
	/// Start emulation.
	///
	void run(TInterruptManager* inInterruptManager,
		TDMAManager* inDMAManager,
		TMemory* inMemory) override;

	///
	/// Write register.
	///
	void WriteRegister(KUInt32 inOffset, KUInt8 inValue) override;

	///
	/// Read register.
	///
	KUInt8 ReadRegister(KUInt32 inOffset) override;

	///
	/// Read DMA register.
	///
	KUInt32 ReadDMARegister(KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister) override;

	///
	/// Write DMA register.
	///
	void WriteDMARegister(KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister, KUInt32 inValue) override;

	///
	/// Save or load the transmit and receive DMA registers.
	///
	void TransferState(TStream* inStream) override;

	///
	/// Stop DMA while the emulator state is saved or loaded.
	///
	void Suspend() override;

	///
	/// Continue DMA after Suspend().
	///
	void Resume() override;

	///
	/// DMA or interrupts trigger a command that must be handled by a derived class.
	///
	virtual void
	TriggerEvent(KUInt8 cmd)
	{
		(void) cmd;
	}

protected:
	///
	/// Read receiving DMA register.
	///
	KUInt32 ReadRxDMARegister(KUInt32 inBank, KUInt32 inRegister);

	///
	/// Write receiving DMA register.
	///
	void WriteRxDMARegister(KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue);

	///
	/// Read transmitting DMA register.
	///
	KUInt32 ReadTxDMARegister(KUInt32 inBank, KUInt32 inRegister);

	///
	/// Write transmitting DMA register.
	///
	void WriteTxDMARegister(KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue);

	/// physical address of transmit DMA buffer start
	KUInt32 mTxDMAPhysicalBufferStart { 0 };

	/// address of byte currently written by DMA
	KUInt32 mTxDMAPhysicalData { 0 };

	/// number of bytes that still need to be sent (DMA register 1.4, Count)
	KUInt32 mTxDMADataCountdown { 0 };

	/// bytes left until the end of the circular buffer, at which point
	/// mTxDMAPhysicalData wraps to mTxDMAPhysicalBufferStart (DMA register 1.5)
	KUInt32 mTxDMABytesToBufferEnd { 0 };

	/// channel interrupt enable (DMA register 2.0, see TDMAManager::kChanInt...).
	/// StartTxDMA sets bit 1 (transfer done), the emulation also uses it to
	/// tell if transmit DMA is running.
	KUInt32 mTxDMAIntEnable { 0 };

	/// the event that triggered the interrupt?
	KUInt32 mTxDMAEvent { 0 };

	/// physical address of receive DMA buffer start
	KUInt32 mRxDMAPhysicalBufferStart { 0 };

	/// address to store next byte read from periphery
	KUInt32 mRxDMAPhysicalData { 0 };

	/// number of free bytes in the circular buffer (DMA register 1.4, Count)
	KUInt32 mRxDMADataCountdown { 0 };

	/// bytes left until the end of the circular buffer, at which point
	/// mRxDMAPhysicalData wraps to mRxDMAPhysicalBufferStart (DMA register 1.5)
	KUInt32 mRxDMABytesToBufferEnd { 0 };

	/// channel interrupt enable (DMA register 2.0, see TDMAManager::kChanInt...).
	/// StartRxDMA writes 0x06 (transfer done, compare), 0x12 for LocalTalk.
	KUInt32 mRxDMAIntEnable { 0 };

	/// the event that triggered the interrupt?
	KUInt32 mRxDMAEvent { 0 };

	/// Held by the worker thread of a derived class while it handles DMA
	/// (emulated memory and the registers above), and by Suspend().
	std::mutex mDMAMutex;
};

#endif
// _T_BASIC_SERIAL_PORT_MANAGER_H

// ================= //
// Byte your tongue. //
// ================= //
