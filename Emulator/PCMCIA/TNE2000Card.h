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
/// Class for a NE2000 Network Adapter card.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TNE2000Card
		: public TPCMCIACard
{
public:
	///
	/// Constructor from the size.
	///
	TNE2000Card();

	///
	/// Destructor.
	///
	~TNE2000Card(void) override;

	///
	/// This is a network card. It has no state of its own to save.
	///
	KUInt32
	GetStateTag(void) override
	{
		return 'ne2k';
	}

	///
	/// Get PCMCIA pins.
	///
	KUInt32 GetVPCPins(void) override;

	///
	/// Set PCMCIA pins.
	///
	void SetVPCPins(KUInt32 inPins) override;

	///
	/// Read attribute space.
	///
	KUInt32 ReadAttr(KUInt32 inOffset) override;

	///
	/// Read attribute space (byte).
	///
	KUInt8 ReadAttrB(KUInt32 inOffset) override;

	///
	/// Read I/O space.
	///
	KUInt32 ReadIO(KUInt32 inOffset) override;

	///
	/// Read I/O space (byte).
	///
	KUInt8 ReadIOB(KUInt32 inOffset) override;

	///
	/// Read memory space.
	///
	KUInt32 ReadMem(KUInt32 inOffset) override;

	///
	/// Read memory space (byte).
	///
	KUInt8 ReadMemB(KUInt32 inOffset) override;

	///
	/// Write attribute space.
	///
	void WriteAttr(KUInt32 inOffset, KUInt32 inValue) override;

	///
	/// Write attribute space (byte).
	///
	void WriteAttrB(KUInt32 inOffset, KUInt8 inValue) override;

	///
	/// Write I/O space.
	///
	void WriteIO(KUInt32 inOffset, KUInt32 inValue) override;

	///
	/// Write I/O space (byte).
	///
	void WriteIOB(KUInt32 inOffset, KUInt8 inValue) override;

	///
	/// Write memory space.
	///
	void WriteMem(KUInt32 inOffset, KUInt32 inValue) override;

	///
	/// Write memory space (byte).
	///
	void WriteMemB(KUInt32 inOffset, KUInt8 inValue) override;

private:
	/// \name Constants
	static const KUInt8 kCISData[90];
};

#endif
// _TLINEARCARD_H

// ======================================================================== //
// Trying to establish voice contact ... please ____yell into keyboard. //
// ======================================================================== //
