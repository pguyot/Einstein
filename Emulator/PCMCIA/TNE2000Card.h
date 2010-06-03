// ==============================
// File:			TNE2000Card.h
// Project:			Einstein
//
// Copyright 2010 by Matthias Melcher (newton@matthiasm.com).
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

#ifndef _TNE2000CARD_H
#define _TNE2000CARD_H

#include <K/Defines/KDefinitions.h>
#include "TPCMCIACard.h"

///
/// Class for a NE2000 Newtwork Adapter card.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TNE2000Card
	:
		public TPCMCIACard
{
public:
	///
	/// Constructor from the size.
	///
	TNE2000Card();

	///
	/// Destructor.
	///
	virtual ~TNE2000Card( void );

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
	static const KUInt8 kCISData[90];
};

#endif
		// _TLINEARCARD_H

// ======================================================================== //
// Trying to establish voice contact ... please ____yell into keyboard. //
// ======================================================================== //
