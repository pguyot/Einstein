// ==============================
// File:			TNewsCard.cpp
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
// Motorola NewsCard / Newton Messaging Card. See TNewsCard.h.
//
// Card model:
// - Common memory 0..0x20000 is RAM: CIS at 0x30, mailbox (request 0xC00,
//   SEM 0xFF0, STAT 0xFFD, OWN 0xFFE, doorbell 0xFFF, reply 0x1000),
//   directory of 16 buckets at 0x1400 (0x80 apart), 64-byte blocks from
//   0x1C00. Flash (128 KB) at 0x02000000 holds the Newton driver package.
// - Reply: [cmd, lenHi, lenLo, status, data..., FF, AA], len = 1 + data;
//   status 06 = ACK, 15 = NAK with the error code as first data byte.
// - Record (64 bytes): +0 next (older) in the bucket, +4 previous (newer,
//   the directory slot for the head), +9 seq, +10 errors, +12/+16 a ring
//   over all records, +20 first text block, +24 last text byte, +36 source
//   = slot*8 + function, +38 flags (bit 2 read), +39 class, +40 blocks,
//   +44 0x80, +45 time YY MM DD hh mm (two-digit year).
// - Text blocks: +0 next, +4 previous, a circular ring; text at +10..+63.
// - Free count (status bytes 18-19) = unused blocks + blocks of read
//   messages (record + text): marking a message read makes it reclaimable.
// ==============================

#include "TNewsCard.h"

#include "Emulator/PCMCIA/TPCMCIAController.h"
#include "K/Misc/CRC32.h"
#include "K/Streams/TStream.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#if TARGET_OS_WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

// CIS at common memory 0x00..0x7F (Ver 1.0.0; the card ignores REG#, so the
// attribute CIS is the same bytes) and config bytes 0x4A..0xFF as returned
// by command 0x11. Defaults when the card folder has no ram.bin, attr.bin or
// config_4a_ff.bin.
// clang-format off
static const KUInt8 kDefaultCIS[0x80] = {
	0x01, 0x00, 0x0B, 0x00, 0xD4, 0x00, 0x1B, 0x00, 0x00, 0x00, 0xDB, 0x00,
	0x00, 0x00, 0xFB, 0x00, 0x00, 0x00, 0x76, 0x00, 0x5A, 0x00, 0x04, 0x00,
	0xFF, 0x00, 0x12, 0x00, 0x04, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xEF, 0x7D, 0xFF, 0xEF,
	0x13, 0x03, 0x43, 0x49, 0x53, 0x18, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xA7, 0x15, 0x34, 0x04, 0x01, 0x4D, 0x6F, 0x74,
	0x6F, 0x72, 0x6F, 0x6C, 0x61, 0x2C, 0x20, 0x49, 0x6E, 0x63, 0x2E, 0x00,
	0x4E, 0x65, 0x77, 0x73, 0x43, 0x61, 0x72, 0x64, 0x20, 0x44, 0x61, 0x74,
	0x61, 0x20, 0x52, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x72, 0x00, 0x56,
	0x65, 0x72, 0x20, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x00, 0x00, 0xFF, 0x12,
	0x04, 0x00, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF,
};
// clang-format on

// clang-format off
static const KUInt8 kDefaultConfig[0x100 - 0x4A] = {
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0xA4, 0x0A,
	0x00, 0x80, 0x00, 0x3A, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4B, 0x90,
	0x34, 0x38, 0x39, 0x42, 0x55, 0x47, 0x32, 0x33, 0x50, 0x37, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x05, 0x80, 0xB9, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCF, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03,
	0x03, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xFF,
};
// clang-format on

// Source of flash.bin, for the error message.
static const char* const kFlashURL = "https://archive.org/details/motorola-newscard-newton-messaging-card-dump";

// The Newton bus swaps byte lanes: Newton address A reaches card offset A^3.
static inline KUInt32
NewtonToCard(KUInt32 inNewtonAddr)
{
	return inNewtonAddr ^ 3;
}

static double
Now(void)
{
	using namespace std::chrono;
	return duration<double>(steady_clock::now().time_since_epoch()).count();
}

// UTC calendar <-> seconds since 1970 (proleptic Gregorian), portable
// replacements for timegm() and gmtime_r().
static time_t
SecondsFromCivil(int inYear, int inMonth, int inDay, int inHour, int inMinute)
{
	int y = inYear - (inMonth <= 2);
	int era = (y >= 0 ? y : y - 399) / 400;
	int yoe = y - era * 400;
	int doy = (153 * (inMonth + (inMonth > 2 ? -3 : 9)) + 2) / 5 + inDay - 1;
	int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	long long days = (long long) era * 146097 + doe - 719468;
	return (time_t) (days * 86400 + inHour * 3600 + inMinute * 60);
}

static void
CivilFromSeconds(time_t inSeconds, int& outYear, int& outMonth, int& outDay, int& outHour, int& outMinute)
{
	long long secs = (long long) inSeconds;
	long long days = secs >= 0 ? secs / 86400 : (secs - 86399) / 86400;
	long long rem = secs - days * 86400;
	outHour = (int) (rem / 3600);
	outMinute = (int) (rem % 3600 / 60);
	days += 719468;
	long long era = (days >= 0 ? days : days - 146096) / 146097;
	long long doe = days - era * 146097;
	long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
	long long mp = (5 * doy + 2) / 153;
	outDay = (int) (doy - (153 * mp + 2) / 5 + 1);
	outMonth = (int) (mp < 10 ? mp + 3 : mp - 9);
	outYear = (int) (yoe + era * 400 + (outMonth <= 2));
}

static bool
EqualNoCase(const std::string& inA, const std::string& inB)
{
	if (inA.size() != inB.size())
		return false;
	for (size_t i = 0; i < inA.size(); i++)
		if (tolower((unsigned char) inA[i]) != tolower((unsigned char) inB[i]))
			return false;
	return true;
}

