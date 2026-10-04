// ==============================
// File:			TDMAManager.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TDMAMANAGER_H
#define _TDMAMANAGER_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TMemory;
class TStream;
class TEmulator;

/*
 DMA controller of the Voyager ASIC (MessagePad 2x00, eMate 300)

 Verified against the MP2x00 ROM: TDMAManager (ROM 0x0007CC4C..0x0007CE1C),
 TSerialDMAEngine (0x001D9304..0x001D995C), PCirrusSoundDriver (0x00059988..),
 and TADC (0x00021518..). Interpretations that could not be confirmed are
 marked "probably".

 There are 8 channels. Each channel has two register banks and one bit in the
 global registers. TDMAManager::RequestAssignment() fills a
 TDMAChannelDiscriptor for a driver:
	bank 1 base:    0x0F080000 + (channel << 13), 8 registers, 0x400 apart
	bank 2 base:    0x0F090000 + (channel << 12), 4 registers, 0x400 apart
	channel bit:    1 << channel, used in the global registers
	interrupt mask: 0x80 << channel (TInterruptManager::kDMAChannel0IntMask...)

 Global registers:

 0x0F08FC00  r/w Channel assignment register, see kAssign... below.
			 TDMAManager::Init() writes 0.
 0x0F098000  r/w Enable/Status. Writing a channel bit starts the channel.
			 Reading returns the bits of the channels that are still
			 running. Drivers poll this after disabling a channel.
 0x0F098400  w   Disable. Writing a channel bit stops the channel.
			 TDMAManager::Init() writes 0xFF.
 0x0F098800  r   Word status. A channel bit is set if the word register (1.2)
			 holds bytes of an incomplete word after a receive DMA stops.

 Channel register bank 1 (offsets from the bank 1 base):

 1.0  +0x0000  r/w Base. Physical address the pointer reloads from when the
				   current buffer ends. Serial: start of the circular buffer.
				   Sound: start of the next buffer (double buffering).
 1.1  +0x0400  r/w Pointer. Physical address of the current transfer. Drivers
				   read it to get the current position.
 1.2  +0x0800  r   Word register. Holds the last, incomplete word of a
				   receive transfer (see Word status).
 1.3  +0x0C00  r/w Control, see kChanCtrl... below.
 1.4  +0x1000  r/w Count. Serial: bytes left to transfer, StopRxDMA computes
				   the received byte count from it. Sound: size of the next
				   buffer, probably reloaded into 1.5 when the buffer ends.
 1.5  +0x1400  r/w Size. Bytes left until the end of the current buffer, at
				   which point the pointer wraps to Base. The low 2 bits tell
				   how many bytes are in the word register.
 1.6  +0x1800  r/w Compare. Serial receive writes its notify level here
				   (probably raises kChanIntCompare), 0 otherwise.
 1.7  +0x1C00      Not used by the ROM.

 Channel register bank 2 (offsets from the bank 2 base):

 2.0  +0x0000  r/w Interrupt enable, see kChanInt... below. Drivers write 0
				   before (re)configuring a channel.
 2.1  +0x0400  r   Interrupt status.
 2.2  +0x0800  w   Interrupt clear, write 1 to clear. Drivers write back the
				   value read from 2.1, or 0x1F.
 2.3  +0x0C00  w   Written once with the same bits as 2.0 by
				   TSerialDMAEngine::ConfigureInterrupts, and with 1 by the
				   sound and ADC drivers. Purpose unknown.

 How the drivers program a channel:

 Serial receive (StartRxDMA): 1.3=0x80, 1.0=buffer, 1.1=buffer+put offset,
	1.4=free bytes, 1.5=bytes to end of buffer, 1.6=notify level, 2.0=0x06
	(0x12 for LocalTalk), then enable.
 Serial transmit (StartTxDMA): 1.3=0xC0, 1.1=buffer+get offset, 1.4=bytes to
	send, 1.5=bytes to end of buffer, 2.0=0x02, then enable.
 Sound output: 1.3=0x40 (0x60 for the last buffer), 1.1/1.5=first buffer,
	1.0/1.4=next buffer, 2.0=0x01.
 Sound input: 1.3 is never written, 1.1/1.5/1.4=first buffer, 1.0=next buffer,
	2.0=0x01.
 Tablet ADC (SetADCXfer): 1.3=0x20, 1.1=sample buffer, 1.5=2*sample count,
	1.6=0, 2.0=0x01.

 DMA power: TDMAManager::PowerOnAssignment() calls IOPowerOn(34+channel),
 which sets bit 0x40 in 0x0F110000 while at least one channel is powered
 (TVoyagerPlatform::PowerOnDMA).
 */

