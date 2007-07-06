// ==============================
// File:			TLinearCard.h
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

#ifndef _TLINEARCARD_H
#define _TLINEARCARD_H

#include <K/Defines/KDefinitions.h>
#include "TPCMCIACard.h"

///
/// Class for a linear card.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TLinearCard
	:
		public TPCMCIACard
{
public:
	///
	/// Constructor from the size.
	///
	TLinearCard( KUInt32 inSize );

	///
	/// Destructor.
	///
	virtual ~TLinearCard( void );

	///
	/// Get PCMCIA pins.
	///
	virtual KUInt32		GetVPCPins( void );
	
	///
	/// Set PCMCIA pins.
	///
	virtual void		SetVPCPins( KUInt32 inPins );

	///
	/// Read attribute space.
	///
	virtual KUInt32		ReadAttr( KUInt32 inOffset );

	///
	/// Read attribute space (byte).
	///
	virtual KUInt8		ReadAttrB( KUInt32 inOffset );

	///
	/// Read I/O space.
	///
	virtual KUInt32		ReadIO( KUInt32 inOffset );

	///
	/// Read I/O space (byte).
	///
	virtual KUInt8		ReadIOB( KUInt32 inOffset );

	///
	/// Read memory space.
	///
	virtual KUInt32		ReadMem( KUInt32 inOffset );

	///
	/// Read memory space (byte).
	///
	virtual KUInt8		ReadMemB( KUInt32 inOffset );

	///
	/// Write attribute space.
	///
	virtual void		WriteAttr( KUInt32 inOffset, KUInt32 inValue );

	///
	/// Write attribute space (byte).
	///
	virtual void		WriteAttrB( KUInt32 inOffset, KUInt8 inValue );

	///
	/// Write I/O space.
	///
	virtual void		WriteIO( KUInt32 inOffset, KUInt32 inValue );

	///
	/// Write I/O space (byte).
	///
	virtual void		WriteIOB( KUInt32 inOffset, KUInt8 inValue );

	///
	/// Write memory space.
	///
	virtual void		WriteMem( KUInt32 inOffset, KUInt32 inValue );

	///
	/// Write memory space (byte).
	///
	virtual void		WriteMemB( KUInt32 inOffset, KUInt8 inValue );

private:
	/// \name Constants
	static const KUInt8 kCISData[0x61];

	/// \name Variables
	const KUInt32		mSize;	///< Size of the linear card.
};

#endif
		// _TLINEARCARD_H

// ======================================================================== //
// Trying to establish voice contact ... please ____yell into keyboard. //
// ======================================================================== //
