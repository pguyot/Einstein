// ==============================
// File:			TROMImage.h
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

#ifndef _TROMIMAGE_H
#define _TROMIMAGE_H

#include <K/Defines/KDefinitions.h>

// POSIX & ANSI C
#if TARGET_OS_WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

// Einstein
#include "Emulator/TMemoryConsts.h"
#include "Emulator/JIT/JIT.h"

class TMappedFile;

///
/// Base class for the ROM Image.
///
class TROMImage
{
public:
	///
	/// Default constructor.
	///
	TROMImage(void);

	///
	/// Destructor.
	///
	~TROMImage(void);

	///
	/// Accessor to the ROM Pointer.
	///
	/// \return a pointer to the ROM data.
	///
	KUInt8*
	GetPointer(void) const
	{
		return mImage->fROM;
	}

	///
	/// Return the error code that may have been set when loading the ROM
	///
	KUInt32
	GetErrorCode()
	{
		return mErrorCode;
	}

	static const KUInt32 kNoError = 0;
	static const KUInt32 kErrorLoadingROMFile = 1;
	static const KUInt32 kErrorLoadingNewtonREXFile = 2;
	static const KUInt32 kErrorLoadingEinsteinREXFile = 3;
	static const KUInt32 kErrorWrongSize = 4;

	///
	/// Compute the checksums.
	///
	void ComputeChecksums(KUInt32 outChecksums[10]) const;

	///
	/// Return the ID of the ROM as calculated by a CRC32 checksum
	///
	KSInt32
	GetROMId()
	{
		return mROMId;
	}

	///
	/// Return true if the CRC32 for the lower 8MB of ROM is valid for the ROM ID.
	///
	bool
	CRCValid()
	{
		return mCRCValid;
	}

	static const KSInt32 kUnknownROM = -1;
	static const KSInt32 k717006 = 0;
	static const KSInt32 kMP2x00DROM = 1;
	static const KSInt32 kEMate300ROM = 2;
	static const KSInt32 kWatsonROM = 3;

	static TROMImage* LoadROMAndREX(const char* theROMImagePath, Boolean useMonitor, Boolean useExternalERex);

protected:
	///
	/// Create the image, flip the endian, and apply patches if available
	///
	void CreateImage(const KUInt8* inBuffer);

	///
	/// Check the modification date of a file.
	/// Update the timespec to reflect the latest modification date.
	///
	static int GetLatestModDate(
		time_t* ioModDate,
		int fd);

	///
	/// Find out what ROM we have by calculating its CRC
	///
	static KSInt32 ComputeROMId(KUInt8* data, bool& outCRCValid);

	KUInt32 mErrorCode = kNoError;

private:
	///
	/// Structure of the image.
	///
	struct SImageInfo {
		KUInt32 fChecksums[10];
	};

	struct SImage {
		KUInt8 fROM[TMemoryConsts::kHighROMEnd]; // 8 MB ROM + 8 MB REX = 16 MB
		SImageInfo fInfo;
	};

	enum {
		kMagic = 0x424C5447,
		kVersion = 2,
	};

	///
	/// Compute the checksums.
	///
	static void DoComputeChecksums(SImage* inImage);

	///
	/// Compute the checksum for a segment.
	///
	static void ComputeSegmentChecksums(
		const KUInt8* inPointer,
		KUInt32 inSize,
		KUInt32 outChecksums[2]);

	///
	/// Analyze the first 8 MB of a flat rom and fill the
	/// ROM Base Size, and Rex Bases and Sizes.
	///
	/// \param inBuffer		8 MB buffer
	/// \param outBaseSize	the base of the ROM
	/// \param outRexBases	the bases of the RExes
	/// \param outRexSizes	the sizes of the RExes
	/// \return the number of rexes found
	///
	static KUInt32 LookForREXes(
		const KUInt8* inBuffer,
		KUInt32* outBaseSize,
		KUInt32 outRexBases[4],
		KUInt32 outRexSizes[4]);

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TROMImage(const TROMImage& inCopy) = delete;

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TROMImage& operator=(const TROMImage& inCopy) = delete;

	SImage* mImage; ///< image structure.

	KSInt32 mROMId = kUnknownROM;

	bool mCRCValid = true;
};

#endif
// _TROMIMAGE_H

// =================================================================== //
// By long-standing tradition, I take this opportunity to savage other //
// designers in the thin disguise of good, clean fun.                  //
//                 -- P.J. Plauger, "Computer Language", 1988, April   //
//                    Fool's column.                                   //
// =================================================================== //