///
/// Class for the DMA Manager.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test	aucun test défini.
///
class TDMAManager
{
public:
	/// Channel numbers, as assigned by the ROM.
	enum {
		kSerialPort0Receive = 0, ///< 'extr' serial port receive
		kSerialPort0Transmit = 1, ///< 'extr' serial port transmit
		kInfraredReceiveAndTransmit = 2, ///< 'infr' IR receive, or receive and transmit when shared
		kSoundInput = 3, ///< PCirrusSoundDriver input, or IR transmit if not shared
		kTabletADC = 4, ///< TADC samples from the BIO (Keynes) ADC, used for the tablet
		kSoundOutput = 5, ///< PCirrusSoundDriver output
		kSerialPort3Receive = 6, ///< 'mdem' serial port receive
		kSerialPort3Transmit = 7 ///< 'mdem' serial port transmit
	};

	/// Request IDs for TDMAManager::RequestAssignment(id, &descriptor).
	/// The ROM maps each ID to a channel and an assignment register value
	/// (tables at ROM 0x003706E0, 0x0037070C, 0x00370738).
	enum {
		kRequestSerial0Rx = 0, ///< channel 0, assignment 0x004
		kRequestSerial0Tx = 1, ///< channel 1, assignment 0x004
		kRequestIRRx = 2, ///< channel 2, assignment 0x028
		kRequestIRTx = 3, ///< channel 3, assignment 0x028
		kRequestTabletADC = 4, ///< channel 4, assignment 0x040 (TADC::Init)
		kRequestChannel5 = 5, ///< channel 5, assignment 0x040 (not requested in the ROM)
		kRequestSerial3Rx = 6, ///< channel 6, assignment 0x400
		kRequestSerial3Tx = 7, ///< channel 7, assignment 0x400
		kRequestIRShared = 8, ///< channel 2, assignment 0x028 (IR uses one channel for rx and tx)
		kRequestSoundInput = 9, ///< channel 3, assignment 0x028
		kRequestSoundOutput = 10 ///< channel 5, assignment 0x040
	};

	/// Channel assignment register (0x0F08FC00). Each field configures a pair
	/// of channels. RequestAssignment() fails with -10078 if the field is
	/// already set to a different value. Only one value per field is used in
	/// the ROM, so the meaning of the other values is unknown.
	enum {
		kAssignChannel01Mask = 0x0007, ///< channels 0 and 1, set to 0x004
		kAssignChannel23Mask = 0x0038, ///< channels 2 and 3, set to 0x028
		kAssignChannel45Mask = 0x00C0, ///< channels 4 and 5, set to 0x040
		kAssignChannel67Mask = 0x0700 ///< channels 6 and 7, set to 0x400
	};

	/// Bank 1 register numbers (inRegister in Read/WriteChannel1Register).
	enum {
		kChan1Base = 0, ///< reload address when the buffer ends
		kChan1Pointer = 1, ///< current physical address
		kChan1Word = 2, ///< incomplete last word of a receive transfer
		kChan1Control = 3, ///< see kChanCtrl...
		kChan1Count = 4, ///< bytes left to transfer, or size of the next buffer
		kChan1Size = 5, ///< bytes left until the end of the current buffer
		kChan1Compare = 6 ///< serial receive notify level
	};

	/// Bank 2 register numbers (inRegister in Read/WriteChannel2Register).
	enum {
		kChan2IntEnable = 0, ///< interrupt enable
		kChan2IntStatus = 1, ///< interrupt status
		kChan2IntClear = 2, ///< write 1 to clear
		kChan2IntConfig = 3 ///< written once with the used interrupt bits, purpose unknown
	};

