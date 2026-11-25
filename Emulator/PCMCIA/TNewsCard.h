// ==============================
// File:			TNewsCard.h
// Project:			Einstein
//
// Copyright 2026 by Vasyl Samoilov (vasyl.samoilov@gmail.com).
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
//
// Motorola NewsCard / Newton Messaging Card (POCSAG pager PC Card).
//
// Emulates the card's mailbox protocol and memory layout. Pages, the switch
// and the battery come in through a control folder (see PollControl()).
// ==============================

#ifndef _TNEWSCARD_H
#define _TNEWSCARD_H

#include "TPCMCIACard.h"

#include <ctime>
#include <map>
#include <set>
#include <string>
#include <vector>

class TNewsCard : public TPCMCIACard
{
public:
	/// inDir is the card folder. Required: flash.bin (128 KB flash at common
	/// 0x02000000, the card's Newton software). Optional, in card byte order:
	/// ram.bin (common memory 0..0x20000; default: an empty card), attr.bin
	/// (4 KB attribute memory; default: the RAM) and config_4a_ff.bin (config
	/// bytes 0x4A..0xFF; default: kDefaultConfig). These files are only read.
	/// Pages come in through <inDir>/ctl/inbox, which must be writable: page
	/// files are renamed when stored. Environment:
	///   EINSTEIN_NEWSCARD_CTL=<dir>   control directory (default <inDir>/ctl)
	///   EINSTEIN_NEWSCARD_TRACE=1     log every mailbox/store access
	explicit TNewsCard(const char* inDir);
	~TNewsCard(void) override;

	KUInt32
	GetStateTag(void) override
	{
		return 'nwsc';
	}

	/// True if inPath is a usable flash.bin (exactly 128 KB, readable).
	static bool IsFlashImage(const char* inPath);

	/// The flash image and the folder identify the card for fast start and
	/// checkpoints.
	KUInt32 GetContentsChecksum(void) override;
	KUInt32
	GetContentsSize(void) override
	{
		return mFlashLoaded ? kFlashSize : 0;
	}
	/// The flash comes from flash.bin and is never replaced.
	bool
	CanWriteContents(void) override
	{
		return false;
	}

	/// The card folder (as the image path of the other cards).
	const char*
	GetImagePath(void) override
	{
		return mDir.c_str();
	}
	/// Fast start and checkpoints: the card is battery-backed, so its RAM
	/// (message store, mailbox), config bytes, clock, switch, battery and
	/// year mode are saved, and a reset keeps them. The flash and attribute
	/// memory come from the card folder.
	void TransferState(TStream* inStream) override;
	KUInt32 GetVPCPins(void) override;
	void SetVPCPins(KUInt32 inPins) override;

	KUInt32 ReadAttr(KUInt32 inOffset) override;
	KUInt8 ReadAttrB(KUInt32 inOffset) override;
	KUInt32 ReadIO(KUInt32 inOffset) override;
	KUInt8 ReadIOB(KUInt32 inOffset) override;
	KUInt32 ReadMem(KUInt32 inOffset) override;
	KUInt8 ReadMemB(KUInt32 inOffset) override;

	void WriteAttr(KUInt32 inOffset, KUInt32 inValue) override;
	void WriteAttrB(KUInt32 inOffset, KUInt8 inValue) override;
	void WriteIO(KUInt32 inOffset, KUInt32 inValue) override;
	void WriteIOB(KUInt32 inOffset, KUInt8 inValue) override;
	void WriteMem(KUInt32 inOffset, KUInt32 inValue) override;
	void WriteMemB(KUInt32 inOffset, KUInt8 inValue) override;

private:
	enum {
		kRAMSize = 0x20000,
		kFlashBase = 0x02000000,
		kFlashSize = 0x20000,
		kCmd = 0x0C00,
		kSem = 0x0FF0,
		kStat = 0x0FFD,
		kOwn = 0x0FFE,
		kDoorbell = 0x0FFF,
		kRsp = 0x1000,
		kDir = 0x1400,
		kHeap = 0x1C00,
		kACK = 0x06,
		kNAK = 0x15
	};