static bool
EndsWith(const std::string& inName, const char* inSuffix)
{
	std::string suffix(inSuffix);
	return inName.size() > suffix.size() && inName.compare(inName.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// The names of the *.page and *.page.work files in a folder.
static std::vector<std::string>
ListPages(const std::string& inDir)
{
	std::vector<std::string> names;
	auto add = [&names](const std::string& inName) {
		if (EndsWith(inName, ".page") || EndsWith(inName, ".page.work"))
			names.push_back(inName);
	};
#if TARGET_OS_WIN32
	WIN32_FIND_DATAA fd;
	HANDLE h = FindFirstFileA((inDir + "\\*.page*").c_str(), &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
			add(fd.cFileName);
		while (FindNextFileA(h, &fd));
		FindClose(h);
	}
#else
	if (DIR* d = opendir(inDir.c_str()))
	{
		while (struct dirent* e = readdir(d))
			add(e->d_name);
		closedir(d);
	}
#endif
	std::sort(names.begin(), names.end());
	return names;
}

// Load exactly inSize bytes. A missing file returns false quietly; a file
// of another size returns false with a message (unless inQuiet).
static bool
LoadFile(const std::string& inPath, std::vector<KUInt8>& outData, size_t inSize, bool inQuiet = false)
{
	FILE* f = fopen(inPath.c_str(), "rb");
	if (!f)
		return false;
	std::vector<KUInt8> data(inSize + 1);
	size_t n = fread(data.data(), 1, inSize + 1, f);
	bool ok = !ferror(f) && n == inSize;
	fclose(f);
	if (!ok)
	{
		if (!inQuiet)
			fprintf(stderr, "NEWSCARD: %s is not %zu bytes, ignored\n", inPath.c_str(), inSize);
		return false;
	}
	data.resize(inSize);
	outData.swap(data);
	if (!inQuiet)
		fprintf(stderr, "NEWSCARD: loaded %s\n", inPath.c_str());
	return true;
}

// Read a whole file of at most inMax bytes. False if it can't be opened or
// read completely, or if it is larger.
static bool
ReadSmallFile(const std::string& inPath, std::string& outContent, size_t inMax = 4096)
{
	outContent.clear();
	FILE* f = fopen(inPath.c_str(), "rb");
	if (!f)
		return false;
	char buf[4096];
	size_t n;
	bool ok = true;
	while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
	{
		outContent.append(buf, n);
		if (outContent.size() > inMax)
		{
			ok = false;
			break;
		}
	}
	if (ferror(f))
		ok = false;
	fclose(f);
	return ok;
}

static std::string
ReadSetting(const std::string& inPath)
{
	std::string s;
	return ReadSmallFile(inPath, s) ? s : std::string();
}

// Rename, atomically replacing an existing target. An existing target is
// kept if the rename fails.
static bool
RenameReplacing(const std::string& inFrom, const std::string& inTo)
{
#if TARGET_OS_WIN32
	return MoveFileExA(inFrom.c_str(), inTo.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
#else
	return rename(inFrom.c_str(), inTo.c_str()) == 0;
#endif
}

// Page header: the first line is exactly two tokens, "slot=S" (0 or 1) and
// "func=F" (A..D, a..d or 0..3), in this order, separated by blanks.
static bool
ParsePageHeader(const std::string& inHeader, int& outSlot, int& outFunc)
{
	std::vector<std::string> tokens;
	size_t i = 0;
	while (i < inHeader.size())
	{
		while (i < inHeader.size() && (inHeader[i] == ' ' || inHeader[i] == '\t' || inHeader[i] == '\r'))
			i++;
		size_t start = i;
		while (i < inHeader.size() && inHeader[i] != ' ' && inHeader[i] != '\t' && inHeader[i] != '\r')
			i++;
		if (i > start)
			tokens.push_back(inHeader.substr(start, i - start));
	}
	if (tokens.size() != 2 || tokens[0].size() != 6 || tokens[1].size() != 6
		|| tokens[0].compare(0, 5, "slot=") != 0 || tokens[1].compare(0, 5, "func=") != 0)
		return false;
	char s = tokens[0][5];
	char f = tokens[1][5];
	if (s != '0' && s != '1')
		return false;
	outSlot = s - '0';
	if (f >= 'A' && f <= 'D')
		outFunc = f - 'A';
	else if (f >= 'a' && f <= 'd')
		outFunc = f - 'a';
	else if (f >= '0' && f <= '3')
		outFunc = f - '0';
	else
		return false;
	return true;
}

bool
TNewsCard::IsFlashImage(const char* inPath)
{
	std::vector<KUInt8> data;
	return inPath && LoadFile(inPath, data, kFlashSize, true);
}

static std::string
Trim(const std::string& s)
{
	size_t a = s.find_first_not_of(" \t\r\n");
	size_t b = s.find_last_not_of(" \t\r\n");
	return (a == std::string::npos) ? std::string() : s.substr(a, b - a + 1);
}

TNewsCard::TNewsCard(const char* inDir) :
		mClockBase(0),
		mClockHost(0),
		mSwitch(0x04),
		mBatteryOK(true),
		mYearMode(kYearTranslate),
		mRecordSetDirty(true),
		mLastCtlPoll(0),
		mTrace(getenv("EINSTEIN_NEWSCARD_TRACE") != nullptr)
{
	std::string dir(inDir ? inDir : ".");
	mDir = dir;
	// Flash: the card's Newton software, loaded by the Newton from the card.
	mFlashLoaded = LoadFile(dir + "/flash.bin", mFlash, kFlashSize);
	if (!mFlashLoaded)
	{
		mFlash.assign(kFlashSize, 0xFF);
		fprintf(stderr, "NEWSCARD: no %s/flash.bin; the Newton will not recognize the card. Get it from %s\n",
			dir.c_str(), kFlashURL);
	}
	// RAM: a saved image (with messages), or an empty card.
	if (!LoadFile(dir + "/ram.bin", mRAM, kRAMSize))
	{
		mRAM.assign(kRAMSize, 0x00);
		memcpy(mRAM.data(), kDefaultCIS, sizeof(kDefaultCIS));
		std::fill(mRAM.begin() + kDir, mRAM.end(), 0xFF); // empty directory and heap
	}
	// Attribute memory: the card ignores REG#, so it shows the RAM unless
	// attr.bin is given.
	mAttrFromFile = LoadFile(dir + "/attr.bin", mAttr, 0x1000);

	// Config space: 0x4A..0xFF readable, NAK code 8 elsewhere.
	memset(mConfig, 0, sizeof(mConfig));
	for (int a = 0; a < 0x100; a++)
		mConfigReadable[a] = (a >= 0x4A);
	std::vector<KUInt8> cfg;
	if (LoadFile(dir + "/config_4a_ff.bin", cfg, 0x100 - 0x4A))
		memcpy(&mConfig[0x4A], cfg.data(), cfg.size());
	else
		memcpy(&mConfig[0x4A], kDefaultConfig, sizeof(kDefaultConfig));

	// The clock starts at 93-01-01 00:00 when the card is switched on.
	mClockBase = SecondsFromCivil(1993, 1, 1, 0, 0);
	mClockHost = time(nullptr);

	const char* ctl = getenv("EINSTEIN_NEWSCARD_CTL");
	mCtlDir = ctl ? ctl : dir + "/ctl";

	KUInt16 total, unread, freeBlocks;
	CountMessages(total, unread, freeBlocks);
	InitLastSeq();
	mRAM[kStat] = unread ? 0x05 : 0x02;
	mRAM[kOwn] = 0x55;
	fprintf(stderr, "NEWSCARD: emulated card, %d messages (%d unread), %d free blocks, control dir %s\n",
		total, unread, freeBlocks, mCtlDir.c_str());
	// Only the settings: pages are taken when the Newton uses the card,
	// after a saved state has been restored.
	ReadControlSettings();
}

TNewsCard::~TNewsCard(void)
{
}

// ---- store helpers ----

// RAM accessors; offsets outside the RAM read as all ones and are not
// written.
KUInt32
TNewsCard::Get32(KUInt32 inOff)
{
	if (inOff > kRAMSize - 4)
		return 0xFFFFFFFF;
	return (mRAM[inOff] << 24) | (mRAM[inOff + 1] << 16) | (mRAM[inOff + 2] << 8) | mRAM[inOff + 3];
}

void
TNewsCard::Put32(KUInt32 inOff, KUInt32 inValue)
{
	if (inOff > kRAMSize - 4)
		return;
	mRAM[inOff] = inValue >> 24;
	mRAM[inOff + 1] = inValue >> 16;
	mRAM[inOff + 2] = inValue >> 8;
	mRAM[inOff + 3] = inValue;
}

KUInt16
TNewsCard::Get16(KUInt32 inOff)
{
	if (inOff > kRAMSize - 2)
		return 0xFFFF;
	return (mRAM[inOff] << 8) | mRAM[inOff + 1];
}

void
TNewsCard::Put16(KUInt32 inOff, KUInt16 inValue)
{
	if (inOff > kRAMSize - 2)
		return;
	mRAM[inOff] = inValue >> 8;
	mRAM[inOff + 1] = inValue;
}

// All records (bucket order) and every block they use. Walks are bounded.
void
TNewsCard::Walk(std::vector<KUInt32>& outRecords, std::set<KUInt32>& outUsed,
	std::map<KUInt32, KUInt32>* outLinkedFrom)
{
	outRecords.clear();
	outUsed.clear();
	if (outLinkedFrom)
		outLinkedFrom->clear();
	for (int k = 0; k < 16; k++)
	{
		KUInt32 from = kDir + k * 0x80; // the slot or record +0 that holds p
		KUInt32 p = Get32(from);
		int guard = 0;
		while (p != 0xFFFFFFFF && guard++ < 2000)
		{
			KUInt32 rec = p & 0x3FFC0;
			if (!InStore(rec) || outUsed.count(rec))
				break;
			outRecords.push_back(rec);
			outUsed.insert(rec);
			if (outLinkedFrom)
				(*outLinkedFrom)[rec] = from;
			KUInt16 n = Get16(rec + 40);
			KUInt32 b = Get32(rec + 20);
			for (int i = 0; i < n && i < 2000 && b != 0xFFFFFFFF; i++)
			{
				KUInt32 blk = b & 0x3FFC0;
				if (!InStore(blk))
					break;
				outUsed.insert(blk);
				b = Get32(blk);
			}
			from = rec;
			p = Get32(rec);
		}
	}
}

void
TNewsCard::CountMessages(KUInt16& outTotal, KUInt16& outUnread, KUInt16& outFree)
{
	std::vector<KUInt32> recs;
	std::set<KUInt32> used;
	Walk(recs, used);
	outTotal = (KUInt16) recs.size();
	outUnread = 0;
	unsigned reclaimable = 0;
	for (KUInt32 r : recs)
	{
		if (mRAM[r + 38] & 4)
			reclaimable += 1 + Get16(r + 40);
		else
			outUnread++;
	}
	unsigned blocks = (kRAMSize - kHeap) / 64;
	outFree = (KUInt16) (blocks - used.size() + reclaimable);
}

bool
TNewsCard::Unlink(KUInt32 inRecord)
{
	std::vector<KUInt32> recs;
	std::set<KUInt32> used;
	std::map<KUInt32, KUInt32> linkedFrom;
	Walk(recs, used, &linkedFrom);
	auto it = linkedFrom.find(inRecord);
	if (it == linkedFrom.end())
		return false;
	// bucket list: +0 next (older), +4 previous (newer or directory slot).
	// The record is unlinked where the walk found it, so it is gone from
	// the list whatever its own pointers say.
	KUInt32 prev = it->second;
	KUInt32 next = Get32(inRecord);
	if (!InStore(next & 0x3FFC0) || (next & 0x3FFC0) == inRecord || used.count(next & 0x3FFC0) == 0)
		next = 0xFFFFFFFF;
	Put32(prev, next); // the directory slot or the newer record's +0
	if (next != 0xFFFFFFFF)
		Put32((next & 0x3FFC0) + 4, prev);
	// ring over all records: +12 next, +16 previous
	KUInt32 rn = Get32(inRecord + 12) & 0x3FFC0;
	KUInt32 rp = Get32(inRecord + 16) & 0x3FFC0;
	if (InStore(rn) && InStore(rp) && rn != inRecord)
	{
		Put32(rp + 12, rn);
		Put32(rn + 16, rp);
	}
	mRecordSetDirty = true;
	return true;
}

// Page text: printable ASCII as is, any POCSAG
// character (7-bit, 0x00..0x7F) as <0xNN>, control characters also by name
// (<CR>, <LF>, <NUL>, ... <US>, <DEL>). Anything else in <> stays literal.
// Decoding stops after kMaxPageChars + 1 characters.
std::string
TNewsCard::DecodePageText(const std::string& inText)
{
	static const char* const kNames[33] = { "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF",
		"VT", "FF", "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC",
		"FS", "GS", "RS", "US", "DEL" };
	std::string out;
	size_t i = 0;
	while (i < inText.size() && out.size() <= kMaxPageChars)
	{
		size_t end = std::string::npos;
		if (inText[i] == '<')
		{
			for (size_t j = i + 1; j < inText.size() && j <= i + 6; j++)
				if (inText[j] == '>')
				{
					end = j;
					break;
				}
		}
		if (end != std::string::npos)
		{
			std::string tok = inText.substr(i + 1, end - i - 1);
			int code = -1;
			if (tok.size() == 4 && (tok[0] == '0') && (tok[1] == 'x' || tok[1] == 'X') && isxdigit((unsigned char) tok[2])
				&& isxdigit((unsigned char) tok[3]))
			{
				code = (int) strtol(tok.c_str() + 2, nullptr, 16);
				if (code > 0x7F)
					code = -1; // POCSAG alphanumeric characters are 7 bits
			} else
			{
				for (int k = 0; k < 33; k++)
					if (EqualNoCase(tok, std::string(kNames[k])))
						code = (k == 32) ? 0x7F : k;
			}
			if (code >= 0)
			{
				out += (char) code;
				i = end + 1;
				continue;
			}
		}
		out += inText[i++];
	}
	return out;
}

// Is record inA newer than record inB? By timestamp (years 93..99 before
// 00..92), then by sequence number, which wraps from 255 to 0.
bool
TNewsCard::IsNewer(KUInt32 inA, KUInt32 inB)
{
	for (int i = 0; i < 5; i++)
	{
		int a = mRAM[inA + 45 + i];
		int b = mRAM[inB + 45 + i];
		if (i == 0)
		{
			a = (a + 7) % 100;
			b = (b + 7) % 100;
		}
		if (a != b)
			return a > b;
	}
	KUInt8 d = (KUInt8) (mRAM[inA + 9] - mRAM[inB + 9]);
	return d != 0 && d < 0x80;
}

// Delete the oldest message that has been read. Returns false if there is
// none.
bool
TNewsCard::ReclaimReadMessage(void)
{
	std::vector<KUInt32> recs;
	std::set<KUInt32> used;
	Walk(recs, used);
	KUInt32 oldest = 0;
	for (KUInt32 r : recs)
		if ((mRAM[r + 38] & 4) && (!oldest || IsNewer(oldest, r)))
			oldest = r;
	return oldest && Unlink(oldest);
}

// The last sequence number given: the newest record's.
void
TNewsCard::InitLastSeq(void)
{
	std::vector<KUInt32> recs;
	std::set<KUInt32> used;
	Walk(recs, used);
	KUInt32 newest = 0;
	for (KUInt32 r : recs)
		if (!newest || IsNewer(r, newest))
			newest = r;
	mLastSeq = newest ? mRAM[newest + 9] : 0;
}

// Room for inBlocks blocks, counting the blocks of read messages.
bool
TNewsCard::HasRoom(size_t inBlocks)
{
	KUInt16 total, unread, freeBlocks;
	CountMessages(total, unread, freeBlocks);
	return freeBlocks >= inBlocks;
}

// Store a page in the card's layout. Read messages are deleted, oldest
// first, when there is no other room. Returns false if memory is full.
bool
TNewsCard::AddPage(int inSlot, int inFunc, const std::string& inText, KUInt32& outRecord)
{
	size_t len = inText.size();
	size_t n = (len + 53) / 54;
	if (n == 0)
		n = 1;
	std::vector<KUInt32> recs;
	std::set<KUInt32> used;
	std::vector<KUInt32> blocks;
	for (int tries = 0;; tries++)
	{
		Walk(recs, used);
		blocks.clear();
		for (KUInt32 b = kHeap; b < kRAMSize && blocks.size() < n + 1; b += 64)
			if (!used.count(b))
				blocks.push_back(b);
		if (blocks.size() >= n + 1)
			break;
		if (tries >= 2048 || !ReclaimReadMessage())
			return false;
	}
	KUInt32 rec = blocks[0];
	KUInt32 newest = 0;
	for (KUInt32 r : recs)
		if (!newest || IsNewer(r, newest))
			newest = r;
	// The next sequence number after the last one given, so a page never gets
	// the number of a message that was just deleted.
	KUInt8 seq = (KUInt8) (mLastSeq + 1);
	mLastSeq = seq;

	// text blocks: circular ring
	for (size_t i = 0; i < n; i++)
	{
		KUInt32 b = blocks[1 + i];
		memset(&mRAM[b], 0, 64);
		Put32(b + 0, blocks[1 + (i + 1) % n]);
		Put32(b + 4, blocks[1 + (i + n - 1) % n]);
		size_t chunk = (len > i * 54) ? len - i * 54 : 0;
		if (chunk > 54)
			chunk = 54;
		if (chunk)
			memcpy(&mRAM[b + 10], inText.data() + i * 54, chunk);
	}
	KUInt32 last = blocks[n];
	KUInt32 end = last + 10 + (KUInt32) (len - (n - 1) * 54) - 1;

	memset(&mRAM[rec], 0, 64);
	KUInt8 bucket = (KUInt8) (inSlot * 8 + inFunc);
	KUInt32 slot = kDir + bucket * 0x80;
	KUInt32 head = Get32(slot);
	if (head != 0xFFFFFFFF && !InStore(head & 0x3FFC0))
		head = 0xFFFFFFFF;
	Put32(rec + 0, head);
	Put32(rec + 4, slot);
	if (head != 0xFFFFFFFF)
		Put32((head & 0x3FFC0) + 4, rec);
	Put32(slot, rec);
	mRAM[rec + 9] = (KUInt8) seq;
	Put16(rec + 10, 0);
	// ring: after the newest record
	if (newest)
	{
		KUInt32 after = Get32(newest + 12) & 0x3FFC0;
		if (!InStore(after))
			after = newest;
		Put32(rec + 12, after);
		Put32(rec + 16, newest);
		Put32(newest + 12, rec);
		Put32(after + 16, rec);
	} else
	{
		Put32(rec + 12, rec);
		Put32(rec + 16, rec);
	}
	Put32(rec + 20, blocks[1]);
	Put32(rec + 24, end);
	mRAM[rec + 36] = bucket;
	mRAM[rec + 38] = (n > 5) ? 0x08 : 0x00; // 0x08 on pages over 5 blocks
	mRAM[rec + 39] = (n > 5) ? 0x10 : 0x08;
	Put16(rec + 40, (KUInt16) n);
	mRAM[rec + 44] = 0x80;
	GetClock(&mRAM[rec + 45]);
	mRAM[kStat] |= 0x05; // bit 0: event pending
	mRecordSetDirty = true;
	outRecord = rec;
	return true;
}

// ---- clock: two-digit year, starts at 93-01-01 00:00, ticks with the host ----

void
TNewsCard::GetClock(KUInt8 outTime[5])
{
	time_t now = mClockBase + (time(nullptr) - mClockHost);
	int y, mo, d, h, mi;
	CivilFromSeconds(now, y, mo, d, h, mi);
	outTime[0] = (KUInt8) (y % 100);
	outTime[1] = (KUInt8) mo;
	outTime[2] = (KUInt8) d;
	outTime[3] = (KUInt8) h;
	outTime[4] = (KUInt8) mi;
}

bool
TNewsCard::SetClock(const KUInt8 inTime[5])
{
	KUInt8 yy = inTime[0];
	if (yy > 99) // NAK code 10
	{
		if (mYearMode == kYearStrict)
			return false;
		yy %= 100; // ctl/year = accept
	}
	mClockBase = SecondsFromCivil((yy >= 93) ? 1900 + yy : 2000 + yy, inTime[1], inTime[2], inTime[3], inTime[4]);
	mClockHost = time(nullptr);
	return true;
}

// ---- control directory ----
//   switch   on | silent | off
//   battery  ok | low
//   year     translate (default) | strict | accept, see EYearMode
//   inbox/*.page  first line "slot=0 func=A", the rest is the text
//                 (ASCII, <0xNN> or <CR>/<LF>/... for any
//                 7-bit POCSAG character; at most 2019 characters);
//                 see ProcessInbox()
void
TNewsCard::PollControl(bool inForce)
{
	if (mCtlDir.empty())
		return;
	double now = Now();
	if (!inForce && now - mLastCtlPoll < 1.0)
		return;
	mLastCtlPoll = now;
	ReadControlSettings();
	ProcessInbox();
}

void
TNewsCard::ReadControlSettings(void)
{
	if (mCtlDir.empty())
		return;
	std::string sw = Trim(ReadSetting(mCtlDir + "/switch"));
	KUInt8 newSwitch = mSwitch;
	if (sw == "on")
		newSwitch = 0x04;
	else if (sw == "silent")
		newSwitch = 0x02;
	else if (sw == "off")
		newSwitch = 0x01;
	if (newSwitch != mSwitch)
	{
		fprintf(stderr, "NEWSCARD: switch %s\n", sw.c_str());
		mSwitch = newSwitch;
	}
	std::string bat = Trim(ReadSetting(mCtlDir + "/battery"));
	bool newBat = mBatteryOK;
	if (bat == "ok")
		newBat = true;
	else if (bat == "low")
		newBat = false;
	if (newBat != mBatteryOK)
	{
		fprintf(stderr, "NEWSCARD: battery %s\n", bat.c_str());
		mBatteryOK = newBat;
	}

	std::string yr = Trim(ReadSetting(mCtlDir + "/year"));
	EYearMode newYear = mYearMode;
	if (yr == "translate")
		newYear = kYearTranslate;
	else if (yr == "strict")
		newYear = kYearStrict;
	else if (yr == "accept")
		newYear = kYearAccept;
	if (newYear != mYearMode)
	{
		fprintf(stderr, "NEWSCARD: year mode %s\n", yr.c_str());
		mYearMode = newYear;
	}
}

// Store the pages in <ctl>/inbox.
//   x.page         new; claimed by renaming it to x.page.work, then read
//   x.page.work    claimed; stored, or kept for the next poll
//   x.page.done    stored
//   x.page.rejected  no valid header, or too long
// Storing is one step: the page is added to the RAM, then x.page.work is
// renamed to x.page.done. If either fails, the RAM is put back as it was and
// x.page.work stays, so a page is in the RAM exactly when its file is .done.
// A new x.page waits while x.page.work exists. Nothing is received while
// the switch is off.
void
TNewsCard::ProcessInbox(void)
{
	if (mCtlDir.empty() || mSwitch == 0x01)
		return;
	std::string inbox = mCtlDir + "/inbox";
	std::vector<std::string> names = ListPages(inbox);
	std::set<std::string> present(names.begin(), names.end());
	for (const std::string& name : names)
	{
		std::string work = inbox + "/" + name;
		if (EndsWith(name, ".page"))
		{
			if (present.count(name + ".work"))
				continue;
			if (!RenameReplacing(inbox + "/" + name, work + ".work"))
				continue;
			work += ".work";
		}
		std::string base = work.substr(0, work.size() - 5); // without .work
		std::string label = base.substr(inbox.size() + 1); // x.page
		std::string content;
		if (!ReadSmallFile(work, content, kMaxPageFileBytes))
		{
			if (content.size() > kMaxPageFileBytes)
			{
				fprintf(stderr, "NEWSCARD: %s is too long, rejected\n", label.c_str());
				RenameReplacing(work, base + ".rejected");
			}
			continue; // unreadable now: try again later
		}
		size_t nl = content.find('\n');
		std::string header = content.substr(0, nl);
		std::string text = (nl == std::string::npos) ? std::string() : content.substr(nl + 1);
		while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
			text.pop_back();
		int slot = 0, func = 0;
		if (!ParsePageHeader(header, slot, func))
		{
			fprintf(stderr, "NEWSCARD: %s has no valid \"slot=N func=X\" line, rejected\n", label.c_str());
			RenameReplacing(work, base + ".rejected");
			continue;
		}
		text = DecodePageText(text);
		if (text.size() > kMaxPageChars)
		{
			fprintf(stderr, "NEWSCARD: %s has more than %d characters, rejected\n", label.c_str(),
				(int) kMaxPageChars);
			RenameReplacing(work, base + ".rejected");
			continue;
		}
		size_t blocks = (text.size() + 53) / 54;
		if (!HasRoom((blocks ? blocks : 1) + 1))
		{
			if (mReportedFull.insert(label).second)
				fprintf(stderr, "NEWSCARD: card full, %s waits\n", label.c_str());
			continue;
		}
		std::vector<KUInt8> savedRAM(mRAM);
		KUInt8 savedSeq = mLastSeq;
		KUInt32 rec = 0;
		bool added = AddPage(slot, func, text, rec);
		if (!added || !RenameReplacing(work, base + ".done"))
		{
			mRAM.swap(savedRAM);
			mLastSeq = savedSeq;
			mRecordSetDirty = true;
			if (mReportedFull.insert(label).second)
				fprintf(stderr, "NEWSCARD: %s waits (%s)\n", label.c_str(), added ? "can't rename it" : "card full");
			continue;
		}
		mReportedFull.erase(label);
		fprintf(stderr, "NEWSCARD: %s stored: seq %d slot %d func %c, %zu chars, record 0x%05X\n", label.c_str(),
			mLastSeq, slot, 'A' + func, text.size(), (unsigned) rec);
	}
}

// ---- mailbox ----

void
TNewsCard::Reply(KUInt8 inCmd, const std::vector<KUInt8>& inData, KUInt8 inStatus)
{
	KUInt16 len = (KUInt16) (inData.size() + 1);
	mRAM[kRsp + 0] = inCmd;
	mRAM[kRsp + 1] = len >> 8;
	mRAM[kRsp + 2] = len;
	mRAM[kRsp + 3] = inStatus;
	for (size_t i = 0; i < inData.size(); i++)
		mRAM[kRsp + 4 + i] = inData[i];
	mRAM[kRsp + 4 + inData.size()] = 0xFF;
	mRAM[kRsp + 5 + inData.size()] = 0xAA;
	fprintf(stderr, "NEWSCARD:   reply %02X status %02X len %d:", inCmd, inStatus, len);
	for (size_t i = 0; i < inData.size(); i++)
		fprintf(stderr, " %02X", inData[i]);
	fprintf(stderr, "\n");
}

// Card -> Newton: two-digit years 0..92 become 100..192 (2000..2092).
void
TNewsCard::TranslateReplyYears(KUInt8 inCmd)
{
	if (mYearMode != kYearTranslate || mRAM[kRsp] != inCmd || mRAM[kRsp + 3] != 0x06)
		return;
	if (inCmd == 0x32)
		mRAM[kRsp + 4] = YearToNewton(mRAM[kRsp + 4]); // YY MM DD hh mm
	else if (inCmd == 0x83)
	{
		// status block: three timestamps at +2, +7, +12 (the last is the clock)
		for (int o : { 2, 7, 12 })
			mRAM[kRsp + 4 + o] = YearToNewton(mRAM[kRsp + 4 + o]);
	}
}

// Is inOffset the start of a message record? (cached; the Newton reads the
// directory before it reads records, which refreshes the cache)
bool
TNewsCard::IsRecord(KUInt32 inOffset)
{
	if (mRecordSetDirty)
	{
		std::vector<KUInt32> recs;
		std::set<KUInt32> used;
		Walk(recs, used);
		mRecordSet = std::set<KUInt32>(recs.begin(), recs.end());
		mRecordSetDirty = false;
	}
	return mRecordSet.count(inOffset) != 0;
}

void
TNewsCard::EmulateCommand(KUInt8 cmd, const KUInt8* p, KUInt16 len)
{
	// Payload bytes each command needs; shorter requests get a NAK.
	KUInt16 need = 0;
	switch (cmd)
	{
		case 0x01:
		case 0x02:
		case 0x31:
			need = 5;
			break;
		case 0x11:
			need = 2;
			break;
		case 0x12:
			need = 4;
			break;
		case 0x41:
			need = 1;
			break;
	}
	if (len < need)
	{
		Reply(cmd, { 0x01 }, kNAK);
		return;
	}
	KUInt16 total, unread, freeBlocks;
	switch (cmd)
	{
		case 0x01: // mark read: ptr32, seq. Reply echoes ptr and seq.
		{
			KUInt32 rec = ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]) & 0x3FFC0;
			std::vector<KUInt32> recs;
			std::set<KUInt32> used;
			Walk(recs, used);
			bool ok = false;
			for (KUInt32 r : recs)
				ok |= (r == rec);
			ok = ok && mRAM[rec + 9] == p[4]; // the same message, not a new one there
			if (ok)
			{
				// +38: set bits 0x14, clear 0x40; +39 = 0x01
				mRAM[rec + 38] = (mRAM[rec + 38] | 0x14) & ~0x40;
				mRAM[rec + 39] = 0x01;
				Reply(cmd, { p[0], p[1], p[2], p[3], p[4] });
			} else
				Reply(cmd, { 0x01 }, kNAK);
			break;
		}
		case 0x02: // delete: ptr32, seq, 0, 0, 0. Reply echoes ptr and seq.
		{
			KUInt32 rec = ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]) & 0x3FFC0;
			// Only the message with this sequence number: a new page may have
			// taken the address of a deleted one.
			bool ok = InStore(rec) && mRAM[rec + 9] == p[4] && Unlink(rec);
			fprintf(stderr, "NEWSCARD:   delete record 0x%05X: %s\n", (unsigned) rec, ok ? "ok" : "not found");
			if (ok)
				Reply(cmd, { p[0], p[1], p[2], p[3], p[4] });
			else
				Reply(cmd, { 0x01 }, kNAK);
			break;
		}
		case 0x11: // read config byte: addr16 -> addrHi addrLo value
		{
			KUInt16 addr = (p[0] << 8) | p[1];
			if (addr < 0x100 && mConfigReadable[addr])
				Reply(cmd, { p[0], p[1], mConfig[addr] });
			else
				Reply(cmd, { 0x08, p[0], p[1] }, kNAK);
			break;
		}
		case 0x12: // write config byte: addr16, value, mask
		{
			KUInt16 addr = (p[0] << 8) | p[1];
			if (addr < 0x100 && mConfigReadable[addr])
			{
				KUInt8& c = mConfig[addr];
				c = (c & ~p[3]) | (p[2] & p[3]);
				fprintf(stderr, "NEWSCARD:   config[%02X] = %02X\n", addr, c);
				Reply(cmd, { p[0], p[1] }); // echoes the address
			} else
				Reply(cmd, { 0x08, p[0], p[1] }, kNAK);
			break;
		}
		case 0x13: // notification, no reply
			break;
		case 0x31: // set time YY MM DD hh mm
			if (SetClock(p))
			{
				fprintf(stderr, "NEWSCARD:   clock set to %02d-%02d-%02d %02d:%02d\n", p[0], p[1], p[2], p[3], p[4]);
				Reply(cmd, {});
			} else
				Reply(cmd, { 0x0A }, kNAK);
			break;
		case 0x32: // get time
		{
			KUInt8 t[5];
			GetClock(t);
			Reply(cmd, { t[0], t[1], t[2], t[3], t[4] });
			break;
		}
		case 0x41: // alert select
			Reply(cmd, {});
			break;
		case 0x44: // battery: nonzero = good
			Reply(cmd, { (KUInt8) (mBatteryOK ? 1 : 0) });
			break;
		case 0x81:
			CountMessages(total, unread, freeBlocks);
			Reply(cmd, { (KUInt8) (unread >> 8), (KUInt8) unread });
			break;
		case 0x82:
			CountMessages(total, unread, freeBlocks);
			Reply(cmd, { (KUInt8) (total >> 8), (KUInt8) total });
			break;
		case 0x83: // status block
		{
			CountMessages(total, unread, freeBlocks);
			std::vector<KUInt8> s(20, 0);
			s[1] = mSwitch;
			const KUInt8 stamp[5] = { 0x5D, 0x01, 0x01, 0x00, 0x00 };
			memcpy(&s[2], stamp, 5);
			memcpy(&s[7], stamp, 5);
			GetClock(&s[12]);
			s[17] = 0x02;
			s[18] = freeBlocks >> 8;
			s[19] = freeBlocks;
			Reply(cmd, s);
			break;
		}
		default:
			Reply(cmd, { 0x01 }, kNAK);
			break;
	}
}

