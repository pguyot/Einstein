// ==============================
// File:			TATACard.h
// Project:			Einstein
//
// Copyright 2003-2026 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TATACARD_H
#define _TATACARD_H

#include <K/Defines/KDefinitions.h>
#include "TPCMCIACard.h"

#include <cstdint>
#include <vector>

///
/// Class for ATA cards.
///
/// The card is a memory mapped PC Card ATA device that is backed by a raw disk
/// image of up to 128MB. It answers the ATA commands of the Newton ATA driver.
/// See TATACard.cpp for the details.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TATACard : public TPCMCIACard
{
public:
	///
	/// Constructor from the path of the image file.
	///
	TATACard(const char* inImagePath);

	///
	/// Destructor.
	///
	virtual ~TATACard(void);

	///
	/// Called by the controller to say we've been inserted.
	///
	virtual int Init(TPCMCIAController* inController);

	///
	/// Called by the controller to say we've been removed.
	///
	virtual void Remove();

	///
	/// Get PCMCIA pins.
	///
	virtual KUInt32 GetVPCPins(void);

	///
	/// Set PCMCIA pins.
	///
	virtual void SetVPCPins(KUInt32 inPins);

	///
	/// Read attribute space.
	///
	virtual KUInt32 ReadAttr(KUInt32 inOffset);

	///
	/// Read attribute space (byte).
	///
	virtual KUInt8 ReadAttrB(KUInt32 inOffset);

	///
	/// Read I/O space.
	///
	virtual KUInt32 ReadIO(KUInt32 inOffset);

	///
	/// Read I/O space (byte).
	///
	virtual KUInt8 ReadIOB(KUInt32 inOffset);

	///
	/// Read memory space.
	///
	virtual KUInt32 ReadMem(KUInt32 inOffset);

	///
	/// Read memory space (byte).
	///
	virtual KUInt8 ReadMemB(KUInt32 inOffset);

	///
	/// Write attribute space.
	///
	virtual void WriteAttr(KUInt32 inOffset, KUInt32 inValue);

	///
	/// Write attribute space (byte).
	///
	virtual void WriteAttrB(KUInt32 inOffset, KUInt8 inValue);

	///
	/// Write I/O space.
	///
	virtual void WriteIO(KUInt32 inOffset, KUInt32 inValue);

	///
	/// Write I/O space (byte).
	///
	virtual void WriteIOB(KUInt32 inOffset, KUInt8 inValue);

	///
	/// Write memory space.
	///
	virtual void WriteMem(KUInt32 inOffset, KUInt32 inValue);

	///
	/// Write memory space (byte).
	///
	virtual void WriteMemB(KUInt32 inOffset, KUInt8 inValue);

private:
	/// The value of the Status register. Busy states end when it is read.
	KUInt8 ReadStatus(void);

	/// Return the next byte of the data FIFO. After the last byte of a block
	/// the next block is loaded, or the command is done.
	KUInt8 ReadFifoByte(void);

	/// Store the next byte of a block sent by the host. When the block is
	/// complete, it is written to the image.
	void WriteFifoByte(KUInt8 inByte);

	/// The host wrote the Command register.
	void StartCommand(KUInt8 inCommand);

	/// Fill mFifo with the 512 bytes of the Identify Drive response.
	void BuildIdentifyData(void);

	/// Fill mFifo with the sector addressed by the task file registers.
	/// Returns false and sets mErrorReg if that sector is not in the image.
	bool BuildSectorData(void);

	/// Write mFifo to the sector addressed by the task file registers, in
	/// memory and in the image file. Returns false and sets mErrorReg if the
	/// file could not be written.
	bool WriteSectorData(void);

	/// Read Verify Sectors: check the sectors addressed by the task file
	/// registers, without transferring any data.
	void VerifySectors(void);

	/// True if the sector addressed by the task file registers is in the image.
	/// Sets mErrorReg to IDNF if it is not. The host can change the registers at
	/// any time, so check again before every access to mData, and use the offset
	/// that this returns in outOffset (the position of the sector in mData).
	bool IsSectorValid(uint64_t* outOffset = nullptr);

	/// Push all writes through to the storage device.
	void FlushImage(void);

	/// Put every register in the state of a card that was just powered up.
	void PowerOn(void);

	/// Reset the ATA side of the card: no command running, FIFO empty, and the
	/// task file registers get their power-on values (the ATA signature).
	void Reset(void);

	/// True while the host holds either SRESET (Configuration Option Register)
	/// or SRST (Device Control register).
	bool IsInReset(void) const;

	/// To be called after either of the two reset bits may have changed.
	/// Enters the reset state, or leaves it again if both bits are clear.
	void UpdateReset(bool inWasInReset);

	/// Make up a plausible geometry for the size of the image.
	void SetGeometry(void);

	/// Sector address from the task file registers, in LBA or CHS mode.
	/// Returns 0xFFFFFFFF if a CHS address is invalid.
	KUInt32 GetLBA(void);

	/// Write a sector address back into the task file registers.
	void SetLBA(KUInt32 inLBA);

	/// \name Variables

	static const KUInt8 kDefaultCISData[];

	/// What the card is doing, and what the Status register shows.
	enum class State {
		Idle, ///< RDY|DSC, waiting for commands
		NoDataBusy, ///< BSY, will change to Idle next
		DataReadBusy, ///< BSY, will go to DataReadReady
		DataReadReady, ///< RDY|DSC|DRQ, the FIFO can be read, back to Idle after the last block
		DataWriteBusy, ///< BSY, will go to DataWriteReady
		DataWriteReady, ///< RDY|DSC|DRQ, the FIFO can be written, back to Idle after the last block
		Error, ///< RDY|DSC|ERR, mErrorReg has the reason, until the next command
		WriteFault, ///< RDY|DSC|DWF|ERR, image is read only or can't be written, until the next command
		InReset, ///< BSY while the host holds SRESET or SRST, will go to NoDataBusy
	};

	State mState { State::Idle };

	char* mFilePath { nullptr };

	/// The image file, kept open for writing for as long as the card exists.
	FILE* mFile { nullptr };

	/// True if the image could not be opened for writing.
	bool mReadOnly { false };

	/// Size of the image file. It is clipped to the size of mData.
	uint64_t mFileSize { 0 };

	/// The image. Reads come from here, writes go here and to mFile.
	std::vector<KUInt8> mData;

	/// Geometry, made up from the size of the image by SetGeometry().
	/// The host uses mSectors if the card says that it can do LBA, CHS
	/// addresses are converted using the other three values.
	KUInt16 mCylinders { 0 };
	KUInt16 mHeads { 1 };
	KUInt16 mSectorsPerTrack { 1 };
	KUInt32 mSectors { 0 }; ///< Total number of sectors in the image

	/// Data the host reads through, or writes to, the data register.
	std::vector<KUInt8> mFifo;

	/// Index of the next byte in mFifo.
	size_t mFifoPos { 0 };

	/// Task file registers in memory space.
	KUInt8 mErrorReg { 0 }; ///< Read side of register 1
	KUInt8 mFeaturesReg { 0 }; ///< Write side of register 1
	KUInt8 mSectorCountReg { 0 };
	KUInt8 mSectorNumberReg { 0 };
	KUInt8 mCylinderLowReg { 0 };
	KUInt8 mCylinderHighReg { 0 };
	KUInt8 mDriveHeadReg { 0 };
	KUInt8 mCommandReg { 0 };
	KUInt8 mDeviceControlReg { 0 }; ///< Write side of register 0x0E

	/// Configuration registers in attribute space.
	KUInt8 mConfigOptionReg { 0 }; ///< At 0x200
	KUInt8 mConfigStatusReg { 0 }; ///< At 0x202, only the bits the host can write
};