	KUInt8 CardRead(KUInt32 inCardOffset, bool inLog);
	/// Store a byte; returns true if it rang the doorbell (0xFF to kDoorbell).
	bool CardWrite(KUInt32 inCardOffset, KUInt8 inValue);
	void ProcessCommand(void);
	void Reply(KUInt8 inCmd, const std::vector<KUInt8>& inData, KUInt8 inStatus = kACK);

	// emulated card
	void EmulateCommand(KUInt8 inCmd, const KUInt8* inPayload, KUInt16 inLen);
	KUInt32 Get32(KUInt32 inOff);
	void Put32(KUInt32 inOff, KUInt32 inValue);
	KUInt16 Get16(KUInt32 inOff);
	void Put16(KUInt32 inOff, KUInt16 inValue);
	/// All records and the blocks they use; outLinkedFrom: for each record,
	/// the directory slot or record +0 that links to it.
	void Walk(std::vector<KUInt32>& outRecords, std::set<KUInt32>& outUsed,
		std::map<KUInt32, KUInt32>* outLinkedFrom = nullptr);
	void CountMessages(KUInt16& outTotal, KUInt16& outUnread, KUInt16& outFree);
	bool Unlink(KUInt32 inRecord);
	bool AddPage(int inSlot, int inFunc, const std::string& inText, KUInt32& outRecord);
	bool HasRoom(size_t inBlocks);
	bool ReclaimReadMessage(void);
	void InitLastSeq(void);
	bool IsNewer(KUInt32 inA, KUInt32 inB);
	static bool
	InStore(KUInt32 inAddr)
	{
		return inAddr >= kHeap && inAddr + 64 <= kRAMSize && (inAddr & 63) == 0;
	}
	void GetClock(KUInt8 outTime[5]);
	bool SetClock(const KUInt8 inTime[5]);
	void PollControl(bool inForce);
	void ReadControlSettings(void);
	void ProcessInbox(void);

	std::vector<KUInt8> mAttr;
	std::string mDir;
	std::vector<KUInt8> mRAM;
	std::vector<KUInt8> mFlash;
	bool mFlashLoaded = false;
	bool mAttrFromFile = false; // attr.bin given; otherwise attribute reads show the RAM
	KUInt8 mConfig[0x100];
	bool mConfigReadable[0x100];
	time_t mClockBase; // emulated clock value (seconds, as UTC) at mClockHost
	time_t mClockHost; // host time when the clock was set
	KUInt8 mSwitch; // status byte 1: 04 on, 02 silent, 01 off
	bool mBatteryOK;
	// Year handling (ctl/year). The card keeps a two-digit year and NAKs
	// year bytes over 99; the Newton sends year - 1900.
	//   kYearTranslate (default): year bytes over 99 from the Newton are
	//     stored minus 100; years 0..92 read by the Newton (clock, status,
	//     record timestamps) are returned plus 100.
	//   kYearStrict: no translation (NAK 10 for year bytes over 99).
	//   kYearAccept: year % 100 stored, nothing translated back.
	enum EYearMode { kYearTranslate,
		kYearStrict,
		kYearAccept };
	EYearMode mYearMode;
	std::set<KUInt32> mRecordSet; // record offsets, for translating +45
	bool mRecordSetDirty;
	static KUInt8
	YearToNewton(KUInt8 inYear)
	{
		return inYear < 93 ? (KUInt8) (inYear + 100) : inYear;
	}
	bool IsRecord(KUInt32 inOffset);
	static std::string DecodePageText(const std::string& inText);
	/// Longest page the card stores.
	static const size_t kMaxPageChars = 2019;
	/// Largest page file read (every character as a 6-byte escape).
	static const size_t kMaxPageFileBytes = 16 * 1024;
	std::set<std::string> mReportedFull; // pages waiting for room, reported once
	KUInt8 mLastSeq = 0; // sequence number of the last page stored
	void TranslateReplyYears(KUInt8 inCmd);
	std::string mCtlDir;
	double mLastCtlPoll;
	bool mTrace;
};

#endif