void
TNewsCard::ProcessCommand(void)
{
	KUInt8 cmd = mRAM[kCmd];
	KUInt16 len = (mRAM[kCmd + 1] << 8) | mRAM[kCmd + 2];
	if (len > 256)
		len = 256;
	KUInt8 payload[256 + 8];
	memset(payload, 0, sizeof(payload));
	memcpy(payload, &mRAM[kCmd + 3], len);
	fprintf(stderr, "NEWSCARD: >>> command %02X len %d:", cmd, len);
	for (int i = 0; i < len && i < 32; i++)
		fprintf(stderr, " %02X", payload[i]);
	fprintf(stderr, "\n");

	ReadControlSettings();
	// The Newton sends year - 1900 (126 in 2026); the card takes 0..99.
	if (cmd == 0x31 && len >= 1 && mYearMode == kYearTranslate && payload[0] > 99)
	{
		fprintf(stderr, "NEWSCARD:   year byte %d -> %d for the card\n", payload[0], payload[0] - 100);
		payload[0] -= 100;
	}
	EmulateCommand(cmd, payload, len);
	TranslateReplyYears(cmd);
	mRecordSetDirty = true;
	// New pages after the command, so records the command names are the
	// ones the Newton read.
	ProcessInbox();
	// The card has taken the request.
	mRAM[kOwn] = 0x55;
}