	/// Bits in the channel control register (1.3).
	enum {
		kChanCtrlSerial = 0x80, ///< set by TSerialDMAEngine, probably stops the channel when Count reaches 0
		kChanCtrlTransmit = 0x40, ///< memory to device (serial tx 0xC0, sound out 0x40)
		kChanCtrlStopAtEnd = 0x20 ///< probably stops at the end of the buffer instead of reloading (ADC, last sound buffer)
	};

	/// Bits in the channel interrupt registers (2.0, 2.1, 2.2).
	enum {
		kChanIntEndOfBuffer = 0x01, ///< used by sound and ADC, probably Size reached 0
		kChanIntDone = 0x02, ///< used by serial tx and rx, StopRxDMA treats it as "buffer full"
		kChanIntCompare = 0x04, ///< used by async serial rx, probably the Compare register matched
		kChanIntEndOfFrame = 0x10 ///< used by LocalTalk rx instead of kChanIntCompare
	};

	///
	/// Constructor from the memory and the interrupt manager.
	///
	TDMAManager(
		TLog* inLog,
		TEmulator* inEmulator,
		TMemory* inMemory,
		TInterruptManager* inInterruptManager);

	///
	/// Destructor.
	///
	~TDMAManager(void);

	///
	/// Read the channel assignment register.
	///
	/// \return the value of the channel assignment register.
	///
	KUInt32 ReadChannelAssignmentRegister(void);

	///
	/// Write the channel assignment register.
	///
	/// \param inValue	value to write to the channel assignment register.
	///
	void WriteChannelAssignmentRegister(KUInt32 inValue);

	///
	/// Write the enable register.
	/// Bits set to 1 will start the DMA transfers.
	///
	/// \param inValue	value to write to the enable register.
	///
	void WriteEnableRegister(KUInt32 inValue);

	///
	/// Read the status register.
	/// Bits set to 1 indicate channels that are still running. Drivers poll
	/// this after writing the disable register until the bit is cleared.
	///
	/// \return the value of the status register.
	///
	KUInt32 ReadStatusRegister(void);

	///
	/// Write the disable register.
	/// Bits set to 1 will abort the DMA transfers.
	///
	/// \param inValue	value to write to the disable register.
	///
	void WriteDisableRegister(KUInt32 inValue);

	///
	/// Read the word status register.
	/// Bits set to 1 indicate words in channel word registers.
	///
	/// \return the value of the word status register.
	///
	KUInt32 ReadWordStatusRegister(void);

	///
	/// Read a channel register from first bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \return the value of the register.
	///
	KUInt32 ReadChannel1Register(KUInt32 inChannel, KUInt32 inRegister);

	///
	/// Write a channel register in first bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \param inValue		word to write to the register.
	/// \return the value of the register.
	///
	void WriteChannel1Register(
		KUInt32 inChannel,
		KUInt32 inRegister,
		KUInt32 inValue);

	///
	/// Read a channel register from second bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \return the value of the register.
	///
	KUInt32 ReadChannel2Register(KUInt32 inChannel, KUInt32 inRegister);

	///
	/// Write a channel register in second bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \param inValue		word to write to the register.
	/// \return the value of the register.
	///
	void WriteChannel2Register(
		KUInt32 inChannel,
		KUInt32 inRegister,
		KUInt32 inValue);

	///
	/// Save or restore the state to or from a file.
	///
	void TransferState(TStream* inStream);

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TDMAManager(const TDMAManager& inCopy) = delete;

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TDMAManager& operator=(const TDMAManager& inCopy) = delete;

	/// \name Variables

	/// Interface for logging.
	TLog* mLog { nullptr };

	/// Reference on the memory.
	TMemory* mMemory { nullptr };

	/// Reference on the interrupt mgr.
	TInterruptManager* mInterruptManager { nullptr };

	/// Reference the emulator
	TEmulator* mEmulator { nullptr };

	/// Assignment register (0x0F08FC00, see kAssign...).
	KUInt32 mAssignmentReg { 0 };
};

#endif
// _TDMAMANAGER_H

// ========================== //
// Memory fault - where am I? //
// ========================== //
