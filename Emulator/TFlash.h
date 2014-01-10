// ==============================
// File:			TFlash.h
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

#ifndef _TFLASH_H
#define _TFLASH_H

#include <K/Defines/KDefinitions.h>

// POSIX & ANSI C
#include <stdio.h>
#include <limits.h>

// K
#include <K/Misc/TMappedFile.h>

class TLog;
class TROMImage;
class TStream;

///
/// Class for the Flash.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TFlash
{
public:
	///
	/// Constructor from the rom, the log and a path.
	///
	TFlash( TLog* inLog, const char* inPath, TROMImage* inROMImage );

	///
	/// Destructor.
	///
	~TFlash( void );

	///
	/// Write to flash.
	///
	/// \param inWord		32 bits word to write.
	/// \param inMask		data mask.
	/// \param inOffset		offset of word to read (in bank).
	/// \param inBank		bank.
	///
	void		Write(
					KUInt32 inWord,
					KUInt32 inMask,
					KUInt32 inOffset,
					KUInt32 inBank );

	///
	/// Erase flash (write FF)
	///
	/// \param inAddress	flash address of the block.
	/// \param inBlockSize  number of bytes to erase (block size).
	/// \return true if the address couldn't be accessed for writing.
	///
	void		Erase(
					KUInt32 inBlockSize,
					KUInt32 inOffset,
					KUInt32 inBank );

	///
	/// Read flash.
	///
	/// \param inOffset		offset of word to read (in bank).
	/// \param inBank		bank.
	///
	KUInt32		Read(
					KUInt32 inOffset,
					KUInt32 inBank );

	///
	/// Read a byte from flash.
	///
	/// \param inOffset		offset of byte to read (in bank).
	/// \param inBank		bank.
	///
	KUInt8		ReadB(
					KUInt32 inOffset,
					KUInt32 inBank );

	///
	/// Save or restor the state to or from a stream.
	///
	void		TransferState( TStream* inStream );
		
	///
	/// Save flash to the flash file.
	///
	void		Save( void ) const;

	///
	/// Power flash on.
	///
	void		PowerOn( void ) const;

	///
	/// Power flash off.
	///
	void		PowerOff( void ) const;

	///
	/// Additional constants related to the memory space.
	///
	enum {
		kFlashBank1Size		= 0x00400000,	///< Size of internal store (bank#1)
		kFlashBank2Size		= 0x00400000	///< Size of internal store (bank#2)
	};
	
private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TFlash( const TFlash& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TFlash& operator = ( const TFlash& inCopy );

	/// \name Variables
	TLog*				mLog;					///< Interface to the log.
	TMappedFile			mFlashFile;				///< Flash file.
	KUInt8*				mFlash;					///< Flash buffer.
};

#endif
		// _TFLASH_H

// =================================================== //
// Staff meeting in the conference room in %d minutes. //
// =================================================== //