// ---- state (fast start, checkpoints) ----

void
TNewsCard::TransferState(TStream* inStream)
{
	// A reset keeps everything: the card is battery-backed.
	if (inStream->IsResetting())
		return;
	// Everything is transferred through copies; a read replaces the card's
	// state only when all of it was read.
	std::vector<KUInt8> ram(mRAM);
	KUInt8 config[sizeof(mConfig)];
	memcpy(config, mConfig, sizeof(config));
	KUInt32 theSize = kRAMSize;
	inStream->TransferInt32BE(theSize);
	if (inStream->IsReading() && theSize != kRAMSize)
		throw std::runtime_error("the NewsCard RAM size does not match");
	inStream->CheckDataSize(theSize, kRAMSize, "the NewsCard RAM");
	inStream->Transfer(ram.data(), &theSize);
	KUInt32 theConfigSize = sizeof(config);
	inStream->Transfer(config, &theConfigSize);
	if (inStream->IsReading() && (theSize != kRAMSize || theConfigSize != sizeof(config)))
		throw std::runtime_error("the NewsCard state is incomplete");
	// The clock is saved as an offset to the host clock, so it keeps running
	// while Einstein is not.
	KSInt32 theOffset = (KSInt32) (mClockBase - mClockHost);
	inStream->TransferInt32BE(theOffset);
	KUInt8 theSwitch = mSwitch;
	inStream->TransferByte(theSwitch);
	bool theBatteryOK = mBatteryOK;
	inStream->TransferBoolean(theBatteryOK);
	KUInt8 theYearMode = (KUInt8) mYearMode;
	inStream->TransferByte(theYearMode);
	KUInt8 theLastSeq = mLastSeq;
	inStream->TransferByte(theLastSeq);
	if (inStream->IsReading())
	{
		mRAM.swap(ram);
		memcpy(mConfig, config, sizeof(mConfig));
		mClockHost = time(nullptr);
		mClockBase = mClockHost + theOffset;
		mSwitch = theSwitch;
		mBatteryOK = theBatteryOK;
		mYearMode = (theYearMode <= kYearAccept) ? (EYearMode) theYearMode : kYearTranslate;
		mLastSeq = theLastSeq;
		mRecordSetDirty = true;
		KUInt16 total, unread, freeBlocks;
		CountMessages(total, unread, freeBlocks);
		fprintf(stderr, "NEWSCARD: state restored, %d messages (%d unread), %d free blocks\n", total, unread,
			freeBlocks);
	}
}

