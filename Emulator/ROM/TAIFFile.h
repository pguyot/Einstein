// ==============================
// File:			TAIFFile.cp
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

#ifndef __TAIFFILE__
#define __TAIFFILE__

#include <K/Defines/KDefinitions.h>

#include <stdio.h>

///
/// Class for to handle ARM Image Format.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 111 $
///
/// \test	aucun test défini.
///
class TAIFFile
{
public:
	///
	/// Constructor from a file.
	///
	/// Reads the header.
	///
	TAIFFile( FILE* inFile );
		
	///
	/// Definition for the 128 bytes header.
	/// From ARM SDT Reference Manual, 13-5 sqq
	///
	struct SHeader
	{
		KUInt32	fNOP_00;			// NOP						All NOPs here are mov r0, r0
		KUInt32	fSelfRelocCodeBL;	// BL SelfRelocCode			NOP if the image is not self-relocating
		KUInt32	fZeroInitBL;		// BL ZeroInit				NOP if the image has none.
		KUInt32	fEntryPoint;		// BL ImageEntryPoint or	BL to make the header addressable via r14 ...but the
									//    EntryPoint Offset		application will not return... Non-executable AIF uses an
									//							offset, not BL.
									//							BL is used to make the header addressable via r14 in a
									//							position-independent manner, and to ensure that the
									//							header will be position-independent.
		KUInt32	fExitInstruction;	// Program Exit Instruction	… last attempt in case of return. The Program Exit
									//							Instruction is usually a SWI causing program
									//							termination. On systems that do not implement a SWI for
									//							this purpose, a branch-to-self is recommended.
									//							Applications are expected to exit directly and not to
									//							return to the AIF header, so this instruction should never
									//							be executed. The ARM linker sets this field to SWI 0x11
									//							by default, but it may be set to any desired value by
									//							providing a template for the AIF header in an area called
									//							AIF_HDR in the first object file in the input list to
									//							armlink.
		KUInt32	fReadOnlySize;		// Image ReadOnly size		Image ReadOnly Size includes the size of the AIF header
									//							only if the AIF type is executable (that is, if the header
									//							itself is part of the image).
		KUInt32	fReadWriteSize;		// Image ReadWrite size		Exact size (a multiple of 4 bytes).
		KUInt32	fDebugSize;			// Image Debug size			Exact size (a multiple of 4 bytes). Includes high-level
									//							and low-level debug size. Bits 0-3 hold the type. Bits
									//							4-31 hold the low level debug size.
		KUInt32	fZeroInitSize;		// Image zero-init size		Exact size (a multiple of 4 bytes).
		KUInt32	fDebugType;			// Image debug type			Valid values for Image debug type are:
									//							0 No debugging data present.
									//							1 Low-level debugging data present.
									//							2 Source level debugging data present.
									//							3 1 and 2 are present together.
									//							All other values of image debug type are reserved.
		KUInt32	fBaseAddress;		// Image base				Address where the image (code) was linked.
		KUInt32	fWorkSpace;			// Work space				Obsolete.
		KUInt32	fAddressMode;		// Address mode: 26/32 +	The word at offset 0x30 is 0, or contains in its least
									//    3 flag bytes			significant byte (using the byte order appropriate to the
									//							target):
									//							26 Indicates that the image was linked for a
									//							26-bit ARM mode, and may not execute
									//							correctly in a 32-bit mode. This is
									//							obsolete.
									//							32 Indicates that the image was linked for a
									//							32-bit ARM mode, and may not execute
									//							correctly in a 26-bit mode.
									//							A value of 0 indicates an old-style 26-bit AIF header.
									//							If the Address mode word has bit 8 set, the image was
									//							linked with separate code and data bases (usually the
									//							data is placed immediately after the code). The word at
									//							offset 0x34 contains the base address of the image’s data.
		KUInt32	fDataBaseAddress;	// Data base				Address where the image data was linked.
		KUInt32	fReserved[2];		// Two reserved words		In Extended AIF images, the word at 0x38 is non-zero. It
									//   (initially 0)			contains the byte offset within the file of the header for
									//							the first non-root load region. This header has a size of
									//							44 bytes, and the following format:
									//							word 0 file offset of header of next region (0 is
									//							none)
									//							word 1 load address
									//							word 2 size in bytes (a multiple of 4)
									//							char[32] the region name padded out with zeros.
									//							The initializing data for the region follows the header.
		KUInt32	fNOP_40;			// NOP
		KUInt32	fZeroInitCode[15];	// Zero-init code 15 words	Header is 32 words long.
									//   as below
	};
	
	///
	/// Accessor to the header.
	///
	/// \return a pointer to the header.
	///
	inline	const SHeader*	GetHeader( void ) const
		{
			return &mHeader;
		}

	///
	/// Accessor to the size of the read/only part of the image.
	///
	/// \return the size of the read/only part of the image.
	///
	inline	KUInt32			GetROImageSize( void ) const
		{
			return mHeader.fReadOnlySize;
		}

	///
	/// Accessor to the base of the read/only part of the image.
	///
	/// \return the base of the read/only part of the image.
	///
	inline	KUInt32			GetROImageBase( void ) const
		{
			return mHeader.fBaseAddress;
		}

	///
	/// Accessor to the size of the read/write part of the image.
	///
	/// \return the size of the read/write part of the image.
	///
	inline	KUInt32			GetRWImageSize( void ) const
		{
			return mHeader.fReadWriteSize;
		}

	///
	/// Read the read/only part of the image.
	/// The size can be obtained with GetROImageSize method.
	/// The buffer must be big enough to be filled with the data.
	///
	void					ReadROImage( KUInt8* outImage );
	
	///
	/// Read the read/write part of the image.
	/// The size can be obtained with GetRWImageSize method.
	/// The buffer must be big enough to be filled with the data.
	///
	void					ReadRWImage( KUInt8* outImage );
	
private:
	FILE*			mFile;		///< File of the image.
	SHeader			mHeader;	///< Header of the ROM image.
};

#endif
		// __TAIFFILE__

// ================================================================== //
// MAC user's dynamic debugging list evaluator?  Never heard of that. //
// ================================================================== //