#endif
// _TATACARD_H

// ============================================================================== //
//         One of the questions that comes up all the time is: How enthusiastic   //
// is our support for UNIX?                                                       //
//         Unix was written on our machines and for our machines many years ago.  //
// Today, much of UNIX being done is done on our machines. Ten percent of our     //
// VAXs are going for UNIX use.  UNIX is a simple language, easy to understand,   //
// easy to get started with. It's great for students, great for somewhat casual   //
// users, and it's great for interchanging programs between different machines.   //
// And so, because of its popularity in these markets, we support it.  We have    //
// good UNIX on VAX and good UNIX on PDP-11s.                                     //
//         It is our belief, however, that serious professional users will run    //
// out of things they can do with UNIX. They'll want a real system and will end   //
// up doing VMS when they get to be serious about programming.                    //
//         With UNIX, if you're looking for something, you can easily and quickly //
// check that small manual and find out that it's not there.  With VMS, no matter //
// what you look for -- it's literally a five-foot shelf of documentation -- if   //
// you look long enough it's there.  That's the difference -- the beauty of UNIX  //
// is it's simple; and the beauty of VMS is that it's all there.                  //
//                 -- Ken Olsen, president of DEC, DECWORLD Vol. 8 No. 5, 1984    //
// [It's been argued that the beauty of UNIX is the same as the beauty of Ken     //
// Olsen's brain.  Ed.]                                                           //
// ============================================================================== //