// ---- memory access ----

KUInt8
TNewsCard::CardRead(KUInt32 inCardOffset, bool inLog)
{
	KUInt8 v;
	if (inCardOffset == kStat)
		PollControl(false);
	if (inCardOffset < kRAMSize)
	{
		v = mRAM[inCardOffset];
		if (inCardOffset >= kDir && inCardOffset < kHeap)
			mRecordSetDirty = true; // a new scan of the store begins
		else if (mYearMode == kYearTranslate && inCardOffset >= kHeap && ((inCardOffset - kHeap) & 63) == 45
			&& IsRecord(inCardOffset - 45))
			v = YearToNewton(v); // record timestamp YY
	} else if (inCardOffset >= kFlashBase && inCardOffset < kFlashBase + kFlashSize)
		v = mFlash[inCardOffset - kFlashBase];
	else if (inCardOffset >= 0x03000000 && inCardOffset < 0x03000000 + kRAMSize)
		v = mRAM[inCardOffset - 0x03000000]; // RAM mirror
	else
		v = 0xFF;
	if (mTrace && inLog && inCardOffset >= kCmd && inCardOffset < kRAMSize && inCardOffset != kSem
		&& inCardOffset != kOwn)
		fprintf(stderr, "NEWSCARD: rd %05X = %02X\n", (unsigned) inCardOffset, v);
	return v;
}

