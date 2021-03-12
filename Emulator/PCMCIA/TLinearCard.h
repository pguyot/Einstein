// ==============================
// File:			TLinearCard.h
// Project:			Einstein
//
// Copyright 2003-2020 by Paul Guyot (pguyot@kallisys.net)
// and Matthias Melcher (einstein@matthiasm.com).
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

#ifndef _TLINEARCARD_H
#define _TLINEARCARD_H

#include <K/Defines/KDefinitions.h>
#include "TPCMCIACard.h"

#include <stdio.h>

#include <mutex>
#include <thread>
#include <vector>
#include <future>

///
/// Class for a linear flash memory card.
///
/// This class emulates the JEDEC PC card standard an Intel Series II linear
/// flash card with two 28F016SA chips. The flash chips provide 1MBit x 8 and
/// are mapped to the 16bit PCMCIA bus. The Voyager chip inside the
/// MessagePad maps the card to a full 32bit bus for reading. The card is written
/// using the JEDEC standard through D16-D31. D0-15 are written by flipping
/// the low word and the high word through the Voyager control register at 0x2000.
///
/// As of July 19th 2020, basic functionaly has been tested with a Flash dump
/// from an existing memory card.
/// 
/// It will be problematic to support all possible kinds of Flash cards from
/// various manufacturers and in various chip configurations. We will need
/// a way to verify that existing cards are handled correctly (or they must be
/// configured as read-only), and that newly created cards have a sensible CIS
/// set and are also handled correctly.
///
/// \todo Thourough testing is needed.
/// \todo Map Flash memory to a file on the host system
/// \todo Add a user interface for creating and managing PCMCIA cards
/// \todo I expected ROM images to work out of the box, but they do not. They seem
///			to mirror the CIS in the memeory area (makes sense: it saves the expense
///			of a CIS chip in the card), but there must be more magic to it, because
///			Einstein does not recognize them at the moment.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \author Matthias Melcher <einstein@matthiasm.com>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TLinearCard
	:
	public TPCMCIACard
{
private:
	typedef TPCMCIACard super;

public:

	// everything ends on a 4 byte boundary, so we should not get any packing effects
	typedef struct ImageInfo {
		int read(FILE* f);
		int write(FILE* f);
		void flipByteOrder();
		KUInt32 pNameSize = 0;	// size of image name in bytes
		KUInt32 pNameStart = 0; // start of image name, name is stored as a utf8 C-string
		KUInt32 pIconSize = 0;	// size of image data in bytes
		KUInt32 pIconStart = 0; // start of image data, optional, image is stored in PNG format
		KUInt32 pCISSize = 0;	// size of CIS data in bytes
		KUInt32 pCISStart = 0;  // start of CIS data, CIS data is stored in natural order(0123 < -x1x0 x3x2)
		KUInt32 pDataSize = 0;	// size of Data block in bytes
		KUInt32 pDataStart = 0;	// start of Data block, Flash Data is stored as seen by the Newton
		KUInt32 pType = 0;		// type of card, according to CIS (only uses the last 4 bits!)
		KUInt32 pVersion = 1;	// version of this format
		char	pIdent[12] = "TLinearCard"; // "TLinearCard\0"
	} ImageInfo;
	static const char* kIdent;

	///
	/// Compose PCMCIA file from Data, CIS, and Image
	///
	static KSInt32 ComposeImageFile(const char* inOutFilename, const char* inDataFilename, const char* inCISFilename, const char* name);

	///
	/// Create a PCMCIA image file for a Flash Card with inSizeMB capacity. 
	/// 
	static KSInt32 CreateImageFile(const char* inName, const char* inImageFilename, KUInt32 inSizeMB);

	static const KSInt32 kErrCantCreateOutFile = -1;
	static const KSInt32 kErrCantOpenDataFile = -2;
	static const KSInt32 kErrCantOpenCISFile = -3;
	static const KSInt32 kErrCorruptDataFile = -4;
	static const KSInt32 kErrDataFileTooBig = -5;
	static const KSInt32 kErrOutOfMemory = -6;
	static const KSInt32 kErrIncompleteImageFile = -7;
	static const KSInt32 kErrCorruptCISFile = -8;

private:
	static const KUInt8 kDefaultCISData[];

public:
	///
	/// Constructor from the size.
	///
	TLinearCard(const char* inImagePath);

	///
	/// Destructor.
	///
	~TLinearCard(void) override;

	///
	/// Called by the controller to say we've been inserted.
	///
	int Init(TPCMCIAController* inController) override;

	///
	/// Called by the controller to say we've been removed.
	///
	void Remove() override;

	///
	/// Get PCMCIA pins.
	///
	KUInt32 GetVPCPins( void ) override;
	
	///
	/// Set PCMCIA pins.
	///
	void SetVPCPins( KUInt32 inPins ) override;

	///
	/// Read attribute space.
	///
	KUInt32 ReadAttr( KUInt32 inOffset ) override;

	///
	/// Read attribute space (byte).
	///
	KUInt8 ReadAttrB( KUInt32 inOffset ) override;

	///
	/// Read I/O space.
	///
	KUInt32 ReadIO( KUInt32 inOffset ) override;

	///
	/// Read I/O space (byte).
	///
	KUInt8 ReadIOB( KUInt32 inOffset ) override;

	///
	/// Read memory space.
	///
	KUInt32 ReadMem( KUInt32 inOffset ) override;

	///
	/// Read memory space (byte).
	///
	KUInt8 ReadMemB( KUInt32 inOffset ) override;

	///
	/// Write attribute space.
	///
	void WriteAttr( KUInt32 inOffset, KUInt32 inValue ) override;

	///
	/// Write attribute space (byte).
	///
	void WriteAttrB( KUInt32 inOffset, KUInt8 inValue ) override;

	///
	/// Write I/O space.
	///
	void WriteIO( KUInt32 inOffset, KUInt32 inValue ) override;

	///
	/// Write I/O space (byte).
	///
	void WriteIOB( KUInt32 inOffset, KUInt8 inValue ) override;

	///
	/// Write memory space.
	///
	void WriteMem( KUInt32 inOffset, KUInt32 inValue ) override;

	///
	/// Write memory space (byte).
	///
	void WriteMemB( KUInt32 inOffset, KUInt8 inValue ) override;

protected:
	void Flush();

private:
	ImageInfo mImageInfo;

	KUInt32 mSize = 0;  ///< Size of the linear card in bytes.

	KUInt8* mMemoryMap = nullptr;

	KUInt8* mCISData = nullptr;

	KUInt32 mCISDataSize = 0;

	char* mFilePath = nullptr;

	FILE* mFile = nullptr;

    enum {
        kReadArray,
        kReadStatusRegister,
        kWriteArray,
        kEraseSetup
    };

    int mState = kReadArray;

    KUInt8 mStatusRegister = 0x80;

	std::mutex mMutex;
	std::promise<void>* mPageFlushPromise = nullptr;
	std::future<void> mPageFlushFuture;
	std::vector<Boolean> mPageDirty;

	const int kPageSizeShift = 14; // 16kByte pages;
	const int kPageSize = 1 << kPageSizeShift;

	void MarkPageDirty(KUInt32 inAddress);
	void FlushDirtyPages();
	void FlushDirtyPagesLater();
	void AbortFlushDirtyPages();
};

#endif
		// _TLINEARCARD_H

// ======================================================================= //
// Flash Card                                                              //
//                                                                         //
// A memory card using flash RAM chips that can be rewritten and maintain  //
// the storage of information when power is removed. Commonly referred to  //
// as a storage card. Not to be confused with a CompactFlash (CF) card,    //
// which is similar in function but different in size and implementation.  //
//                                                                         //
//  - Newton Glossary                                                      //
// ======================================================================= //