bool
TNewsCard::CardWrite(KUInt32 inCardOffset, KUInt8 inValue)
{
	if (inCardOffset >= kRAMSize)
	{
		fprintf(stderr, "NEWSCARD: write outside RAM %08X = %02X (ignored)\n", (unsigned) inCardOffset, inValue);
		return false;
	}
	if (mTrace && inCardOffset != kSem && inCardOffset != kOwn)
		fprintf(stderr, "NEWSCARD: wr %05X = %02X\n", (unsigned) inCardOffset, inValue);
	mRAM[inCardOffset] = inValue;
	if (inCardOffset >= kDir)
		mRecordSetDirty = true; // the store may have changed
	return inCardOffset == kDoorbell && inValue == 0xFF;
}

KUInt32
TNewsCard::GetVPCPins(void)
{
	return TPCMCIAController::k1C00_CardIsPresent | TPCMCIAController::k1C00_Pin16_37
		| TPCMCIAController::k1C00_Pin62_45 | TPCMCIAController::k1C00_Pin63_46;
}

void
TNewsCard::SetVPCPins(KUInt32 inPins)
{
	if (mTrace)
		fprintf(stderr, "NEWSCARD: SetVPCPins %04X\n", (unsigned) inPins);
}

// The card ignores REG#: attribute memory is the RAM (or attr.bin).
KUInt8
TNewsCard::ReadAttrB(KUInt32 inOffset)
{
	KUInt32 o = NewtonToCard(inOffset);
	if (mAttrFromFile)
		return (o < mAttr.size()) ? mAttr[o] : 0xFF;
	return (o < 0x1000) ? mRAM[o] : 0xFF;
}

// The flash and the card folder: a saved state (the card's RAM) belongs to
// its folder, and is not restored into another one.
KUInt32
TNewsCard::GetContentsChecksum(void)
{
	if (!mFlashLoaded)
		return 0;
	std::vector<KUInt8> identity(mFlash);
	identity.insert(identity.end(), mDir.begin(), mDir.end());
	return GetCRC32(identity.data(), (KUInt32) identity.size());
}

KUInt32
TNewsCard::ReadAttr(KUInt32 inOffset)
{
	return (ReadAttrB(inOffset) << 24) | (ReadAttrB(inOffset + 1) << 16) | (ReadAttrB(inOffset + 2) << 8)
		| ReadAttrB(inOffset + 3);
}

void
TNewsCard::WriteAttrB(KUInt32 inOffset, KUInt8 inValue)
{
	fprintf(stderr, "NEWSCARD: attr wr %04X = %02X (ignored)\n", (unsigned) NewtonToCard(inOffset), inValue);
}

void
TNewsCard::WriteAttr(KUInt32 inOffset, KUInt32 inValue)
{
	fprintf(stderr, "NEWSCARD: attr wr32 %04X = %08X (ignored)\n", (unsigned) inOffset, (unsigned) inValue);
}

KUInt32
TNewsCard::ReadIO(KUInt32 inOffset)
{
	(void) inOffset;
	return 0;
}

KUInt8
TNewsCard::ReadIOB(KUInt32 inOffset)
{
	(void) inOffset;
	return 0;
}

void
TNewsCard::WriteIO(KUInt32 inOffset, KUInt32 inValue)
{
	(void) inOffset;
	(void) inValue;
}

void
TNewsCard::WriteIOB(KUInt32 inOffset, KUInt8 inValue)
{
	(void) inOffset;
	(void) inValue;
}

KUInt8
TNewsCard::ReadMemB(KUInt32 inOffset)
{
	return CardRead(NewtonToCard(inOffset), true);
}

KUInt32
TNewsCard::ReadMem(KUInt32 inOffset)
{
	// A 32-bit read of Newton address A gets card bytes A^3 .. A^3-3, which
	// is the same as four byte reads of A .. A+3.
	return (CardRead(NewtonToCard(inOffset), false) << 24) | (CardRead(NewtonToCard(inOffset + 1), false) << 16)
		| (CardRead(NewtonToCard(inOffset + 2), false) << 8) | CardRead(NewtonToCard(inOffset + 3), false);
}

void
TNewsCard::WriteMemB(KUInt32 inOffset, KUInt8 inValue)
{
	if (CardWrite(NewtonToCard(inOffset), inValue))
		ProcessCommand();
}

// The doorbell rings after all four bytes are stored, so the command sees
// the whole write and its OWN result is not overwritten.
void
TNewsCard::WriteMem(KUInt32 inOffset, KUInt32 inValue)
{
	bool ring = CardWrite(NewtonToCard(inOffset), inValue >> 24);
	ring |= CardWrite(NewtonToCard(inOffset + 1), inValue >> 16);
	ring |= CardWrite(NewtonToCard(inOffset + 2), inValue >> 8);
	ring |= CardWrite(NewtonToCard(inOffset + 3), inValue);
	if (ring)
		ProcessCommand();
}
